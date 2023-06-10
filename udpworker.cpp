#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent)
{
}

/*!
 * @brief Метод инициализирует UDP сервер
 */
void UDPworker::InitSocket()
{
    serviceUdpSocket =  new QUdpSocket(this);
    mesUdpSocket =      new QUdpSocket(this);
    /*
     * Соединяем присваиваем адрес и порт серверу и соединяем функцию
     * обраотчик принятых пакетов с сокетом
     */
    serviceUdpSocket->  bind(QHostAddress::LocalHost, BIND_PORT);
    mesUdpSocket->      bind(QHostAddress::LocalHost, MES_PORT);
    connect(serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);
    connect(mesUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readMesDatagrams);
}

/*!
 * @brief Метод осуществляет обработку принятой датаграммы
 */
void UDPworker::ReadDatagram(const QNetworkDatagram &datagram)
{
    QByteArray data = datagram.data();
    QDataStream inStr(&data, QIODevice::ReadOnly);

    switch (datagram.destinationPort())
    {
    case BIND_PORT:
    {
        QDateTime dateTime;
        inStr >> dateTime;
        emit sig_sendTimeToGUI(dateTime);
        break;
    }

    case MES_PORT:
    {
        // Принято сообщение от адрес отправителя, размер сообщения(байт)
        MESSAGE message;
        inStr >> message.mes;
        message.size = message.mes.size();
        message.adr = datagram.senderAddress().toString();
        emit sig_sendMesToGUI(message);
        break;
    }

    default:
        break;
    }
}
/*!
 * @brief Метод осуществляет опередачу датаграммы
 */
void UDPworker::SendDatagram(const QByteArray &data, const int port)
{
    // Отправляем данные на localhost и задефайненный порт
    switch (port)
    {
    case BIND_PORT:
        serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, port);
        break;

    case MES_PORT:
        mesUdpSocket->writeDatagram(data, QHostAddress::LocalHost, port);
        break;

    default:
        break;
    }
}

/*!
 * @brief Метод осуществляет чтение датаграм из сокета
 */
void UDPworker::readPendingDatagrams()
{
    // Производим чтение принятых датаграмм
    while(serviceUdpSocket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
        ReadDatagram(datagram);
    }
}

void UDPworker::readMesDatagrams()
{
    // Производим чтение принятых датаграмм
    while(mesUdpSocket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = mesUdpSocket->receiveDatagram();
        ReadDatagram(datagram);
    }
}
