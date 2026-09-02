#include "QrGenerator.h"
#include <stdint.h>
#include <stddef.h>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QDir>
#include <vector>
#include <string.h>

namespace Telegram {
namespace Crypto {

// Lightweight QR Code Generator (ISO/IEC 18004 - C++98 compliant)
// Supporting Byte mode for text length up to 150 chars (Version 1-10)

static const int NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
    // Version: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ...
    {0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4}, // Low
    {0, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5}, // Medium
    {0, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8}, // Quartile
    {0, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8}  // High
};

static const int ECC_CODEWORDS_PER_BLOCK[4][41] = {
    {0,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18}, // Low
    {0, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26}, // Medium
    {0, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24}, // Quartile
    {0, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28}  // High
};

static const int TOTAL_CODEWORDS[41] = {
    0, 26, 44, 70, 100, 134, 172, 196, 242, 292, 346
};

// Galois Field GF(256) math
static uint8_t GF_EXP[512];
static uint8_t GF_LOG[256];
static bool gfInitialized = false;

static void initGF() {
    if (gfInitialized) return;
    int val = 1;
    for (int i = 0; i < 255; ++i) {
        GF_EXP[i] = static_cast<uint8_t>(val);
        GF_EXP[i + 255] = static_cast<uint8_t>(val);
        GF_LOG[val] = static_cast<uint8_t>(i);
        val <<= 1;
        if (val & 0x100) val ^= 0x11D;
    }
    GF_LOG[0] = 0;
    gfInitialized = true;
}

static uint8_t gfMul(uint8_t a, uint8_t b) {
    if (a == 0 || b == 0) return 0;
    return GF_EXP[GF_LOG[a] + GF_LOG[b]];
}

static void computeEcc(const uint8_t* data, int dataLen, int eccLen, uint8_t* outEcc) {
    initGF();
    std::vector<uint8_t> gen(eccLen + 1, 0);
    gen[0] = 1;
    for (int i = 0; i < eccLen; ++i) {
        uint8_t root = GF_EXP[i];
        for (int j = i + 1; j >= 1; --j) {
            gen[j] = gen[j] ^ gfMul(gen[j - 1], root);
        }
    }

    std::vector<uint8_t> remainder(eccLen, 0);
    for (int i = 0; i < dataLen; ++i) {
        uint8_t factor = data[i] ^ remainder[0];
        for (int j = 0; j < eccLen - 1; ++j) {
            remainder[j] = remainder[j + 1] ^ gfMul(gen[j + 1], factor);
        }
        remainder[eccLen - 1] = gfMul(gen[eccLen], factor);
    }
    for (int i = 0; i < eccLen; ++i) {
        outEcc[i] = remainder[i];
    }
}

// Simple bit buffer
class BitBuffer {
public:
    std::vector<uint8_t> data;
    int bitCount;

    BitBuffer() : bitCount(0) {}

    void appendBits(uint32_t val, int len) {
        for (int i = len - 1; i >= 0; --i) {
            int bit = (val >> i) & 1;
            if (bitCount % 8 == 0) data.push_back(0);
            data[bitCount / 8] |= (bit << (7 - (bitCount % 8)));
            bitCount++;
        }
    }
};

static void drawFinderPattern(std::vector<std::vector<int> >& matrix, int top, int left, int qrSize) {
    for (int r = 0; r < 7; ++r) {
        for (int c = 0; c < 7; ++c) {
            if (r == 0 || r == 6 || c == 0 || c == 6 || (r >= 2 && r <= 4 && c >= 2 && c <= 4)) {
                matrix[top + r][left + c] = 1;
            } else {
                matrix[top + r][left + c] = 0;
            }
        }
    }
    for (int r = -1; r <= 7; ++r) {
        for (int c = -1; c <= 7; ++c) {
            if (r == -1 || r == 7 || c == -1 || c == 7) {
                int rr = top + r, cc = left + c;
                if (rr >= 0 && rr < qrSize && cc >= 0 && cc < qrSize && matrix[rr][cc] == -1) {
                    matrix[rr][cc] = 0;
                }
            }
        }
    }
}

bool QrGenerator::generateQrPng(const QString& text, const QString& outputPath, int pixelSize) {
    QByteArray bytes = text.toLatin1();
    int dataLen = bytes.size();

    // Select Version 1-10 with Medium ECC
    int version = 1;
    for (int v = 1; v <= 10; ++v) {
        int totalCodewords = TOTAL_CODEWORDS[v];
        int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[1][v];
        int eccBlockLen = ECC_CODEWORDS_PER_BLOCK[1][v];
        int dataCapacity = totalCodewords - (numBlocks * eccBlockLen);
        int maxDataBits = (dataCapacity * 8);
        int requiredBits = 4 + (v < 10 ? 8 : 16) + (dataLen * 8);
        if (requiredBits <= maxDataBits) {
            version = v;
            break;
        }
    }

    int totalCodewords = TOTAL_CODEWORDS[version];
    int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[1][version];
    int eccBlockLen = ECC_CODEWORDS_PER_BLOCK[1][version];
    int totalDataCodewords = totalCodewords - (numBlocks * eccBlockLen);

    BitBuffer bb;
    // 1. Mode indicator: Byte mode = 0100 (4 bits)
    bb.appendBits(0x04, 4);
    // 2. Character count indicator: 8 bits for versions 1-9, 16 bits for version 10
    bb.appendBits(dataLen, version < 10 ? 8 : 16);
    // 3. Payload
    for (int i = 0; i < dataLen; ++i) {
        bb.appendBits(static_cast<uint8_t>(bytes[i]), 8);
    }
    // 4. Terminator (up to 4 zeroes)
    int maxDataBits = totalDataCodewords * 8;
    int padZeros = maxDataBits - bb.bitCount;
    if (padZeros > 4) padZeros = 4;
    bb.appendBits(0, padZeros);
    // Byte alignment
    if (bb.bitCount % 8 != 0) {
        bb.appendBits(0, 8 - (bb.bitCount % 8));
    }
    // Pad bytes (0xEC, 0x11 alternating)
    uint8_t padBytes[2] = {0xEC, 0x11};
    int padIdx = 0;
    while (static_cast<int>(bb.data.size()) < totalDataCodewords) {
        bb.data.push_back(padBytes[padIdx]);
        padIdx = 1 - padIdx;
    }

    // 5. Compute Error Correction Blocks
    int shortBlockLen = totalDataCodewords / numBlocks;
    int longBlocks = totalDataCodewords % numBlocks;
    int shortBlocks = numBlocks - longBlocks;

    std::vector<std::vector<uint8_t> > dataBlocks(numBlocks);
    std::vector<std::vector<uint8_t> > eccBlocks(numBlocks);

    int offset = 0;
    for (int b = 0; b < numBlocks; ++b) {
        int bLen = (b < shortBlocks) ? shortBlockLen : (shortBlockLen + 1);
        dataBlocks[b].assign(bb.data.begin() + offset, bb.data.begin() + offset + bLen);
        offset += bLen;

        eccBlocks[b].resize(eccBlockLen);
        computeEcc(&dataBlocks[b][0], bLen, eccBlockLen, &eccBlocks[b][0]);
    }

    // 6. Interleave data and ecc codewords
    std::vector<uint8_t> allCodewords;
    for (int i = 0; i <= shortBlockLen; ++i) {
        for (int b = 0; b < numBlocks; ++b) {
            if (i < static_cast<int>(dataBlocks[b].size())) {
                allCodewords.push_back(dataBlocks[b][i]);
            }
        }
    }
    for (int i = 0; i < eccBlockLen; ++i) {
        for (int b = 0; b < numBlocks; ++b) {
            allCodewords.push_back(eccBlocks[b][i]);
        }
    }

    // 7. Grid Matrix
    int qrSize = 17 + version * 4;
    std::vector<std::vector<int> > matrix(qrSize, std::vector<int>(qrSize, -1));

    drawFinderPattern(matrix, 0, 0, qrSize);
    drawFinderPattern(matrix, 0, qrSize - 7, qrSize);
    drawFinderPattern(matrix, qrSize - 7, 0, qrSize);

    // Timing patterns
    for (int i = 8; i < qrSize - 8; ++i) {
        if (matrix[6][i] == -1) matrix[6][i] = (i % 2 == 0) ? 1 : 0;
        if (matrix[i][6] == -1) matrix[i][6] = (i % 2 == 0) ? 1 : 0;
    }

    // Alignment patterns for version >= 2
    if (version >= 2) {
        int alignPos = qrSize - 7;
        for (int r = alignPos - 2; r <= alignPos + 2; ++r) {
            for (int c = alignPos - 2; c <= alignPos + 2; ++c) {
                if (r == alignPos - 2 || r == alignPos + 2 || c == alignPos - 2 || c == alignPos + 2 || (r == alignPos && c == alignPos)) {
                    matrix[r][c] = 1;
                } else {
                    matrix[r][c] = 0;
                }
            }
        }
    }

    // Dark module
    matrix[4 * version + 9][8] = 1;

    // Reserve format bits area
    for (int i = 0; i < 9; ++i) {
        if (matrix[8][i] == -1) matrix[8][i] = 0;
        if (matrix[i][8] == -1) matrix[i][8] = 0;
        if (matrix[8][qrSize - 1 - i] == -1) matrix[8][qrSize - 1 - i] = 0;
        if (matrix[qrSize - 1 - i][8] == -1) matrix[qrSize - 1 - i][8] = 0;
    }

    // Fill data bits
    int bitIdx = 0;
    int totalBits = static_cast<int>(allCodewords.size()) * 8;
    int row = qrSize - 1;
    int dir = -1;

    for (int col = qrSize - 1; col > 0; col -= 2) {
        if (col == 6) col = 5;
        for (int v = 0; v < qrSize; ++v) {
            int r = row;
            for (int c = 0; c < 2; ++c) {
                int cc = col - c;
                if (matrix[r][cc] == -1) {
                    int bit = 0;
                    if (bitIdx < totalBits) {
                        bit = (allCodewords[bitIdx / 8] >> (7 - (bitIdx % 8))) & 1;
                        bitIdx++;
                    }
                    // Apply Mask 0: (row + col) % 2 == 0
                    if ((r + cc) % 2 == 0) {
                        bit ^= 1;
                    }
                    matrix[r][cc] = bit;
                }
            }
            row += dir;
        }
        dir = -dir;
        row += dir;
    }

    // Draw Format Bits: ECC M, Mask 0
    uint16_t formatBits = 0x7CA4;
    for (int i = 0; i < 15; ++i) {
        int b = (formatBits >> (14 - i)) & 1;
        // Top-left
        if (i < 6) matrix[8][i] = b;
        else if (i < 8) matrix[8][i + 1] = b;
        else if (i == 8) matrix[7][8] = b;
        else matrix[14 - i][8] = b;

        // Top-right & bottom-left
        if (i < 8) matrix[qrSize - 1 - i][8] = b;
        else matrix[8][qrSize - 15 + i] = b;
    }

    // 8. Render to QImage (Crisp White Margin & Sharp Black Modules)
    int margin = 4;
    int fullGridSize = qrSize + (margin * 2);
    int modulePixels = pixelSize / fullGridSize;
    if (modulePixels < 4) modulePixels = 4;
    int imgDim = fullGridSize * modulePixels;

    QImage image(imgDim, imgDim, QImage::Format_RGB32);
    image.fill(QColor(255, 255, 255));

    QPainter painter(&image);
    painter.setBrush(QColor(0, 0, 0));
    painter.setPen(Qt::NoPen);

    for (int r = 0; r < qrSize; ++r) {
        for (int c = 0; c < qrSize; ++c) {
            if (matrix[r][c] == 1) {
                int px = (c + margin) * modulePixels;
                int py = (r + margin) * modulePixels;
                painter.drawRect(px, py, modulePixels, modulePixels);
            }
        }
    }
    painter.end();

    QDir().mkpath("data");
    return image.save(outputPath, "PNG");
}

} // namespace Crypto
} // namespace Telegram
