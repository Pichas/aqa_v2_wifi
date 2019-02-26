#include "connector.h"

connector::connector(QObject *parent) : QObject(parent)
{
    socket.reset(new QTcpSocket(nullptr));
}

connector::~connector()
{
    socket->disconnectFromHost();
    if (socket->state() == QAbstractSocket::UnconnectedState ||
            socket->waitForDisconnected(1000))
        qDebug("Disconnected!");
}

bool connector::conn()
{
    //корректно отключится
    socket->disconnectFromHost();
    if (socket->state() == QAbstractSocket::UnconnectedState ||
            socket->waitForDisconnected(1000))
        qDebug("Disconnected!");

    socket.reset(new QTcpSocket(nullptr));

    socket->connectToHost(SETTING_INI->value("CONNECT/IP", "127.0.0.1").toString(),
                          static_cast<uint16_t>(SETTING_INI->value("CONNECT/Port", 5555).toInt()));

    if(socket->waitForConnected(500)) {
        qDebug() << "Connected!";
        connect(socket.data(), &QIODevice::readyRead, this, &connector::readData);
        return true;
    } else {
        qDebug() << "socket Error!";
        return false;
    }
}

void connector::send(QString msg)
{
    command = msg;
    msg.append("\r\n");
    qDebug() << msg << " " << writeData(msg.toLatin1());
}

void connector::readData()
{
    QByteArray buffer;
    while (socket->bytesAvailable() > 0)
    {
        buffer.append(socket->readAll());
    }
    qDebug() << buffer;
    emit sendResult(QString(buffer));
}

bool connector::writeData(QByteArray data)
{
    if(socket && socket->state() == QAbstractSocket::ConnectedState)
    {
        qApp->processEvents();
        socket->write(data); //write the data itself
        return socket->waitForBytesWritten();
    }
    else
        return false;
}
