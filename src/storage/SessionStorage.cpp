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

    emit sessionCleared();
}

} // namespace Storage
} // namespace Telegram
