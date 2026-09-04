#ifndef TELEGRAM_CHAT_LIST_CONTROLLER_H
#define TELEGRAM_CHAT_LIST_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>
#include <QTimer>
#include <bb/cascades/GroupDataModel>

namespace Telegram {
namespace Core {
    class MTProtoSession;
}
namespace Storage {
    class SessionStorage;
}

namespace Controllers {

class ChatListController : public QObject {
    Q_OBJECT

    Q_PROPERTY(bb::cascades::GroupDataModel* model READ model CONSTANT)
    Q_PROPERTY(bb::cascades::GroupDataModel* messagesModel READ messagesModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(int dialogsCount READ dialogsCount NOTIFY countChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(QString selectedPeerTitle READ selectedPeerTitle NOTIFY selectedPeerChanged)
    Q_PROPERTY(qint64 selectedPeerId READ selectedPeerId NOTIFY selectedPeerChanged)
    Q_PROPERTY(bool canSend READ canSend NOTIFY canSendChanged)
    Q_PROPERTY(int folderFilter READ folderFilter WRITE setFolderFilter NOTIFY folderFilterChanged)
    Q_PROPERTY(int unreadGroupsCount READ unreadGroupsCount NOTIFY countsUpdated)
    Q_PROPERTY(int unreadChannelsCount READ unreadChannelsCount NOTIFY countsUpdated)
    Q_PROPERTY(int unreadTotalCount READ unreadTotalCount NOTIFY countsUpdated)

public:
    explicit ChatListController(Core::MTProtoSession* session, Storage::SessionStorage* storage, QObject* parent = 0);
    virtual ~ChatListController();

    bb::cascades::GroupDataModel* model() const { return m_model; }
    bb::cascades::GroupDataModel* messagesModel() const { return m_messagesModel; }
    bool isLoading() const { return m_isLoading; }
    int dialogsCount() const { return m_allDialogs.size(); }
    QString searchQuery() const { return m_searchQuery; }
    QString selectedPeerTitle() const { return m_selectedPeerTitle; }
    qint64 selectedPeerId() const { return m_selectedPeerId; }
    bool canSend() const { return m_canSend; }
    int folderFilter() const { return m_folderFilter; }
    int unreadGroupsCount() const { return m_unreadGroupsCount; }
    int unreadChannelsCount() const { return m_unreadChannelsCount; }
    int unreadTotalCount() const { return m_unreadTotalCount; }

    void setSearchQuery(const QString& query);
    Q_INVOKABLE void setFolderFilter(int folder);

    Q_INVOKABLE void refreshDialogs();
    Q_INVOKABLE void selectDialog(const QVariantList& indexPath);
    Q_INVOKABLE void openChat(qint64 peerId, int peerType, const QString& title, quint64 accessHash);
    Q_INVOKABLE void loadHistory(int peerType, const QString& peerIdStr, const QString& accessHashStr);
    Q_INVOKABLE void sendMessage(int peerType, const QString& peerIdStr, const QString& accessHashStr, const QString& text);
    Q_INVOKABLE void addInitialMessage(const QString& text, const QString& time);
    Q_INVOKABLE void logDiagnostic(const QString& msg);

public slots:
    void onDialogsReceived(const QList<QVariantMap>& dialogs);
    void onHistoryReceived(qint64 peerId, const QList<QVariantMap>& messages);
    void onAvatarDownloaded(qint64 peerId, const QString& localPath);
    void onSessionRestored();
    void onNewMessageReceived(qint64 peerId, int peerType, const QVariantMap& message);
    void onMessageSent(int messageId, int date);
    void retryDialogs();

signals:
    void loadingChanged(bool loading);
    void countChanged(int count);
    void searchQueryChanged(const QString& query);
    void selectedPeerChanged();
    void chatOpened(qint64 peerId, int peerType, const QString& title, quint64 accessHash);
    void canSendChanged();
    void folderFilterChanged(int folder);
    void countsUpdated();

private:
    void populateModel(const QList<QVariantMap>& dialogs);
    void applyFilters();
    void updateCounts();

private:
    Core::MTProtoSession* m_session;
    Storage::SessionStorage* m_storage;
    bb::cascades::GroupDataModel* m_model;
    bb::cascades::GroupDataModel* m_messagesModel;
    QList<QVariantMap> m_allDialogs;
    bool m_isLoading;
    bool m_dialogsReceived;
    bool m_canSend;
    QTimer* m_retryTimer;
    QString m_searchQuery;
    QString m_selectedPeerTitle;
    qint64 m_selectedPeerId;
    qint64 m_lastSentPeerId;
    int m_folderFilter;
    int m_unreadGroupsCount;
    int m_unreadChannelsCount;
    int m_unreadTotalCount;
};

} // namespace Controllers
} // namespace Telegram

#endif // TELEGRAM_CHAT_LIST_CONTROLLER_H
