#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QStateMachine>
#include <QQueue>
#include <QTimer>
#include <QSharedPointer>
#include <QScopedPointer>




#include "connector.h"


class drvSender : public QObject
{
    Q_OBJECT
public:
    explicit drvSender(QObject *parent = nullptr);
    ~drvSender();

    void send(const QString command);
    void restartSender();


private:
    QQueue<QString> commands;
    QTimer* connectionSupportTimer;
    QTimer* responseTimeOut;


    QStateMachine sendMachine;
    connector *con;


signals:
    void sigNextStep();
    void sigError();
    void sigSendResultData(QString);
    void sigSendStatus(QString);



private slots:
    void initState();
    void waitCommandState();
    void sendCommandState();
    void waitResponseStateEnter();
    void waitResponseStateExited();
    void errorState();



public slots:
};

#endif // SENDER_H
