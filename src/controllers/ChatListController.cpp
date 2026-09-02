#include "ChatListController.h"
#include "../core/MTProtoSession.h"
#include "../storage/SessionStorage.h"

namespace Telegram {
namespace Controllers {

ChatListController::ChatListController(Core::MTProtoSession* session, Storage::SessionStorage* storage, QObject* parent)
    : QObject(parent),
      m_session(session),
      m_storage(storage),
      m_isLoading(false),
      m_searchQuery(""),
      m_selectedPeerTitle(""),
      m_selectedPeerId(0) {

    m_model = new bb::cascades::GroupDataModel(this);
    m_model->setGrouping(bb::cascades::ItemGrouping::None);

    m_messagesModel = new bb::cascades::GroupDataModel(this);
    m_messagesModel->setGrouping(bb::cascades::ItemGrouping::None);

    connect(m_session, SIGNAL(dialogsReceived(QList<QVariantMap>)),
            this, SLOT(onDialogsReceived(QList<QVariantMap>)));
    connect(m_session, SIGNAL(historyReceived(qint64, QList<QVariantMap>)),
            this, SLOT(onHistoryReceived(qint64, QList<QVariantMap>)));

    // Load initial cached dialogs from local disk immediately
    QList<QVariantMap> cached = m_storage->loadDialogs();
    if (!cached.isEmpty()) {
        m_allDialogs = cached;
        populateModel(m_allDialogs);
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

void ChatListController::setSearchQuery(const QString& query) {
    if (m_searchQuery == query) return;
    m_searchQuery = query;
    emit searchQueryChanged(m_searchQuery);

    if (m_searchQuery.trimmed().isEmpty()) {
        populateModel(m_allDialogs);
    } else {
        QList<QVariantMap> filtered;
        QString q = m_searchQuery.trimmed().toLower();
        for (int i = 0; i < m_allDialogs.size(); ++i) {
            const QVariantMap& item = m_allDialogs[i];
            QString title = item.value("title").toString().toLower();
            QString username = item.value("username").toString().toLower();
            QString lastMsg = item.value("lastMessage").toString().toLower();

            if (title.contains(q) || username.contains(q) || lastMsg.contains(q)) {
                filtered.append(item);
            }
        }
        populateModel(filtered);
    }
}

void ChatListController::refreshDialogs() {
    m_isLoading = true;
    emit loadingChanged(true);
    m_session->sendMessagesGetDialogs(0, 0, 40);
}

void ChatListController::selectDialog(const QVariantList& indexPath) {
    if (indexPath.isEmpty()) return;

    QVariant data = m_model->data(indexPath);
    QVariantMap map = data.toMap();
    if (map.isEmpty()) return;

    qint64 peerId = map.value("peerId").toLongLong();
    int peerType = map.value("peerType").toInt();
    QString title = map.value("title").toString();
    quint64 accessHash = map.value("accessHash").toULongLong();
    QString lastMsg = map.value("lastMessage").toString();
    QString time = map.value("formattedTime").toString();

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
    m_session->sendMessagesGetHistory(peerType, peerId, accessHash, 0, 50);
}

void ChatListController::openChat(qint64 peerId, int peerType, const QString& title, quint64 accessHash) {
    m_selectedPeerId = peerId;
    m_selectedPeerTitle = title;
    emit selectedPeerChanged();
    emit chatOpened(peerId, peerType, title, accessHash);
}

void ChatListController::onDialogsReceived(const QList<QVariantMap>& dialogs) {
    m_allDialogs = dialogs;
    m_storage->saveDialogs(m_allDialogs);

    if (m_searchQuery.trimmed().isEmpty()) {
        populateModel(m_allDialogs);
    } else {
        setSearchQuery(m_searchQuery);
    }

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
    m_session->sendMessagesGetHistory(peerType, peerId, accessHash, 0, 50);
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
    Q_UNUSED(peerId);
    if (!messages.isEmpty()) {
        m_messagesModel->clear();
        for (int i = 0; i < messages.size(); ++i) {
            m_messagesModel->insert(messages[i]);
        }
    }
}

void ChatListController::onSessionRestored() {
    refreshDialogs();
}

} // namespace Controllers
} // namespace Telegram
