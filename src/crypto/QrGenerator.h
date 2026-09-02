#ifndef TELEGRAM_QR_GENERATOR_H
#define TELEGRAM_QR_GENERATOR_H

#include <QString>
#include <QImage>

namespace Telegram {
namespace Crypto {

class QrGenerator {
public:
    static bool generateQrPng(const QString& text, const QString& outputPath, int pixelSize = 280);
};

} // namespace Crypto
} // namespace Telegram

#endif // TELEGRAM_QR_GENERATOR_H
