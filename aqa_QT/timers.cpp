#include "timers.h"

timers::timers(connector *con_, timersModelTable* model_, QObject *parent)
    : QObject(parent), con(con_), model(model_)
{
    //state machine timers
    QState* sInit = new QState(&timerMachine);
    QState* sQueryTimer = new QState(&timerMachine);
    QState* sWorkTimer = new QState(&timerMachine);
    QState* sErrorTimer = new QState(&timerMachine);

    sInit->addTransition(this, &timers::nextStep, sQueryTimer);
    sQueryTimer->addTransition(this, &timers::nextStep, sWorkTimer);
    sWorkTimer->addTransition(this, &timers::nextStep, sQueryTimer);
    sWorkTimer->addTransition(this, &timers::errorTimerNum, sErrorTimer);

    connect(sInit, &QState::entered, this, &timers::initState);
    connect(sQueryTimer,  &QState::entered, this,   &timers::queryTimer);
    connect(sErrorTimer, &QState::entered, this,   &timers::errorTimer);




    timerMachine.setInitialState(sInit);

    connect(con, &connector::sendResult, this, &timers::getData);

}

timers::~timers()
{
    timerMachine.stop();
    disconnect(con, &connector::sendResult, this, &timers::getData);
}

void timers::startLoading()
{
    timerMachine.start();

}



void timers::getData(QString msg)
{
    if (msg[0] == 'T'){ //значение таймера
        model->insertRow(QTime((msg[1].toLatin1() - 0x30) * 10 + msg[2].toLatin1() - 0x30,
                            (msg[3].toLatin1() - 0x30) * 10 + msg[4].toLatin1() - 0x30),
                msg[5].toLatin1(),
                msg[6].toLatin1(),
                msg[7].toLatin1() - 0x30);
        emit nextStep();
    }
    if (msg[0] == 'E'){ //ошибка
        emit errorTimerNum();
    }
}

void timers::initState()
{
    model->clear();
    curTimer = 0;
    emit nextStep();
}

void timers::queryTimer()
{
    QString buf;
    buf.sprintf("T%02d", curTimer++);
    con->send(buf);
    emit nextStep();
}

void timers::errorTimer()
{
    qDebug() << "End load. Total " << curTimer - 1;
    this->deleteLater();
}
