#include "DialogItem.h"
#include <QDateTime>
#include <QStringList>

namespace Telegram {
namespace Models {

static const char* s_avatarColors[] = {
    "#e17076", // red
    "#faa774", // orange
    "#a695e7", // violet
    "#7bc862", // green
    "#6ec9cb", // cyan
    "#65aadd", // blue
    "#ee7aae"  // pink
};
static const int s_avatarColorsCount = sizeof(s_avatarColors) / sizeof(s_avatarColors[0]);

QVariantMap DialogItem::toMap() const {
    QVariantMap map;
    map["peerId"] = peerId;
    map["peerType"] = static_cast<int>(peerType);
    map["accessHash"] = accessHash;
    map["title"] = title;
    map["username"] = username;
    map["lastMessage"] = lastMessage;
    map["date"] = date;
    map["formattedTime"] = formattedTime.isEmpty() ? formatTimestamp(date) : formattedTime;
    map["unreadCount"] = unreadCount;
    map["isPinned"] = isPinned;
    map["isOutgoing"] = isOutgoing;
    map["initials"] = initials.isEmpty() ? computeInitials(title) : initials;
    map["avatarColor"] = avatarColor.isEmpty() ? computeAvatarColor(peerId) : avatarColor;
    return map;
}

DialogItem DialogItem::fromMap(const QVariantMap& map) {
    DialogItem item;
    item.peerId = map.value("peerId").toLongLong();
    item.peerType = static_cast<PeerType>(map.value("peerType").toInt());
    item.accessHash = map.value("accessHash").toULongLong();
    item.title = map.value("title").toString();
    item.username = map.value("username").toString();
    item.lastMessage = map.value("lastMessage").toString();
    item.date = map.value("date").toInt();
    item.formattedTime = map.value("formattedTime").toString();
    item.unreadCount = map.value("unreadCount").toInt();
    item.isPinned = map.value("isPinned").toBool();
    item.isOutgoing = map.value("isOutgoing").toBool();
    item.initials = map.value("initials").toString();
    item.avatarColor = map.value("avatarColor").toString();
    return item;
}

QString DialogItem::formatTimestamp(qint32 epoch) {
    if (epoch <= 0) return QString();

    QDateTime msgTime = QDateTime::fromTime_t(static_cast<uint>(epoch));
    QDateTime now = QDateTime::currentDateTime();

    if (msgTime.date() == now.date()) {
        return msgTime.toString("hh:mm");
    } else if (msgTime.daysTo(now) == 1) {
        return "Yesterday";
    } else if (msgTime.daysTo(now) < 7) {
        return msgTime.toString("ddd");
    } else {
        return msgTime.toString("d MMM");
    }
}

QString DialogItem::computeInitials(const QString& title) {
    QString clean = title.trimmed();
    if (clean.isEmpty()) return "?";

    QStringList parts = clean.split(' ', QString::SkipEmptyParts);
    if (parts.size() >= 2) {
        QString first = parts[0].left(1);
        QString second = parts[1].left(1);
        return (first + second).toUpper();
    }
    return clean.left(1).toUpper();
}

QString DialogItem::computeAvatarColor(qint64 peerId) {
    quint64 absId = static_cast<quint64>(peerId < 0 ? -peerId : peerId);
    int idx = static_cast<int>(absId % s_avatarColorsCount);
    return QString::fromLatin1(s_avatarColors[idx]);
}

} // namespace Models
} // namespace Telegram
