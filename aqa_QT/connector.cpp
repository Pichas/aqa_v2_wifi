#include "connector.h"

Connector::Connector(QObject *parent) : QObject(parent)
{
    m_mUrl.setHost(SETTING_INI->value("CONNECT/IP", "127.0.0.1").toString());
    m_mUrl.setPort(SETTING_INI->value("CONNECT/Port", 5555).toInt());
}

Connector::~Connector()
{
    if (!m_socket.data()) return;
    m_socket->disconnectFromHost();
    if (m_socket->state() == QAbstractSocket::UnconnectedState ||
            m_socket->waitForDisconnected(1000))
        qDebug("Disconnected!");
}

bool Connector::connectToDevice()
{
    m_socket.reset(new QTcpSocket(nullptr));
    connect(m_socket.data(), &QTcpSocket::stateChanged, this, &Connector::stateChanged);

    m_socket->connectToHost(m_mUrl.host(), static_cast<uint16_t>(m_mUrl.port()));
    if(m_socket->waitForConnected(500)) {
        connect(m_socket.data(), &QIODevice::readyRead, this, &Connector::readData);
        return true;
    } else {
        return false;
    }
}

void Connector::send(QString msg)
{
    msg.append("\r\n");
    writeData(msg.toLatin1());
}

QByteArray Connector::result() const
{
    return m_buffer;
}

void Connector::readData()
{
    m_buffer.clear();
    while (m_socket->bytesAvailable() > 0)
        m_buffer.append(m_socket->readAll());
    emit ready();
}

bool Connector::writeData(QByteArray data)
{
    if(m_socket && m_socket->state() == QAbstractSocket::ConnectedState)
    {
        qApp->processEvents();
        m_socket->write(data); //write the data itself
        return m_socket->waitForBytesWritten();
    }
    else
        return false;
}

void Connector::stateChanged(QAbstractSocket::SocketState socketState)
{
    switch (socketState) {
    case QAbstractSocket::HostLookupState:
        emit sendState("Поиск узла", 1000);
        break;
    case QAbstractSocket::ConnectingState:
        emit sendState("Подключение", 1000);
        break;
    case QAbstractSocket::ConnectedState:
        emit sendState("Успешное подключение к " + m_mUrl.host() + ":" + QString::number(m_mUrl.port()), 1000);
        break;

    case QAbstractSocket::ClosingState:
        emit sendState("Отключено от " + m_mUrl.host() + ":" + QString::number(m_mUrl.port()), 0);
        break;
    }
}
