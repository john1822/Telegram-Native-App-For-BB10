/* 
 * QR Code generator library (C++)
 * 
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/qr-code-generator-library
 */

#ifndef TELEGRAM_QRCODEGEN_H
#define TELEGRAM_QRCODEGEN_H

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

namespace qrcodegen {

class QrSegment;

class QrCode {
public:
    enum Ecc {
        LOW = 0,
        MEDIUM,
        QUARTILE,
        HIGH
    };

    static QrCode encodeText(const char *text, Ecc ecc);
    static QrCode encodeBinary(const std::vector<uint8_t> &data, Ecc ecc);
    static QrCode encodeSegments(const std::vector<QrSegment> &segs, Ecc ecc,
        int minVersion = 1, int maxVersion = 40, int mask = -1, bool boostEcl = true);

    int getVersion() const;
    int getSize() const;
    Ecc getErrorCorrectionLevel() const;
    int getMask() const;
    bool getModule(int x, int y) const;

public:
    QrCode(int version, Ecc ecc, const std::vector<uint8_t> &dataCodewords, int mask);

private:
    int size;
    int version;
    Ecc errorCorrectionLevel;
    int mask;
    std::vector<std::vector<bool> > modules;
    std::vector<std::vector<bool> > isFunction;

    void drawFunctionPatterns();
    void drawFormatBits(int mask);
    void drawVersion();
    void drawFinderPattern(int x, int y);
    void drawAlignmentPattern(int x, int y);
    void setFunctionModule(int x, int y, bool isBlack);
    static std::vector<uint8_t> addEccAndInterleave(const std::vector<uint8_t> &data, int ver, Ecc ecl);
    static int getNumDataCodewords(int ver, Ecc ecl);
    static int getNumRawDataModules(int ver);
    void drawCodewords(const std::vector<uint8_t> &data);
    void applyMask(int mask);
    long getPenaltyScore() const;
    static std::vector<int> getAlignmentPatternPositions(int ver);
    static uint8_t reedSolomonMultiply(uint8_t x, uint8_t y);
    static std::vector<uint8_t> reedSolomonComputeDivisor(int degree);
    static std::vector<uint8_t> reedSolomonComputeRemainder(const std::vector<uint8_t> &data, const std::vector<uint8_t> &divisor);

public:
    static const int MIN_VERSION = 1;
    static const int MAX_VERSION = 40;
};

class QrSegment {
public:
    enum Mode {
        NUMERIC,
        ALPHANUMERIC,
        BYTE,
        KANJI,
        ECI
    };

    static QrSegment makeBytes(const std::vector<uint8_t> &data);
    static QrSegment makeNumeric(const char *digits);
    static QrSegment makeAlphanumeric(const char *text);
    static QrSegment makeEci(long assignVal);
    static std::vector<QrSegment> makeSegments(const char *text);

    QrSegment(Mode md, int numCh, const std::vector<bool> &bitData);
    QrSegment(Mode md, int numCh, const std::vector<uint8_t> &bData, int bitLen);

    Mode getMode() const;
    int getNumChars() const;
    const std::vector<bool> &getData() const;
    static int numCharCountBits(Mode mode, int version);
    static int getTotalBits(const std::vector<QrSegment> &segs, int version);

    friend class QrCode;

private:
    Mode mode;
    int numChars;
    std::vector<bool> data;
};

} // namespace qrcodegen

#endif // TELEGRAM_QRCODEGEN_H
