#ifndef CONNETCTOR_H
#define CONNETCTOR_H

#include <QObject>
#include <QTcpSocket>
#include <QUrl>
#include <QScopedPointer>
#include <QDebug>


#include "e:/_Work/QT/lib/mySet/myset.h"

class connector : public QObject
{
    Q_OBJECT
public:
    explicit connector(QObject *parent = nullptr);
    ~connector();

    bool conn();
    void send(QString msg);

private:

    QUrl mUrl;
    QScopedPointer<QTcpSocket>socket;

signals:
    void sendResult(QString data);

public slots:


private slots:

    void readData();
    bool writeData(QByteArray data);



};

#endif // CONNETCTOR_H
