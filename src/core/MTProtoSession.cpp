#include "MTProtoSession.h"
#include "TcpTransport.h"
#include "CryptoEngine.h"
#include "TLBuffer.h"
#include "TLTypes.h"
#include "Config.h"
#include "../models/DialogItem.h"
#include "../storage/MediaCache.h"

#include <QDateTime>
#include <QDebug>
#include <QSet>
#include <string.h>

namespace Telegram {
namespace Core {

// Official Telegram RSA Public Keys
static const char* const OFFICIAL_RSA_PEMS[] = {
"-----BEGIN RSA PUBLIC KEY-----\n"
"MIIBCgKCAQEA6LszBcC1LGzyr992NzE0ieY+BSaOW622Aa9Bd4ZHLl+TuFQ4lo4g\n"
"5nKaMBwK/BIb9xUfg0Q29/2mgIR6Zr9krM7HjuIcCzFvDtr+L0GQjae9H0pRB2OO\n"
"62cECs5HKhT5DZ98K33vmWiLowc621dQuwKWSQKjWf50XYFw42h21P2KXUGyp2y/\n"
"+aEyZ+uVgLLQbRA1dEjSDZ2iGRy12Mk5gpYc397aYp438fsJoHIgJ2lgMv5h7WY9\n"
"t6N/byY9Nw9p21Og3AoXSL2q/2IJ1WRUhebgAdGVMlV1fkuOQoEzR7EdpqtQD9Cs\n"
"5+bfo3Nhmcyvk5ftB0WkJ9z6bNZ7yxrP8wIDAQAB\n"
"-----END RSA PUBLIC KEY-----",

"-----BEGIN RSA PUBLIC KEY-----\n"
"MIIBCgKCAQEAyMEdY1aR+sCR3ZSJrtztKTKqigvO/vBfqACJLZtS7QMgCGXJ6XIR\n"
"yy7mx66W0/sOFa7/1mAZtEoIokDP3ShoqF4fVNb6XeqgQfaUHd8wJpDWHcR2OFwv\n"
"plUUI1PLTktZ9uW2WE23b+ixNwJjJGwBDJPQEQFBE+vfmH0JP503wr5INS1poWg/\n"
"j25sIWeYPHYeOrFp/eXaqhISP6G+q2IeTaWTXpwZj4LzXq5YOpk4bYEQ6mvRq7D1\n"
"aHWfYmlEGepfaYR8Q0YqvvhYtMte3ITnuSJs171+GDqpdKcSwHnd6FudwGO4pcCO\n"
"j4WcDuXc2CTHgH8gFTNhp/Y8/SpDOhvn9QIDAQAB\n"
"-----END RSA PUBLIC KEY-----"
};

struct SessionDialogEntry {
    qint64 peerId;
    int peerType; // 1=user, 2=chat, 3=channel
    int topMessage;
    int unreadCount;
    bool isPinned;
};

MTProtoSession::MTProtoSession(QObject* parent)
    : QObject(parent),
      m_transport(new Network::TcpTransport(this)),
      m_state(STATE_DISCONNECTED),
      m_dcId(Config::DEFAULT_DC_ID),
      m_host(Config::DEFAULT_DC_IP),
      m_port(Config::DEFAULT_DC_PORT),
      m_autoReconnect(true),
      m_pingTimer(new QTimer(this)),
      m_reconnectTimer(new QTimer(this)),
      m_authKeyId(0),
      m_serverSalt(0),
      m_sessionId(0),
      m_seqNo(0),
      m_lastMsgId(0),
      m_timeOffset(0),
      m_pwdG(0),
      m_pwdSrpId(0),
      m_pwdHint("") {
    
    connect(m_transport, SIGNAL(connected()), this, SLOT(onTransportConnected()));
    connect(m_transport, SIGNAL(disconnected()), this, SLOT(onTransportDisconnected()));
    connect(m_transport, SIGNAL(packetReceived(QByteArray)), this, SLOT(onPacketReceived(QByteArray)));
    connect(m_transport, SIGNAL(errorOccurred(QString)), this, SLOT(onTransportError(QString)));
    connect(m_transport, SIGNAL(logMessage(QString)), this, SIGNAL(logMessage(QString)));

    m_pingTimer->setInterval(25000); // Send heartbeat every 25s
    connect(m_pingTimer, SIGNAL(timeout()), this, SLOT(onPingTimer()));

    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, SIGNAL(timeout()), this, SLOT(onReconnectTimer()));
}

MTProtoSession::~MTProtoSession() {
    stop();
}

void MTProtoSession::start(const QString& host, quint16 port) {
    m_host = host;
    m_port = port;
    m_autoReconnect = true;
    m_state = STATE_CONNECTING;
    emit stateChanged(static_cast<int>(m_state), "Connecting to Telegram DC...");
    m_transport->connectToHost(host, port);
}

void MTProtoSession::stop() {
    m_autoReconnect = false;
    m_pingTimer->stop();
    m_reconnectTimer->stop();
    m_transport->disconnectFromHost();
    m_state = STATE_DISCONNECTED;
    emit stateChanged(static_cast<int>(m_state), "Disconnected");
}

SessionState MTProtoSession::state() const {
    return m_state;
}

QString MTProtoSession::stateString() const {
    switch (m_state) {
        case STATE_DISCONNECTED: return "Disconnected";
        case STATE_CONNECTING: return "Connecting...";
        case STATE_CONNECTED: return "Connected";
        case STATE_HANDSHAKE_REQ_PQ: return "Handshake: Sending req_pq_multi";
        case STATE_HANDSHAKE_REQ_DH: return "Handshake: Factoring PQ & Sending req_DH_params";
        case STATE_HANDSHAKE_SET_DH: return "Handshake: Computing DH & Sending set_client_DH_params";
        case STATE_ENCRYPTED_READY: return "MTProto 2.0 Encrypted Session Ready";
        default: return "Unknown";
    }
}

void MTProtoSession::onTransportConnected() {
    emit logMessage("TCP Connection established. Sending MTProto Intermediate transport handshake...");
    m_state = STATE_CONNECTED;
    emit stateChanged(static_cast<int>(m_state), "Connected to Telegram TCP transport");

    if (m_authKeyId != 0 && m_authKey.size() == 256) {
        // We already have a valid Auth Key for this session: assign fresh sessionId and reset seqNo
        Crypto::CryptoEngine::generateRandomBytes(reinterpret_cast<uint8_t*>(&m_sessionId), 8);
        m_seqNo = 0;
        m_lastMsgId = 0;

        m_state = STATE_ENCRYPTED_READY;
        emit stateChanged(static_cast<int>(m_state), "Resumed encrypted session");
        if (!m_pingTimer->isActive()) {
            m_pingTimer->start();
        }
        sendGetNearestDc();
    } else {
        // Begin MTProto Handshake
        sendReqPQMulti();
    }
}

void MTProtoSession::onTransportDisconnected() {
    emit logMessage("TCP Connection closed by host.");
    m_pingTimer->stop();
    m_state = STATE_DISCONNECTED;
    emit stateChanged(static_cast<int>(m_state), "Disconnected");

    if (m_autoReconnect) {
        emit logMessage("Connection dropped. Auto-reconnecting in 3 seconds...");
        m_reconnectTimer->start(3000);
    }
}

void MTProtoSession::onTransportError(const QString& error) {
    emit errorOccurred(QString("TCP Socket: %1").arg(error));
}

void MTProtoSession::onPingTimer() {
    if (m_state == STATE_ENCRYPTED_READY) {
        sendPingDelayDisconnect();
    }
}

void MTProtoSession::onReconnectTimer() {
    if (m_autoReconnect) {
        emit logMessage(QString("Reconnecting to Telegram DC %1:%2...").arg(m_host).arg(m_port));
        start(m_host, m_port);
    }
}

void MTProtoSession::onMigrateTimer() {
    emit logMessage(QString("Connecting to Migrated DC %1 (%2:%3)...").arg(m_dcId).arg(m_host).arg(m_port));
    start(m_host, m_port);
}

void MTProtoSession::migrateToDc(int dcId) {
    m_dcId = dcId;
    QString newIp;
    int newPort;
    if (Config::getDcAddress(dcId, newIp, newPort)) {
        m_host = newIp;
        m_port = static_cast<quint16>(newPort);
    } else {
        emit errorOccurred(QString("Unknown DC ID %1 for migration").arg(dcId));
        return;
    }

    emit logMessage(QString("=================================================="));
    emit logMessage(QString("MIGRATING TO DC %1 (%2:%3)").arg(m_dcId).arg(m_host).arg(m_port));
    emit logMessage(QString("=================================================="));

    m_pingTimer->stop();
    m_reconnectTimer->stop();
    m_authKeyId = 0;
    m_authKey.clear();
    m_serverSalt = 0;

    m_transport->disconnectFromHost();
    m_state = STATE_DISCONNECTED;
    emit stateChanged(static_cast<int>(m_state), QString("Migrating to DC %1...").arg(m_dcId));
    emit dcMigrated(m_dcId);

    QTimer::singleShot(150, this, SLOT(onMigrateTimer()));
}

void MTProtoSession::restoreSession(int dcId, const QString& dcIp, int dcPort, quint64 authKeyId, const QByteArray& authKey, quint64 serverSalt) {
    m_dcId = dcId;
    m_host = dcIp;
    m_port = static_cast<quint16>(dcPort);
    m_authKeyId = authKeyId;
    m_authKey = authKey;
    m_serverSalt = serverSalt;

    Crypto::CryptoEngine::generateRandomBytes(reinterpret_cast<uint8_t*>(&m_sessionId), 8);
    m_seqNo = 0;
    m_lastMsgId = 0;

    start(m_host, m_port);
}

qint64 MTProtoSession::generateMessageId() {
    qint64 nowSec = QDateTime::currentDateTimeUtc().toTime_t() + m_timeOffset;
    qint64 nowMs = QDateTime::currentDateTimeUtc().time().msec();
    qint64 msgId = (nowSec << 32) | ((nowMs * 4294967) / 1000);
    msgId = (msgId & ~3); // Client messages must have lowest 2 bits equal to 0

    if (msgId <= m_lastMsgId) {
        msgId = m_lastMsgId + 4;
    }
    m_lastMsgId = msgId;
    return msgId;
}

quint32 MTProtoSession::generateSeqNo(bool isContentRelated) {
    quint32 seq = m_seqNo * 2;
    if (isContentRelated) {
        seq += 1;
        m_seqNo += 1;
    }
    return seq;
}

// --------------------------------------------------------------------------
// MTProto Handshake Step 1: req_pq_multi
// --------------------------------------------------------------------------
void MTProtoSession::sendReqPQMulti() {
    m_state = STATE_HANDSHAKE_REQ_PQ;
    emit stateChanged(static_cast<int>(m_state), "Handshake: Sending req_pq_multi");

    Crypto::CryptoEngine::generateRandomBytes(m_nonce, 16);

    TL::TLBuffer buf;
    buf.writeInt64(0); // auth_key_id = 0 (unencrypted handshake)
    buf.writeInt64(generateMessageId());
    buf.writeInt32(20); // message length: 4 (constructor) + 16 (nonce)

    buf.writeUInt32(TL::ID_REQ_PQ_MULTI);
    buf.writeInt128(m_nonce);

    QByteArray packet(reinterpret_cast<const char*>(buf.data()), buf.size());
    m_transport->sendPacket(packet);
}

void MTProtoSession::onPacketReceived(const QByteArray& packet) {
    if (packet.size() < 8) return;

    uint64_t authKeyId = 0;
    for (int i = 0; i < 8; ++i) {
        authKeyId |= (static_cast<uint64_t>(static_cast<uint8_t>(packet[i])) << (i * 8));
    }

    if (authKeyId == 0) {
        handleUnencryptedPacket(reinterpret_cast<const uint8_t*>(packet.constData()), packet.size());
    } else {
        handleEncryptedPacket(reinterpret_cast<const uint8_t*>(packet.constData()), packet.size());
    }
}

void MTProtoSession::handleUnencryptedPacket(const quint8* data, size_t size) {
    if (size < 20) return;

    TL::TLBuffer buf(data, size);
    int64_t authKeyId, msgId;
    int32_t msgLen;
    buf.readInt64(authKeyId);
    buf.readInt64(msgId);
    buf.readInt32(msgLen);

    if (msgLen <= 0 || static_cast<size_t>(msgLen) > buf.remaining()) return;

    uint32_t constructor;
    buf.readUInt32(constructor);

    if (constructor == TL::ID_RESPQ) {
        handleResPQ(buf.data() + buf.offset(), buf.remaining());
    } else if (constructor == TL::ID_SERVER_DH_PARAMS_OK || constructor == TL::ID_SERVER_DH_PARAMS_FAIL) {
        handleServerDHParams(buf.data() + buf.offset() - 4, buf.remaining() + 4);
    } else if (constructor == TL::ID_DH_GEN_OK || constructor == TL::ID_DH_GEN_RETRY || constructor == TL::ID_DH_GEN_FAIL) {
        handleSetClientDHParamsAnswer(constructor, buf.data() + buf.offset(), buf.remaining());
    }
}

void MTProtoSession::handleResPQ(const quint8* data, size_t size) {
    TL::TLBuffer buf(data, size);
    uint8_t resNonce[16];
    buf.readInt128(resNonce);

    if (memcmp(m_nonce, resNonce, 16) != 0) {
        emit errorOccurred("Handshake: resPQ nonce mismatch");
        return;
    }

    buf.readInt128(m_serverNonce);

    QByteArray pqBytes;
    buf.readBytes(pqBytes);

    uint32_t vectorConstructor;
    int32_t fpCount;
    buf.readUInt32(vectorConstructor);
    buf.readInt32(fpCount);

    QVector<uint64_t> serverFingerprints;
    for (int i = 0; i < fpCount && buf.remaining() >= 8; ++i) {
        int64_t fp;
        buf.readInt64(fp);
        serverFingerprints.append(static_cast<uint64_t>(fp));
        emit logMessage(QString("Server RSA Fingerprint [%1]: 0x%2").arg(i).arg(QString::number(static_cast<uint64_t>(fp), 16)));
    }

    emit logMessage(QString("resPQ received. Factoring PQ (size: %1 bytes, %2 RSA fingerprints)...").arg(pqBytes.size()).arg(fpCount));

    uint64_t pqVal = 0;
    for (int i = 0; i < pqBytes.size(); ++i) {
        pqVal = (pqVal << 8) | static_cast<uint8_t>(pqBytes[i]);
    }

    uint32_t p = 0, q = 0;
    if (!Crypto::CryptoEngine::factorizePQ(pqVal, p, q)) {
        emit errorOccurred("Handshake: Failed to factorize PQ");
        return;
    }

    emit logMessage(QString("PQ factorized successfully: P=%1, Q=%2").arg(p).arg(q));

    QString selectedN, selectedE;
    uint64_t selectedFp = 0;
    bool keyFound = false;

    for (int i = 0; i < 2; ++i) {
        QString nHex, eHex;
        uint64_t pemFp = 0;
        if (Crypto::CryptoEngine::loadPemPublicKey(OFFICIAL_RSA_PEMS[i], nHex, eHex, pemFp)) {
            emit logMessage(QString("Official PEM Key [%1] parsed fingerprint: 0x%2").arg(i).arg(QString::number(pemFp, 16)));
            for (int j = 0; j < serverFingerprints.size(); ++j) {
                if (serverFingerprints[j] == pemFp) {
                    selectedN = nHex;
                    selectedE = eHex;
                    selectedFp = pemFp;
                    keyFound = true;
                    break;
                }
            }
        }
        if (keyFound) break;
    }

    if (!keyFound) {
        selectedN = "C150023E2F70DB7985D0182B93060764A8D361F8D5CE44D878DF6084D9842F557434190DF442475B92A72B8B11F247942F80B91C2E299C942DDE3C25D9B424075591230A5844EDF0E3DF33B91739DCE9482C5F5E46270E42C04D81DB44D1DF1AFEDBD8BEE4B728615A89E62C106E1A3C2F4BEA15";
        selectedE = "010001";
        selectedFp = serverFingerprints.isEmpty() ? 0 : serverFingerprints[0];
    }

    emit logMessage(QString("Selected RSA Public Key Fingerprint: 0x%1").arg(QString::number(selectedFp, 16)));

    // Generate new_nonce (32 bytes)
    Crypto::CryptoEngine::generateRandomBytes(m_newNonce, 32);

    // Build p_q_inner_data_dc#a9f55f95
    TL::TLBuffer innerBuf;
    innerBuf.writeUInt32(TL::ID_P_Q_INNER_DATA_DC);
    innerBuf.writeBytes(pqBytes);

    QByteArray pBytes, qBytes;
    for (int i = 3; i >= 0; --i) pBytes.append(static_cast<char>((p >> (i * 8)) & 0xFF));
    for (int i = 3; i >= 0; --i) qBytes.append(static_cast<char>((q >> (i * 8)) & 0xFF));

    innerBuf.writeBytes(pBytes);
    innerBuf.writeBytes(qBytes);
    innerBuf.writeInt128(m_nonce);
    innerBuf.writeInt128(m_serverNonce);
    innerBuf.writeRaw(m_newNonce, 32);
    innerBuf.writeInt32(m_dcId);

    QByteArray encryptedPayload;
    if (!Crypto::CryptoEngine::rsaEncryptHandshake(innerBuf.buffer(), selectedN.toLatin1().constData(), selectedE.toLatin1().constData(), encryptedPayload)) {
        emit errorOccurred("Handshake: RSA encryption of req_DH_params failed");
        return;
    }

    emit logMessage(QString("RSA Encrypted req_DH_params payload ready (size: %1 bytes). Sending...").arg(encryptedPayload.size()));

    m_state = STATE_HANDSHAKE_REQ_DH;
    emit stateChanged(static_cast<int>(m_state), "Handshake: Sending req_DH_params");

    TL::TLBuffer reqDhBuf;
    reqDhBuf.writeInt64(0);
    reqDhBuf.writeInt64(generateMessageId());
    reqDhBuf.writeInt32(4 + 16 + 16 + 4 + 4 + 8 + 4 + encryptedPayload.size());

    reqDhBuf.writeUInt32(TL::ID_REQ_DH_PARAMS);
    reqDhBuf.writeInt128(m_nonce);
    reqDhBuf.writeInt128(m_serverNonce);
    reqDhBuf.writeBytes(pBytes);
    reqDhBuf.writeBytes(qBytes);
    reqDhBuf.writeInt64(static_cast<int64_t>(selectedFp));
    reqDhBuf.writeBytes(encryptedPayload);

    QByteArray packet(reinterpret_cast<const char*>(reqDhBuf.data()), reqDhBuf.size());
    m_transport->sendPacket(packet);
}

void MTProtoSession::handleServerDHParams(const quint8* data, size_t size) {
    TL::TLBuffer buf(data, size);
    uint32_t constructor;
    buf.readUInt32(constructor);

    if (constructor == TL::ID_SERVER_DH_PARAMS_FAIL) {
        emit errorOccurred("Handshake: Server rejected DH params (server_DH_params_fail)");
        return;
    }

    uint8_t rNonce[16], rServerNonce[16];
    buf.readInt128(rNonce);
    buf.readInt128(rServerNonce);

    if (memcmp(m_nonce, rNonce, 16) != 0 || memcmp(m_serverNonce, rServerNonce, 16) != 0) {
        emit errorOccurred("Handshake: server_DH_params nonce mismatch");
        return;
    }

    QByteArray encryptedAnswer;
    buf.readBytes(encryptedAnswer);

    emit logMessage(QString("server_DH_params_ok received (encrypted size: %1 bytes). Decrypting with tmp_aes_key...").arg(encryptedAnswer.size()));

    // tmp_aes_key = SHA1(new_nonce + server_nonce) + SHA1(server_nonce + new_nonce)[0..11]
    QByteArray nPlusS = QByteArray(reinterpret_cast<const char*>(m_newNonce), 32) + QByteArray(reinterpret_cast<const char*>(m_serverNonce), 16);
    QByteArray sPlusN = QByteArray(reinterpret_cast<const char*>(m_serverNonce), 16) + QByteArray(reinterpret_cast<const char*>(m_newNonce), 32);
    QByteArray nPlusN = QByteArray(reinterpret_cast<const char*>(m_newNonce), 32) + QByteArray(reinterpret_cast<const char*>(m_newNonce), 32);

    QByteArray hash1 = Crypto::CryptoEngine::sha1(nPlusS);
    QByteArray hash2 = Crypto::CryptoEngine::sha1(sPlusN);
    QByteArray hash3 = Crypto::CryptoEngine::sha1(nPlusN);

    m_tmpAesKey = hash1 + hash2.left(12);
    m_tmpAesIv = hash2.mid(12, 8) + hash3 + QByteArray(reinterpret_cast<const char*>(m_newNonce), 4);

    QByteArray decryptedAnswer;
    decryptedAnswer.resize(encryptedAnswer.size());
    if (!Crypto::CryptoEngine::aesIgeDecrypt(reinterpret_cast<const uint8_t*>(encryptedAnswer.constData()),
                                            reinterpret_cast<uint8_t*>(decryptedAnswer.data()),
                                            encryptedAnswer.size(),
                                            reinterpret_cast<const uint8_t*>(m_tmpAesKey.constData()),
                                            reinterpret_cast<const uint8_t*>(m_tmpAesIv.constData()))) {
        emit errorOccurred("Handshake: Failed to decrypt server_DH_inner_data");
        return;
    }

    TL::TLBuffer innerBuf(reinterpret_cast<const uint8_t*>(decryptedAnswer.constData() + 20), decryptedAnswer.size() - 20);
    uint32_t innerConstructor;
    innerBuf.readUInt32(innerConstructor);

    uint8_t inNonce[16], inServerNonce[16];
    innerBuf.readInt128(inNonce);
    innerBuf.readInt128(inServerNonce);

    int32_t g;
    innerBuf.readInt32(g);

    QByteArray dhPrimeBytes, g_a_bytes;
    innerBuf.readBytes(dhPrimeBytes);
    innerBuf.readBytes(g_a_bytes);

    int32_t serverTime;
    innerBuf.readInt32(serverTime);

    m_timeOffset = serverTime - static_cast<qint32>(QDateTime::currentDateTimeUtc().toTime_t());

    emit logMessage(QString("DH Parameters parsed: g=%1, dh_prime=%2 bytes, serverTime=%3 (timeOffset=%4s)")
                    .arg(g).arg(dhPrimeBytes.size()).arg(serverTime).arg(m_timeOffset));

    // Generate random b (256 bytes)
    m_bBytes = Crypto::CryptoEngine::randomBytes(256);

    // Compute g_b = g^b mod dh_prime
    QByteArray g_b_bytes;
    if (!Crypto::CryptoEngine::computeDH(g, m_bBytes, dhPrimeBytes, g_b_bytes)) {
        emit errorOccurred("Handshake: Failed to compute g_b");
        return;
    }

    // Compute auth_key = g_a^b mod dh_prime (256 bytes)
    if (!Crypto::CryptoEngine::computeAuthKey(g_a_bytes, m_bBytes, dhPrimeBytes, m_authKey)) {
        emit errorOccurred("Handshake: Failed to compute MTProto auth_key");
        return;
    }

    emit logMessage(QString("Auth Key successfully computed (256 bytes). Constructing client_DH_inner_data..."));

    // Build client_DH_inner_data#6643b654
    TL::TLBuffer clientInnerBuf;
    clientInnerBuf.writeUInt32(TL::ID_CLIENT_DH_INNER_DATA);
    clientInnerBuf.writeInt128(m_nonce);
    clientInnerBuf.writeInt128(m_serverNonce);
    clientInnerBuf.writeInt64(0); // retry_id = 0
    clientInnerBuf.writeBytes(g_b_bytes);

    QByteArray clientEncrypted;
    QByteArray rawClientData = clientInnerBuf.buffer();
    QByteArray clientHash = Crypto::CryptoEngine::sha1(rawClientData);
    QByteArray clientPayload = clientHash + rawClientData;

    size_t padLen = (16 - (clientPayload.size() % 16)) % 16;
    if (padLen > 0) {
        clientPayload.append(Crypto::CryptoEngine::randomBytes(padLen));
    }

    clientEncrypted.resize(clientPayload.size());
    Crypto::CryptoEngine::aesIgeEncrypt(reinterpret_cast<const uint8_t*>(clientPayload.constData()),
                                        reinterpret_cast<uint8_t*>(clientEncrypted.data()),
                                        clientPayload.size(),
                                        reinterpret_cast<const uint8_t*>(m_tmpAesKey.constData()),
                                        reinterpret_cast<const uint8_t*>(m_tmpAesIv.constData()));

    m_state = STATE_HANDSHAKE_SET_DH;
    emit stateChanged(static_cast<int>(m_state), "Handshake: Sending set_client_DH_params");

    TL::TLBuffer setDhBuf;
    setDhBuf.writeInt64(0);
    setDhBuf.writeInt64(generateMessageId());
    setDhBuf.writeInt32(4 + 16 + 16 + 4 + clientEncrypted.size());

    setDhBuf.writeUInt32(TL::ID_SET_CLIENT_DH_PARAMS);
    setDhBuf.writeInt128(m_nonce);
    setDhBuf.writeInt128(m_serverNonce);
    setDhBuf.writeBytes(clientEncrypted);

    QByteArray packet(reinterpret_cast<const char*>(setDhBuf.data()), setDhBuf.size());
    m_transport->sendPacket(packet);
}

void MTProtoSession::handleSetClientDHParamsAnswer(quint32 constructor, const quint8* data, size_t size) {
    if (constructor != TL::ID_DH_GEN_OK) {
        emit errorOccurred("Handshake: set_client_DH_params failed (dh_gen_retry/fail)");
        return;
    }

    TL::TLBuffer buf(data, size);
    uint8_t rNonce[16], rServerNonce[16], newNonceHash[16];
    buf.readInt128(rNonce);
    buf.readInt128(rServerNonce);
    buf.readRaw(newNonceHash, 16);

    // Compute auth_key_id = lower 64 bits of SHA1(auth_key)
    QByteArray authKeySha1 = Crypto::CryptoEngine::sha1(m_authKey);
    m_authKeyId = 0;
    for (int i = 12; i < 20; ++i) {
        m_authKeyId |= (static_cast<uint64_t>(static_cast<uint8_t>(authKeySha1[i])) << ((i - 12) * 8));
    }

    // Compute server_salt = new_nonce[0..7] ^ server_nonce[0..7]
    uint64_t saltA = 0, saltB = 0;
    for (int i = 0; i < 8; ++i) {
        saltA |= (static_cast<uint64_t>(m_newNonce[i]) << (i * 8));
        saltB |= (static_cast<uint64_t>(m_serverNonce[i]) << (i * 8));
    }
    m_serverSalt = saltA ^ saltB;

    Crypto::CryptoEngine::generateRandomBytes(reinterpret_cast<uint8_t*>(&m_sessionId), 8);
    m_seqNo = 0;

    m_state = STATE_ENCRYPTED_READY;
    emit stateChanged(static_cast<int>(m_state), "MTProto 2.0 Encrypted Session Ready");
    emit authKeyGenerated(m_authKeyId);

    emit logMessage(QString("=================================================="));
    emit logMessage(QString("SUCCESS: MTProto 2.0 Auth Key Created!"));
    emit logMessage(QString("AuthKeyID: 0x%1").arg(m_authKeyId, 16, 16, QChar('0')));
    emit logMessage(QString("ServerSalt: 0x%1, SessionID: 0x%2").arg(QString::number(m_serverSalt, 16)).arg(QString::number(m_sessionId, 16)));
    emit logMessage(QString("=================================================="));

    sendGetNearestDc();
}

// --------------------------------------------------------------------------
// MTProto RPC Queries (Layer 195 Dispatcher)
// --------------------------------------------------------------------------
void MTProtoSession::sendPingDelayDisconnect() {
    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(TL::ID_PING_DELAY_DISCONNECT);
    uint64_t pingId = 0;
    Crypto::CryptoEngine::generateRandomBytes(reinterpret_cast<uint8_t*>(&pingId), 8);
    rpcBuf.writeInt64(static_cast<int64_t>(pingId));
    rpcBuf.writeInt32(35); // disconnect_delay = 35s

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

void MTProtoSession::sendGetNearestDc() {
    emit logMessage(QString("Sending Encrypted RPC: invokeWithLayer(layer 195, initConnection(help.getNearestDc))..."));

    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer#da9b0d0d
    rpcBuf.writeInt32(195);        // layer: 195

    rpcBuf.writeUInt32(0xc1cd5ea9); // initConnection#c1cd5ea9
    rpcBuf.writeInt32(0);          // flags = 0
    rpcBuf.writeInt32(Config::API_ID);
    rpcBuf.writeString(Config::DEVICE_MODEL);
    rpcBuf.writeString(Config::SYSTEM_VERSION);
    rpcBuf.writeString(Config::APP_VERSION);
    rpcBuf.writeString(Config::SYSTEM_LANG_CODE);
    rpcBuf.writeString(Config::LANG_PACK);
    rpcBuf.writeString(Config::LANG_CODE);

    rpcBuf.writeUInt32(TL::ID_HELP_GET_NEAREST_DC); // query: help.getNearestDc

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

void MTProtoSession::sendAuthSendCode(const QString& phoneNumber) {
    if (m_state != STATE_ENCRYPTED_READY) {
        emit errorOccurred("Cannot send code: MTProto session is not encrypted");
        return;
    }

    QString cleanPhone = phoneNumber;
    cleanPhone.remove('+').remove(' ').remove('-').remove('(').remove(')');

    emit logMessage(QString("Requesting Telegram login code for +%1...").arg(cleanPhone));

    // invokeWithLayer#da9b0d0d layer:int query:!X = X;
    // auth.sendCode#a677244f phone_number:string api_id:int api_hash:string settings:CodeSettings = auth.SentCode;
    // codeSettings#ad253d78 flags:#
    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195
    rpcBuf.writeUInt32(TL::ID_AUTH_SEND_CODE);
    rpcBuf.writeString(cleanPhone);
    rpcBuf.writeInt32(Config::API_ID);
    rpcBuf.writeString(Config::API_HASH);

    rpcBuf.writeUInt32(TL::ID_CODE_SETTINGS);
    rpcBuf.writeInt32(0); // flags = 0

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

void MTProtoSession::sendAuthResendCode(const QString& phoneNumber, const QString& phoneCodeHash) {
    if (m_state != STATE_ENCRYPTED_READY) {
        emit errorOccurred("Cannot resend code: MTProto session is not encrypted");
        return;
    }

    QString cleanPhone = phoneNumber;
    cleanPhone.remove('+').remove(' ').remove('-').remove('(').remove(')');
    QString cleanHash = phoneCodeHash.trimmed();

    emit logMessage(QString("Requesting Telegram code resend (SMS) for +%1 (hash: %2)...").arg(cleanPhone).arg(cleanHash));

    // invokeWithLayer#da9b0d0d layer:int query:!X = X;
    // auth.resendCode#cae47523 flags:# phone_number:string phone_code_hash:string reason:flags.0?string = auth.SentCode;
    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195
    rpcBuf.writeUInt32(TL::ID_AUTH_RESEND_CODE); // 0xcae47523
    rpcBuf.writeInt32(0);          // flags = 0
    rpcBuf.writeString(cleanPhone);
    rpcBuf.writeString(cleanHash);

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

void MTProtoSession::sendAuthSignIn(const QString& phoneNumber, const QString& phoneCodeHash, const QString& phoneCode) {
    if (m_state != STATE_ENCRYPTED_READY) {
        emit errorOccurred("Cannot sign in: MTProto session is not encrypted");
        return;
    }

    QString cleanPhone = phoneNumber;
    cleanPhone.remove('+').remove(' ').remove('-').remove('(').remove(')');
    QString cleanCode = phoneCode.trimmed();
    QString cleanHash = phoneCodeHash.trimmed();

    emit logMessage(QString("Submitting login verification code %1 for +%2 (hash: %3)...").arg(cleanCode).arg(cleanPhone).arg(cleanHash));

    // invokeWithLayer#da9b0d0d layer:int query:!X = X;
    // auth.signIn#8d52a951 flags:# phone_number:string phone_code_hash:string phone_code:flags.0?string email_verification:flags.1?EmailVerification = auth.Authorization;
    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195
    rpcBuf.writeUInt32(TL::ID_AUTH_SIGN_IN); // 0x8d52a951
    rpcBuf.writeInt32(1);          // flags = 1 (phone_code is present)
    rpcBuf.writeString(cleanPhone);
    rpcBuf.writeString(cleanHash);
    rpcBuf.writeString(cleanCode);

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

void MTProtoSession::sendAccountGetPassword() {
    if (m_state != STATE_ENCRYPTED_READY) {
        emit errorOccurred("Cannot get password: MTProto session is not encrypted");
        return;
    }

    emit logMessage("Requesting 2FA Cloud Password parameters (account.getPassword)...");

    // NOTE: account.getPassword must NOT be wrapped in invokeWithLayer.
    // The MTProto layer (195) is already negotiated during session setup via the
    // initial invokeWithLayer -> initConnection exchange. Wrapping a subsequent
    // (non-initial) method in invokeWithLayer causes the server to reject it with
    // INPUT_METHOD_INVALID (400). Send the method bare like all post-negotiation calls.
    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(TL::ID_ACCOUNT_GET_PASSWORD); // 0x548a30f5

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

void MTProtoSession::sendAuthCheckPassword(const QString& password) {
    if (m_state != STATE_ENCRYPTED_READY) {
        emit errorOccurred("Cannot check password: MTProto session is not encrypted");
        return;
    }

    if (m_pwdSrpId == 0 || m_pwdP.isEmpty() || m_pwdSrpB.isEmpty()) {
        emit logMessage("SRP session expired or missing parameters. Requesting fresh account.getPassword...");
        m_pendingPassword = password;
        sendAccountGetPassword();
        return;
    }

    emit logMessage(QString("Computing SRP-6A cryptographic proof for 2FA Cloud Password (srp_id: %1)...").arg(m_pwdSrpId));

    QByteArray srpA, srpM1;
    if (!Crypto::CryptoEngine::computeSRP6A(password, m_pwdSalt1, m_pwdSalt2, m_pwdG, m_pwdP, m_pwdSrpB, srpA, srpM1)) {
        emit errorOccurred("Failed to compute SRP-6A proof for 2FA password");
        return;
    }

    emit logMessage(QString("Submitting auth.checkPassword with SRP-6A proof (srp_id: %1, A: %2 bytes, M1: %3 bytes)...")
                    .arg(m_pwdSrpId).arg(srpA.size()).arg(srpM1.size()));

    int64_t srpIdToSend = m_pwdSrpId;
    m_pwdSrpId = 0; // Invalidate current srp_id immediately so it can never be reused

    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195
    rpcBuf.writeUInt32(TL::ID_AUTH_CHECK_PASSWORD);
    rpcBuf.writeUInt32(TL::ID_INPUT_CHECK_PASSWORD_SRP);
    rpcBuf.writeInt64(srpIdToSend);
    rpcBuf.writeBytes(srpA);
    rpcBuf.writeBytes(srpM1);

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

void MTProtoSession::sendAuthLogOut() {
    if (m_state != STATE_ENCRYPTED_READY) return;

    emit logMessage("Logging out session (auth.logOut)...");

    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195
    rpcBuf.writeUInt32(TL::ID_AUTH_LOG_OUT); // 0x3e72ba19

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

void MTProtoSession::sendExportLoginToken() {
    if (m_state != STATE_ENCRYPTED_READY) return;

    emit logMessage("Requesting QR Code Login Token (auth.exportLoginToken)...");

    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195
    rpcBuf.writeUInt32(TL::ID_AUTH_EXPORT_LOGIN_TOKEN); // 0xb7e085fe
    rpcBuf.writeInt32(Config::API_ID);
    rpcBuf.writeString(Config::API_HASH);
    rpcBuf.writeUInt32(TL::ID_VECTOR);
    rpcBuf.writeInt32(0); // except_ids count = 0

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

bool MTProtoSession::canSendToPeer(qint64 peerId) const {
    if (m_entityCanSend.contains(peerId)) {
        return m_entityCanSend.value(peerId);
    }
    // Default to sendable for peers we have no per-peer flag data for
    // (private chats, plain groups, and users default to sendable).
    return true;
}

void MTProtoSession::sendMessagesGetDialogs(int offsetDate, int offsetId, int limit) {
    if (m_state != STATE_ENCRYPTED_READY) {
        emit errorOccurred("Cannot fetch dialogs: MTProto session is not encrypted");
        return;
    }

    emit logMessage(QString("Requesting live Telegram dialogs (limit: %1, offset_id: %2)...").arg(limit).arg(offsetId));

    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195
    rpcBuf.writeUInt32(TL::ID_MESSAGES_GET_DIALOGS); // 0xa0f4cb4f
    rpcBuf.writeInt32(0);          // flags = 0
    rpcBuf.writeInt32(offsetDate); // offset_date
    rpcBuf.writeInt32(offsetId);   // offset_id
    rpcBuf.writeUInt32(TL::ID_INPUT_PEER_EMPTY); // offset_peer: inputPeerEmpty#7f07465a
    rpcBuf.writeInt32(limit);      // limit
    rpcBuf.writeInt64(0);          // hash = 0

    sendEncryptedMessage(rpcBuf.buffer(), true);
}

// --------------------------------------------------------------------------
// MTProto 2.0 Encrypted Message Transmission & Decryption
// --------------------------------------------------------------------------
void MTProtoSession::sendEncryptedMessage(const QByteArray& messageData, bool isContentRelated) {
    qint64 msgId = generateMessageId();
    quint32 seqNo = generateSeqNo(isContentRelated);

    if (isContentRelated) {
        // Remember in-flight content messages so they can be re-sent with the
        // exact same msg_id/seq_no after a bad_server_salt (per MTProto spec).
        PendingContentMessage pcm;
        pcm.data = messageData;
        pcm.msgId = msgId;
        pcm.seqNo = seqNo;
        m_pendingContentMessages.append(pcm);
    }

    sendEncryptedMessage(messageData, msgId, seqNo);
}

void MTProtoSession::sendEncryptedMessage(const QByteArray& messageData, qint64 msgId, quint32 seqNo) {
    TL::TLBuffer plainBuf;
    plainBuf.writeInt64(static_cast<int64_t>(m_serverSalt));
    plainBuf.writeInt64(static_cast<int64_t>(m_sessionId));
    plainBuf.writeInt64(msgId);
    plainBuf.writeInt32(static_cast<int32_t>(seqNo));
    plainBuf.writeInt32(static_cast<int32_t>(messageData.size()));
    plainBuf.writeRaw(reinterpret_cast<const uint8_t*>(messageData.constData()), messageData.size());

    size_t unpaddedLen = plainBuf.size();
    size_t padLen = (16 - (unpaddedLen % 16));
    if (padLen < 12) {
        padLen += 16;
    }
    QByteArray padBytes = Crypto::CryptoEngine::randomBytes(padLen);
    plainBuf.writeRaw(reinterpret_cast<const uint8_t*>(padBytes.constData()), padBytes.size());

    uint8_t msgKey[16];
    Crypto::CryptoEngine::computeMTProto2MsgKey(reinterpret_cast<const uint8_t*>(m_authKey.constData()), plainBuf.data(), plainBuf.size(), true, msgKey);

    uint8_t aesKey[32], aesIv[32];
    Crypto::CryptoEngine::deriveMTProto2Keys(reinterpret_cast<const uint8_t*>(m_authKey.constData()), msgKey, true, aesKey, aesIv);

    QByteArray encrypted;
    encrypted.resize(plainBuf.size());
    Crypto::CryptoEngine::aesIgeEncrypt(plainBuf.data(), reinterpret_cast<uint8_t*>(encrypted.data()), plainBuf.size(), aesKey, aesIv);

    TL::TLBuffer envelope;
    envelope.writeInt64(static_cast<int64_t>(m_authKeyId));
    envelope.writeRaw(msgKey, 16);
    envelope.writeRaw(reinterpret_cast<const uint8_t*>(encrypted.constData()), encrypted.size());

    QByteArray packet(reinterpret_cast<const char*>(envelope.data()), envelope.size());
    m_transport->sendPacket(packet);
}

void MTProtoSession::resendPendingContentMessage() {
    if (m_pendingContentMessages.isEmpty()) {
        emit logMessage("[WARN] bad_server_salt received but no pending content messages to re-send");
        return;
    }
    emit logMessage(QString("Re-sending %1 pending content message(s) with corrected server salt...").arg(m_pendingContentMessages.size()));
    // Re-send ALL pending messages with the exact same msg_id/seq_no each.
    // The salt has already been refreshed; subsequent bad_server_salt notifications
    // (one per in-flight message that used the old salt) will find an empty queue.
    QList<PendingContentMessage> pending = m_pendingContentMessages;
    m_pendingContentMessages.clear();
    for (int i = 0; i < pending.size(); ++i) {
        emit logMessage(QString("  Re-sending MsgId %1, SeqNo %2").arg(pending[i].msgId).arg(pending[i].seqNo));
        sendEncryptedMessage(pending[i].data, pending[i].msgId, pending[i].seqNo);
    }
}

void MTProtoSession::handleEncryptedPacket(const quint8* data, size_t size) {
    if (size < 24) return;

    uint64_t authKeyId = 0;
    for (int i = 0; i < 8; ++i) {
        authKeyId |= (static_cast<uint64_t>(data[i]) << (i * 8));
    }

    if (authKeyId != m_authKeyId) {
        emit logMessage(QString("[WARN] Received packet for unknown auth_key_id: 0x%1").arg(authKeyId, 16, 16, QChar('0')));
        return;
    }

    const uint8_t* msgKey = data + 8;
    const uint8_t* encryptedData = data + 24;
    size_t encryptedLen = size - 24;

    if (encryptedLen % 16 != 0) {
        emit errorOccurred("Invalid encrypted payload length");
        return;
    }

    uint8_t aesKey[32], aesIv[32];
    Crypto::CryptoEngine::deriveMTProto2Keys(reinterpret_cast<const uint8_t*>(m_authKey.constData()), msgKey, false, aesKey, aesIv);

    QByteArray decrypted;
    decrypted.resize(static_cast<int>(encryptedLen));
    if (!Crypto::CryptoEngine::aesIgeDecrypt(encryptedData, reinterpret_cast<uint8_t*>(decrypted.data()), encryptedLen, aesKey, aesIv)) {
        emit errorOccurred("Failed to decrypt MTProto 2.0 encrypted packet");
        return;
    }

    uint8_t computedMsgKey[16];
    Crypto::CryptoEngine::computeMTProto2MsgKey(reinterpret_cast<const uint8_t*>(m_authKey.constData()),
                                                reinterpret_cast<const uint8_t*>(decrypted.constData()),
                                                decrypted.size(), false, computedMsgKey);
    if (memcmp(msgKey, computedMsgKey, 16) != 0) {
        emit errorOccurred("MTProto 2.0 Decryption Error: msg_key verification failed");
        return;
    }

    TL::TLBuffer plainBuf(reinterpret_cast<const uint8_t*>(decrypted.constData()), decrypted.size());
    int64_t serverSalt, sessionId, msgId;
    int32_t seqNo, msgLen;

    plainBuf.readInt64(serverSalt);
    plainBuf.readInt64(sessionId);
    plainBuf.readInt64(msgId);
    plainBuf.readInt32(seqNo);
    plainBuf.readInt32(msgLen);

    if (msgLen <= 0 || static_cast<size_t>(msgLen) > plainBuf.remaining()) {
        emit errorOccurred("Invalid inner message length in decrypted payload");
        return;
    }

    if (!m_pingTimer->isActive()) {
        m_pingTimer->start();
    }

    processPlainMessage(plainBuf);
}

void MTProtoSession::processPlainMessage(TL::TLBuffer& plainBuf) {
    if (plainBuf.remaining() < 4) return;

    uint32_t constructor;
    plainBuf.readUInt32(constructor);

    emit logMessage(QString("Encrypted MTProto 2.0 Response received! Constructor: 0x%1").arg(constructor, 8, 16, QChar('0')));

    if (constructor == TL::ID_MSG_CONTAINER) {
        int32_t count = 0;
        plainBuf.readInt32(count);
        emit logMessage(QString("Message container received containing %1 messages.").arg(count));
        for (int i = 0; i < count && plainBuf.remaining() >= 16; ++i) {
            int64_t innerMsgId;
            int32_t innerSeqNo, innerBytes;
            plainBuf.readInt64(innerMsgId);
            plainBuf.readInt32(innerSeqNo);
            plainBuf.readInt32(innerBytes);
            processPlainMessage(plainBuf);
        }
    } else if (constructor == TL::ID_NEW_SESSION_CREATED) {
        int64_t firstMsgId, uniqueId, serverSalt;
        plainBuf.readInt64(firstMsgId);
        plainBuf.readInt64(uniqueId);
        plainBuf.readInt64(serverSalt);
        m_serverSalt = static_cast<uint64_t>(serverSalt);
        emit logMessage(QString("New MTProto Session confirmed. Server Salt updated: 0x%1").arg(QString::number(m_serverSalt, 16)));
    } else if (constructor == TL::ID_BAD_SERVER_SALT) {
        int64_t badMsgId;
        int32_t badSeqNo, errCode;
        int64_t newServerSalt;
        plainBuf.readInt64(badMsgId);
        plainBuf.readInt32(badSeqNo);
        plainBuf.readInt32(errCode);
        plainBuf.readInt64(newServerSalt);
        m_serverSalt = static_cast<uint64_t>(newServerSalt);
        emit logMessage(QString("Server salt refreshed: 0x%1.").arg(QString::number(m_serverSalt, 16)));

        // Re-send the exact message that failed (same msg_id/seq_no) with the corrected salt,
        // rather than building a fresh RPC that would inflate the seq_no counter.
        resendPendingContentMessage();
    } else if (constructor == TL::ID_BAD_MSG_NOTIFICATION) {
        int64_t badMsgId;
        int32_t badSeqNo, errCode;
        plainBuf.readInt64(badMsgId);
        plainBuf.readInt32(badSeqNo);
        plainBuf.readInt32(errCode);
        emit logMessage(QString("[WARN] Bad Message Notification for MsgId %1, Error Code: %2").arg(badMsgId).arg(errCode));
    } else if (constructor == TL::ID_PONG) {
        int64_t pongMsgId, pingId;
        plainBuf.readInt64(pongMsgId);
        plainBuf.readInt64(pingId);
        emit logMessage(QString("Heartbeat PONG received from Telegram (ping_id: %1)").arg(pingId));
    } else if (constructor == TL::ID_MSGS_ACK) {
        uint32_t vectorConstructor;
        int32_t ackCount;
        plainBuf.readUInt32(vectorConstructor);
        plainBuf.readInt32(ackCount);
        for (int i = 0; i < ackCount && plainBuf.remaining() >= 8; ++i) {
            int64_t ackMsgId;
            plainBuf.readInt64(ackMsgId);
        }
        emit logMessage(QString("Server acknowledged %1 messages.").arg(ackCount));
    } else if (constructor == TL::ID_GZIP_PACKED) {
        QByteArray packedData;
        plainBuf.readBytes(packedData);
        QByteArray uncompressed;
        if (Crypto::CryptoEngine::gzipDecompress(packedData, uncompressed)) {
            emit logMessage(QString("Decompressed outer GZIP packed message (%1 -> %2 bytes)").arg(packedData.size()).arg(uncompressed.size()));
            TL::TLBuffer uncompBuf(uncompressed);
            processPlainMessage(uncompBuf);
        } else {
            emit logMessage("[ERROR] Failed to decompress outer GZIP packed message");
        }
    } else if (constructor == TL::ID_RPC_RESULT) {
        int64_t reqMsgId;
        plainBuf.readInt64(reqMsgId);
        uint32_t innerRpcConstructor;
        plainBuf.readUInt32(innerRpcConstructor);

        emit logMessage(QString("RPC Result for ReqMsgId: %1, Inner Constructor: 0x%2").arg(reqMsgId).arg(innerRpcConstructor, 8, 16, QChar('0')));
        handleRpcResult(reqMsgId, innerRpcConstructor, plainBuf);
    } else if (constructor == TL::ID_NEAREST_DC) {
        QString country;
        int32_t thisDc, nearestDc;
        plainBuf.readString(country);
        plainBuf.readInt32(thisDc);
        plainBuf.readInt32(nearestDc);

        emit nearestDcReceived(country, thisDc, nearestDc);
    } else if (constructor == TL::ID_UPDATES || constructor == TL::ID_UPDATES_COMBINED) {
        emit logMessage(QString("Incoming real-time MTProto updates container (constructor: 0x%1)").arg(constructor, 8, 16, QChar('0')));

        // Scan raw payload for updateNewMessage (0x9a1caff9) entries
        const QByteArray& rawData = plainBuf.buffer();
        int foundNewMessages = 0;

        for (int pos = 0; pos <= rawData.size() - 20; ++pos) {
            uint32_t cons = *reinterpret_cast<const uint32_t*>(rawData.constData() + pos);
            if (cons == TL::ID_UPDATE_NEW_MESSAGE || cons == TL::ID_UPDATE_NEW_CHANNEL_MESSAGE) {
                // Scan forward from this update for an ID_MESSAGE (0x3ae56482) object
                for (int mPos = pos + 4; mPos <= rawData.size() - 40; ++mPos) {
                    uint32_t mCons = *reinterpret_cast<const uint32_t*>(rawData.constData() + mPos);
                    if (mCons == TL::ID_MESSAGE) {
                        TL::TLBuffer msgBuf(rawData.mid(mPos + 4, 80));
                        int32_t mFlags = 0;
                        int32_t msgId = 0;
                        int64_t fromId = 0;
                        int64_t peerId = 0;
                        int32_t date = 0;
                        QString text;

                        msgBuf.readInt32(mFlags);
                        msgBuf.readInt32(msgId);

                        if (mFlags & (1 << 0)) {
                            // from_id (peer)
                            uint32_t fCons = 0;
                            msgBuf.readUInt32(fCons);
                            msgBuf.readInt64(fromId);
                        }
                        // to_id (peer)
                        uint32_t tCons = 0;
                        msgBuf.readUInt32(tCons);
                        msgBuf.readInt64(peerId);
                        if (tCons == TL::ID_PEER_USER) {
                            // user message: check if outgoing
                            if (mFlags & (1 << 2)) {
                                fromId = peerId;
                            }
                        }

                        msgBuf.readInt32(date);
                        msgBuf.readString(text);

                        int effectivePeerType = 0;
                        if (tCons == TL::ID_PEER_USER) effectivePeerType = 1;
                        else if (tCons == TL::ID_PEER_CHAT || tCons == 0xbad052c3) effectivePeerType = 2;
                        else if (tCons == TL::ID_PEER_CHANNEL) effectivePeerType = 3;

                        if (msgId != 0 && effectivePeerType > 0) {
                            bool isOut = (mFlags & (1 << 2)) != 0;
                            QVariantMap msgMap;
                            msgMap["id"] = msgId;
                            msgMap["peerId"] = peerId;
                            msgMap["peerType"] = effectivePeerType;
                            msgMap["text"] = text;
                            msgMap["isOutgoing"] = isOut;
                            msgMap["date"] = date;
                            msgMap["formattedTime"] = QDateTime::fromTime_t(date).toString("hh:mm");

                            emit logMessage(QString("[NEW MESSAGE] id %1 peer %2 (type %3): '%4' (out: %5)")
                                            .arg(msgId).arg(peerId).arg(effectivePeerType)
                                            .arg(text.left(60)).arg(isOut ? "YES" : "NO"));

                            emit newMessageReceived(peerId, effectivePeerType, msgMap);
                            foundNewMessages++;
                        }
                        break;
                    }
                }
            }
        }
        emit logMessage(QString("Updates container: found %1 new message(s)").arg(foundNewMessages));
    } else if (constructor == TL::ID_UPDATE_SHORT_MESSAGE || constructor == TL::ID_UPDATE_SHORT_CHAT_MESSAGE) {
        int32_t flags;
        plainBuf.readInt32(flags);
        bool isOut       = (flags & (1 << 1)) != 0;
        bool mentioned   = (flags & (1 << 2)) != 0;
        bool silent      = (flags & (1 << 5)) != 0;

        int32_t msgId = 0;
        plainBuf.readInt32(msgId);

        int64_t fromPeerId = 0;
        int32_t fromPeerType = 0;
        int64_t chatId = 0;

        if (constructor == TL::ID_UPDATE_SHORT_CHAT_MESSAGE) {
            // updateShortChatMessage: from_id (peer), chat_id
            uint32_t fromCons = 0;
            plainBuf.readUInt32(fromCons);
            plainBuf.readInt64(fromPeerId);
            if (fromCons == TL::ID_PEER_USER) fromPeerType = 1;
            else if (fromCons == TL::ID_PEER_CHAT || fromCons == 0xbad052c3) fromPeerType = 2;
            else if (fromCons == TL::ID_PEER_CHANNEL) fromPeerType = 3;
            plainBuf.readInt64(chatId);
        } else {
            // updateShortMessage: user_id
            plainBuf.readInt64(fromPeerId);
            fromPeerType = 1; // user
        }

        QString text;
        plainBuf.readString(text);

        int32_t pts = 0, ptsCount = 0, date = 0;
        plainBuf.readInt32(pts);
        plainBuf.readInt32(ptsCount);
        plainBuf.readInt32(date);

        // Determine the effective peerId for this message
        int64_t effectivePeerId = 0;
        int effectivePeerType = 0;
        if (constructor == TL::ID_UPDATE_SHORT_CHAT_MESSAGE) {
            effectivePeerId = chatId;
            effectivePeerType = 2; // chat (group)
        } else {
            effectivePeerId = fromPeerId;
            effectivePeerType = fromPeerType;
        }

        QVariantMap msgMap;
        msgMap["id"] = msgId;
        msgMap["peerId"] = effectivePeerId;
        msgMap["peerType"] = effectivePeerType;
        msgMap["text"] = text;
        msgMap["isOutgoing"] = isOut;
        msgMap["mentioned"] = mentioned;
        msgMap["silent"] = silent;
        msgMap["date"] = date;
        msgMap["formattedTime"] = QDateTime::fromTime_t(date).toString("hh:mm");

        emit logMessage(QString("[NEW MESSAGE] id %1 from peer %2 (type %3): '%4' (out: %5)")
                        .arg(msgId).arg(effectivePeerId).arg(effectivePeerType)
                        .arg(text.left(60)).arg(isOut ? "YES" : "NO"));

        emit newMessageReceived(effectivePeerId, effectivePeerType, msgMap);
    }
}

// Forward declarations for the anonymous-namespace TL walking helpers below.
// Opening the anonymous namespace here (same enclosing namespace) lets these be
// used by handleRpcResult before their definitions later in the file.
namespace {
int tlSkipString(TL::TLBuffer& b);
int tlReadString(TL::TLBuffer& b, QString& out);
int tlSkipPeer(TL::TLBuffer& b);
int tlSkipReplyHeader(TL::TLBuffer& b);
int tlReadReplyHeader(TL::TLBuffer& b, int32_t& outReplyToId, QString& outQuoteText);
int tlSkipFwdHeader(TL::TLBuffer& b);
int tlReadMessageLeading(TL::TLBuffer& b, int32_t& outId, int32_t& outDate,
                         bool& outIsOut, bool& outHasMedia, QString& outText, int& outConsumed,
                         int32_t& outReplyToId, QString& outReplyQuoteText);
} // namespace

void MTProtoSession::handleRpcResult(qint64 reqMsgId, quint32 innerRpcConstructor, TL::TLBuffer& plainBuf) {
    if (innerRpcConstructor == TL::ID_GZIP_PACKED) {
        QByteArray packedData;
        plainBuf.readBytes(packedData);
        QByteArray uncompressed;
        if (Crypto::CryptoEngine::gzipDecompress(packedData, uncompressed)) {
            emit logMessage(QString("Decompressed RPC GZIP packed payload for ReqMsgId %1 (%2 -> %3 bytes)").arg(reqMsgId).arg(packedData.size()).arg(uncompressed.size()));
            TL::TLBuffer uncompBuf(uncompressed);
            uint32_t uncompConstructor = 0;
            if (uncompBuf.readUInt32(uncompConstructor)) {
                emit logMessage(QString("Unpacked GZIP Inner Constructor: 0x%1").arg(uncompConstructor, 8, 16, QChar('0')));
                handleRpcResult(reqMsgId, uncompConstructor, uncompBuf);
            }
        } else {
            emit logMessage(QString("[ERROR] Failed to decompress RPC GZIP packed payload for ReqMsgId %1").arg(reqMsgId));
        }
        return;
    }

    if (innerRpcConstructor == TL::ID_MESSAGES_SENT_MESSAGE) {
        // messages.sentMessage#d1f4ee35 flags:# out:flags.1?true msg_id:int date:int pts:int pts_count:int
        int32_t sFlags = 0;
        int32_t msgId = 0;
        int32_t date = 0;
        plainBuf.readInt32(sFlags);
        plainBuf.readInt32(msgId);
        plainBuf.readInt32(date);

        QVariantMap sentMap;
        sentMap["id"] = msgId;
        sentMap["date"] = date;
        sentMap["formattedTime"] = QDateTime::fromTime_t(date).toString("hh:mm");

        emit logMessage(QString("MESSAGE SENT CONFIRMED: id %1, date %2").arg(msgId).arg(date));
        emit messageSent(0, 0, msgId, date);
        return;
    }

    if (innerRpcConstructor == TL::ID_NEAREST_DC) {
        QString country;
        int32_t thisDc, nearestDc;
        plainBuf.readString(country);
        plainBuf.readInt32(thisDc);
        plainBuf.readInt32(nearestDc);

        emit nearestDcReceived(country, thisDc, nearestDc);

        emit logMessage(QString("=================================================="));
        emit logMessage(QString("LIVE TELEGRAM RPC SUCCESS: nearestDc"));
        emit logMessage(QString("Country: %1, Current DC: %2, Nearest Recommended DC: %3").arg(country).arg(thisDc).arg(nearestDc));
        emit logMessage(QString("=================================================="));
    } else if (innerRpcConstructor == TL::ID_AUTH_SENT_CODE) {
        // auth.sentCode#5e002502 flags:# type:auth.SentCodeType phone_code_hash:string next_type:flags.1?auth.CodeType timeout:flags.2?int = auth.SentCode;
        int32_t flags;
        plainBuf.readInt32(flags);
        uint32_t typeConstructor;
        plainBuf.readUInt32(typeConstructor);

        QString typeStr = "Telegram App";
        if (typeConstructor == TL::ID_AUTH_SENT_CODE_TYPE_APP) {
            typeStr = "Telegram App";
            int32_t length; plainBuf.readInt32(length);
        } else if (typeConstructor == TL::ID_AUTH_SENT_CODE_TYPE_SMS) {
            typeStr = "SMS";
            int32_t length; plainBuf.readInt32(length);
        } else if (typeConstructor == TL::ID_AUTH_SENT_CODE_TYPE_CALL) {
            typeStr = "Phone Call";
            int32_t length; plainBuf.readInt32(length);
        } else if (typeConstructor == TL::ID_AUTH_SENT_CODE_TYPE_FLASH_CALL) {
            typeStr = "Flash Call";
            QString pattern; plainBuf.readString(pattern);
        } else if (typeConstructor == TL::ID_AUTH_SENT_CODE_TYPE_MISSED_CALL) {
            typeStr = "Missed Call";
            QString prefix; plainBuf.readString(prefix);
            int32_t length; plainBuf.readInt32(length);
        } else if (typeConstructor == TL::ID_AUTH_SENT_CODE_TYPE_EMAIL_CODE) {
            typeStr = "Email";
            int32_t emailFlags; plainBuf.readInt32(emailFlags);
            QString emailPattern; plainBuf.readString(emailPattern);
            int32_t length; plainBuf.readInt32(length);
        }

        QString phoneCodeHash;
        plainBuf.readString(phoneCodeHash);

        int32_t timeout = 60;
        if (flags & (1 << 1)) {
            uint32_t nextType; plainBuf.readUInt32(nextType);
        }
        if (flags & (1 << 2)) {
            plainBuf.readInt32(timeout);
        }

        emit logMessage(QString("=================================================="));
        emit logMessage(QString("TELEGRAM LOGIN CODE SENT"));
        emit logMessage(QString("Delivery Type: %1, Timeout: %2s, Hash: %3").arg(typeStr).arg(timeout).arg(phoneCodeHash));
        emit logMessage(QString("=================================================="));

        emit authSentCodeReceived(phoneCodeHash, typeStr, timeout);
    } else if (innerRpcConstructor == TL::ID_AUTH_AUTHORIZATION || innerRpcConstructor == TL::ID_AUTH_AUTHORIZATION_CD) {
        // auth.authorization#2ea2c0d4 flags:# setup_password_required:flags.1?true otherwise_relogin_days:flags.3?int tmp_sessions:flags.0?int future_auth_token:flags.2?bytes user:User = auth.Authorization;
        int32_t authFlags;
        plainBuf.readInt32(authFlags);
        if (authFlags & (1 << 0)) { int32_t tmp; plainBuf.readInt32(tmp); }
        if (authFlags & (1 << 2)) { QByteArray token; plainBuf.readBytes(token); }
        if (authFlags & (1 << 3)) { int32_t days; plainBuf.readInt32(days); }

        uint32_t userConstructor;
        plainBuf.readUInt32(userConstructor);

        int32_t userFlags;
        plainBuf.readInt32(userFlags);
        if (userConstructor == 0xb1b8cc83 || userConstructor == 0x83314fca || userConstructor == 0x2e566735) {
            int32_t userFlags2 = 0;
            plainBuf.readInt32(userFlags2);
        }

        int64_t userId = 0, accessHash = 0;
        plainBuf.readInt64(userId);

        if (userFlags & (1 << 0)) {
            plainBuf.readInt64(accessHash);
        }

        QString firstName, lastName, username, phone;
        if (userFlags & (1 << 1)) plainBuf.readString(firstName);
        if (userFlags & (1 << 2)) plainBuf.readString(lastName);
        if (userFlags & (1 << 3)) plainBuf.readString(username);
        if (userFlags & (1 << 4)) plainBuf.readString(phone);

        emit logMessage(QString("=================================================="));
        emit logMessage(QString("SUCCESSFUL TELEGRAM AUTHENTICATION!"));
        emit logMessage(QString("User ID: %1, Name: %2 %3 (@%4), Phone: %5").arg(userId).arg(firstName).arg(lastName).arg(username).arg(phone));
        emit logMessage(QString("=================================================="));

        emit authSuccessReceived(userId, static_cast<quint64>(accessHash), firstName, lastName, username, phone);
        sendMessagesGetDialogs(0, 0, 100);
    } else if (innerRpcConstructor == TL::ID_AUTH_SIGN_UP_REQUIRED) {
        emit logMessage("[AUTH] Sign Up is required for this new phone number");
        emit authSignUpRequiredReceived();
    } else if (innerRpcConstructor == TL::ID_ACCOUNT_PASSWORD) {
        // account.password#957b50fb flags:# has_recovery:flags.0?true has_secure_values:flags.1?true has_password:flags.2?true current_algo:flags.2?PasswordKdfAlgo srp_B:flags.2?bytes srp_id:flags.2?long hint:flags.3?string ...
        int32_t pwdFlags;
        plainBuf.readInt32(pwdFlags);

        if (pwdFlags & (1 << 2)) {
            uint32_t algoConstructor;
            plainBuf.readUInt32(algoConstructor);
            plainBuf.readBytes(m_pwdSalt1);
            plainBuf.readBytes(m_pwdSalt2);
            int32_t gVal; plainBuf.readInt32(gVal); m_pwdG = gVal;
            plainBuf.readBytes(m_pwdP);

            plainBuf.readBytes(m_pwdSrpB);
            plainBuf.readInt64(m_pwdSrpId);
        }

        if (pwdFlags & (1 << 3)) {
            plainBuf.readString(m_pwdHint);
        }

        emit logMessage(QString("2FA Cloud Password Needed. Hint: '%1'").arg(m_pwdHint));
        emit authPasswordNeeded(m_pwdHint);

        if (!m_pendingPassword.isEmpty()) {
            QString pwd = m_pendingPassword;
            m_pendingPassword.clear();
            sendAuthCheckPassword(pwd);
        }
    } else if (innerRpcConstructor == TL::ID_AUTH_LOGIN_TOKEN) {
        // auth.loginToken#629f1980 expires:int token:bytes = auth.LoginToken;
        int32_t expires;
        QByteArray tokenBytes;
        plainBuf.readInt32(expires);
        plainBuf.readBytes(tokenBytes);

        emit logMessage(QString("QR Login Token received (expires in %1s)").arg(expires));
        emit authLoginTokenReceived(tokenBytes, expires);
    } else if (innerRpcConstructor == TL::ID_AUTH_LOGIN_TOKEN_SUCCESS) {
        emit logMessage("QR Code successfully scanned and authorized!");
        emit authLoginSuccessReceived();
    } else if (innerRpcConstructor == TL::ID_AUTH_LOGIN_TOKEN_MIGRATE_TO) {
        int32_t targetDc;
        QByteArray tokenBytes;
        plainBuf.readInt32(targetDc);
        plainBuf.readBytes(tokenBytes);
        emit logMessage(QString("QR Login Token requires migration to DC %1").arg(targetDc));
        migrateToDc(targetDc);
    } else if (innerRpcConstructor == TL::ID_RPC_ERROR) {
        int32_t errCode;
        QString errMsg;
        plainBuf.readInt32(errCode);
        plainBuf.readString(errMsg);

        emit logMessage(QString("[RPC ERROR] Code: %1, Message: %2").arg(errCode).arg(errMsg));

        if (errMsg.startsWith("PHONE_MIGRATE_") || errMsg.startsWith("NETWORK_MIGRATE_")) {
            int targetDc = errMsg.section('_', -1).toInt();
            if (targetDc > 0) {
                emit logMessage(QString("Server requested DC migration to DC %1").arg(targetDc));
                migrateToDc(targetDc);
                return;
            }
        } else if (errMsg == "SESSION_PASSWORD_NEEDED" || errMsg == "PASSWORD_HASH_INVALID" || errMsg == "SRP_ID_INVALID" || errMsg == "SRP_PASSWORD_CHANGED") {
            m_pwdSrpId = 0;
            sendAccountGetPassword();
            if (errMsg == "SESSION_PASSWORD_NEEDED") {
                return;
            }
        }

        emit rpcErrorReceived(errCode, errMsg);
    } else if (innerRpcConstructor == TL::ID_MESSAGES_DIALOGS || innerRpcConstructor == TL::ID_MESSAGES_DIALOGS_SLICE) {
        emit logMessage(QString("=================================================="));
        emit logMessage(QString("LIVE TELEGRAM DIALOGS RECEIVED (constructor: 0x%1)").arg(innerRpcConstructor, 8, 16, QChar('0')));
        emit logMessage(QString("=================================================="));

        if (innerRpcConstructor == TL::ID_MESSAGES_DIALOGS_SLICE) {
            int32_t totalCount;
            plainBuf.readInt32(totalCount);
            emit logMessage(QString("Dialogs slice count: %1").arg(totalCount));
        }

        // 1. Scan payload for Dialogs (TL::ID_DIALOG = 0xd58a08c6)
        const QByteArray& rawData = plainBuf.buffer();
        QList<SessionDialogEntry> dialogEntries;

        for (int pos = 0; pos <= rawData.size() - 40; ++pos) {
            uint32_t cons = *reinterpret_cast<const uint32_t*>(rawData.constData() + pos);
            if (cons == TL::ID_DIALOG) {
                TL::TLBuffer dBuf(rawData.mid(pos + 4, 36));
                int32_t dFlags = 0;
                uint32_t peerCons = 0;
                int64_t peerId = 0;
                int32_t topMsg = 0, readInbox = 0, readOutbox = 0, unreadCount = 0;

                dBuf.readInt32(dFlags);
                dBuf.readUInt32(peerCons);
                dBuf.readInt64(peerId);
                dBuf.readInt32(topMsg);
                dBuf.readInt32(readInbox);
                dBuf.readInt32(readOutbox);
                dBuf.readInt32(unreadCount);

                int peerType = 0;
                if (peerCons == TL::ID_PEER_USER) {
                    peerType = 1;
                } else if (peerCons == TL::ID_PEER_CHAT || peerCons == 0xbad052c3) {
                    peerType = 2;
                } else if (peerCons == TL::ID_PEER_CHANNEL) {
                    peerType = 3;
                }

                if (peerType > 0 && peerId != 0) {
                    SessionDialogEntry de;
                    de.peerId = peerId;
                    de.peerType = peerType;
                    de.topMessage = topMsg;
                    de.unreadCount = (unreadCount >= 0 && unreadCount < 50000) ? unreadCount : 0;
                    de.isPinned = (dFlags & (1 << 2)) != 0;
                    dialogEntries.append(de);
                }
            }
        }
        emit logMessage(QString("Total valid dialogs scanned: %1").arg(dialogEntries.size()));

        // 2. Scan for Users, Channels, Chats, and Messages
        QMap<qint64, QString> entityNames;
        QMap<qint64, quint64> entityAccessHashes;
        QMap<qint64, QString> entityUsernames;
        QMap<int32_t, QPair<QString, int32_t> > messagesMap;
        QMap<qint64, QString> lastMessageByPeer;

        for (int pos = 0; pos <= rawData.size() - 16; ++pos) {
            uint32_t cons = *reinterpret_cast<const uint32_t*>(rawData.constData() + pos);

            if (cons == 0xb1b8cc83 || cons == 0x83314fca || cons == 0x31774388) { // user
                TL::TLBuffer uBuf(rawData.mid(pos + 4));
                int32_t uFlags = 0;
                uBuf.readInt32(uFlags);
                if (cons == 0xb1b8cc83 || cons == 0x83314fca) {
                    int32_t uFlags2 = 0;
                    uBuf.readInt32(uFlags2);
                }
                int64_t uId = 0;
                uBuf.readInt64(uId);
                int64_t aHash = 0;
                if (uFlags & (1 << 0)) {
                    uBuf.readInt64(aHash);
                }
                QString fName, lName, uName, phone;
                if (uFlags & (1 << 1)) uBuf.readString(fName);
                if (uFlags & (1 << 2)) uBuf.readString(lName);
                if (uFlags & (1 << 3)) uBuf.readString(uName);
                if (uFlags & (1 << 4)) uBuf.readString(phone);

                if (uFlags & (1 << 5)) { // photo:UserProfilePhoto
                    uint32_t pCons = 0;
                    if (uBuf.readUInt32(pCons) && (pCons == TL::ID_USER_PROFILE_PHOTO || pCons == 0xd559d318)) {
                        int32_t pFlags = 0;
                        int64_t photoId = 0;
                        if (uBuf.readInt32(pFlags) && uBuf.readInt64(photoId) && photoId != 0) {
                            m_entityPhotoIds[uId] = photoId;
                            QByteArray stripped;
                            if (pFlags & (1 << 1)) {
                                uBuf.readBytes(stripped);
                            }
                            int32_t dcId = 0;
                            uBuf.readInt32(dcId);
                            emit logMessage(QString("   [USER PHOTO] %1: photoId %2, dcId %3, stripped: %4 bytes (hex: %5)")
                                            .arg(uId).arg(photoId).arg(dcId).arg(stripped.size()).arg(QString(stripped.left(16).toHex())));
                            if (!stripped.isEmpty()) {
                                Storage::MediaCache::instance()->saveAvatar(uId, stripped);
                            }
                        }
                    }
                }

                QString title = fName + (lName.isEmpty() ? "" : " " + lName);
                if (title.trimmed().isEmpty()) title = uName.isEmpty() ? phone : "@" + uName;
                if (!title.isEmpty() && uId != 0) {
                    entityNames[uId] = title;
                    entityAccessHashes[uId] = static_cast<quint64>(aHash);
                    entityUsernames[uId] = uName;
                    emit logMessage(QString("[USER] %1: '%2' (@%3)").arg(uId).arg(title).arg(uName));
                }
            } else if (cons == 0xd49f34c6 || cons == 0xfe4478bd || cons == 0x1c32b11c || cons == 0x83d3b767) { // channel
                TL::TLBuffer cBuf(rawData.mid(pos + 4));
                int32_t cFlags = 0;
                cBuf.readInt32(cFlags);
                if (cons == 0xd49f34c6 || cons == 0xfe4478bd) {
                    int32_t cFlags2 = 0;
                    cBuf.readInt32(cFlags2);
                }
                int64_t cId = 0;
                cBuf.readInt64(cId);
                int64_t aHash = 0;
                if (cFlags & (1 << 13)) {
                    cBuf.readInt64(aHash);
                }
                QString title, uName;
                cBuf.readString(title);
                if (cFlags & (1 << 6)) {
                    cBuf.readString(uName);
                }

                // Determine whether the current user can send messages to this
                // channel. Not kicked (bit 1) and not left (bit 2) plus
                // megagroup membership (bit 6) grants posting rights; broadcast
                // channels are read-only for non-admin/editor participants.
                bool canPost = !(cFlags & (1 << 1)) && !(cFlags & (1 << 2));
                if (canPost) {
                    bool isBroadcast = (cFlags & (1 << 4)) != 0;
                    bool isMegagroup = (cFlags & (1 << 6)) != 0;
                    bool isCreator    = (cFlags & (1 << 0)) != 0;
                    bool isEditor     = (cFlags & (1 << 3)) != 0;
                    if (isBroadcast && !isMegagroup && !isCreator && !isEditor) {
                        canPost = false;
                    }
                }
                if (cId != 0) {
                    m_entityCanSend[cId] = canPost;
                }

                uint32_t pCons = 0;
                if (cBuf.readUInt32(pCons) && (pCons == TL::ID_CHAT_PHOTO || pCons == 0x475cdbd5)) {
                    int32_t pFlags = 0;
                    int64_t photoId = 0;
                    if (cBuf.readInt32(pFlags) && cBuf.readInt64(photoId) && photoId != 0) {
                        m_entityPhotoIds[cId] = photoId;
                        QByteArray stripped;
                        if (pFlags & (1 << 1)) {
                            cBuf.readBytes(stripped);
                        }
                        int32_t dcId = 0;
                        cBuf.readInt32(dcId);
                        emit logMessage(QString("   [CHANNEL PHOTO] %1: photoId %2, dcId %3, stripped: %4 bytes (hex: %5)")
                                        .arg(cId).arg(photoId).arg(dcId).arg(stripped.size()).arg(QString(stripped.left(16).toHex())));
                        if (!stripped.isEmpty()) {
                            Storage::MediaCache::instance()->saveAvatar(cId, stripped);
                        }
                    }
                }

                if (!title.isEmpty() && cId != 0) {
                    entityNames[cId] = title;
                    entityAccessHashes[cId] = static_cast<quint64>(aHash);
                    entityUsernames[cId] = uName;
                    emit logMessage(QString("[CHANNEL] %1: '%2' (@%3)").arg(cId).arg(title).arg(uName));
                }
            } else if (cons == 0xd91cdd54 || cons == 0x41cbf256) { // chat
                TL::TLBuffer chBuf(rawData.mid(pos + 4));
                int32_t chFlags;
                int64_t chId;
                if (chBuf.readInt32(chFlags) && chBuf.readInt64(chId)) {
                    QString title;
                    chBuf.readString(title);
                    if (!title.isEmpty() && chId != 0) {
                        entityNames[chId] = title;
                    }
                }
            } else if (cons == TL::ID_MESSAGE) { // 0x3ae56482
                TL::TLBuffer mBuf(rawData.mid(pos + 4));
                int32_t mFlags, mId;
                if (mBuf.readInt32(mFlags) && mBuf.readInt32(mId)) {
                    if (mFlags & (1 << 8)) {
                        uint32_t pCons; mBuf.readUInt32(pCons);
                        int64_t fid; mBuf.readInt64(fid);
                    }
                    uint32_t peerCons; mBuf.readUInt32(peerCons);
                    int64_t pid; mBuf.readInt64(pid);

                    int32_t date = 0;
                    QString text;
                    if (!(mFlags & (1 << 2)) && !(mFlags & (1 << 11)) && !(mFlags & (1 << 3))) {
                        mBuf.readInt32(date);
                        mBuf.readString(text);
                        if (!text.isEmpty()) {
                            messagesMap[mId] = qMakePair(text, date);
                            if (pid != 0) {
                                lastMessageByPeer[pid] = text;
                            }
                        }
                    }
                }
            }
        }

        // Build list of DialogItem QVariantMaps
        m_entityAccessHashes = entityAccessHashes;
        for (int k = 0; k < dialogEntries.size(); ++k) {
            m_entityPeerTypes[dialogEntries[k].peerId] = dialogEntries[k].peerType;
        }

        QList<QVariantMap> dialogList;
        QSet<qint64> seenPeers;
        for (int i = 0; i < dialogEntries.size(); ++i) {
            const SessionDialogEntry& de = dialogEntries[i];
            if (seenPeers.contains(de.peerId)) continue;
            seenPeers.insert(de.peerId);

            Models::DialogItem item;
            item.peerId = de.peerId;
            item.peerType = static_cast<Models::PeerType>(de.peerType);
            item.unreadCount = de.unreadCount;
            item.isPinned = de.isPinned;

            item.title = entityNames.value(de.peerId);
            if (item.title.isEmpty()) {
                if (de.peerId == 777000) {
                    item.title = "Telegram";
                } else if (de.peerType == Models::PEER_CHANNEL) {
                    item.title = QString("Channel %1").arg(de.peerId);
                } else if (de.peerType == Models::PEER_CHAT) {
                    item.title = QString("Group %1").arg(de.peerId);
                } else {
                    item.title = QString("User %1").arg(de.peerId);
                }
            }
            item.username = entityUsernames.value(de.peerId);
            item.accessHash = entityAccessHashes.value(de.peerId, 0);

            if (messagesMap.contains(de.topMessage)) {
                item.lastMessage = messagesMap.value(de.topMessage).first;
                item.date = messagesMap.value(de.topMessage).second;
            } else if (lastMessageByPeer.contains(de.peerId)) {
                item.lastMessage = lastMessageByPeer.value(de.peerId);
                item.date = QDateTime::currentDateTime().toTime_t();
            } else {
                item.lastMessage = "";
                item.date = QDateTime::currentDateTime().toTime_t();
            }

            item.formattedTime = Models::DialogItem::formatTimestamp(item.date);
            item.initials = Models::DialogItem::computeInitials(item.title);
            item.avatarColor = Models::DialogItem::computeAvatarColor(item.peerId);

            if (Storage::MediaCache::instance()->hasAvatar(item.peerId)) {
                item.avatarPath = Storage::MediaCache::instance()->avatarPath(item.peerId);
            }

            dialogList.append(item.toMap());
            emit logMessage(QString("Dialog [%1]: %2 (Unread: %3, Pinned: %4, Avatar: %5)")
                            .arg(i + 1).arg(item.title).arg(item.unreadCount).arg(item.isPinned ? "YES" : "NO").arg(item.avatarPath.isEmpty() ? "NO" : "YES"));
        }

        emit dialogsReceived(dialogList);

        // Background download avatars for visible dialogs
        // First 10 get full-size (big=true), rest get small to avoid flooding
        for (int i = 0; i < qMin(30, dialogEntries.size()); ++i) {
            qint64 pId = dialogEntries[i].peerId;
            if (m_entityPhotoIds.contains(pId)) {
                bool wantBig = (i < 10);
                sendUploadGetPeerPhoto(pId, dialogEntries[i].peerType, entityAccessHashes.value(pId, 0), m_entityPhotoIds.value(pId), wantBig);
            }
        }
    } else if (innerRpcConstructor == TL::ID_MESSAGES_MESSAGES ||
               innerRpcConstructor == TL::ID_MESSAGES_MESSAGES_SLICE ||
               innerRpcConstructor == TL::ID_MESSAGES_CHANNEL_MESSAGES) {
        emit logMessage(QString("MESSAGE HISTORY RECEIVED (constructor: 0x%1)").arg(innerRpcConstructor, 8, 16, QChar('0')));

        const QByteArray& rawData = plainBuf.buffer();
        QList<QVariantMap> messagesList;
        QSet<int32_t> seenIds;

        // Extract messages from payload. For each matched Message constructor we
        // first attempt a strict TL walk of the leading fields (correct id, date
        // and message text); if that fails we fall back to the legacy heuristic.
        for (int pos = 0; pos <= rawData.size() - 20; ++pos) {
            uint32_t cons = *reinterpret_cast<const uint32_t*>(rawData.constData() + pos);
            bool isMsgCons = (cons == 0x38116eed || cons == 0x761450c3 || cons == 0x94345242 ||
                              cons == 0x835014c3 || cons == 0x77045b37 || cons == 0x55dd8ae8 ||
                              cons == TL::ID_MESSAGE || cons == TL::ID_MESSAGE_SERVICE);
            if (!isMsgCons) continue;

            int32_t msgId = 0, msgDate = 0;
            bool isOut = false, hasMedia = false;
            QString text;
            int consumed = 0;
            int32_t replyToMsgId = 0;
            QString replyQuoteText;

            TL::TLBuffer stBuf(rawData.mid(pos));
            int leadRes = tlReadMessageLeading(stBuf, msgId, msgDate, isOut, hasMedia, text, consumed, replyToMsgId, replyQuoteText);
            // leadRes: 0 = full walk OK; -2 = fixed id captured but the later
            // flag-gated walk could not be completed; -1 = not a message/unsupported.
            bool idKnown = (leadRes == 0 || leadRes == -2);

            if (leadRes != 0) {
                // Fallback: locate a plausible UTF-8 string for the text, but
                // keep a correctly-captured id from the strict read when we have
                // one (media/reply/fwd-heavy messages still recover their id).
                int msgStart = pos;
                bool found = false;
                for (int sPos = pos + 16; sPos <= qMin(pos + 240, rawData.size() - 4); ++sPos) {
                    quint8 len = static_cast<quint8>(rawData.at(sPos));
                    if (len == 0 || len >= 120 || (sPos + 1 + len > rawData.size())) continue;
                    QByteArray textBytes = rawData.mid(sPos + 1, len);
                    bool ok = true;
                    for (int k = 0; k < textBytes.size(); ++k) {
                        quint8 ch = static_cast<quint8>(textBytes.at(k));
                        if (ch < 32 && ch != '\n' && ch != '\r' && ch != '\t') { ok = false; break; }
                    }
                    if (ok && textBytes.trimmed().length() > 0) {
                        text = QString::fromUtf8(textBytes.constData(), textBytes.size());
                        if (!idKnown) msgDate = QDateTime::currentDateTime().toTime_t();
                        pos = sPos + len;
                        found = true;
                        break;
                    }
                }
                if (!found && !idKnown) continue;
                if (!idKnown) {
                    // Recover id + flags via the fixed-offset read path so the
                    // heuristic does not confuse flags2 with the id.
                    TL::TLBuffer iBuf(rawData.mid(msgStart));
                    int32_t f0 = 0, f1 = 0, newId = 0;
                    if (iBuf.readInt32(f0)) {
                        isOut = (f0 & (1 << 1)) != 0;
                        if (cons == 0x94345242) { // message: flags, flags2, then id
                            if (iBuf.readInt32(f1) && iBuf.readInt32(newId)) msgId = newId;
                        } else if (iBuf.readInt32(newId)) { // service: flags, then id
                            msgId = newId;
                        }
                    }
                }
                if (msgId != 0 && seenIds.contains(msgId)) continue;
            } else {
                if (seenIds.contains(msgId)) continue; // duplicate scan hit
            }

            seenIds.insert(msgId);

            QVariantMap msgMap;
            msgMap["id"] = msgId;
            msgMap["text"] = text;
            msgMap["isOutgoing"] = isOut;
            msgMap["replyToMsgId"] = replyToMsgId;
            if (!replyQuoteText.isEmpty()) {
                msgMap["replySnippet"] = replyQuoteText;
            }
            msgMap["date"] = msgDate ? msgDate : QDateTime::currentDateTime().toTime_t();
            msgMap["dateStr"] = QDateTime::fromTime_t(
                    msgDate ? msgDate : QDateTime::currentDateTime().toTime_t()).toString("MMMM d");
            msgMap["formattedTime"] = QDateTime::fromTime_t(
                    msgDate ? msgDate : QDateTime::currentDateTime().toTime_t()).toString("h:mm AP");

            // Check for photo media inside this message's byte span.
            QString mediaPath;
            int photoScanLimit = qMin(pos + 600, rawData.size() - 24);
            for (int pScan = pos + 4; pScan < photoScanLimit; ++pScan) {
                uint32_t pCons = *reinterpret_cast<const uint32_t*>(rawData.constData() + pScan);
                if (pCons == 0xe0b0bc2e) { // photoStrippedSize
                    TL::TLBuffer sBuf(rawData.mid(pScan + 4));
                    QString sType;
                    QByteArray strippedBytes;
                    if (sBuf.readString(sType) && sBuf.readBytes(strippedBytes) && !strippedBytes.isEmpty()) {
                        qint64 photoId = static_cast<qint64>(msgId);
                        mediaPath = Storage::MediaCache::instance()->savePhoto(photoId, strippedBytes);
                        emit logMessage(QString("   [PHOTO MEDIA] msgId %1: extracted %2 bytes stripped photo")
                                        .arg(msgId).arg(strippedBytes.size()));
                        break;
                    }
                }
            }
            if (!mediaPath.isEmpty()) {
                msgMap["mediaPath"] = mediaPath;
            }

            messagesList.append(msgMap);
            emit logMessage(QString("[HISTORY MSG] id %1: '%2' (out: %3, media: %4)")
                            .arg(msgId).arg(text).arg(isOut ? "YES" : "NO").arg(mediaPath.isEmpty() ? "NO" : "YES"));

            // Move scanning cursor forward so we don't re-match this message.
            if (leadRes == 0) {
                pos = pos + qMax(consumed, 4) - 1;
            }
        }

        emit logMessage(QString("Extracted %1 messages from history payload").arg(messagesList.size()));
        emit historyReceived(0, messagesList);
    } else if (innerRpcConstructor == TL::ID_UPLOAD_FILE) {
        uint32_t fileType = 0;
        int32_t mtime = 0;
        QByteArray fileBytes;
        plainBuf.readUInt32(fileType);
        plainBuf.readInt32(mtime);
        plainBuf.readBytes(fileBytes);

        emit logMessage(QString("UPLOAD FILE RECEIVED: ReqMsgId %1, type: 0x%2, size: %3 bytes")
                        .arg(reqMsgId).arg(fileType, 8, 16, QChar('0')).arg(fileBytes.size()));

        if (m_pendingPhotoRequests.contains(reqMsgId)) {
            qint64 peerId = m_pendingPhotoRequests.take(reqMsgId);
            Storage::MediaCache::instance()->saveAvatar(peerId, fileBytes);
            emit peerPhotoReceived(peerId, fileBytes);
        }

        emit fileReceived(reqMsgId, fileBytes);
    } else if (innerRpcConstructor == TL::ID_USERS_USER_FULL || innerRpcConstructor == 0x3b6d152e ||
               innerRpcConstructor == 0x3b02414e || innerRpcConstructor == 0xef464d26 || innerRpcConstructor == 0x1f440409) {
        emit logMessage(QString("=================================================="));
        emit logMessage(QString("LIVE TELEGRAM USER_FULL RECEIVED (0x%1)").arg(innerRpcConstructor, 8, 16, QChar('0')));
        emit logMessage(QString("=================================================="));

        const QByteArray& rawData = plainBuf.buffer();
        qint64 userId = 0;
        QString bio;
        QString username;
        QString phone;

        // 1. Scan for user constructor in payload
        for (int pos = 0; pos <= rawData.size() - 16; ++pos) {
            uint32_t cons = *reinterpret_cast<const uint32_t*>(rawData.constData() + pos);
            if (cons == 0xb1b8cc83 || cons == 0x83314fca || cons == 0x31774388 || cons == 0x2e5fb3b0) { // user
                TL::TLBuffer uBuf(rawData.mid(pos + 4));
                int32_t uFlags = 0;
                uBuf.readInt32(uFlags);
                if (cons == 0xb1b8cc83 || cons == 0x83314fca) {
                    int32_t uFlags2 = 0;
                    uBuf.readInt32(uFlags2);
                }
                int64_t uId = 0;
                uBuf.readInt64(uId);
                if (uId != 0 && userId == 0) userId = uId;
                int64_t aHash = 0;
                if (uFlags & (1 << 0)) uBuf.readInt64(aHash);
                QString fName, lName, uName, ph;
                if (uFlags & (1 << 1)) uBuf.readString(fName);
                if (uFlags & (1 << 2)) uBuf.readString(lName);
                if (uFlags & (1 << 3)) { uBuf.readString(uName); if (!uName.isEmpty()) username = uName; }
                if (uFlags & (1 << 4)) { uBuf.readString(ph); if (!ph.isEmpty()) phone = ph; }
            }
        }

        // 2. Scan for userFull structure: userFull#a02bc13e flags:# flags2:# id:long about:flags.1?string
        for (int pos = 0; pos <= rawData.size() - 20; ++pos) {
            uint32_t c = *reinterpret_cast<const uint32_t*>(rawData.constData() + pos);
            if (c == TL::ID_USER_FULL || c == 0xa02bc13e || c == 0x3b02414e || c == 0xef464d26) {
                TL::TLBuffer fullBuf(rawData.mid(pos + 4));
                int32_t ufFlags = 0;
                if (fullBuf.readInt32(ufFlags)) {
                    int32_t ufFlags2 = 0;
                    fullBuf.readInt32(ufFlags2);
                    int64_t ufId = 0;
                    fullBuf.readInt64(ufId);
                    if (ufId != 0 && userId == 0) userId = ufId;
                    if (ufFlags & (1 << 1)) { // bit 1: about
                        QString aboutText;
                        fullBuf.readString(aboutText);
                        if (!aboutText.isEmpty()) bio = aboutText;
                    }
                }
            }
        }

        emit logMessage(QString("[USER_FULL] userId: %1, bio: '%2', username: @%3, phone: %4")
                        .arg(userId).arg(bio).arg(username).arg(phone));
        emit userFullReceived(userId, bio, username, phone);
        emit myProfileReceived(bio, username, phone);
    } else if (innerRpcConstructor == TL::ID_MESSAGES_CHATS || innerRpcConstructor == TL::ID_MESSAGES_CHATS_SLICE ||
               innerRpcConstructor == 0x64ff9fd5 || innerRpcConstructor == 0x9cd81144 || innerRpcConstructor == 0x9c3e200b) {
        emit logMessage(QString("=================================================="));
        emit logMessage(QString("LIVE TELEGRAM COMMON CHATS RECEIVED (0x%1)").arg(innerRpcConstructor, 8, 16, QChar('0')));
        emit logMessage(QString("=================================================="));

        const QByteArray& rawData = plainBuf.buffer();
        QList<QVariantMap> chats;

        for (int pos = 0; pos <= rawData.size() - 24; ++pos) {
            uint32_t cons = *reinterpret_cast<const uint32_t*>(rawData.constData() + pos);
            if (cons == 0xd49f34c6 || cons == 0xfe4478bd || cons == 0x1c32b11c || cons == 0x83d3b767) { // channel
                TL::TLBuffer cBuf(rawData.mid(pos + 4));
                int32_t cFlags = 0; cBuf.readInt32(cFlags);
                if (cons == 0xd49f34c6 || cons == 0xfe4478bd) { int32_t cFlags2 = 0; cBuf.readInt32(cFlags2); }
                int64_t cId = 0; cBuf.readInt64(cId);
                int64_t aHash = 0; if (cFlags & (1 << 13)) cBuf.readInt64(aHash);
                QString title, uName;
                cBuf.readString(title);
                if (cFlags & (1 << 6)) cBuf.readString(uName);

                int32_t participantsCount = 0;
                if (cFlags & (1 << 17)) {
                    cBuf.readInt32(participantsCount);
                }

                if (!title.isEmpty() && cId != 0) {
                    QVariantMap cMap;
                    cMap["id"] = QString::number(cId);
                    cMap["title"] = title;
                    cMap["username"] = uName;
                    cMap["initials"] = Models::DialogItem::computeInitials(title);
                    cMap["avatarColor"] = Models::DialogItem::computeAvatarColor(cId);
                    cMap["membersText"] = participantsCount > 0 ? QString("%1 members").arg(participantsCount) : "member";
                    chats.append(cMap);
                }
            } else if (cons == 0xd91cdd54 || cons == 0x41cbf256) { // chat
                TL::TLBuffer chBuf(rawData.mid(pos + 4));
                int32_t chFlags; int64_t chId;
                if (chBuf.readInt32(chFlags) && chBuf.readInt64(chId)) {
                    QString title; chBuf.readString(title);
                    int32_t count = 0; chBuf.readInt32(count);
                    if (!title.isEmpty() && chId != 0) {
                        QVariantMap cMap;
                        cMap["id"] = QString::number(chId);
                        cMap["title"] = title;
                        cMap["initials"] = Models::DialogItem::computeInitials(title);
                        cMap["avatarColor"] = Models::DialogItem::computeAvatarColor(chId);
                        cMap["membersText"] = count > 0 ? QString("%1 members").arg(count) : "member";
                        chats.append(cMap);
                    }
                }
            }
        }

        emit logMessage(QString("Total common chats parsed: %1").arg(chats.size()));
        emit commonChatsReceived(0, chats);
    }
}

void MTProtoSession::sendMessagesGetHistory(int peerType, qint64 peerId, quint64 accessHash, int offsetId, int limit) {
    if (peerId == 0) {
        emit logMessage("[WARN] Cannot request message history for peer 0!");
        return;
    }
    if (accessHash == 0 && m_entityAccessHashes.contains(peerId)) {
        accessHash = m_entityAccessHashes.value(peerId);
    }
    if (peerType == 0 && m_entityPeerTypes.contains(peerId)) {
        peerType = m_entityPeerTypes.value(peerId);
    }

    emit logMessage(QString("Requesting message history for peer %1 (type: %2, accessHash: 0x%3, limit: %4)...")
                    .arg(peerId).arg(peerType).arg(accessHash, 0, 16).arg(limit));
    TL::TLBuffer buf;

    buf.writeUInt32(0xda9b0d0d); // invokeWithLayer#da9b0d0d
    buf.writeInt32(195);        // layer 195

    // messages.getHistory#4423e6c5 peer:InputPeer offset_id:int offset_date:int add_offset:int limit:int max_id:int min_id:int hash:long = messages.Messages;
    buf.writeUInt32(TL::ID_MESSAGES_GET_HISTORY);

    // InputPeer
    if (peerType == Models::PEER_CHANNEL) {
        buf.writeUInt32(TL::ID_INPUT_PEER_CHANNEL);
        buf.writeInt64(peerId);
        buf.writeInt64(static_cast<int64_t>(accessHash));
    } else if (peerType == Models::PEER_CHAT) {
        buf.writeUInt32(TL::ID_INPUT_PEER_CHAT);
        buf.writeInt64(peerId);
    } else {
        buf.writeUInt32(TL::ID_INPUT_PEER_USER);
        buf.writeInt64(peerId);
        buf.writeInt64(static_cast<int64_t>(accessHash));
    }

    buf.writeInt32(offsetId);
    buf.writeInt32(0); // offset_date
    buf.writeInt32(0); // add_offset
    buf.writeInt32(limit);
    buf.writeInt32(0); // max_id
    buf.writeInt32(0); // min_id
    buf.writeInt64(0); // hash: long

    sendEncryptedMessage(buf.buffer(), true);
}

void MTProtoSession::sendMessagesSendMessage(int peerType, qint64 peerId, quint64 accessHash, const QString& message) {
    if (peerId == 0) {
        emit logMessage("[WARN] Cannot send message to peer 0!");
        return;
    }
    if (accessHash == 0 && m_entityAccessHashes.contains(peerId)) {
        accessHash = m_entityAccessHashes.value(peerId);
    }
    if (peerType == 0 && m_entityPeerTypes.contains(peerId)) {
        peerType = m_entityPeerTypes.value(peerId);
    }

    emit logMessage(QString("Sending message to peer %1: '%2'").arg(peerId).arg(message));
    TL::TLBuffer buf;

    buf.writeUInt32(0xda9b0d0d); // invokeWithLayer#da9b0d0d
    buf.writeInt32(195);        // layer 195

    // messages.sendMessage#0983f972 flags:# peer:InputPeer message:string random_id:long ...
    buf.writeUInt32(TL::ID_MESSAGES_SEND_MESSAGE);
    buf.writeInt32(0); // flags = 0

    // InputPeer
    if (peerType == Models::PEER_CHANNEL) {
        buf.writeUInt32(TL::ID_INPUT_PEER_CHANNEL);
        buf.writeInt64(peerId);
        buf.writeInt64(static_cast<int64_t>(accessHash));
    } else if (peerType == Models::PEER_CHAT) {
        buf.writeUInt32(TL::ID_INPUT_PEER_CHAT);
        buf.writeInt64(peerId);
    } else {
        buf.writeUInt32(TL::ID_INPUT_PEER_USER);
        buf.writeInt64(peerId);
        buf.writeInt64(static_cast<int64_t>(accessHash));
    }

    buf.writeString(message);

    QByteArray rnd = Crypto::CryptoEngine::randomBytes(8);
    int64_t randomId = *reinterpret_cast<const int64_t*>(rnd.constData());
    buf.writeInt64(randomId);

    sendEncryptedMessage(buf.buffer(), true);
}

void MTProtoSession::sendUsersGetFullUser(qint64 userId, quint64 accessHash) {
    if (userId == 0) return;
    if (accessHash == 0 && m_entityAccessHashes.contains(userId)) {
        accessHash = m_entityAccessHashes.value(userId);
    }

    emit logMessage(QString("Requesting live Telegram full user for %1 (accessHash: 0x%2)...")
                    .arg(userId).arg(accessHash, 0, 16));

    TL::TLBuffer buf;
    buf.writeUInt32(0xda9b0d0d); // invokeWithLayer#da9b0d0d
    buf.writeInt32(195);        // layer 195

    // users.getFullUser#b60dc69b id:InputUser = users.UserFull;
    buf.writeUInt32(TL::ID_USERS_GET_FULL_USER);

    // inputUser#f21158c9 user_id:long access_hash:long = InputUser;
    buf.writeUInt32(TL::ID_INPUT_USER);
    buf.writeInt64(userId);
    buf.writeInt64(static_cast<int64_t>(accessHash));

    sendEncryptedMessage(buf.buffer(), true);
}

void MTProtoSession::sendUsersGetMyFull() {
    emit logMessage("Requesting live Telegram full user for current logged-in user (inputUserSelf)...");

    TL::TLBuffer buf;
    buf.writeUInt32(0xda9b0d0d); // invokeWithLayer#da9b0d0d
    buf.writeInt32(195);        // layer 195

    // users.getFullUser#b60f5918 id:InputUser = users.UserFull;
    buf.writeUInt32(TL::ID_USERS_GET_FULL_USER);

    // inputUserSelf#f7c1b13f = InputUser;
    buf.writeUInt32(TL::ID_INPUT_USER_SELF);

    sendEncryptedMessage(buf.buffer(), true);
}

void MTProtoSession::sendMessagesGetCommonChats(qint64 userId, quint64 accessHash, int limit) {
    if (userId == 0) return;
    if (accessHash == 0 && m_entityAccessHashes.contains(userId)) {
        accessHash = m_entityAccessHashes.value(userId);
    }

    emit logMessage(QString("Requesting live Telegram common chats for user %1 (limit: %2)...")
                    .arg(userId).arg(limit));

    TL::TLBuffer buf;
    buf.writeUInt32(0xda9b0d0d); // invokeWithLayer#da9b0d0d
    buf.writeInt32(195);        // layer 195

    // messages.getCommonChats#e0800be2 user_id:InputUser max_id:long limit:int = messages.Chats;
    buf.writeUInt32(TL::ID_MESSAGES_GET_COMMON_CHATS);

    buf.writeUInt32(TL::ID_INPUT_USER);
    buf.writeInt64(userId);
    buf.writeInt64(static_cast<int64_t>(accessHash));

    buf.writeInt64(0); // max_id: 0
    buf.writeInt32(limit);

    sendEncryptedMessage(buf.buffer(), true);
}

void MTProtoSession::sendUploadGetPeerPhoto(qint64 peerId, int peerType, quint64 accessHash, qint64 photoId, bool big) {
    if (peerId == 0) return;
    if (accessHash == 0 && m_entityAccessHashes.contains(peerId)) {
        accessHash = m_entityAccessHashes.value(peerId);
    }
    if (peerType == 0 && m_entityPeerTypes.contains(peerId)) {
        peerType = m_entityPeerTypes.value(peerId);
    }

    emit logMessage(QString("Requesting peer photo for %1 (type: %2, photoId: %3, big: %4)...")
                    .arg(peerId).arg(peerType).arg(photoId).arg(big ? "YES" : "NO"));

    TL::TLBuffer buf;
    buf.writeUInt32(0xda9b0d0d); // invokeWithLayer#da9b0d0d
    buf.writeInt32(195);        // layer 195

    // upload.getFile#be53356a flags:# precise:flags.0?true cdn_supported:flags.1?true location:InputFileLocation offset:long limit:int = upload.File;
    buf.writeUInt32(TL::ID_UPLOAD_GET_FILE);
    buf.writeInt32(0); // flags = 0

    // inputPeerPhotoFileLocation#37257e9f flags:# big:flags.0?true peer:InputPeer photo_id:long = InputFileLocation;
    buf.writeUInt32(TL::ID_INPUT_PEER_PHOTO_FILE_LOCATION);
    buf.writeInt32(big ? 1 : 0); // flags (bit 0: big)

    if (peerType == Models::PEER_CHANNEL) {
        buf.writeUInt32(TL::ID_INPUT_PEER_CHANNEL);
        buf.writeInt64(peerId);
        buf.writeInt64(static_cast<int64_t>(accessHash));
    } else if (peerType == Models::PEER_CHAT) {
        buf.writeUInt32(TL::ID_INPUT_PEER_CHAT);
        buf.writeInt64(peerId);
    } else {
        buf.writeUInt32(TL::ID_INPUT_PEER_USER);
        buf.writeInt64(peerId);
        buf.writeInt64(static_cast<int64_t>(accessHash));
    }

    buf.writeInt64(photoId); // photo_id: long
    buf.writeInt64(0);       // offset: long (0)
    buf.writeInt32(131072);  // limit: int (128 KB)

    sendEncryptedMessage(buf.buffer(), true);
    m_pendingPhotoRequests[m_lastMsgId] = peerId;
}

void MTProtoSession::sendUploadGetFile(qint64 fileId, quint64 accessHash, const QByteArray& fileReference, int offset, int limit) {
    emit logMessage(QString("Requesting document/photo file chunk %1 (offset: %2, limit: %3)...")
                    .arg(fileId).arg(offset).arg(limit));

    TL::TLBuffer buf;
    buf.writeUInt32(0xda9b0d0d); // invokeWithLayer#da9b0d0d
    buf.writeInt32(195);        // layer 195

    buf.writeUInt32(TL::ID_UPLOAD_GET_FILE);
    buf.writeInt32(0); // flags = 0

    // inputDocumentFileLocation#bad07584 id:long access_hash:long file_reference:bytes thumb_size:string = InputFileLocation;
    buf.writeUInt32(TL::ID_INPUT_DOCUMENT_FILE_LOCATION);
    buf.writeInt64(fileId);
    buf.writeInt64(static_cast<int64_t>(accessHash));
    buf.writeBytes(fileReference);
    buf.writeString(""); // thumb_size

    buf.writeInt64(offset);
    buf.writeInt32(limit);

    sendEncryptedMessage(buf.buffer(), true);
}

// --------------------------------------------------------------------------
// TL parsing helpers for verbose Message / Media object walking (layer 195)
// --------------------------------------------------------------------------
namespace {

// Reads a TL-string (length-prefixed bytes) and returns its length in bytes
// consumed (including the 4-byte alignment padding). Returns -1 on overflow.
int tlSkipString(TL::TLBuffer& b) {
    size_t start = b.offset();
    if (b.remaining() < 1) return -1;
    quint8 lead = (quint8)b.buffer().at((int)b.offset());
    int len = -1;
    size_t header = 1;
    if (lead == 0xfe) {
        if (b.remaining() < 5) return -1;
        b.setOffset(b.offset() + 1);
        int32_t l = 0;
        if (!b.readInt32(l)) return -1;
        len = l;
        header = 5;
    } else if (lead > 253) {
        return -1;
    } else {
        len = lead;
        b.setOffset(b.offset() + 1);
    }
    if (len < 0 || len > 16 * 1024 * 1024) { b.setOffset(start); return -1; }
    size_t total = ((size_t)len + header + 3) & ~3u; // round the whole field to 4
    if (b.remaining() < total) { b.setOffset(start); return -1; }
    b.setOffset(start + total);
    return static_cast<int>(b.offset() - start);
}

// Reads a TL-string and stores decoded UTF-8 in `out`. Returns bytes consumed
// on success, -1 on failure.
int tlReadString(TL::TLBuffer& b, QString& out) {
    size_t start = b.offset();
    if (b.remaining() < 1) return -1;
    quint8 lead = (quint8)b.buffer().at((int)b.offset());
    int len = -1;
    if (lead == 0xfe) {
        if (b.remaining() < 5) return -1;
        b.setOffset(b.offset() + 1);
        int32_t l = 0;
        if (!b.readInt32(l)) return -1;
        len = l;
        } else if (lead > 253) {
        return -1;
    } else {
        len = lead;
        b.setOffset(b.offset() + 1);
    }
    if (len < 0 || len > 16 * 1024 * 1024) { b.setOffset(start); return -1; }
    if (b.remaining() < (size_t)len) { b.setOffset(start); return -1; }
    QByteArray raw = b.buffer().mid((int)b.offset(), len);
    b.setOffset(b.offset() + len);
    size_t pad = (4 - ((b.offset() - start) % 4)) % 4;
    if (b.remaining() < pad) { b.setOffset(start); return -1; }
    b.setOffset(b.offset() + pad);
    out = QString::fromUtf8(raw);
    return static_cast<int>(b.offset() - start);
}

// Reads a TL `bytes` field raw length. Returns bytes consumed, -1 on failure.
int tlSkipBytes(TL::TLBuffer& b) {
    QString tmp;
    return tlReadString(b, tmp); // bytes are serialised exactly like string
}

// Reads a Vector<T> head (cons + count) and returns the element bytes count.
// The caller must then walk `count` elements of `size` bytes each; if `size`
// is 0 it returns -1 (cannot advance without a per-element size).
int tlVectorHead(TL::TLBuffer& b, int& count) {
    uint32_t cons = 0;
    if (!b.readUInt32(cons)) return -1;
    if (cons != TL::ID_VECTOR && cons != 0x1cb5c415) return -1;
    int32_t n = 0;
    if (!b.readInt32(n)) return -1;
    if (n < 0 || n > 100000) return -1;
    count = n;
    return 0;
}

// Skips a single Peer object (peerUser/peerChat/peerChannel). Returns bytes
// consumed, or -1 on failure.
int tlSkipPeer(TL::TLBuffer& b) {
    uint32_t cons = 0;
    if (!b.readUInt32(cons)) return -1;      // advances b by 4
    bool known = (cons == TL::ID_PEER_USER || cons == TL::ID_PEER_CHAT || cons == TL::ID_PEER_CHANNEL);
    if (!known) return -1;
    int64_t id = 0;
    if (!b.readInt64(id)) return -1;          // advances b by 8
    return 12;
}

// Reads a MessageReplyHeader. Returns bytes consumed or -1 on failure.
int tlReadReplyHeader(TL::TLBuffer& b, int32_t& outReplyToId, QString& outQuoteText) {
    uint32_t cons = 0;
    if (!b.readUInt32(cons)) return -1;
    if (cons != 0xafbc09db) return -1;
    int32_t flags = 0;
    int32_t replyToMsgId = 0;
    if (!b.readInt32(flags)) return -1;
    if (!b.readInt32(replyToMsgId)) return -1;
    outReplyToId = replyToMsgId;
    if (flags & (1 << 0)) { if (tlSkipPeer(b) < 0) return -1; }
    if (flags & (1 << 1)) { int32_t t = 0; if (!b.readInt32(t)) return -1; }
    if (flags & (1 << 2)) return -1; // extended media: fall back
    if (flags & (1 << 3)) { int32_t t = 0; if (!b.readInt32(t)) return -1; }
    if (flags & (1 << 6)) {
        if (tlReadString(b, outQuoteText) < 0) return -1;
    }
    if (flags & (1 << 7)) return -1; // entities in reply: fall back
    if (flags & (1 << 8)) { int32_t t = 0; if (!b.readInt32(t)) return -1; }
    if (flags & (1 << 9)) return -1; // nested fwd in reply: fall back
    return 0;
}

// Skips a MessageReplyHeader. Returns bytes consumed or -1 on failure.
int tlSkipReplyHeader(TL::TLBuffer& b) {
    int32_t dummyId = 0;
    QString dummyQuote;
    return tlReadReplyHeader(b, dummyId, dummyQuote);
}

// Skips a MessageFwdHeader. Returns bytes consumed or -1 on failure.
int tlSkipFwdHeader(TL::TLBuffer& b) {
    uint32_t cons = 0;
    if (!b.readUInt32(cons)) return -1;
    if (cons != 0xc78722ab && cons != 0x4328579) return -1;
    int32_t flags = 0;
    if (!b.readInt32(flags)) return -1;
    // messageFwdHeader field order does NOT follow flag-bit order:
    if (flags & (1 << 0)) { if (tlSkipPeer(b) < 0) return -1; }   // from_id:Peer
    if (flags & (1 << 5)) { if (tlSkipString(b) < 0) return -1; } // from_name:string
    int32_t date = 0;
    if (!b.readInt32(date)) return -1;                             // date:int (always)
    if (flags & (1 << 2)) { int32_t t = 0; if (!b.readInt32(t)) return -1; } // channel_post:int
    if (flags & (1 << 3)) { if (tlSkipString(b) < 0) return -1; } // post_author:string
    if (flags & (1 << 4)) {
        if (tlSkipPeer(b) < 0) return -1;                          // saved_from_peer:Peer
        int32_t t = 0; if (!b.readInt32(t)) return -1;             // saved_from_msg_id:int
    }
    if (flags & (1 << 6)) { if (tlSkipString(b) < 0) return -1; } // psa_type:string
    return 0;
}

// Parses the leading fields of a `message#94345242` (and messageService#2b085862)
// up to and including `message:string`. Fills id/outDate/outIsOut/outText and
// reports whether a media flag is present. Returns 0 on success, -1 if the
// object is not walkable. `outConsumed` receives the bytes consumed.
int tlReadMessageLeading(TL::TLBuffer& b, int32_t& outId, int32_t& outDate,
                         bool& outIsOut, bool& outHasMedia, QString& outText,
                         int& outConsumed, int32_t& outReplyToId, QString& outReplyQuoteText) {
    outReplyToId = 0;
    outReplyQuoteText = QString();
    size_t start = b.offset();
    uint32_t cons = 0;
    if (!b.readUInt32(cons)) return -1;
    bool isMsg = (cons == 0x94345242);
    bool isService = (cons == TL::ID_MESSAGE_SERVICE); // 0x2b085862
    if (!isMsg && !isService) return -1;

    int32_t flags = 0;
    if (!b.readInt32(flags)) return -1;
    int32_t flags2 = 0;
    if (isMsg) {
        if (!b.readInt32(flags2)) return -1;
    }
    outIsOut = (flags & (1 << 1)) != 0;

    int32_t id = 0;
    if (!b.readInt32(id)) return -1;
    outId = id;                       // id captured reliably at fixed offset
    outHasMedia = isMsg && (flags & (1 << 9)) != 0;

    // ---- leading flag-gated fields (before date/message) ----
    if (isMsg) {
        if (flags & (1 << 8)) { if (tlSkipPeer(b) < 0) return -2; }       // from_id
        if (flags & (1 << 29)) { int32_t tb = 0; if (!b.readInt32(tb)) return -2; } // from_boosts_applied:int
        if (tlSkipPeer(b) < 0) return -2;                                 // peer_id (always)
        if (flags & (1 << 28)) { if (tlSkipPeer(b) < 0) return -2; }      // saved_peer_id
        if (flags & (1 << 2)) { if (tlSkipFwdHeader(b) < 0) return -2; }  // fwd_from
        if (flags & (1 << 11)) { int64_t tb = 0; if (!b.readInt64(tb)) return -2; } // via_bot_id:long
        if (flags2 & (1 << 0)) { int64_t tb = 0; if (!b.readInt64(tb)) return -2; } // via_business_bot_id
        if (flags & (1 << 3)) { if (tlReadReplyHeader(b, outReplyToId, outReplyQuoteText) < 0) return -2; }// reply_to
    } else {
        // messageService#2b085862
        if (flags & (1 << 8)) { if (tlSkipPeer(b) < 0) return -2; }       // from_id
        if (tlSkipPeer(b) < 0) return -2;                                 // peer_id
        if (flags & (1 << 3)) { if (tlReadReplyHeader(b, outReplyToId, outReplyQuoteText) < 0) return -2; }// reply_to
    }

    int32_t date = 0;
    if (!b.readInt32(date)) return -2;
    outDate = date;

    if (isService) {
        // messageService#2b085862 carries `action:MessageAction` (a typed
        // object), not a free-text `message` string, so there is no message
        // text to read. Return success with the id/date already captured.
        outText = QString();
        outConsumed = static_cast<int>(b.offset() - start);
        return 0;
    }

    QString text;
    if (tlReadString(b, text) < 0) return -2;   // id is still valid, walk incomplete
    outText = text;
    outConsumed = static_cast<int>(b.offset() - start);
    return 0;
}

} // namespace

} // namespace Core
} // namespace Telegram

