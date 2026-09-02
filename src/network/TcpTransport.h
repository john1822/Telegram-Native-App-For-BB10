#ifndef TELEGRAM_TCP_TRANSPORT_H
#define TELEGRAM_TCP_TRANSPORT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QString>

namespace Telegram {
namespace Network {

class TcpTransport : public QObject {
    Q_OBJECT
public:
    explicit TcpTransport(QObject* parent = NULL);
    ~TcpTransport();

    void connectToHost(const QString& host, quint16 port);
    void disconnectFromHost();
    bool isConnected() const;

    void sendPacket(const QByteArray& payload);

signals:
    void connected();
    void disconnected();
    void packetReceived(const QByteArray& packet);
    void errorOccurred(const QString& errorMsg);
    void logMessage(const QString& log);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    QTcpSocket* m_socket;
    QByteArray m_readBuffer;
    bool m_handshakeSent;
};

} // namespace Network
} // namespace Telegram

#endif // TELEGRAM_TCP_TRANSPORT_H
