#ifndef TELEGRAM_MEDIA_CACHE_H
#define TELEGRAM_MEDIA_CACHE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDir>
#include <QFile>

namespace Telegram {
namespace Storage {

class MediaCache : public QObject {
    Q_OBJECT

public:
    explicit MediaCache(QObject* parent = 0);
    virtual ~MediaCache();

    static MediaCache* instance();

    bool hasAvatar(qint64 peerId) const;
    QString avatarPath(qint64 peerId) const;
    QString saveAvatar(qint64 peerId, const QByteArray& data);

    static QByteArray strippedPhotoToJpg(const QByteArray& stripped);

    bool hasPhoto(qint64 photoId) const;
    QString photoPath(qint64 photoId) const;
    QString savePhoto(qint64 photoId, const QByteArray& data);

signals:
    void avatarDownloaded(qint64 peerId, const QString& localPath);
    void photoDownloaded(qint64 photoId, const QString& localPath);

private:
    void initDirectories();

    QString m_baseDir;
    QString m_avatarsDir;
    QString m_photosDir;

    static MediaCache* s_instance;
};

} // namespace Storage
} // namespace Telegram

#endif // TELEGRAM_MEDIA_CACHE_H
