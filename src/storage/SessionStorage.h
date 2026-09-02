#ifndef TELEGRAM_SESSION_STORAGE_H
#define TELEGRAM_SESSION_STORAGE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <qglobal.h>

namespace Telegram {
namespace Storage {

class SessionStorage : public QObject {
    Q_OBJECT

public:
    explicit SessionStorage(QObject* parent = 0);
    virtual ~SessionStorage();

    bool hasActiveSession() const;
    bool loadSession();
    bool saveSession(int dcId, const QString& dcIp, int dcPort,
                     quint64 authKeyId, const QByteArray& authKey, quint64 serverSalt);

    bool saveUserProfile(qint64 userId, quint64 accessHash,
                         const QString& firstName, const QString& lastName,
                         const QString& username, const QString& phone);

    void clearSession();

    // Getters
    int dcId() const { return m_dcId; }
    QString dcIp() const { return m_dcIp; }
    int dcPort() const { return m_dcPort; }
    quint64 authKeyId() const { return m_authKeyId; }
    QByteArray authKey() const { return m_authKey; }
    quint64 serverSalt() const { return m_serverSalt; }

    qint64 userId() const { return m_userId; }
    quint64 accessHash() const { return m_accessHash; }
    QString firstName() const { return m_firstName; }
    QString lastName() const { return m_lastName; }
    QString username() const { return m_username; }
    QString phone() const { return m_phone; }
    bool isLoggedIn() const { return m_isLoggedIn; }

signals:
    void sessionLoaded(bool valid);
    void sessionSaved();
    void sessionCleared();

private:
    QString sessionFilePath() const;

private:
    int m_dcId;
    QString m_dcIp;
    int m_dcPort;
    quint64 m_authKeyId;
    QByteArray m_authKey;
    quint64 m_serverSalt;

    qint64 m_userId;
    quint64 m_accessHash;
    QString m_firstName;
    QString m_lastName;
    QString m_username;
    QString m_phone;
    bool m_isLoggedIn;
};

} // namespace Storage
} // namespace Telegram

#endif // TELEGRAM_SESSION_STORAGE_H
