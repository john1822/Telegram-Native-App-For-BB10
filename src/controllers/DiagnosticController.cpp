#include "DiagnosticController.h"
#include "MTProtoSession.h"
#include "Config.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>

namespace Telegram {
namespace Controllers {

DiagnosticController::DiagnosticController(QObject* parent)
    : QObject(parent),
      m_session(new Core::MTProtoSession(this)),
      m_statusText("Ready to connect"),
      m_authKeyId("None (Not Generated)"),
      m_dcInfo(QString("DC %1 (%2:%3)").arg(Config::DEFAULT_DC_ID).arg(Config::DEFAULT_DC_IP).arg(Config::DEFAULT_DC_PORT)),
      m_country("Unknown"),
      m_connected(false),
      m_encrypted(false) {

    connect(m_session, SIGNAL(stateChanged(Telegram::Core::SessionState, QString)),
            this, SLOT(onStateChanged(int, QString)));
    connect(m_session, SIGNAL(logMessage(QString)), this, SLOT(onLogMessage(QString)));
    connect(m_session, SIGNAL(authKeyGenerated(quint64)), this, SLOT(onAuthKeyGenerated(quint64)));
    connect(m_session, SIGNAL(nearestDcReceived(QString, int, int)), this, SLOT(onNearestDcReceived(QString, int, int)));
    connect(m_session, SIGNAL(errorOccurred(QString)), this, SLOT(onErrorOccurred(QString)));
}

DiagnosticController::~DiagnosticController() {}

QString DiagnosticController::statusText() const {
    return m_statusText;
}

QString DiagnosticController::authKeyId() const {
    return m_authKeyId;
}

QString DiagnosticController::dcInfo() const {
    return m_dcInfo;
}

QString DiagnosticController::country() const {
    return m_country;
}

QString DiagnosticController::logsText() const {
    return m_logs.join("\n");
}

bool DiagnosticController::isConnected() const {
    return m_connected;
}

bool DiagnosticController::isEncrypted() const {
    return m_encrypted;
}

void DiagnosticController::startConnection() {
    onLogMessage(QString("Initiating direct MTProto 2.0 connection to DC %1 (%2:%3)...")
                 .arg(Config::DEFAULT_DC_ID).arg(Config::DEFAULT_DC_IP).arg(Config::DEFAULT_DC_PORT));
    m_session->start(Config::DEFAULT_DC_IP, Config::DEFAULT_DC_PORT);
}

void DiagnosticController::reconnect() {
    m_session->stop();
    startConnection();
}

void DiagnosticController::sendTestRpc() {
    onLogMessage("Manually triggering help.getNearestDc RPC...");
    m_session->sendGetNearestDc();
}

void DiagnosticController::clearLogs() {
    m_logs.clear();
    emit logsChanged();
}

void DiagnosticController::onStateChanged(int state, const QString& stateText) {
    m_statusText = stateText;
    m_connected = (state != Core::STATE_DISCONNECTED && state != Core::STATE_CONNECTING);
    m_encrypted = (state == Core::STATE_ENCRYPTED_READY);
    emit statusTextChanged();
    emit connectionChanged();
    emit encryptionChanged();
}

void DiagnosticController::onLogMessage(const QString& log) {
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString entry = QString("[%1] %2").arg(timeStr).arg(log);
    m_logs.append(entry);
    if (m_logs.size() > 500) {
        m_logs.removeFirst();
    }
    emit logsChanged();

    // Write to persistent text log file in sandbox
    QFile file("data/app_log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << entry << "\n";
        file.close();
    }
}

void DiagnosticController::onAuthKeyGenerated(quint64 authKeyIdHex) {
    m_authKeyId = QString("0x%1").arg(QString::number(authKeyIdHex, 16).rightJustified(16, '0'));
    emit authKeyIdChanged();
}

void DiagnosticController::onNearestDcReceived(const QString& country, int thisDc, int nearestDc) {
    m_country = country;
    m_dcInfo = QString("Current DC: %1, Recommended DC: %2 (Country: %3)").arg(thisDc).arg(nearestDc).arg(country);
    emit countryChanged();
    emit dcInfoChanged();
}

void DiagnosticController::onErrorOccurred(const QString& error) {
    onLogMessage(QString("[CRITICAL ERROR] %1").arg(error));
}

} // namespace Controllers
} // namespace Telegram
