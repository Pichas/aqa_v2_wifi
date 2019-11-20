#ifndef CONNETCTOR_H
#define CONNETCTOR_H

#include <QObject>
#include <QTcpSocket>
#include <QUrl>
#include <QScopedPointer>
#include <QDebug>


#include "mySet/myset.h"

class Connector : public QObject
{
    Q_OBJECT
public:
    explicit Connector(QObject *parent = nullptr);
    ~Connector();

    bool connectToDevice();
    void send(QString msg);
    QByteArray result() const;

private:
    QByteArray m_buffer;
    QUrl m_mUrl;
    QScopedPointer<QTcpSocket>m_socket;

signals:
    void sendState(QString state, int time = 0);
    void ready();

public slots:


private slots:

    void readData();
    bool writeData(QByteArray data);
    void stateChanged(QAbstractSocket::SocketState socketState);



};

#endif // CONNETCTOR_H
