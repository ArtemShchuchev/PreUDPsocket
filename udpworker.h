#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>
#include <QDataStream>


#define BIND_PORT   12345
#define MES_PORT    12346

struct MESSAGE
{
    QString mes;
    QString size;
    QString adr;
};

class UDPworker : public QObject
{
    Q_OBJECT

public:
    explicit UDPworker(QObject *parent = nullptr);
    void InitSocket();
    void ReadDatagram(const QNetworkDatagram& datagram);
    void SendDatagram(const QByteArray& data, const int port = BIND_PORT);

private slots:
    void readPendingDatagrams();
    void readMesDatagrams();

private:
    QUdpSocket* serviceUdpSocket;
    QUdpSocket* mesUdpSocket;

signals:
    void sig_sendTimeToGUI(const QDateTime data);
    void sig_sendMesToGUI(const MESSAGE message);
};

#endif // UDPWORKER_H
