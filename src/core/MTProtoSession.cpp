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
      m_authKeyId(0),
      m_serverSalt(0),
      m_sessionId(0),
      m_seqNo(0),
      m_lastMsgId(0),
      m_timeOffset(0) {
    
    connect(m_transport, SIGNAL(connected()), this, SLOT(onTransportConnected()));
    connect(m_transport, SIGNAL(disconnected()), this, SLOT(onTransportDisconnected()));
    connect(m_transport, SIGNAL(packetReceived(QByteArray)), this, SLOT(onPacketReceived(QByteArray)));
    connect(m_transport, SIGNAL(errorOccurred(QString)), this, SLOT(onTransportError(QString)));
    connect(m_transport, SIGNAL(logMessage(QString)), this, SIGNAL(logMessage(QString)));
}

MTProtoSession::~MTProtoSession() {
    stop();
}

void MTProtoSession::start(const QString& host, quint16 port) {
    m_state = STATE_CONNECTING;
    emit stateChanged(m_state, "Connecting to Telegram DC...");
    m_transport->connectToHost(host, port);
}

void MTProtoSession::stop() {
    m_transport->disconnectFromHost();
    m_state = STATE_DISCONNECTED;
    emit stateChanged(m_state, "Disconnected");
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

int64_t MTProtoSession::generateMessageId() {
    int64_t timeSec = QDateTime::currentDateTimeUtc().toTime_t() + m_timeOffset;
    int64_t timeNsec = QDateTime::currentDateTimeUtc().time().msec() * 1000000;
    int64_t msgId = (timeSec << 32) | ((timeNsec / 1000) << 2);
    if (msgId <= m_lastMsgId) {
        msgId = m_lastMsgId + 4;
    }
    m_lastMsgId = msgId;
    return msgId;
}

uint32_t MTProtoSession::generateSeqNo(bool isContentRelated) {
    uint32_t seq = m_seqNo * 2 + (isContentRelated ? 1 : 0);
    if (isContentRelated) {
        m_seqNo++;
    }
    return seq;
}

void MTProtoSession::onTransportConnected() {
    m_state = STATE_CONNECTED;
    emit stateChanged(m_state, "TCP Connected. Initiating MTProto 2.0 Handshake...");
    sendReqPQMulti();
}

void MTProtoSession::onTransportDisconnected() {
    m_state = STATE_DISCONNECTED;
    emit stateChanged(m_state, "Disconnected");
}

void MTProtoSession::onTransportError(const QString& error) {
    emit errorOccurred(error);
}

void MTProtoSession::onPacketReceived(const QByteArray& packet) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(packet.constData());
    size_t size = static_cast<size_t>(packet.size());

    if (size < 8) return;

    uint64_t authKeyId = 0;
    for (int i = 0; i < 8; ++i) {
        authKeyId |= (static_cast<uint64_t>(data[i]) << (i * 8));
    }

    if (authKeyId == 0) {
        handleUnencryptedPacket(data, size);
    } else {
        handleEncryptedPacket(data, size);
    }
}

void MTProtoSession::sendReqPQMulti() {
    m_state = STATE_HANDSHAKE_REQ_PQ;
    emit stateChanged(m_state, "Generating 128-bit Nonce & Sending req_pq_multi...");

    Crypto::CryptoEngine::generateRandomBytes(m_nonce, 16);

    TL::TLBuffer buf;
    buf.writeInt64(0);                      // auth_key_id = 0
    buf.writeInt64(generateMessageId());    // message_id
    buf.writeInt32(20);                     // message_length = 4 (constructor) + 16 (nonce)
    buf.writeUInt32(TL::ID_REQ_PQ_MULTI);   // constructor
    buf.writeInt128(m_nonce);               // nonce

    QByteArray packet(reinterpret_cast<const char*>(buf.data()), buf.size());
    m_transport->sendPacket(packet);
}

void MTProtoSession::handleUnencryptedPacket(const uint8_t* data, size_t size) {
    if (size < 20) return;

    TL::TLBuffer buf(data, size);
    int64_t authKeyId;
    int64_t msgId;
    int32_t msgLen;
    uint32_t constructor;

    buf.readInt64(authKeyId);
    buf.readInt64(msgId);
    buf.readInt32(msgLen);
    buf.readUInt32(constructor);

    switch (constructor) {
        case TL::ID_RESPQ:
            handleResPQ(buf.data() + buf.offset(), buf.remaining());
            break;
        case TL::ID_SERVER_DH_PARAMS_OK:
            handleServerDHParams(buf.data() + buf.offset(), buf.remaining());
            break;
        case TL::ID_SERVER_DH_PARAMS_FAIL:
            emit errorOccurred("Server DH params failed (server_DH_params_fail received)");
            break;
        case TL::ID_DH_GEN_OK:
        case TL::ID_DH_GEN_RETRY:
        case TL::ID_DH_GEN_FAIL:
            handleSetClientDHParamsAnswer(constructor, buf.data() + buf.offset(), buf.remaining());
            break;
        default:
            emit logMessage(QString("[WARN] Unhandled unencrypted constructor: 0x%1").arg(constructor, 8, 16, QChar('0')));
            break;
    }
}

void MTProtoSession::handleResPQ(const uint8_t* data, size_t size) {
    TL::TLBuffer buf(data, size);

    uint8_t nonce[16];
    buf.readInt128(nonce);
    if (memcmp(nonce, m_nonce, 16) != 0) {
        emit errorOccurred("Handshake Error: Server nonce does not match client nonce");
        return;
    }

    buf.readInt128(m_serverNonce);

    QByteArray pqBytes;
    buf.readBytes(pqBytes);

    uint32_t vectorConstructor;
    buf.readUInt32(vectorConstructor);
    int32_t fpCount;
    buf.readInt32(fpCount);

    QList<uint64_t> fingerprints;
    for (int i = 0; i < fpCount; ++i) {
        int64_t fp;
        buf.readInt64(fp);
        fingerprints.append(static_cast<uint64_t>(fp));
        emit logMessage(QString("Server RSA Fingerprint [%1]: 0x%2").arg(i).arg(QString::number(static_cast<uint64_t>(fp), 16)));
    }

    emit logMessage(QString("resPQ received. Factoring PQ (size: %1 bytes, %2 RSA fingerprints)...").arg(pqBytes.size()).arg(fpCount));

    // Convert PQ bytes to 64-bit int
    uint64_t pqVal = 0;
    const uint8_t* pqPtr = reinterpret_cast<const uint8_t*>(pqBytes.constData());
    for (int i = 0; i < pqBytes.size(); ++i) {
        pqVal = (pqVal << 8) | pqPtr[i];
    }

    uint32_t p = 0, q = 0;
    if (!Crypto::CryptoEngine::factorizePQ(pqVal, p, q)) {
        emit errorOccurred("Failed to factorize PQ");
        return;
    }

    emit logMessage(QString("PQ factorized successfully: P=%1, Q=%2").arg(p).arg(q));

    // Find matching RSA key by parsing official PEM keys
    QString selectedNHex, selectedEHex;
    uint64_t selectedFingerprint = 0;

    for (size_t k = 0; k < sizeof(OFFICIAL_RSA_PEMS) / sizeof(OFFICIAL_RSA_PEMS[0]); ++k) {
        QString nHex, eHex;
        uint64_t computedFp = 0;
        if (Crypto::CryptoEngine::loadPemPublicKey(OFFICIAL_RSA_PEMS[k], nHex, eHex, computedFp)) {
            emit logMessage(QString("Official PEM Key [%1] parsed fingerprint: 0x%2").arg(k).arg(QString::number(computedFp, 16)));
            for (int f = 0; f < fingerprints.size(); ++f) {
                if (computedFp == fingerprints[f]) {
                    selectedNHex = nHex;
                    selectedEHex = eHex;
                    selectedFingerprint = computedFp;
                    break;
                }
            }
        }
        if (selectedFingerprint != 0) break;
    }

    if (selectedFingerprint == 0) {
        Crypto::CryptoEngine::loadPemPublicKey(OFFICIAL_RSA_PEMS[0], selectedNHex, selectedEHex, selectedFingerprint);
    }

    emit logMessage(QString("Selected RSA Public Key Fingerprint: 0x%1").arg(QString::number(selectedFingerprint, 16)));

    m_state = STATE_HANDSHAKE_REQ_DH;
    emit stateChanged(m_state, "Constructing RSA encrypted req_DH_params...");

    // Generate new_nonce (32 bytes)
    Crypto::CryptoEngine::generateRandomBytes(m_newNonce, 32);

    // Prepare P and Q byte vectors (Exactly 4 bytes Big Endian)
    QByteArray pBytes(4, 0);
    pBytes[0] = static_cast<char>((p >> 24) & 0xFF);
    pBytes[1] = static_cast<char>((p >> 16) & 0xFF);
    pBytes[2] = static_cast<char>((p >> 8) & 0xFF);
    pBytes[3] = static_cast<char>(p & 0xFF);

    QByteArray qBytes(4, 0);
    qBytes[0] = static_cast<char>((q >> 24) & 0xFF);
    qBytes[1] = static_cast<char>((q >> 16) & 0xFF);
    qBytes[2] = static_cast<char>((q >> 8) & 0xFF);
    qBytes[3] = static_cast<char>(q & 0xFF);

    // Construct p_q_inner_data_dc
    TL::TLBuffer innerBuf;
    innerBuf.writeUInt32(TL::ID_P_Q_INNER_DATA_DC);
    innerBuf.writeBytes(pqBytes);
    innerBuf.writeBytes(pBytes);
    innerBuf.writeBytes(qBytes);
    innerBuf.writeInt128(m_nonce);
    innerBuf.writeInt128(m_serverNonce);
    innerBuf.writeInt256(m_newNonce);
    innerBuf.writeInt32(Config::DEFAULT_DC_ID);

    QByteArray rsaEncrypted;
    if (!Crypto::CryptoEngine::rsaEncryptHandshake(innerBuf.buffer(),
                                                  selectedNHex.toAscii().constData(),
                                                  selectedEHex.toAscii().constData(),
                                                  rsaEncrypted)) {
        emit errorOccurred("Modern RSA Handshake encryption failed");
        return;
    }

    emit logMessage(QString("RSA Encrypted req_DH_params payload ready (size: %1 bytes). Sending...").arg(rsaEncrypted.size()));

    // Send req_DH_params
    TL::TLBuffer reqDhBuf;
    reqDhBuf.writeInt64(0);
    reqDhBuf.writeInt64(generateMessageId());
    
    TL::TLBuffer payloadBuf;
    payloadBuf.writeUInt32(TL::ID_REQ_DH_PARAMS);
    payloadBuf.writeInt128(m_nonce);
    payloadBuf.writeInt128(m_serverNonce);
    payloadBuf.writeBytes(pBytes);
    payloadBuf.writeBytes(qBytes);
    payloadBuf.writeInt64(static_cast<int64_t>(selectedFingerprint));
    payloadBuf.writeBytes(rsaEncrypted);

    reqDhBuf.writeInt32(static_cast<int32_t>(payloadBuf.size()));
    reqDhBuf.writeRaw(payloadBuf.data(), payloadBuf.size());

    QByteArray packet(reinterpret_cast<const char*>(reqDhBuf.data()), reqDhBuf.size());
    m_transport->sendPacket(packet);
}

void MTProtoSession::handleServerDHParams(const uint8_t* data, size_t size) {
    TL::TLBuffer buf(data, size);

    uint8_t nonce[16], serverNonce[16];
    buf.readInt128(nonce);
    buf.readInt128(serverNonce);

    if (memcmp(nonce, m_nonce, 16) != 0 || memcmp(serverNonce, m_serverNonce, 16) != 0) {
        emit errorOccurred("Server DH params nonce mismatch");
        return;
    }

    QByteArray encryptedAnswer;
    buf.readBytes(encryptedAnswer);

    emit logMessage(QString("server_DH_params_ok received (encrypted size: %1 bytes). Decrypting with tmp_aes_key...").arg(encryptedAnswer.size()));

    // tmp_aes_key = SHA1(new_nonce + server_nonce) + SHA1(server_nonce + new_nonce)[0..11] (32 bytes)
    QByteArray b1;
    b1.append(reinterpret_cast<const char*>(m_newNonce), 32);
    b1.append(reinterpret_cast<const char*>(m_serverNonce), 16);
    QByteArray sha1_1 = Crypto::CryptoEngine::sha1(b1);

    QByteArray b2;
    b2.append(reinterpret_cast<const char*>(m_serverNonce), 16);
    b2.append(reinterpret_cast<const char*>(m_newNonce), 32);
    QByteArray sha1_2 = Crypto::CryptoEngine::sha1(b2);

    m_tmpAesKey.clear();
    m_tmpAesKey.append(sha1_1.constData(), 20);
    m_tmpAesKey.append(sha1_2.constData(), 12);

    // tmp_aes_iv = SHA1(server_nonce + new_nonce)[12..19] + SHA1(new_nonce + new_nonce) + new_nonce[0..3] (32 bytes)
    QByteArray b3;
    b3.append(reinterpret_cast<const char*>(m_newNonce), 32);
    b3.append(reinterpret_cast<const char*>(m_newNonce), 32);
    QByteArray sha1_3 = Crypto::CryptoEngine::sha1(b3);

    m_tmpAesIv.clear();
    m_tmpAesIv.append(sha1_2.constData() + 12, 8);
    m_tmpAesIv.append(sha1_3.constData(), 20);
    m_tmpAesIv.append(reinterpret_cast<const char*>(m_newNonce), 4);

    // Decrypt encryptedAnswer
    QByteArray decrypted;
    decrypted.resize(encryptedAnswer.size());
    if (!Crypto::CryptoEngine::aesIgeDecrypt(reinterpret_cast<const uint8_t*>(encryptedAnswer.constData()),
                                            reinterpret_cast<uint8_t*>(decrypted.data()),
                                            encryptedAnswer.size(),
                                            reinterpret_cast<const uint8_t*>(m_tmpAesKey.constData()),
                                            reinterpret_cast<const uint8_t*>(m_tmpAesIv.constData()))) {
        emit errorOccurred("Failed to decrypt server_DH_inner_data");
        return;
    }

    // First 20 bytes is SHA1(server_DH_inner_data)
    TL::TLBuffer innerBuf(reinterpret_cast<const uint8_t*>(decrypted.constData()) + 20, decrypted.size() - 20);
    uint32_t innerConstructor;
    innerBuf.readUInt32(innerConstructor);

    if (innerConstructor != TL::ID_SERVER_DH_INNER_DATA) {
        emit errorOccurred(QString("Invalid server_DH_inner_data constructor: 0x%1").arg(innerConstructor, 8, 16, QChar('0')));
        return;
    }

    uint8_t innerNonce[16], innerServerNonce[16];
    innerBuf.readInt128(innerNonce);
    innerBuf.readInt128(innerServerNonce);

    int32_t g;
    innerBuf.readInt32(g);

    QByteArray dhPrime;
    innerBuf.readBytes(dhPrime);

    QByteArray g_a;
    innerBuf.readBytes(g_a);

    int32_t serverTime;
    innerBuf.readInt32(serverTime);

    m_timeOffset = serverTime - static_cast<int32_t>(QDateTime::currentDateTimeUtc().toTime_t());
    emit logMessage(QString("DH Parameters parsed: g=%1, dh_prime=%2 bytes, serverTime=%3 (timeOffset=%4s)").arg(g).arg(dhPrime.size()).arg(serverTime).arg(m_timeOffset));

    m_state = STATE_HANDSHAKE_SET_DH;
    emit stateChanged(m_state, "Computing 2048-bit Diffie-Hellman exponentiation...");

    // Generate random b (256 bytes)
    m_bBytes = Crypto::CryptoEngine::randomBytes(256);

    // Compute g_b = (g^b) mod dh_prime
    QByteArray g_b;
    if (!Crypto::CryptoEngine::computeDH(g, m_bBytes, dhPrime, g_b)) {
        emit errorOccurred("Failed to compute g_b in Diffie-Hellman");
        return;
    }

    // Compute auth_key = (g_a)^b mod dh_prime
    if (!Crypto::CryptoEngine::computeAuthKey(g_a, m_bBytes, dhPrime, m_authKey)) {
        emit errorOccurred("Failed to compute auth_key in Diffie-Hellman");
        return;
    }

    emit logMessage(QString("Auth Key successfully computed (%1 bytes). Constructing client_DH_inner_data...").arg(m_authKey.size()));

    // Construct client_DH_inner_data
    TL::TLBuffer clientDhBuf;
    clientDhBuf.writeUInt32(TL::ID_CLIENT_DH_INNER_DATA);
    clientDhBuf.writeInt128(m_nonce);
    clientDhBuf.writeInt128(m_serverNonce);
    clientDhBuf.writeInt64(0); // retry_id = 0
    clientDhBuf.writeBytes(g_b);

    // Compute SHA1(client_DH_inner_data) + client_DH_inner_data + random padding to 16 bytes
    QByteArray clientSha1 = Crypto::CryptoEngine::sha1(clientDhBuf.buffer());
    QByteArray clientPlaintext;
    clientPlaintext.append(clientSha1);
    clientPlaintext.append(clientDhBuf.buffer());

    int padLen = (16 - (clientPlaintext.size() % 16)) % 16;
    if (padLen > 0) {
        QByteArray pad = Crypto::CryptoEngine::randomBytes(padLen);
        clientPlaintext.append(pad);
    }

    QByteArray clientEncrypted;
    clientEncrypted.resize(clientPlaintext.size());
    if (!Crypto::CryptoEngine::aesIgeEncrypt(reinterpret_cast<const uint8_t*>(clientPlaintext.constData()),
                                            reinterpret_cast<uint8_t*>(clientEncrypted.data()),
                                            clientPlaintext.size(),
                                            reinterpret_cast<const uint8_t*>(m_tmpAesKey.constData()),
                                            reinterpret_cast<const uint8_t*>(m_tmpAesIv.constData()))) {
        emit errorOccurred("Failed to encrypt client_DH_inner_data");
        return;
    }

    // Send set_client_DH_params
    TL::TLBuffer setDhBuf;
    setDhBuf.writeInt64(0);
    setDhBuf.writeInt64(generateMessageId());

    TL::TLBuffer setPayload;
    setPayload.writeUInt32(TL::ID_SET_CLIENT_DH_PARAMS);
    setPayload.writeInt128(m_nonce);
    setPayload.writeInt128(m_serverNonce);
    setPayload.writeBytes(clientEncrypted);

    setDhBuf.writeInt32(static_cast<int32_t>(setPayload.size()));
    setDhBuf.writeRaw(setPayload.data(), setPayload.size());

    QByteArray packet(reinterpret_cast<const char*>(setDhBuf.data()), setDhBuf.size());
    m_transport->sendPacket(packet);
}

void MTProtoSession::handleSetClientDHParamsAnswer(uint32_t constructor, const uint8_t* data, size_t size) {
    TL::TLBuffer buf(data, size);

    if (constructor == TL::ID_DH_GEN_OK) {
        uint8_t nonce[16], serverNonce[16], newNonceHash1[16];
        buf.readInt128(nonce);
        buf.readInt128(serverNonce);
        buf.readInt128(newNonceHash1);

        // Compute auth_key_id = lower 64 bits of SHA1(auth_key)
        QByteArray authKeySha1 = Crypto::CryptoEngine::sha1(m_authKey);
        const uint8_t* shaPtr = reinterpret_cast<const uint8_t*>(authKeySha1.constData());
        m_authKeyId = 0;
        for (int i = 0; i < 8; ++i) {
            m_authKeyId |= (static_cast<uint64_t>(shaPtr[12 + i]) << (i * 8));
        }

        // server_salt = new_nonce[0..7] ^ server_nonce[0..7]
        uint64_t nn64 = 0, sn64 = 0;
        memcpy(&nn64, m_newNonce, 8);
        memcpy(&sn64, m_serverNonce, 8);
        m_serverSalt = nn64 ^ sn64;

        // Generate session_id
        Crypto::CryptoEngine::generateRandomBytes(reinterpret_cast<uint8_t*>(&m_sessionId), 8);
        m_seqNo = 0;

        m_state = STATE_ENCRYPTED_READY;
        emit stateChanged(m_state, "MTProto 2.0 Encrypted Channel Active!");
        emit authKeyGenerated(m_authKeyId);

        emit logMessage(QString("=================================================="));
        emit logMessage(QString("SUCCESS: MTProto 2.0 Auth Key Created!"));
        emit logMessage(QString("AuthKeyID: 0x%1").arg(QString::number(m_authKeyId, 16).rightJustified(16, '0')));
        emit logMessage(QString("ServerSalt: 0x%1, SessionID: 0x%2").arg(QString::number(m_serverSalt, 16)).arg(QString::number(m_sessionId, 16)));
        emit logMessage(QString("=================================================="));

        // Auto-test RPC: send help.getNearestDc
        sendGetNearestDc();
    } else {
        emit errorOccurred(QString("DH handshake failed with code: 0x%1").arg(constructor, 8, 16, QChar('0')));
    }
}

void MTProtoSession::sendGetNearestDc() {
    if (m_state != STATE_ENCRYPTED_READY) {
        emit errorOccurred("Cannot send RPC: MTProto session is not encrypted");
        return;
    }

    emit logMessage("Sending Encrypted RPC: invokeWithLayer(layer 195, initConnection(help.getNearestDc))...");

    // invokeWithLayer#da9b0d0d layer:int query:!X = X;
    // initConnection#c1cd5ea9 flags:# api_id:int device_model:string system_version:string app_version:string system_lang_code:string lang_pack:string lang_code:string query:!X = X;
    TL::TLBuffer rpcBuf;
    rpcBuf.writeUInt32(0xda9b0d0d); // invokeWithLayer
    rpcBuf.writeInt32(195);        // layer 195

    rpcBuf.writeUInt32(0xc1cd5ea9); // initConnection
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

void MTProtoSession::sendEncryptedMessage(const QByteArray& messageData, bool isContentRelated) {
    // MTProto 2.0 Message Header
    TL::TLBuffer plainBuf;
    plainBuf.writeInt64(static_cast<int64_t>(m_serverSalt));
    plainBuf.writeInt64(static_cast<int64_t>(m_sessionId));
    plainBuf.writeInt64(generateMessageId());
    plainBuf.writeInt32(static_cast<int32_t>(generateSeqNo(isContentRelated)));
    plainBuf.writeInt32(static_cast<int32_t>(messageData.size()));
    plainBuf.writeRaw(reinterpret_cast<const uint8_t*>(messageData.constData()), messageData.size());

    // MTProto 2.0 Padding: 12 to 1024 bytes of random padding such that total length % 16 == 0
    size_t unpaddedLen = plainBuf.size();
    size_t padLen = (16 - (unpaddedLen % 16));
    if (padLen < 12) {
        padLen += 16;
    }
    QByteArray padBytes = Crypto::CryptoEngine::randomBytes(padLen);
    plainBuf.writeRaw(reinterpret_cast<const uint8_t*>(padBytes.constData()), padBytes.size());

    // Compute msg_key (16 bytes)
    uint8_t msgKey[16];
    Crypto::CryptoEngine::computeMTProto2MsgKey(reinterpret_cast<const uint8_t*>(m_authKey.constData()), plainBuf.data(), plainBuf.size(), true, msgKey);

    // Derive aes_key and aes_iv
    uint8_t aesKey[32], aesIv[32];
    Crypto::CryptoEngine::deriveMTProto2Keys(reinterpret_cast<const uint8_t*>(m_authKey.constData()), msgKey, true, aesKey, aesIv);

    // Encrypt plaintext with AES-256-IGE
    QByteArray encrypted;
    encrypted.resize(plainBuf.size());
    Crypto::CryptoEngine::aesIgeEncrypt(plainBuf.data(), reinterpret_cast<uint8_t*>(encrypted.data()), plainBuf.size(), aesKey, aesIv);

    // Construct MTProto Envelope: auth_key_id (8 bytes) + msg_key (16 bytes) + encrypted_data
    TL::TLBuffer envelope;
    envelope.writeInt64(static_cast<int64_t>(m_authKeyId));
    envelope.writeRaw(msgKey, 16);
    envelope.writeRaw(reinterpret_cast<const uint8_t*>(encrypted.constData()), encrypted.size());

    QByteArray packet(reinterpret_cast<const char*>(envelope.data()), envelope.size());
    m_transport->sendPacket(packet);
}

void MTProtoSession::handleEncryptedPacket(const uint8_t* data, size_t size) {
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

    // Derive aes_key and aes_iv for server-to-client (isClient = false)
    uint8_t aesKey[32], aesIv[32];
    Crypto::CryptoEngine::deriveMTProto2Keys(reinterpret_cast<const uint8_t*>(m_authKey.constData()), msgKey, false, aesKey, aesIv);

    QByteArray decrypted;
    decrypted.resize(static_cast<int>(encryptedLen));
    if (!Crypto::CryptoEngine::aesIgeDecrypt(encryptedData, reinterpret_cast<uint8_t*>(decrypted.data()), encryptedLen, aesKey, aesIv)) {
        emit errorOccurred("Failed to decrypt MTProto 2.0 encrypted packet");
        return;
    }

    // Verify msg_key
    uint8_t computedMsgKey[16];
    Crypto::CryptoEngine::computeMTProto2MsgKey(reinterpret_cast<const uint8_t*>(m_authKey.constData()),
                                                reinterpret_cast<const uint8_t*>(decrypted.constData()),
                                                decrypted.size(), false, computedMsgKey);
    if (memcmp(msgKey, computedMsgKey, 16) != 0) {
        emit errorOccurred("MTProto 2.0 Decryption Error: msg_key verification failed");
        return;
    }

    // Parse decrypted payload: server_salt (8), session_id (8), msg_id (8), seq_no (4), msg_len (4)
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

    uint32_t constructor;
    plainBuf.readUInt32(constructor);

    emit logMessage(QString("Encrypted MTProto 2.0 Response received! Constructor: 0x%1").arg(constructor, 8, 16, QChar('0')));

    if (constructor == TL::ID_RPC_RESULT) {
        int64_t reqMsgId;
        plainBuf.readInt64(reqMsgId);
        uint32_t innerRpcConstructor;
        plainBuf.readUInt32(innerRpcConstructor);

        emit logMessage(QString("RPC Result for ReqMsgId: %1, Inner Constructor: 0x%2").arg(reqMsgId).arg(innerRpcConstructor, 8, 16, QChar('0')));

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
        } else if (innerRpcConstructor == TL::ID_RPC_ERROR) {
            int32_t errCode;
            QString errMsg;
            plainBuf.readInt32(errCode);
            plainBuf.readString(errMsg);
            emit logMessage(QString("[RPC ERROR] Code: %1, Message: %2").arg(errCode).arg(errMsg));
        }
    } else if (constructor == TL::ID_NEAREST_DC) {
        QString country;
        int32_t thisDc, nearestDc;
        plainBuf.readString(country);
        plainBuf.readInt32(thisDc);
        plainBuf.readInt32(nearestDc);

        emit nearestDcReceived(country, thisDc, nearestDc);
    }
}

} // namespace Core
} // namespace Telegram
