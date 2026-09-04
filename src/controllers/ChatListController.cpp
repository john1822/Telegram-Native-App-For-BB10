#include "ChatListController.h"
#include "../core/MTProtoSession.h"
#include "../storage/SessionStorage.h"
#include "../storage/MediaCache.h"

#include <QFile>
#include <QDir>
#include <QTextStream>

namespace Telegram {
namespace Controllers {

namespace {
    void chatLog(const QString& msg) {
        QDir().mkpath("data");
        QFile f("data/app_log.txt");
        if (f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream s(&f);
            s << "[CHAT] " << msg << "\n";
            s.flush();
        }
    }
}

ChatListController::ChatListController(Core::MTProtoSession* session, Storage::SessionStorage* storage, QObject* parent)
    : QObject(parent),
      m_session(session),
      m_storage(storage),
      m_isLoading(false),
      m_dialogsReceived(false),
      m_canSend(true),
      m_searchQuery(""),
      m_selectedPeerTitle(""),
      m_selectedPeerId(0),
      m_lastSentPeerId(0),
      m_folderFilter(0),
      m_unreadGroupsCount(0),
      m_unreadChannelsCount(0),
      m_unreadTotalCount(0) {

    m_model = new bb::cascades::GroupDataModel(this);
    m_model->setGrouping(bb::cascades::ItemGrouping::None);

    m_messagesModel = new bb::cascades::GroupDataModel(this);
    m_messagesModel->setGrouping(bb::cascades::ItemGrouping::None);

    m_retryTimer = new QTimer(this);
    m_retryTimer->setSingleShot(true);
    m_retryTimer->setInterval(5000);
    connect(m_retryTimer, SIGNAL(timeout()), this, SLOT(retryDialogs()));

    connect(m_session, SIGNAL(dialogsReceived(QList<QVariantMap>)),
            this, SLOT(onDialogsReceived(QList<QVariantMap>)));
    connect(m_session, SIGNAL(historyReceived(qint64, QList<QVariantMap>)),
            this, SLOT(onHistoryReceived(qint64, QList<QVariantMap>)));
    connect(m_session, SIGNAL(newMessageReceived(qint64, int, QVariantMap)),
            this, SLOT(onNewMessageReceived(qint64, int, QVariantMap)));
    connect(m_session, SIGNAL(messageSent(qint64, qint64, int, int)),
            this, SLOT(onMessageSent(int, int)));
    connect(Storage::MediaCache::instance(), SIGNAL(avatarDownloaded(qint64, QString)),
            this, SLOT(onAvatarDownloaded(qint64, QString)));

    // Load initial cached dialogs from local disk immediately
    QList<QVariantMap> cached = m_storage->loadDialogs();
    if (!cached.isEmpty()) {
        m_allDialogs = cached;
        updateCounts();
        applyFilters();
        emit countChanged(m_allDialogs.size());
    }
}

ChatListController::~ChatListController() {
}

void ChatListController::populateModel(const QList<QVariantMap>& dialogs) {
    m_model->clear();
    for (int i = 0; i < dialogs.size(); ++i) {
        m_model->insert(dialogs[i]);
    }
}

void ChatListController::updateCounts() {
    int groups = 0;
    int channels = 0;
    int unread = 0;
    for (int i = 0; i < m_allDialogs.size(); ++i) {
        int pt = m_allDialogs[i].value("peerType").toInt();
        int u = m_allDialogs[i].value("unreadCount").toInt();
        if (u > 0) {
            unread++;
            if (pt == 2) {
                groups++;
            } else if (pt == 3) {
                channels++;
            }
        }
    }
    m_unreadGroupsCount = groups;
    m_unreadChannelsCount = channels;
    m_unreadTotalCount = unread;
    emit countsUpdated();
}

void ChatListController::setFolderFilter(int folder) {
    if (m_folderFilter == folder) return;
    m_folderFilter = folder;
    emit folderFilterChanged(m_folderFilter);
    applyFilters();
}

void ChatListController::applyFilters() {
    QList<QVariantMap> filtered;
    QString q = m_searchQuery.trimmed().toLower();

    for (int i = 0; i < m_allDialogs.size(); ++i) {
        const QVariantMap& item = m_allDialogs[i];
        int peerType = item.value("peerType").toInt();
        int unread = item.value("unreadCount").toInt();

        // Check folder filter: 0=All, 1=Private, 2=Groups, 3=Channels, 4=Unread
        if (m_folderFilter == 1 && peerType != 1) continue;
        if (m_folderFilter == 2 && peerType != 2) continue;
        if (m_folderFilter == 3 && peerType != 3) continue;
        if (m_folderFilter == 4 && unread <= 0) continue;

        if (!q.isEmpty()) {
            QString title = item.value("title").toString().toLower();
            QString username = item.value("username").toString().toLower();
            QString lastMsg = item.value("lastMessage").toString().toLower();
            if (!title.contains(q) && !username.contains(q) && !lastMsg.contains(q)) {
                continue;
            }
        }
        filtered.append(item);
    }
    populateModel(filtered);
}

void ChatListController::setSearchQuery(const QString& query) {
    if (m_searchQuery == query) return;
    m_searchQuery = query;
    emit searchQueryChanged(m_searchQuery);
    applyFilters();
}

void ChatListController::refreshDialogs() {
    m_isLoading = true;
    m_dialogsReceived = false;
    emit loadingChanged(true);
    m_session->sendMessagesGetDialogs(0, 0, 100);
    m_retryTimer->start();
}

void ChatListController::logDiagnostic(const QString& msg) {
    chatLog("DIAG " + msg);
}

void ChatListController::selectDialog(const QVariantList& indexPath) {
    chatLog("selectDialog called. indexPath size = " + QString::number(indexPath.size()));
    if (indexPath.isEmpty()) return;

    QVariant data = m_model->data(indexPath);
    QVariantMap map = data.toMap();
    if (map.isEmpty()) {
        chatLog("selectDialog: data empty for indexPath");
        return;
    }

    qint64 peerId = map.value("peerId").toLongLong();
    int peerType = map.value("peerType").toInt();
    QString title = map.value("title").toString();
    quint64 accessHash = map.value("accessHash").toULongLong();
    QString lastMsg = map.value("lastMessage").toString();
    QString time = map.value("formattedTime").toString();

    chatLog(QString("selectDialog peerId=%1 type=%2 '%3'").arg(peerId).arg(peerType).arg(title));

    m_messagesModel->clear();
    if (!lastMsg.isEmpty()) {
        QVariantMap m;
        m["id"] = 0;
        m["text"] = lastMsg;
        m["isOutgoing"] = map.value("isOutgoing").toBool();
        m["formattedTime"] = time;
        m_messagesModel->insert(m);
    }

    openChat(peerId, peerType, title, accessHash);
    m_session->sendMessagesGetHistory(peerType, peerId, accessHash, 0, 100);
}

void ChatListController::openChat(qint64 peerId, int peerType, const QString& title, quint64 accessHash) {
    chatLog(QString("openChat peerId=%1 type=%2").arg(peerId).arg(peerType));
    m_selectedPeerId = peerId;
    m_selectedPeerTitle = title;
    m_lastSentPeerId = 0;

    // Determine whether this chat allows sending messages.
    // Private chats and groups are always sendable; channels use the permission
    // flags parsed into the session (read-only broadcast channels hide the
    // composer).
    m_canSend = m_session->canSendToPeer(peerId);

    emit canSendChanged();
    emit selectedPeerChanged();
    emit chatOpened(peerId, peerType, title, accessHash);
}

void ChatListController::onDialogsReceived(const QList<QVariantMap>& dialogs) {
    m_dialogsReceived = true;
    m_retryTimer->stop();
    m_allDialogs = dialogs;
    m_storage->saveDialogs(m_allDialogs);

    updateCounts();
    applyFilters();

    m_isLoading = false;
    emit loadingChanged(false);
    emit countChanged(m_allDialogs.size());
}

void ChatListController::loadHistory(int peerType, const QString& peerIdStr, const QString& accessHashStr) {
    qint64 peerId = peerIdStr.toLongLong();
    quint64 accessHash = accessHashStr.toULongLong();
    if (peerId == 0) {
        peerId = m_selectedPeerId;
    }
    if (peerId == 0) return;
    m_session->sendMessagesGetHistory(peerType, peerId, accessHash, 0, 100);
}

void ChatListController::sendMessage(int peerType, const QString& peerIdStr, const QString& accessHashStr, const QString& text) {
    if (text.trimmed().isEmpty()) return;
    qint64 peerId = peerIdStr.toLongLong();
    quint64 accessHash = accessHashStr.toULongLong();
    if (peerId == 0) peerId = m_selectedPeerId;

    QVariantMap optMsg;
    optMsg["id"] = 0;
    optMsg["text"] = text;
    optMsg["isOutgoing"] = true;
    optMsg["formattedTime"] = QDateTime::currentDateTime().toString("hh:mm");
    m_messagesModel->insert(optMsg);

    m_session->sendMessagesSendMessage(peerType, peerId, accessHash, text);
}

void ChatListController::addInitialMessage(const QString& text, const QString& time) {
    if (text.trimmed().isEmpty()) return;
    if (m_messagesModel->size() == 0) {
        QVariantMap m;
        m["id"] = 0;
        m["text"] = text;
        m["isOutgoing"] = false;
        m["formattedTime"] = time.isEmpty() ? QDateTime::currentDateTime().toString("hh:mm") : time;
        m_messagesModel->insert(m);
    }
}

void ChatListController::onHistoryReceived(qint64 peerId, const QList<QVariantMap>& messages) {
    if (peerId != 0 && peerId != m_selectedPeerId) {
        return;
    }
    if (!messages.isEmpty()) {
        m_messagesModel->clear();
        for (int i = 0; i < messages.size(); ++i) {
            m_messagesModel->insert(messages[i]);
        }
    }
}

void ChatListController::onNewMessageReceived(qint64 peerId, int peerType, const QVariantMap& message) {
    // If the message is for the currently open chat, add it to the messages model
    if (peerId == m_selectedPeerId && m_selectedPeerId != 0) {
        bool isOut = message.value("isOutgoing").toBool();
        int newId = message.value("id").toInt();
        // 1) If this exact message id already exists (e.g. confirmed via the
        //    sentMessage RPC), update it in place instead of inserting a copy.
        bool found = false;
        for (int r = 0; r < m_messagesModel->size(); ++r) {
            QVariantList indexPath;
            indexPath << r;
            QVariantMap item = m_messagesModel->data(indexPath).toMap();
            if (item.value("id").toInt() == newId && newId != 0) {
                m_messagesModel->updateItem(indexPath, message);
                found = true;
                break;
            }
        }
        if (found) {
            return;
        }
        if (isOut) {
            // 2) Otherwise, replace a matching optimistic sent message (id == 0)
            //    with the confirmed server message instead of inserting a duplicate.
            bool replaced = false;
            for (int r = m_messagesModel->size() - 1; r >= 0; --r) {
                QVariantList indexPath;
                indexPath << r;
                QVariantMap item = m_messagesModel->data(indexPath).toMap();
                if (item.value("id").toInt() == 0 && item.value("isOutgoing").toBool() &&
                    item.value("text").toString() == message.value("text").toString()) {
                    QVariantMap updated = message;
                    item["formattedTime"] = message.value("formattedTime");
                    m_messagesModel->updateItem(indexPath, item);
                    replaced = true;
                    break;
                }
            }
            if (!replaced) {
                m_messagesModel->insert(message);
            }
        } else {
            m_messagesModel->insert(message);
        }
    }

    // Update the dialog list: move this dialog to the top, update lastMessage and unread
    for (int i = 0; i < m_allDialogs.size(); ++i) {
        if (m_allDialogs[i].value("peerId").toLongLong() == peerId) {
            bool isOut = message.value("isOutgoing").toBool();
            if (!isOut) {
                int unread = m_allDialogs[i].value("unreadCount").toInt();
                m_allDialogs[i]["unreadCount"] = unread + 1;
            }
            m_allDialogs[i]["lastMessage"] = message.value("text");
            m_allDialogs[i]["formattedTime"] = message.value("formattedTime");

            // Move to front of model for most-recent-first ordering
            QVariantMap item = m_allDialogs.takeAt(i);
            m_allDialogs.prepend(item);
            updateCounts();
            applyFilters();
            return;
        }
    }
}

void ChatListController::onAvatarDownloaded(qint64 peerId, const QString& localPath) {
    for (int i = 0; i < m_allDialogs.size(); ++i) {
        if (m_allDialogs[i].value("peerId").toLongLong() == peerId) {
            m_allDialogs[i]["avatarPath"] = localPath;
            break;
        }
    }

    for (int r = 0; r < m_model->size(); ++r) {
        QVariantList indexPath;
        indexPath << r;
        QVariantMap item = m_model->data(indexPath).toMap();
        if (item.value("peerId").toLongLong() == peerId) {
            item["avatarPath"] = localPath;
            m_model->updateItem(indexPath, item);
            break;
        }
    }
}

void ChatListController::onSessionRestored() {
    m_dialogsReceived = false;
    refreshDialogs();
    m_retryTimer->start();
}

void ChatListController::onMessageSent(int messageId, int date) {
    // Replace the last pending optimistic message (id == 0) with the confirmed
    // server message id and timestamp, so it stays in place (no duplicate).
    for (int r = m_messagesModel->size() - 1; r >= 0; --r) {
        QVariantList indexPath;
        indexPath << r;
        QVariantMap item = m_messagesModel->data(indexPath).toMap();
        if (item.value("id").toInt() == 0 && item.value("isOutgoing").toBool()) {
            item["id"] = messageId;
            item["date"] = date;
            item["formattedTime"] = QDateTime::fromTime_t(date).toString("hh:mm");
            m_messagesModel->updateItem(indexPath, item);
            return;
        }
    }
}

void ChatListController::retryDialogs() {
    if (!m_dialogsReceived) {
        refreshDialogs();
        m_retryTimer->start();
    }
}

} // namespace Controllers
} // namespace Telegram
