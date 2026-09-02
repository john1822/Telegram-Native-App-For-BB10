#include "CryptoEngine.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <zlib.h>
#include <string.h>
#include <stdlib.h>

namespace Telegram {
namespace Crypto {

bool CryptoEngine::loadPemPublicKey(const char* pemStr, QString& nHexOut, QString& eHexOut, uint64_t& fpOut) {
    if (!pemStr) return false;
    BIO* bio = BIO_new_mem_buf(const_cast<char*>(pemStr), -1);
    if (!bio) return false;

    RSA* rsa = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
    if (!rsa) {
        BIO_reset(bio);
        rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    }
    BIO_free(bio);

    if (!rsa) return false;

    char* nHex = BN_bn2hex(rsa->n);
    char* eHex = BN_bn2hex(rsa->e);
    if (nHex && eHex) {
        nHexOut = QString::fromAscii(nHex);
        eHexOut = QString::fromAscii(eHex);
        fpOut = computeRsaFingerprint(nHex, eHex);
    } else {
        fpOut = 0;
    }
    if (nHex) OPENSSL_free(nHex);
    if (eHex) OPENSSL_free(eHex);

    RSA_free(rsa);
    return true;
}

static uint64_t gcd64(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

void CryptoEngine::generateRandomBytes(uint8_t* out, size_t length) {
    if (RAND_bytes(out, static_cast<int>(length)) != 1) {
        for (size_t i = 0; i < length; ++i) {
            out[i] = static_cast<uint8_t>(rand() & 0xFF);
        }
    }
}

QByteArray CryptoEngine::randomBytes(size_t length) {
    QByteArray buf;
    buf.resize(static_cast<int>(length));
    generateRandomBytes(reinterpret_cast<uint8_t*>(buf.data()), length);
    return buf;
}

void CryptoEngine::sha1(const uint8_t* data, size_t length, uint8_t* out) {
    SHA1(data, length, out);
}

QByteArray CryptoEngine::sha1(const QByteArray& data) {
    QByteArray out;
    out.resize(20);
    sha1(reinterpret_cast<const uint8_t*>(data.constData()), data.size(), reinterpret_cast<uint8_t*>(out.data()));
    return out;
}

void CryptoEngine::sha256(const uint8_t* data, size_t length, uint8_t* out) {
    SHA256(data, length, out);
}

QByteArray CryptoEngine::sha256(const QByteArray& data) {
    QByteArray out;
    out.resize(32);
    sha256(reinterpret_cast<const uint8_t*>(data.constData()), data.size(), reinterpret_cast<uint8_t*>(out.data()));
    return out;
}

bool CryptoEngine::aesIgeEncrypt(const uint8_t* in, uint8_t* out, size_t length, const uint8_t* key, const uint8_t* iv) {
    if (length % 16 != 0) return false;
    AES_KEY aesKey;
    if (AES_set_encrypt_key(key, 256, &aesKey) < 0) return false;
    uint8_t ivec[32];
    memcpy(ivec, iv, 32);
    AES_ige_encrypt(in, out, length, &aesKey, ivec, AES_ENCRYPT);
    return true;
}

bool CryptoEngine::aesIgeDecrypt(const uint8_t* in, uint8_t* out, size_t length, const uint8_t* key, const uint8_t* iv) {
    if (length % 16 != 0) return false;
    AES_KEY aesKey;
    if (AES_set_decrypt_key(key, 256, &aesKey) < 0) return false;
    uint8_t ivec[32];
    memcpy(ivec, iv, 32);
    AES_ige_encrypt(in, out, length, &aesKey, ivec, AES_DECRYPT);
    return true;
}

static uint64_t mulMod(uint64_t a, uint64_t b, uint64_t m) {
    uint64_t res = 0;
    a %= m;
    while (b > 0) {
        if (b & 1) {
            res = (res + a) % m;
        }
        a = (a * 2) % m;
        b >>= 1;
    }
    return res;
}

bool CryptoEngine::factorizePQ(uint64_t pq, uint32_t& p, uint32_t& q) {
    if (pq < 2) return false;
    if ((pq & 1) == 0) {
        p = 2;
        q = static_cast<uint32_t>(pq / 2);
        return true;
    }
    
    // Trial division for small primes
    for (uint64_t i = 3; i <= 1000; i += 2) {
        if (pq % i == 0) {
            uint64_t other = pq / i;
            p = static_cast<uint32_t>(i < other ? i : other);
            q = static_cast<uint32_t>(i < other ? other : i);
            return true;
        }
    }

    // Pollard's rho algorithm
    uint64_t x = 2, y = 2, d = 1, c = 1;
    int iters = 0;
    while (d == 1 && iters < 200000) {
        uint64_t sq = mulMod(x, x, pq);
        x = (sq + c) % pq;
        
        uint64_t sq1 = mulMod(y, y, pq);
        uint64_t y1 = (sq1 + c) % pq;
        uint64_t sq2 = mulMod(y1, y1, pq);
        y = (sq2 + c) % pq;

        uint64_t diff = (x > y) ? (x - y) : (y - x);
        d = gcd64(diff, pq);
        iters++;
        if (d == pq) {
            x = (rand() % (pq - 2)) + 2;
            y = x;
            c = (rand() % (pq - 1)) + 1;
            d = 1;
        }
    }
    
    if (d > 1 && d < pq) {
        uint64_t other = pq / d;
        p = static_cast<uint32_t>(d < other ? d : other);
        q = static_cast<uint32_t>(d < other ? other : d);
        return true;
    }
    return false;
}

uint64_t CryptoEngine::computeRsaFingerprint(const char* nHex, const char* eHex) {
    BIGNUM* n = NULL;
    BIGNUM* e = NULL;
    BN_hex2bn(&n, nHex);
    BN_hex2bn(&e, eHex);
    if (!n || !e) {
        if (n) BN_free(n);
        if (e) BN_free(e);
        return 0;
    }

    int nLen = BN_num_bytes(n);
    int eLen = BN_num_bytes(e);
    QByteArray nBytes, eBytes;
    nBytes.resize(nLen);
    eBytes.resize(eLen);
    BN_bn2bin(n, reinterpret_cast<uint8_t*>(nBytes.data()));
    BN_bn2bin(e, reinterpret_cast<uint8_t*>(eBytes.data()));

    BN_free(n);
    BN_free(e);

    // TL string serialization: string n, string e
    QByteArray tlKey;
    if (nLen < 254) {
        tlKey.append(static_cast<char>(nLen));
        tlKey.append(nBytes);
        int pad = (4 - ((1 + nLen) % 4)) % 4;
        for (int i = 0; i < pad; ++i) tlKey.append(static_cast<char>(0));
    } else {
        tlKey.append(static_cast<char>(0xFE));
        tlKey.append(static_cast<char>(nLen & 0xFF));
        tlKey.append(static_cast<char>((nLen >> 8) & 0xFF));
        tlKey.append(static_cast<char>((nLen >> 16) & 0xFF));
        tlKey.append(nBytes);
        int pad = (4 - (nLen % 4)) % 4;
        for (int i = 0; i < pad; ++i) tlKey.append(static_cast<char>(0));
    }

    if (eLen < 254) {
        tlKey.append(static_cast<char>(eLen));
        tlKey.append(eBytes);
        int pad = (4 - ((1 + eLen) % 4)) % 4;
        for (int i = 0; i < pad; ++i) tlKey.append(static_cast<char>(0));
    } else {
        tlKey.append(static_cast<char>(0xFE));
        tlKey.append(static_cast<char>(eLen & 0xFF));
        tlKey.append(static_cast<char>((eLen >> 8) & 0xFF));
        tlKey.append(static_cast<char>((eLen >> 16) & 0xFF));
        tlKey.append(eBytes);
        int pad = (4 - (eLen % 4)) % 4;
        for (int i = 0; i < pad; ++i) tlKey.append(static_cast<char>(0));
    }

    QByteArray sha = sha1(tlKey);
    const uint8_t* p = reinterpret_cast<const uint8_t*>(sha.constData());
    uint64_t fp = 0;
    for (int i = 0; i < 8; ++i) {
        fp |= (static_cast<uint64_t>(p[12 + i]) << (i * 8));
    }
    return fp;
}

bool CryptoEngine::rsaEncrypt(const uint8_t* data, size_t length, 
                             const char* nHex, const char* eHex, 
                             QByteArray& out) {
    BIGNUM* n = NULL;
    BIGNUM* e = NULL;
    BIGNUM* m = NULL;
    BIGNUM* r = NULL;
    BN_CTX* ctx = BN_CTX_new();
    if (!ctx) return false;

    BN_hex2bn(&n, nHex);
    BN_hex2bn(&e, eHex);
    m = BN_bin2bn(data, static_cast<int>(length), NULL);
    r = BN_new();

    bool success = false;
    if (n && e && m && r) {
        if (BN_mod_exp(r, m, e, n, ctx) == 1) {
            int numBytes = BN_num_bytes(n);
            out.resize(numBytes);
            out.fill(0);
            int rBytes = BN_num_bytes(r);
            int offset = numBytes - rBytes;
            BN_bn2bin(r, reinterpret_cast<uint8_t*>(out.data()) + offset);
            success = true;
        }
    }

    if (n) BN_free(n);
    if (e) BN_free(e);
    if (m) BN_free(m);
    if (r) BN_free(r);
    BN_CTX_free(ctx);
    return success;
}

bool CryptoEngine::rsaEncryptHandshake(const QByteArray& innerData,
                                       const char* nHex, const char* eHex,
                                       QByteArray& out) {
    if (innerData.size() > 144) return false;

    BIGNUM* n = NULL;
    BIGNUM* e = NULL;
    BN_hex2bn(&n, nHex);
    BN_hex2bn(&e, eHex);
    if (!n || !e) {
        if (n) BN_free(n);
        if (e) BN_free(e);
        return false;
    }

    uint8_t nBytes[256];
    memset(nBytes, 0, 256);
    int nLen = BN_num_bytes(n);
    int nShift = 256 - nLen;
    BN_bn2bin(n, nBytes + (nShift > 0 ? nShift : 0));

    const uint32_t keySize = 32;
    const uint32_t paddedDataSize = 192;
    const uint32_t additionalSize = paddedDataSize - static_cast<uint32_t>(innerData.size());

    uint8_t payload[256];
    uint8_t forwardData[192];
    memcpy(forwardData, innerData.constData(), innerData.size());

    BN_CTX* ctx = BN_CTX_new();
    if (!ctx) {
        BN_free(n);
        BN_free(e);
        return false;
    }

    bool success = false;
    for (int attempt = 0; attempt < 1000; ++attempt) {
        // 1. Fill random padding in forwardData
        if (additionalSize > 0) {
            generateRandomBytes(forwardData + innerData.size(), additionalSize);
        }

        // 2. Reverse forwardData into payload[32..223]
        for (uint32_t i = 0; i < paddedDataSize; ++i) {
            payload[keySize + i] = forwardData[paddedDataSize - 1 - i];
        }

        // 3. Random 32-byte temp_key in payload[0..31]
        uint8_t tempKey[32];
        generateRandomBytes(tempKey, keySize);
        memcpy(payload, tempKey, keySize);

        // 4. SHA256(temp_key + forwardData) into payload[224..255]
        SHA256_CTX sha256Ctx;
        SHA256_Init(&sha256Ctx);
        SHA256_Update(&sha256Ctx, tempKey, keySize);
        SHA256_Update(&sha256Ctx, forwardData, paddedDataSize);
        SHA256_Final(payload + keySize + paddedDataSize, &sha256Ctx);

        // 5. AES-IGE encrypt payload[32..255] (224 bytes) using tempKey and zero_iv
        uint8_t zeroIv[32];
        memset(zeroIv, 0, 32);
        uint8_t encryptedBuf[224];
        aesIgeEncrypt(payload + keySize, encryptedBuf, 224, tempKey, zeroIv);
        memcpy(payload + keySize, encryptedBuf, 224);

        // 6. SHA256(encryptedBuf)
        uint8_t hash2[32];
        sha256(encryptedBuf, 224, hash2);

        // 7. payload[0..31] ^= hash2
        for (uint32_t i = 0; i < keySize; ++i) {
            payload[i] = tempKey[i] ^ hash2[i];
        }

        // 8. Check if payload (256 bytes Big Endian) < N
        bool isLess = false;
        for (int i = 0; i < 256; ++i) {
            if (payload[i] > nBytes[i]) {
                break;
            } else if (payload[i] < nBytes[i]) {
                isLess = true;
                break;
            }
        }

        if (isLess) {
            // 9. RSA Modular Exponentiation: C = payload^E mod N
            BIGNUM* a = BN_bin2bn(payload, 256, NULL);
            BIGNUM* r = BN_new();
            if (a && r && BN_mod_exp(r, a, e, n, ctx) == 1) {
                int rBytes = BN_num_bytes(r);
                out.resize(256);
                out.fill(0);
                int offset = 256 - rBytes;
                BN_bn2bin(r, reinterpret_cast<uint8_t*>(out.data()) + (offset > 0 ? offset : 0));
                success = true;
            }
            if (a) BN_free(a);
            if (r) BN_free(r);
            break;
        }
    }

    BN_free(n);
    BN_free(e);
    BN_CTX_free(ctx);
    return success;
}

bool CryptoEngine::computeDH(int g, const QByteArray& bBytes,
                            const QByteArray& dhPrimeBytes,
                            QByteArray& g_b_out) {
    BIGNUM* g_bn = BN_new();
    BIGNUM* b_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(bBytes.constData()), bBytes.size(), NULL);
    BIGNUM* prime_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(dhPrimeBytes.constData()), dhPrimeBytes.size(), NULL);
    BIGNUM* res_bn = BN_new();
    BN_CTX* ctx = BN_CTX_new();

    BN_set_word(g_bn, static_cast<BN_ULONG>(g));

    bool success = false;
    if (g_bn && b_bn && prime_bn && res_bn && ctx) {
        if (BN_mod_exp(res_bn, g_bn, b_bn, prime_bn, ctx) == 1) {
            int primeSize = BN_num_bytes(prime_bn);
            g_b_out.resize(primeSize);
            g_b_out.fill(0);
            int resSize = BN_num_bytes(res_bn);
            int offset = primeSize - resSize;
            BN_bn2bin(res_bn, reinterpret_cast<uint8_t*>(g_b_out.data()) + offset);
            success = true;
        }
    }

    if (g_bn) BN_free(g_bn);
    if (b_bn) BN_free(b_bn);
    if (prime_bn) BN_free(prime_bn);
    if (res_bn) BN_free(res_bn);
    if (ctx) BN_CTX_free(ctx);
    return success;
}

bool CryptoEngine::computeAuthKey(const QByteArray& g_a_bytes,
                                 const QByteArray& b_bytes,
                                 const QByteArray& dhPrimeBytes,
                                 QByteArray& authKeyOut) {
    BIGNUM* ga_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(g_a_bytes.constData()), g_a_bytes.size(), NULL);
    BIGNUM* b_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(b_bytes.constData()), b_bytes.size(), NULL);
    BIGNUM* prime_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(dhPrimeBytes.constData()), dhPrimeBytes.size(), NULL);
    BIGNUM* res_bn = BN_new();
    BN_CTX* ctx = BN_CTX_new();

    bool success = false;
    if (ga_bn && b_bn && prime_bn && res_bn && ctx) {
        if (BN_mod_exp(res_bn, ga_bn, b_bn, prime_bn, ctx) == 1) {
            int primeSize = BN_num_bytes(prime_bn);
            authKeyOut.resize(primeSize);
            authKeyOut.fill(0);
            int resSize = BN_num_bytes(res_bn);
            int offset = primeSize - resSize;
            BN_bn2bin(res_bn, reinterpret_cast<uint8_t*>(authKeyOut.data()) + offset);
            success = true;
        }
    }

    if (ga_bn) BN_free(ga_bn);
    if (b_bn) BN_free(b_bn);
    if (prime_bn) BN_free(prime_bn);
    if (res_bn) BN_free(res_bn);
    if (ctx) BN_CTX_free(ctx);
    return success;
}

void CryptoEngine::deriveMTProto2Keys(const uint8_t* authKey, const uint8_t* msgKey, bool isClient,
                                     uint8_t* aesKeyOut, uint8_t* aesIvOut) {
    size_t x = isClient ? 0 : 8;

    uint8_t bufA[16 + 36];
    memcpy(bufA, msgKey, 16);
    memcpy(bufA + 16, authKey + x, 36);
    uint8_t sha256_a[32];
    sha256(bufA, sizeof(bufA), sha256_a);

    uint8_t bufB[36 + 16];
    memcpy(bufB, authKey + x + 40, 36);
    memcpy(bufB + 36, msgKey, 16);
    uint8_t sha256_b[32];
    sha256(bufB, sizeof(bufB), sha256_b);

    memcpy(aesKeyOut, sha256_a, 8);
    memcpy(aesKeyOut + 8, sha256_b + 8, 16);
    memcpy(aesKeyOut + 24, sha256_a + 24, 8);

    memcpy(aesIvOut, sha256_b, 8);
    memcpy(aesIvOut + 8, sha256_a + 8, 16);
    memcpy(aesIvOut + 24, sha256_b + 24, 8);
}

void CryptoEngine::computeMTProto2MsgKey(const uint8_t* authKey, const uint8_t* plaintext, size_t length,
                                         bool isClient, uint8_t* msgKeyOut) {
    size_t x = isClient ? 88 : 96;
    QByteArray buf;
    buf.resize(static_cast<int>(32 + length));
    memcpy(buf.data(), authKey + x, 32);
    memcpy(buf.data() + 32, plaintext, length);
    uint8_t hash[32];
    sha256(reinterpret_cast<const uint8_t*>(buf.constData()), buf.size(), hash);
    memcpy(msgKeyOut, hash + 8, 16);
}

bool CryptoEngine::computeSRP6A(const QString& password, const QByteArray& salt1, const QByteArray& salt2,
                               int g, const QByteArray& pBytes, const QByteArray& srpB,
                               QByteArray& srpAOut, QByteArray& srpM1Out) {
    if (password.isEmpty() || pBytes.isEmpty() || srpB.isEmpty()) return false;

    // 1. Password hashing with PBKDF2 (SHA-512)
    QByteArray pwdUtf8 = password.toUtf8();
    QByteArray pHash1 = sha256(salt1 + pwdUtf8 + salt1);
    QByteArray pHash2 = sha256(salt2 + pHash1 + salt2);

    unsigned char xKey[64];
    if (!PKCS5_PBKDF2_HMAC(pHash2.constData(), pHash2.size(),
                           reinterpret_cast<const unsigned char*>(salt1.constData()), salt1.size(),
                           100000, EVP_sha512(), sizeof(xKey), xKey)) {
        return false;
    }

    QByteArray xBytes(reinterpret_cast<const char*>(xKey), sizeof(xKey));
    QByteArray pHashFinal = sha256(salt2 + xBytes + salt2);

    // 2. OpenSSL BIGNUM setup
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* p_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(pBytes.constData()), pBytes.size(), NULL);
    BIGNUM* g_bn = BN_new();
    BN_set_word(g_bn, static_cast<BN_ULONG>(g));
    BIGNUM* b_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(srpB.constData()), srpB.size(), NULL);
    BIGNUM* x_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(pHashFinal.constData()), pHashFinal.size(), NULL);

    // Generate random ephemeral 'a' (256 bytes)
    QByteArray aBytes = randomBytes(256);
    BIGNUM* a_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(aBytes.constData()), aBytes.size(), NULL);

    // A = g^a mod p
    BIGNUM* A_bn = BN_new();
    BN_mod_exp(A_bn, g_bn, a_bn, p_bn, ctx);

    int pLen = pBytes.size();
    srpAOut.resize(pLen);
    srpAOut.fill(0);
    int aNumBytes = BN_num_bytes(A_bn);
    BN_bn2bin(A_bn, reinterpret_cast<uint8_t*>(srpAOut.data()) + (pLen - aNumBytes));

    // Pad g to 2048-bit big-endian (matching pLen = 256)
    QByteArray gBytes(pLen, 0);
    gBytes[pLen - 1] = static_cast<char>(g);

    // Pad B to 2048-bit big-endian (matching pLen = 256) exactly as in TDLib
    QByteArray bPadded = srpB;
    if (bPadded.size() < pLen) {
        bPadded.prepend(QByteArray(pLen - bPadded.size(), 0));
    }

    // u = H(A | B_padded)
    QByteArray uHash = sha256(srpAOut + bPadded);
    BIGNUM* u_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(uHash.constData()), uHash.size(), NULL);

    // k = H(p | g)
    QByteArray kHash = sha256(pBytes + gBytes);
    BIGNUM* k_bn = BN_bin2bn(reinterpret_cast<const uint8_t*>(kHash.constData()), kHash.size(), NULL);

    // v = g^x mod p
    BIGNUM* v_bn = BN_new();
    BN_mod_exp(v_bn, g_bn, x_bn, p_bn, ctx);

    // kv = (k * v) mod p
    BIGNUM* kv_bn = BN_new();
    BN_mod_mul(kv_bn, k_bn, v_bn, p_bn, ctx);

    // b_minus_kv = (B - kv) mod p (positive modulo, exactly matching TDLib)
    BIGNUM* b_minus_kv = BN_new();
    BN_mod_sub(b_minus_kv, b_bn, kv_bn, p_bn, ctx);

    // exp = (a + u * x)
    BIGNUM* ux_bn = BN_new();
    BN_mul(ux_bn, u_bn, x_bn, ctx);
    BIGNUM* exp_bn = BN_new();
    BN_add(exp_bn, a_bn, ux_bn);

    // S = (B - kv)^(a + ux) mod p
    BIGNUM* S_bn = BN_new();
    BN_mod_exp(S_bn, b_minus_kv, exp_bn, p_bn, ctx);

    QByteArray sBytes(pLen, 0);
    int sNumBytes = BN_num_bytes(S_bn);
    BN_bn2bin(S_bn, reinterpret_cast<uint8_t*>(sBytes.data()) + (pLen - sNumBytes));
    QByteArray k_session = sha256(sBytes);

    // M1 = H(H(p) ^ H(g) | H(salt1) | H(salt2) | A | B_padded | K)
    QByteArray pHash = sha256(pBytes);
    QByteArray gHash = sha256(gBytes);
    QByteArray pgXor(32, 0);
    for (int i = 0; i < 32; ++i) {
        pgXor[i] = pHash[i] ^ gHash[i];
    }
    QByteArray salt1Hash = sha256(salt1);
    QByteArray salt2Hash = sha256(salt2);

    QByteArray m1Data = pgXor + salt1Hash + salt2Hash + srpAOut + bPadded + k_session;
    srpM1Out = sha256(m1Data);

    BN_free(p_bn);
    BN_free(g_bn);
    BN_free(b_bn);
    BN_free(x_bn);
    BN_free(a_bn);
    BN_free(A_bn);
    BN_free(u_bn);
    BN_free(k_bn);
    BN_free(v_bn);
    BN_free(kv_bn);
    BN_free(b_minus_kv);
    BN_free(ux_bn);
    BN_free(exp_bn);
    BN_free(S_bn);
    BN_CTX_free(ctx);

    return true;
}

bool CryptoEngine::gzipDecompress(const QByteArray& inData, QByteArray& outData) {
    if (inData.isEmpty()) return false;

    z_stream strm;
    memset(&strm, 0, sizeof(strm));
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(inData.constData()));
    strm.avail_in = static_cast<uInt>(inData.size());

    // 16 + MAX_WBITS handles gzip format with gzip header/footer
    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
        return false;
    }

    outData.clear();
    char buffer[4096];
    int ret = Z_OK;

    while (ret == Z_OK) {
        strm.next_out = reinterpret_cast<Bytef*>(buffer);
        strm.avail_out = sizeof(buffer);

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_OK || ret == Z_STREAM_END) {
            int have = sizeof(buffer) - strm.avail_out;
            outData.append(buffer, have);
        }
    }

    inflateEnd(&strm);
    return (ret == Z_STREAM_END);
}

} // namespace Crypto
} // namespace Telegram
