#ifndef TELEGRAM_MTPROTO_SESSION_H
#define TELEGRAM_MTPROTO_SESSION_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <stdint.h>
#include <stddef.h>

namespace Telegram {

namespace Network {
class TcpTransport;
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

    // RPC Methods
    void sendGetNearestDc();

signals:
    void stateChanged(Telegram::Core::SessionState newState, const QString& stateText);
    void logMessage(const QString& log);
    void authKeyGenerated(quint64 authKeyIdHex);
    void nearestDcReceived(const QString& country, int thisDc, int nearestDc);
    void errorOccurred(const QString& error);

private slots:
    void onTransportConnected();
    void onTransportDisconnected();
    void onPacketReceived(const QByteArray& packet);
    void onTransportError(const QString& error);

private:
    void sendReqPQMulti();
    void handleUnencryptedPacket(const uint8_t* data, size_t size);
    void handleResPQ(const uint8_t* data, size_t size);
    void handleServerDHParams(const uint8_t* data, size_t size);
    void handleSetClientDHParamsAnswer(uint32_t constructor, const uint8_t* data, size_t size);

    void handleEncryptedPacket(const uint8_t* data, size_t size);
    void sendEncryptedMessage(const QByteArray& messageData, bool isContentRelated = true);

    int64_t generateMessageId();
    uint32_t generateSeqNo(bool isContentRelated);

private:
    Network::TcpTransport* m_transport;
    SessionState m_state;

    // Handshake state
    uint8_t m_nonce[16];
    uint8_t m_serverNonce[16];
    uint8_t m_newNonce[32];
    QByteArray m_bBytes;
    QByteArray m_tmpAesKey;
    QByteArray m_tmpAesIv;

    // MTProto 2.0 Encrypted Session
    uint64_t m_authKeyId;
    QByteArray m_authKey;
    uint64_t m_serverSalt;
    uint64_t m_sessionId;
    int32_t m_seqNo;
    int64_t m_lastMsgId;
    int32_t m_timeOffset;
};

} // namespace Core
} // namespace Telegram

#endif // TELEGRAM_MTPROTO_SESSION_H
