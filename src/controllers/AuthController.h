#ifndef TELEGRAM_AUTH_CONTROLLER_H
#define TELEGRAM_AUTH_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <qglobal.h>

namespace Telegram {
namespace Core {
    class MTProtoSession;
}
namespace Storage {
    class SessionStorage;
}

namespace Controllers {

class AuthController : public QObject {
    Q_OBJECT

    Q_PROPERTY(int authState READ authState NOTIFY authStateChanged)
    Q_PROPERTY(QString authStateString READ authStateString NOTIFY authStateChanged)
    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString phoneCodeHash READ phoneCodeHash NOTIFY phoneCodeHashChanged)
    Q_PROPERTY(QString deliveryType READ deliveryType NOTIFY deliveryTypeChanged)
    Q_PROPERTY(int codeTimeout READ codeTimeout NOTIFY codeTimeoutChanged)
    Q_PROPERTY(QString passwordHint READ passwordHint NOTIFY passwordHintChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    
    // Logged in User Profile
    Q_PROPERTY(QString userName READ userName NOTIFY userProfileChanged)
    Q_PROPERTY(QString userHandle READ userHandle NOTIFY userProfileChanged)
    Q_PROPERTY(QString userId READ userId NOTIFY userProfileChanged)
    Q_PROPERTY(QString userPhone READ userPhone NOTIFY userProfileChanged)
    Q_PROPERTY(QString userBio READ userBio NOTIFY userProfileChanged)
    Q_PROPERTY(QString qrTokenUrl READ qrTokenUrl NOTIFY qrTokenUrlChanged)
    Q_PROPERTY(QString qrImagePath READ qrImagePath NOTIFY qrImagePathChanged)

public:
    enum AuthState {
        STATE_INITIALIZING = 0,
        STATE_CONNECTING = 1,
        STATE_PHONE_INPUT = 2,
        STATE_CODE_INPUT = 3,
        STATE_PASSWORD_INPUT = 4,
        STATE_LOGGED_IN = 5,
        STATE_QR_LOGIN = 6
    };

    explicit AuthController(Core::MTProtoSession* session, Storage::SessionStorage* storage, QObject* parent = 0);
    virtual ~AuthController();

    int authState() const { return static_cast<int>(m_authState); }
    QString authStateString() const;
    QString phoneNumber() const { return m_phoneNumber; }
    QString phoneCodeHash() const { return m_phoneCodeHash; }
    QString deliveryType() const { return m_deliveryType; }
    int codeTimeout() const { return m_codeTimeout; }
    QString passwordHint() const { return m_passwordHint; }
    bool isBusy() const { return m_isBusy; }
    QString statusMessage() const { return m_statusMessage; }

    QString userName() const { return m_userName; }
    QString userHandle() const { return m_userHandle; }
    QString userId() const { return m_userId; }
    QString userPhone() const { return m_userPhone; }
    QString userBio() const { return m_userBio; }
    QString qrTokenUrl() const { return m_qrTokenUrl; }
    QString qrImagePath() const { return m_qrImagePath; }

    void setPhoneNumber(const QString& phone);

public slots:
    void start();
    void submitPhoneNumber(const QString& phone);
    void submitLoginCode(const QString& code);
    void submitPassword(const QString& password);
    void resendCode();
    void requestQrLogin();
    void changePhoneNumber();
    void cancelQrLogin();
    void logout();

signals:
    void authStateChanged(int state);
    void phoneNumberChanged(const QString& phone);
    void phoneCodeHashChanged(const QString& hash);
    void deliveryTypeChanged(const QString& type);
    void codeTimeoutChanged(int timeout);
    void passwordHintChanged(const QString& hint);
    void isBusyChanged(bool busy);
    void statusMessageChanged(const QString& msg);
    void userProfileChanged();
    void qrTokenUrlChanged(const QString& url);
    void qrImagePathChanged(const QString& path);
    void authErrorOccurred(const QString& error);

private slots:
    void onSessionStateChanged(int newState, const QString& stateText);
    void onAuthKeyGenerated(quint64 authKeyIdHex);
    void onAuthSentCodeReceived(const QString& phoneCodeHash, const QString& type, int timeout);
    void onAuthPasswordNeeded(const QString& hint);
    void onAuthSuccessReceived(qint64 userId, quint64 accessHash, const QString& firstName, const QString& lastName, const QString& username, const QString& phone);
    void onAuthSignUpRequired();
    void onAuthLoginTokenReceived(const QByteArray& token, int expires);
    void onAuthLoginSuccess();
    void onDcMigrated(int newDcId);
    void onRpcErrorReceived(int errorCode, const QString& errorMessage);
    void onSessionError(const QString& error);
    void onQrPollTimer();
    void onMyProfileReceived(const QString& bio, const QString& username, const QString& phone);

private:
    void setAuthState(AuthState state);
    void setBusy(bool busy);
    void setStatus(const QString& msg);

private:
    Core::MTProtoSession* m_session;
    Storage::SessionStorage* m_storage;
    QTimer* m_qrPollTimer;

    AuthState m_authState;
    QString m_phoneNumber;
    QString m_phoneCodeHash;
    QString m_deliveryType;
    int m_codeTimeout;
    QString m_passwordHint;
    bool m_isBusy;
    QString m_statusMessage;

    QString m_userName;
    QString m_userHandle;
    QString m_userId;
    QString m_userPhone;
    QString m_userBio;
    QString m_qrTokenUrl;
    QString m_qrImagePath;
};

} // namespace Controllers
} // namespace Telegram

#endif // TELEGRAM_AUTH_CONTROLLER_H
