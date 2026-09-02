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

namespace Telegram {
namespace Qr {

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

    int getSize() const;
    bool getModule(int x, int y) const;

private:
    int m_size;
    std::vector<bool> m_modules;

public:
    QrCode(int version, Ecc ecc, const std::vector<uint8_t> &dataCodewords, int mask);
};

} // namespace Qr
} // namespace Telegram

#endif // TELEGRAM_QRCODEGEN_H
