#include "AuthController.h"
#include "../core/MTProtoSession.h"
#include "../storage/SessionStorage.h"
#include "../crypto/QrGenerator.h"
#include "../Config.h"
#include <QDir>
#include <QFile>

namespace Telegram {
namespace Controllers {

AuthController::AuthController(Core::MTProtoSession* session, Storage::SessionStorage* storage, QObject* parent)
    : QObject(parent),
      m_session(session),
      m_storage(storage),
      m_qrPollTimer(new QTimer(this)),
      m_authState(STATE_INITIALIZING),
      m_phoneNumber(""),
      m_phoneCodeHash(""),
      m_deliveryType(""),
      m_codeTimeout(60),
      m_passwordHint(""),
      m_isBusy(false),
      m_statusMessage(""),
      m_userName(""),
      m_userHandle(""),
      m_userId(""),
      m_userPhone(""),
      m_qrTokenUrl(""),
      m_qrImagePath("") {

    connect(m_session, SIGNAL(stateChanged(int,QString)),
            this, SLOT(onSessionStateChanged(int,QString)));
    connect(m_session, SIGNAL(authKeyGenerated(quint64)),
            this, SLOT(onAuthKeyGenerated(quint64)));
    connect(m_session, SIGNAL(authSentCodeReceived(QString,QString,int)),
            this, SLOT(onAuthSentCodeReceived(QString,QString,int)));
    connect(m_session, SIGNAL(authPasswordNeeded(QString)),
            this, SLOT(onAuthPasswordNeeded(QString)));
    connect(m_session, SIGNAL(authSuccessReceived(qint64,quint64,QString,QString,QString,QString)),
            this, SLOT(onAuthSuccessReceived(qint64,quint64,QString,QString,QString,QString)));
    connect(m_session, SIGNAL(authSignUpRequiredReceived()),
            this, SLOT(onAuthSignUpRequired()));
    connect(m_session, SIGNAL(authLoginTokenReceived(QByteArray,int)),
            this, SLOT(onAuthLoginTokenReceived(QByteArray,int)));
    connect(m_session, SIGNAL(authLoginSuccessReceived()),
            this, SLOT(onAuthLoginSuccess()));
    connect(m_session, SIGNAL(dcMigrated(int)),
            this, SLOT(onDcMigrated(int)));
    connect(m_session, SIGNAL(rpcErrorReceived(int,QString)),
            this, SLOT(onRpcErrorReceived(int,QString)));
    connect(m_session, SIGNAL(myProfileReceived(QString,QString,QString)),
            this, SLOT(onMyProfileReceived(QString,QString,QString)));
    connect(m_session, SIGNAL(errorOccurred(QString)),
            this, SLOT(onSessionError(QString)));

    m_qrPollTimer->setInterval(20000);
    connect(m_qrPollTimer, SIGNAL(timeout()), this, SLOT(onQrPollTimer()));
}

AuthController::~AuthController() {
}

QString AuthController::authStateString() const {
    switch (m_authState) {
        case STATE_INITIALIZING: return "Initializing...";
        case STATE_CONNECTING: return "Connecting to Telegram...";
        case STATE_PHONE_INPUT: return "Enter Phone Number";
        case STATE_CODE_INPUT: return "Enter Verification Code";
        case STATE_PASSWORD_INPUT: return "Enter 2FA Password";
        case STATE_LOGGED_IN: return "Logged In";
        case STATE_QR_LOGIN: return "Scan QR Code";
        default: return "";
    }
}

void AuthController::setPhoneNumber(const QString& phone) {
    if (m_phoneNumber != phone) {
        m_phoneNumber = phone;
        emit phoneNumberChanged(m_phoneNumber);
    }
}

void AuthController::setAuthState(AuthState state) {
    if (m_authState != state) {
        m_authState = state;
        emit authStateChanged(static_cast<int>(m_authState));
    }
}

void AuthController::setBusy(bool busy) {
    if (m_isBusy != busy) {
        m_isBusy = busy;
        emit isBusyChanged(m_isBusy);
    }
}

void AuthController::setStatus(const QString& msg) {
    m_statusMessage = msg;
    emit statusMessageChanged(m_statusMessage);
}

void AuthController::start() {
    setBusy(true);
    setStatus("Loading session...");

    if (m_storage->loadSession()) {
        if (m_storage->isLoggedIn()) {
            QString first = m_storage->firstName().trimmed();
            QString last = m_storage->lastName().trimmed();
            m_userName = last.isEmpty() ? first : (first + " " + last);
            if (m_userName.isEmpty()) {
                m_userName = "John";
            }
            m_userHandle = m_storage->username().isEmpty() ? "" : (m_storage->username().startsWith("@") ? m_storage->username() : "@" + m_storage->username());
            m_userId = QString::number(m_storage->userId());
            m_userPhone = m_storage->phone();
            emit userProfileChanged();

            setAuthState(STATE_LOGGED_IN);
            setStatus(QString("Logged in as %1").arg(m_userName));
            setBusy(false);

            m_session->restoreSession(m_storage->dcId(), m_storage->dcIp(), m_storage->dcPort(),
                                     m_storage->authKeyId(), m_storage->authKey(), m_storage->serverSalt());
            return;
        } else if (m_storage->authKeyId() != 0 && m_storage->authKey().size() == 256) {
            setAuthState(STATE_PHONE_INPUT);
            setStatus("Connected to Telegram");
            setBusy(false);

            m_session->restoreSession(m_storage->dcId(), m_storage->dcIp(), m_storage->dcPort(),
                                     m_storage->authKeyId(), m_storage->authKey(), m_storage->serverSalt());
            return;
        }
    }

    // Connect fresh session to default DC
    setAuthState(STATE_PHONE_INPUT);
    setStatus("Ready");
    setBusy(false);
    m_session->start(Config::DEFAULT_DC_IP, Config::DEFAULT_DC_PORT);
}

void AuthController::changePhoneNumber() {
    m_qrPollTimer->stop();
    m_phoneCodeHash.clear();
    m_deliveryType.clear();
    setAuthState(STATE_PHONE_INPUT);
    setStatus("Enter your phone number");
    setBusy(false);
}

void AuthController::cancelQrLogin() {
    m_qrPollTimer->stop();
    setAuthState(STATE_PHONE_INPUT);
    setStatus("Sign in with phone number");
    setBusy(false);
}

void AuthController::submitPhoneNumber(const QString& phone) {
    QString cleanPhone = phone.trimmed();
    if (cleanPhone.isEmpty()) {
        emit authErrorOccurred("Please enter a valid phone number with country code.");
        return;
    }

    if (!cleanPhone.startsWith("+")) {
        cleanPhone.prepend("+");
    }

    setPhoneNumber(cleanPhone);
    setBusy(true);
    setStatus("Requesting verification code from Telegram...");

    m_session->sendAuthSendCode(cleanPhone);
}

void AuthController::submitLoginCode(const QString& code) {
    QString cleanCode = code.trimmed();
    if (cleanCode.isEmpty()) {
        emit authErrorOccurred("Please enter the verification code.");
        return;
    }

    setBusy(true);
    setStatus("Verifying login code with Telegram...");

    m_session->sendAuthSignIn(m_phoneNumber, m_phoneCodeHash, cleanCode);
}

void AuthController::submitPassword(const QString& password) {
    if (password.isEmpty()) {
        emit authErrorOccurred("Please enter your 2FA password.");
        return;
    }

    setBusy(true);
    setStatus("Verifying 2FA Cloud Password...");

    m_session->sendAuthCheckPassword(password);
}

void AuthController::resendCode() {
    if (!m_phoneNumber.isEmpty() && !m_phoneCodeHash.isEmpty()) {
        setBusy(true);
        setStatus("Requesting SMS verification code from Telegram...");
        m_session->sendAuthResendCode(m_phoneNumber, m_phoneCodeHash);
    } else if (!m_phoneNumber.isEmpty()) {
        submitPhoneNumber(m_phoneNumber);
    }
}

void AuthController::requestQrLogin() {
    setBusy(true);
    setStatus("Generating QR Code for Telegram Login...");
    setAuthState(STATE_QR_LOGIN);
    m_session->sendExportLoginToken();
}

void AuthController::onQrPollTimer() {
    if (m_authState == STATE_QR_LOGIN) {
        m_session->sendExportLoginToken();
    }
}

void AuthController::logout() {
    m_qrPollTimer->stop();
    setBusy(true);
    setStatus("Logging out...");

    m_session->sendAuthLogOut();
    m_storage->clearSession();

    m_userName.clear();
    m_userHandle.clear();
    m_userId.clear();
    m_userPhone.clear();
    m_userBio.clear();
    m_qrTokenUrl.clear();
    m_qrImagePath.clear();
    emit userProfileChanged();
    emit qrTokenUrlChanged(m_qrTokenUrl);
    emit qrImagePathChanged(m_qrImagePath);

    setAuthState(STATE_PHONE_INPUT);
    setStatus("Logged out");
    setBusy(false);
}

void AuthController::onSessionStateChanged(int newState, const QString& stateText) {
    Q_UNUSED(stateText);
    if (newState == 6 /* STATE_ENCRYPTED_READY */) {
        if (m_authState == STATE_CONNECTING || m_authState == STATE_INITIALIZING) {
            setAuthState(STATE_PHONE_INPUT);
            setStatus("Connected to Telegram");
        } else if (m_authState == STATE_LOGGED_IN) {
            m_session->sendMessagesGetDialogs();
            m_session->sendUsersGetMyFull();
        }
    }
}

void AuthController::onAuthKeyGenerated(quint64 authKeyIdHex) {
    Q_UNUSED(authKeyIdHex);
    m_storage->saveSession(m_session->currentDcId(),
                           Config::DEFAULT_DC_IP,
                           Config::DEFAULT_DC_PORT,
                           m_session->authKeyId(),
                           m_session->authKey(),
                           m_session->serverSalt());
}

void AuthController::onAuthSentCodeReceived(const QString& phoneCodeHash, const QString& type, int timeout) {
    m_qrPollTimer->stop();
    m_phoneCodeHash = phoneCodeHash;
    m_deliveryType = type;
    m_codeTimeout = timeout;

    emit phoneCodeHashChanged(m_phoneCodeHash);
    emit deliveryTypeChanged(m_deliveryType);
    emit codeTimeoutChanged(m_codeTimeout);

    setAuthState(STATE_CODE_INPUT);
    setStatus(QString("Code sent via %1 to %2. Please check your Telegram app (Chat 777000)!").arg(m_deliveryType).arg(m_phoneNumber));
    setBusy(false);
}

void AuthController::onAuthPasswordNeeded(const QString& hint) {
    m_qrPollTimer->stop();
    m_passwordHint = hint;
    emit passwordHintChanged(m_passwordHint);

    setAuthState(STATE_PASSWORD_INPUT);
    setStatus(hint.isEmpty() ? "2FA Cloud Password required" : QString("Hint: %1").arg(hint));
    setBusy(false);
}

void AuthController::onAuthSuccessReceived(qint64 userId, quint64 accessHash,
                                          const QString& firstName, const QString& lastName,
                                          const QString& username, const QString& phone) {
    m_qrPollTimer->stop();
    m_userName = firstName + (lastName.isEmpty() ? "" : " " + lastName);
    m_userHandle = username.isEmpty() ? "" : "@" + username;
    m_userId = QString::number(userId);
    m_userPhone = phone;

    m_storage->saveUserProfile(userId, accessHash, firstName, lastName, username, phone);

    emit userProfileChanged();
    setAuthState(STATE_LOGGED_IN);
    setStatus(QString("Welcome, %1!").arg(m_userName));
    setBusy(false);
    m_session->sendUsersGetMyFull();
}

void AuthController::onAuthSignUpRequired() {
    m_qrPollTimer->stop();
    setBusy(false);
    setStatus("New user registration is required for this number.");
    emit authErrorOccurred("This phone number is not registered on Telegram yet.");
}

void AuthController::onAuthLoginTokenReceived(const QByteArray& token, int expires) {
    QByteArray b64 = token.toBase64();
    b64.replace('+', '-').replace('/', '_');
    while (b64.endsWith('=')) {
        b64.chop(1);
    }
    m_qrTokenUrl = QString("tg://login?token=%1").arg(QString::fromLatin1(b64));
    emit qrTokenUrlChanged(m_qrTokenUrl);

    // Generate crisp QR code PNG image in sandbox data directory
    static int qrCounter = 0;
    qrCounter = (qrCounter + 1) % 10;
    QString qrFile = QString("data/qr_%1.png").arg(qrCounter);
    Crypto::QrGenerator::generateQrPng(m_qrTokenUrl, qrFile, 320);

    m_qrImagePath = QString("file://%1/%2").arg(QDir::currentPath()).arg(qrFile);
    emit qrImagePathChanged(m_qrImagePath);

    setAuthState(STATE_QR_LOGIN);
    setBusy(false);
    setStatus(QString("Point your phone camera to scan QR Code (expires in %1s)").arg(expires));

    if (!m_qrPollTimer->isActive()) {
        m_qrPollTimer->start();
    }
}

void AuthController::onAuthLoginSuccess() {
    m_qrPollTimer->stop();
    setStatus("QR Code successfully scanned! Authenticating...");
}

void AuthController::onDcMigrated(int newDcId) {
    m_qrPollTimer->stop();
    setStatus(QString("Switching to datacenter %1...").arg(newDcId));
    if (!m_phoneNumber.isEmpty() && m_authState == STATE_PHONE_INPUT) {
        submitPhoneNumber(m_phoneNumber);
    }
}

void AuthController::onRpcErrorReceived(int errorCode, const QString& errorMessage) {
    Q_UNUSED(errorCode);
    setBusy(false);

    if (errorMessage == "AUTH_RESTART") {
        if (!m_phoneNumber.isEmpty()) {
            setStatus("Restarting authorization session...");
            QTimer::singleShot(600, this, SLOT(resendCode()));
            return;
        }
    }

    QString userFriendlyMsg;
    if (errorMessage == "SEND_CODE_UNAVAILABLE") {
        userFriendlyMsg = "Code was sent to your official Telegram app (Chat 777000). Please check notifications on your other device.";
    } else if (errorMessage == "PHONE_CODE_INVALID") {
        userFriendlyMsg = "Incorrect verification code. Please check Chat 777000 in your Telegram app and re-enter.";
    } else if (errorMessage == "PHONE_CODE_EXPIRED") {
        userFriendlyMsg = "Verification code has expired. Please request a new code.";
    } else if (errorMessage == "PASSWORD_HASH_INVALID") {
        userFriendlyMsg = "Incorrect 2FA password. Please check your password and re-enter.";
    } else if (errorMessage == "SRP_ID_INVALID" || errorMessage == "SRP_PASSWORD_CHANGED") {
        userFriendlyMsg = "2FA parameters refreshed. Please enter your password and submit again.";
    } else if (errorMessage.startsWith("FLOOD_WAIT_")) {
        int seconds = errorMessage.section('_', -1).toInt();
        userFriendlyMsg = QString("Too many attempts. Please wait %1 seconds before retrying.").arg(seconds);
    } else {
        userFriendlyMsg = QString("Telegram Error: %1").arg(errorMessage);
    }

    setStatus(userFriendlyMsg);
    emit authErrorOccurred(userFriendlyMsg);
}

void AuthController::onSessionError(const QString& error) {
    setBusy(false);
    setStatus(error);
    emit authErrorOccurred(error);
}

void AuthController::onMyProfileReceived(const QString& bio, const QString& username, const QString& phone) {
    if (!bio.isEmpty()) {
        m_userBio = bio;
    }
    if (!username.isEmpty()) {
        m_userHandle = username.startsWith("@") ? username : "@" + username;
    }
    if (!phone.isEmpty()) {
        m_userPhone = phone;
    }
    emit userProfileChanged();
}

} // namespace Controllers
} // namespace Telegram
