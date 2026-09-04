#ifndef TELEGRAM_DIALOG_ITEM_H
#define TELEGRAM_DIALOG_ITEM_H

#include <QString>
#include <QVariantMap>
#include <qglobal.h>

namespace Telegram {
namespace Models {

enum PeerType {
    PEER_USER = 1,
    PEER_CHAT = 2,
    PEER_CHANNEL = 3
};

struct DialogItem {
    qint64 peerId;
    PeerType peerType;
    quint64 accessHash;
    QString title;
    QString username;
    QString lastMessage;
    qint32 date;
    QString formattedTime;
    int unreadCount;
    bool isPinned;
    bool isOutgoing;
    QString initials;
    QString avatarColor;
    QString avatarPath;

    DialogItem()
        : peerId(0), peerType(PEER_USER), accessHash(0), date(0),
          unreadCount(0), isPinned(false), isOutgoing(false) {}

    QVariantMap toMap() const;
    static DialogItem fromMap(const QVariantMap& map);
    static QString formatTimestamp(qint32 epoch);
    static QString computeInitials(const QString& title);
    static QString computeAvatarColor(qint64 peerId);
};

} // namespace Models
} // namespace Telegram

#endif // TELEGRAM_DIALOG_ITEM_H
