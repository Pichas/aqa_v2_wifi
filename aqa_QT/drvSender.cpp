#include "drvSender.h"

drvSender::drvSender(QObject *parent) : QObject(parent)
{
    //state machine timers
    QState* sInit = new QState(&sendMachine);
    QState* sWaitCommand = new QState(&sendMachine);
    QState* sSendCommand = new QState(&sendMachine);
    QState* sWaitResponse = new QState(&sendMachine);
    QState* sError = new QState(&sendMachine);

    sInit->addTransition            (this, &drvSender::sigNextStep, sWaitCommand);
    sWaitCommand->addTransition     (this, &drvSender::sigNextStep, sSendCommand);
    sSendCommand->addTransition     (this, &drvSender::sigNextStep, sWaitResponse);
    sWaitResponse->addTransition    (this, &drvSender::sigNextStep, sWaitCommand);
    sWaitResponse->addTransition    (this, &drvSender::sigError,    sError);
    sError->addTransition           (this, &drvSender::sigNextStep, sWaitCommand);

    connect(sInit,          &QState::entered, this, &drvSender::initState);
    connect(sWaitCommand,   &QState::entered, this, &drvSender::waitCommandState);
    connect(sSendCommand,   &QState::entered, this, &drvSender::sendCommandState);
    connect(sWaitResponse,  &QState::entered, this, &drvSender::waitResponseStateEnter);
    connect(sWaitResponse,  &QState::exited,  this, &drvSender::waitResponseStateExited);
    connect(sError,         &QState::entered, this, &drvSender::errorState);


    con = new connector();
    connect(con, &connector::sendResult, this, &drvSender::sigNextStep); //next step when data in
    connect(con, &connector::sendResult, this, &drvSender::sigSendResultData); //send data when data in

    connectionSupportTimer = new QTimer(this);
    connect(connectionSupportTimer, &QTimer::timeout, [&]{
        if (commands.isEmpty()) commands.enqueue("Z");
    });

    responseTimeOut = new QTimer(this);
    connect(responseTimeOut, &QTimer::timeout, this, &drvSender::sigError);

    connect(&sendMachine, &QStateMachine::stopped, &sendMachine, &QStateMachine::start);

    sendMachine.setInitialState(sInit);
    sendMachine.start();
}

drvSender::~drvSender()
{
    connectionSupportTimer->stop();
    connectionSupportTimer->deleteLater();

    commands.clear();

    disconnect(&sendMachine, &QStateMachine::stopped, &sendMachine, &QStateMachine::start);
    sendMachine.stop();

    con->deleteLater();
}

void drvSender::send(const QString command)
{
    commands.enqueue(command);
}

void drvSender::restartSender()
{
    sendMachine.stop();
}

void drvSender::initState()
{
    commands.clear();
    connectionSupportTimer->start(5 * 1000);

    if(con->conn()){
        emit sigSendStatus("Соединение установлено");
        emit sigNextStep();
    }else {
        emit sigSendStatus("Ошибка соединения");
    }
}

void drvSender::waitCommandState()
{
    while (commands.isEmpty()) {
        qApp->processEvents();
    }
    emit sigNextStep();
    emit sigSendStatus("Длина очереди сообщений: " + QString::number(commands.count()));
}

void drvSender::sendCommandState()
{
    con->send(commands.dequeue());
    sigNextStep();
}

void drvSender::waitResponseStateEnter()
{
    responseTimeOut->start(500); //
}

void drvSender::waitResponseStateExited()
{
    responseTimeOut->stop();
}

void drvSender::errorState()
{
    commands.enqueue(con->lastCommand());   //repeat command
    qDebug() << "ERROR " << con->lastCommand();
    emit sigNextStep();
}
