#ifndef TELEGRAM_MTPROTO_SESSION_H
#define TELEGRAM_MTPROTO_SESSION_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QTimer>
#include <QVariantMap>
#include <QList>
#include <stdint.h>
#include <stddef.h>

namespace Telegram {

namespace Network {
class TcpTransport;
}

namespace TL {
class TLBuffer;
}

namespace Core {

enum SessionState {
    STATE_DISCONNECTED,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_HANDSHAKE_REQ_PQ,
    STATE_HANDSHAKE_REQ_DH,
    STATE_HANDSHAKE_SET_DH,
    STATE_ENCRYPTED_READY
};

class MTProtoSession : public QObject {
    Q_OBJECT
public:
    explicit MTProtoSession(QObject* parent = NULL);
    ~MTProtoSession();

    void start(const QString& host, quint16 port);
    void stop();
    SessionState state() const;
    QString stateString() const;

    void sendGetNearestDc();
    void sendPingDelayDisconnect();

    // Phase 2 Authentication RPCs
    void sendAuthSendCode(const QString& phoneNumber);
    void sendAuthResendCode(const QString& phoneNumber, const QString& phoneCodeHash);
    void sendAuthSignIn(const QString& phoneNumber, const QString& phoneCodeHash, const QString& phoneCode);
    void sendAccountGetPassword();
    void sendAuthCheckPassword(const QString& password);
    void sendAuthLogOut();
    void sendExportLoginToken();

    // Phase 3 & 4: Dialogs & Messaging
    void sendMessagesGetDialogs(int offsetDate = 0, int offsetId = 0, int limit = 30);
    void sendMessagesGetHistory(int peerType, qint64 peerId, quint64 accessHash, int offsetId = 0, int limit = 50);
    void sendMessagesSendMessage(int peerType, qint64 peerId, quint64 accessHash, const QString& message);

    void migrateToDc(int dcId);
    void restoreSession(int dcId, const QString& dcIp, int dcPort, quint64 authKeyId, const QByteArray& authKey, quint64 serverSalt);

    int currentDcId() const { return m_dcId; }
    quint64 authKeyId() const { return m_authKeyId; }
    QByteArray authKey() const { return m_authKey; }
    quint64 serverSalt() const { return m_serverSalt; }

signals:
    void stateChanged(int newState, const QString& stateText);
    void logMessage(const QString& log);
    void authKeyGenerated(quint64 authKeyIdHex);
    void dcMigrated(int newDcId);
    void nearestDcReceived(const QString& country, int thisDc, int nearestDc);
    void errorOccurred(const QString& error);

    // Phase 2 Authentication Signals
    void authSentCodeReceived(const QString& phoneCodeHash, const QString& type, int timeout);
    void authPasswordNeeded(const QString& hint);
    void authSuccessReceived(qint64 userId, quint64 accessHash, const QString& firstName, const QString& lastName, const QString& username, const QString& phone);
    void authSignUpRequiredReceived();
    void authLoginTokenReceived(const QByteArray& token, int expires);
    void authLoginSuccessReceived();
    void rpcErrorReceived(int errorCode, const QString& errorMessage);

    // Phase 3 & 4: Dialogs & Messaging Signals
    void dialogsReceived(const QList<QVariantMap>& dialogs);
    void historyReceived(qint64 peerId, const QList<QVariantMap>& messages);
    void messageSent(qint64 peerId, qint64 randomId, int messageId, int date);

private slots:
    void onTransportConnected();
    void onTransportDisconnected();
    void onPacketReceived(const QByteArray& packet);
    void onTransportError(const QString& error);
    void onPingTimer();
    void onReconnectTimer();
    void onMigrateTimer();

private:
    void sendReqPQMulti();
    void handleUnencryptedPacket(const quint8* data, size_t size);
    void handleResPQ(const quint8* data, size_t size);
    void handleServerDHParams(const quint8* data, size_t size);
    void handleSetClientDHParamsAnswer(quint32 constructor, const quint8* data, size_t size);

    void handleEncryptedPacket(const quint8* data, size_t size);
    void processPlainMessage(TL::TLBuffer& plainBuf);
    void handleRpcResult(qint64 reqMsgId, quint32 innerRpcConstructor, TL::TLBuffer& plainBuf);
    void sendEncryptedMessage(const QByteArray& messageData, bool isContentRelated = true);

    qint64 generateMessageId();
    quint32 generateSeqNo(bool isContentRelated);

private:
    Network::TcpTransport* m_transport;
    SessionState m_state;
    int m_dcId;
    QString m_host;
    quint16 m_port;
    bool m_autoReconnect;
    QTimer* m_pingTimer;
    QTimer* m_reconnectTimer;

    // Handshake state
    quint8 m_nonce[16];
    quint8 m_serverNonce[16];
    quint8 m_newNonce[32];
    QByteArray m_bBytes;
    QByteArray m_tmpAesKey;
    QByteArray m_tmpAesIv;

    // MTProto 2.0 Encrypted Session
    quint64 m_authKeyId;
    QByteArray m_authKey;
    quint64 m_serverSalt;
    quint64 m_sessionId;
    qint32 m_seqNo;
    qint64 m_lastMsgId;
    qint32 m_timeOffset;

    // 2FA Cloud Password State
    QByteArray m_pwdSalt1;
    QByteArray m_pwdSalt2;
    QByteArray m_pwdP;
    QByteArray m_pwdSrpB;
    int m_pwdG;
    qint64 m_pwdSrpId;
    QString m_pwdHint;
    QString m_pendingPassword;

    // Entity Metadata Cache
    QMap<qint64, quint64> m_entityAccessHashes;
    QMap<qint64, int> m_entityPeerTypes;
};

} // namespace Core
} // namespace Telegram

#endif // TELEGRAM_MTPROTO_SESSION_H
