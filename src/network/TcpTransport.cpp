#include "TcpTransport.h"
#include <QDebug>

namespace Telegram {
namespace Network {

TcpTransport::TcpTransport(QObject* parent)
    : QObject(parent), m_socket(new QTcpSocket(this)), m_handshakeSent(false) {
    connect(m_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
}

TcpTransport::~TcpTransport() {
    disconnectFromHost();
}

void TcpTransport::connectToHost(const QString& host, quint16 port) {
    m_readBuffer.clear();
    m_handshakeSent = false;
    emit logMessage(QString("Connecting to Telegram DC at %1:%2 via MTProto TCP...").arg(host).arg(port));
    m_socket->connectToHost(host, port);
}

void TcpTransport::disconnectFromHost() {
    if (m_socket && m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

bool TcpTransport::isConnected() const {
    return m_socket && (m_socket->state() == QAbstractSocket::ConnectedState);
}

void TcpTransport::sendPacket(const QByteArray& payload) {
    if (!isConnected()) {
        emit errorOccurred("Cannot send packet: TCP socket is not connected");
        return;
    }

    // Intermediate transport framing: 4-byte length prefix + payload
    quint32 len = static_cast<quint32>(payload.size());
    QByteArray frame;
    frame.resize(4 + payload.size());
    frame[0] = static_cast<char>(len & 0xFF);
    frame[1] = static_cast<char>((len >> 8) & 0xFF);
    frame[2] = static_cast<char>((len >> 16) & 0xFF);
    frame[3] = static_cast<char>((len >> 24) & 0xFF);
    memcpy(frame.data() + 4, payload.constData(), payload.size());

    m_socket->write(frame);
    m_socket->flush();
}

void TcpTransport::onSocketConnected() {
    emit logMessage("TCP Connection established. Sending MTProto Intermediate transport handshake...");
    
    // Intermediate transport handshake header: 0xeeeeeeee
    const char handshake[4] = { static_cast<char>(0xee), static_cast<char>(0xee), static_cast<char>(0xee), static_cast<char>(0xee) };
    m_socket->write(handshake, 4);
    m_socket->flush();
    m_handshakeSent = true;

    emit connected();
}

void TcpTransport::onSocketDisconnected() {
    emit logMessage("TCP Connection closed by host.");
    m_handshakeSent = false;
    emit disconnected();
}

void TcpTransport::onSocketReadyRead() {
    m_readBuffer.append(m_socket->readAll());

    while (m_readBuffer.size() >= 4) {
        const quint8* raw = reinterpret_cast<const quint8*>(m_readBuffer.constData());
        quint32 packetLen = static_cast<quint32>(raw[0]) |
                           (static_cast<quint32>(raw[1]) << 8) |
                           (static_cast<quint32>(raw[2]) << 16) |
                           (static_cast<quint32>(raw[3]) << 24);

        if (packetLen == 0 || packetLen > 2 * 1024 * 1024) {
            emit errorOccurred(QString("Received invalid MTProto packet length: %1").arg(packetLen));
            m_readBuffer.clear();
            m_socket->disconnectFromHost();
            return;
        }

        if (static_cast<quint32>(m_readBuffer.size()) < (4 + packetLen)) {
            // Need more data
            break;
        }

        QByteArray packet = m_readBuffer.mid(4, packetLen);
        m_readBuffer.remove(0, 4 + packetLen);

        emit packetReceived(packet);
    }
}

void TcpTransport::onSocketError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    QString errStr = m_socket->errorString();
    emit errorOccurred(QString("TCP Socket Error: %1").arg(errStr));
    emit logMessage(QString("[ERROR] TCP Socket: %1").arg(errStr));
}

} // namespace Network
} // namespace Telegram
