#include "QrGenerator.h"
#include "qrcodegen.h"
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QDir>

namespace Telegram {
namespace Crypto {

bool QrGenerator::generateQrPng(const QString& text, const QString& outputPath, int pixelSize) {
    try {
        using qrcodegen::QrCode;
        QrCode qr = QrCode::encodeText(text.toUtf8().constData(), QrCode::MEDIUM);
        int qrSize = qr.getSize();

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

        for (int y = 0; y < qrSize; ++y) {
            for (int x = 0; x < qrSize; ++x) {
                if (qr.getModule(x, y)) {
                    int px = (x + margin) * modulePixels;
                    int py = (y + margin) * modulePixels;
                    painter.drawRect(px, py, modulePixels, modulePixels);
                }
            }
        }
        painter.end();

        QDir().mkpath("data");
        return image.save(outputPath, "PNG");
    } catch (...) {
        return false;
    }
}

} // namespace Crypto
} // namespace Telegram
