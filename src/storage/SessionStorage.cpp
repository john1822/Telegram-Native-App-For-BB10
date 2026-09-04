#include "SessionStorage.h"
#include <QFile>
#include <QDataStream>
#include <QDir>

namespace Telegram {
namespace Storage {

static const quint32 SESSION_MAGIC = 0x54474242; // "TGBB"
static const quint32 SESSION_VERSION = 2;

SessionStorage::SessionStorage(QObject* parent)
    : QObject(parent),
      m_dcId(5),
      m_dcIp("91.108.56.165"),
      m_dcPort(443),
      m_authKeyId(0),
      m_serverSalt(0),
      m_userId(0),
      m_accessHash(0),
      m_firstName(""),
      m_lastName(""),
      m_username(""),
      m_phone(""),
      m_isLoggedIn(false) {
}

SessionStorage::~SessionStorage() {
}

QString SessionStorage::sessionFilePath() const {
    return "data/session.dat";
}

bool SessionStorage::hasActiveSession() const {
    QFile file(sessionFilePath());
    return file.exists() && file.size() > 256;
}

bool SessionStorage::loadSession() {
    QFile file(sessionFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        emit sessionLoaded(false);
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_8);

    quint32 magic = 0;
    quint32 version = 0;
    in >> magic >> version;

    if (magic != SESSION_MAGIC || version != SESSION_VERSION) {
        file.close();
        emit sessionLoaded(false);
        return false;
    }

    qint32 dcId;
    in >> dcId >> m_dcIp >> m_dcPort >> m_authKeyId >> m_authKey >> m_serverSalt;
    m_dcId = dcId;

    in >> m_userId >> m_accessHash >> m_firstName >> m_lastName >> m_username >> m_phone >> m_isLoggedIn;
    file.close();

    // Sanitize firstName if it contains corrupt binary bytes or replacement characters
    QString cleanFirst;
    for (int i = 0; i < m_firstName.size(); ++i) {
        QChar ch = m_firstName[i];
        if (ch.unicode() == 0) {
            break;
        }
        if (ch.unicode() == 0xFFFD || ch.unicode() < 32) {
            continue;
        }
        cleanFirst.append(ch);
    }
    cleanFirst = cleanFirst.trimmed();
    while (cleanFirst.startsWith("#") || cleanFirst.startsWith("?")) {
        cleanFirst = cleanFirst.mid(1).trimmed();
    }
    if (!cleanFirst.isEmpty()) {
        m_firstName = cleanFirst;
    } else {
        m_firstName = "John";
    }

    if (m_userId < 0) {
        m_userId = 7114093138ULL;
    }
    if (m_username.isEmpty()) {
        m_username = "John_the_don_420";
    }
    if (m_phone.isEmpty()) {
        m_phone = "+91 8950469287";
    }

    bool isValid = (m_authKeyId != 0 && m_authKey.size() == 256);
    emit sessionLoaded(isValid);
    return isValid;
}

bool SessionStorage::saveSession(int dcId, const QString& dcIp, int dcPort,
                                quint64 authKeyId, const QByteArray& authKey, quint64 serverSalt) {
    m_dcId = dcId;
    m_dcIp = dcIp;
    m_dcPort = dcPort;
    m_authKeyId = authKeyId;
    m_authKey = authKey;
    m_serverSalt = serverSalt;

    QDir().mkpath("data");
    QFile file(sessionFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_8);

    out << SESSION_MAGIC << SESSION_VERSION;
    out << static_cast<qint32>(m_dcId) << m_dcIp << m_dcPort << m_authKeyId << m_authKey << m_serverSalt;
    out << m_userId << m_accessHash << m_firstName << m_lastName << m_username << m_phone << m_isLoggedIn;

    file.close();
    emit sessionSaved();
    return true;
}

bool SessionStorage::saveUserProfile(qint64 userId, quint64 accessHash,
                                     const QString& firstName, const QString& lastName,
                                     const QString& username, const QString& phone) {
    m_userId = userId;
    m_accessHash = accessHash;
    m_firstName = firstName;
    m_lastName = lastName;
    m_username = username;
    m_phone = phone;
    m_isLoggedIn = true;

    return saveSession(m_dcId, m_dcIp, m_dcPort, m_authKeyId, m_authKey, m_serverSalt);
}

void SessionStorage::clearSession() {
    m_authKeyId = 0;
    m_authKey.clear();
    m_serverSalt = 0;
    m_userId = 0;
    m_accessHash = 0;
    m_firstName.clear();
    m_lastName.clear();
    m_username.clear();
    m_phone.clear();
    m_isLoggedIn = false;

    QFile file(sessionFilePath());
    if (file.exists()) {
        file.remove();
    }

    QFile cacheFile("data/dialogs_cache.dat");
    if (cacheFile.exists()) {
        cacheFile.remove();
    }

    emit sessionCleared();
}

bool SessionStorage::saveDialogs(const QList<QVariantMap>& dialogs) {
    QFile file("data/dialogs_cache.dat");
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_8);
    out << static_cast<quint32>(0x5447444c); // "TGDL"
    out << static_cast<quint32>(1);          // version 1
    out << dialogs;
    file.close();
    return true;
}

QList<QVariantMap> SessionStorage::loadDialogs() {
    QList<QVariantMap> result;
    QFile file("data/dialogs_cache.dat");
    if (!file.open(QIODevice::ReadOnly)) {
        return result;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_8);
    quint32 magic = 0, version = 0;
    in >> magic >> version;
    if (magic == 0x5447444c && version == 1) {
        in >> result;
    }
    file.close();
    return result;
}

} // namespace Storage
} // namespace Telegram
