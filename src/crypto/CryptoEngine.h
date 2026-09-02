#ifndef TELEGRAM_CRYPTO_ENGINE_H
#define TELEGRAM_CRYPTO_ENGINE_H

#include <stdint.h>
#include <stddef.h>
#include <QByteArray>
#include <QString>

namespace Telegram {
namespace Crypto {

class CryptoEngine {
public:
    static void generateRandomBytes(uint8_t* out, size_t length);
    static QByteArray randomBytes(size_t length);
    
    static void sha1(const uint8_t* data, size_t length, uint8_t* out);
    static QByteArray sha1(const QByteArray& data);

    static void sha256(const uint8_t* data, size_t length, uint8_t* out);
    static QByteArray sha256(const QByteArray& data);

    static bool loadPemPublicKey(const char* pemStr, QString& nHexOut, QString& eHexOut, uint64_t& fpOut);

    static bool aesIgeEncrypt(const uint8_t* in, uint8_t* out, size_t length, const uint8_t* key, const uint8_t* iv);
    static bool aesIgeDecrypt(const uint8_t* in, uint8_t* out, size_t length, const uint8_t* key, const uint8_t* iv);

    static bool factorizePQ(uint64_t pq, uint32_t& p, uint32_t& q);

    static uint64_t computeRsaFingerprint(const char* nHex, const char* eHex);

    static bool rsaEncrypt(const uint8_t* data, size_t length, 
                           const char* nHex, const char* eHex, 
                           QByteArray& out);

    static bool rsaEncryptHandshake(const QByteArray& innerData,
                                    const char* nHex, const char* eHex,
                                    QByteArray& out);

    static bool computeDH(int g, const QByteArray& bBytes,
                          const QByteArray& dhPrimeBytes,
                          QByteArray& g_b_out);

    static bool computeAuthKey(const QByteArray& g_a_bytes,
                               const QByteArray& b_bytes,
                               const QByteArray& dhPrimeBytes,
                               QByteArray& authKeyOut);

    // MTProto 2.0 key derivation
    static void deriveMTProto2Keys(const uint8_t* authKey, const uint8_t* msgKey, bool isClient,
                                  uint8_t* aesKeyOut, uint8_t* aesIvOut);

    static void computeMTProto2MsgKey(const uint8_t* authKey, const uint8_t* plaintext, size_t length,
                                      bool isClient, uint8_t* msgKeyOut);

    // 2FA SRP-6A Computation
    static bool computeSRP6A(const QString& password, const QByteArray& salt1, const QByteArray& salt2,
                            int g, const QByteArray& pBytes, const QByteArray& srpB,
                            QByteArray& srpAOut, QByteArray& srpM1Out);
};

} // namespace Crypto
} // namespace Telegram

#endif // TELEGRAM_CRYPTO_ENGINE_H
