#ifndef TELEGRAM_DIAGNOSTIC_CONTROLLER_H
#define TELEGRAM_DIAGNOSTIC_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>

namespace Telegram {

namespace Core {
class MTProtoSession;
}

namespace Controllers {

class DiagnosticController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString authKeyId READ authKeyId NOTIFY authKeyIdChanged)
    Q_PROPERTY(QString dcInfo READ dcInfo NOTIFY dcInfoChanged)
    Q_PROPERTY(QString country READ country NOTIFY countryChanged)
    Q_PROPERTY(QString logsText READ logsText NOTIFY logsChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)
    Q_PROPERTY(bool isEncrypted READ isEncrypted NOTIFY encryptionChanged)

public:
    explicit DiagnosticController(Core::MTProtoSession* session = NULL, QObject* parent = NULL);
    virtual ~DiagnosticController();

    QString statusText() const;
    QString authKeyId() const;
    QString dcInfo() const;
    QString country() const;
    QString logsText() const;
    bool isConnected() const;
    bool isEncrypted() const;

    Q_INVOKABLE void startConnection();
    Q_INVOKABLE void reconnect();
    Q_INVOKABLE void sendTestRpc();
    Q_INVOKABLE void clearLogs();

signals:
    void statusTextChanged();
    void authKeyIdChanged();
    void dcInfoChanged();
    void countryChanged();
    void logsChanged();
    void connectionChanged();
    void encryptionChanged();

private slots:
    void onStateChanged(int state, const QString& stateText);
    void onLogMessage(const QString& log);
    void onAuthKeyGenerated(quint64 authKeyIdHex);
    void onNearestDcReceived(const QString& country, int thisDc, int nearestDc);
    void onErrorOccurred(const QString& error);

private:
    Core::MTProtoSession* m_session;
    QString m_statusText;
    QString m_authKeyId;
    QString m_dcInfo;
    QString m_country;
    QStringList m_logs;
    bool m_connected;
    bool m_encrypted;
};

} // namespace Controllers
} // namespace Telegram

#endif // TELEGRAM_DIAGNOSTIC_CONTROLLER_H
