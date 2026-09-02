#ifndef TELEGRAM_CHAT_LIST_CONTROLLER_H
#define TELEGRAM_CHAT_LIST_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>
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
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(int dialogsCount READ dialogsCount NOTIFY countChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(QString selectedPeerTitle READ selectedPeerTitle NOTIFY selectedPeerChanged)
    Q_PROPERTY(qint64 selectedPeerId READ selectedPeerId NOTIFY selectedPeerChanged)

public:
    explicit ChatListController(Core::MTProtoSession* session, Storage::SessionStorage* storage, QObject* parent = 0);
    virtual ~ChatListController();

    bb::cascades::GroupDataModel* model() const { return m_model; }
    bool isLoading() const { return m_isLoading; }
    int dialogsCount() const { return m_allDialogs.size(); }
    QString searchQuery() const { return m_searchQuery; }
    QString selectedPeerTitle() const { return m_selectedPeerTitle; }
    qint64 selectedPeerId() const { return m_selectedPeerId; }

    void setSearchQuery(const QString& query);

    Q_INVOKABLE void refreshDialogs();
    Q_INVOKABLE void selectDialog(const QVariantList& indexPath);
    Q_INVOKABLE void openChat(qint64 peerId, int peerType, const QString& title, quint64 accessHash);

public slots:
    void onDialogsReceived(const QList<QVariantMap>& dialogs);
    void onSessionRestored();

signals:
    void loadingChanged(bool loading);
    void countChanged(int count);
    void searchQueryChanged(const QString& query);
    void selectedPeerChanged();
    void chatOpened(qint64 peerId, int peerType, const QString& title, quint64 accessHash);

private:
    void populateModel(const QList<QVariantMap>& dialogs);

private:
    Core::MTProtoSession* m_session;
    Storage::SessionStorage* m_storage;
    bb::cascades::GroupDataModel* m_model;
    QList<QVariantMap> m_allDialogs;
    bool m_isLoading;
    QString m_searchQuery;
    QString m_selectedPeerTitle;
    qint64 m_selectedPeerId;
};

} // namespace Controllers
} // namespace Telegram

#endif // TELEGRAM_CHAT_LIST_CONTROLLER_H
