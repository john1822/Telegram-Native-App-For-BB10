#include "MTProtoSession.h"
#include "TcpTransport.h"
#include "CryptoEngine.h"
#include "TLBuffer.h"
#include "TLTypes.h"
#include "Config.h"

#include <QDateTime>
#include <QDebug>
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

    sendEncryptedMessage(rpcBuf.buffer(), false);
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

    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195
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
    rpcBuf.writeUInt32(TL::ID_AUTH_LOG_OUT); // 0x3e72ba14

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

// --------------------------------------------------------------------------
// MTProto 2.0 Encrypted Message Transmission & Decryption
// --------------------------------------------------------------------------
void MTProtoSession::sendEncryptedMessage(const QByteArray& messageData, bool isContentRelated) {
    TL::TLBuffer plainBuf;
    plainBuf.writeInt64(static_cast<int64_t>(m_serverSalt));
    plainBuf.writeInt64(static_cast<int64_t>(m_sessionId));
    plainBuf.writeInt64(generateMessageId());
    plainBuf.writeInt32(static_cast<int32_t>(generateSeqNo(isContentRelated)));
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
    }
}

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
    }
}

} // namespace Core
} // namespace Telegram
