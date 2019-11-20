#include "devicesm.h"


DeviceSM::DeviceSM(QObject *parent) : QObject(parent)
{

    //automata
    QState *sInit                   = new QState();
    QState *sWaitingNewData         = new QState();
    QState *sSendData               = new QState();
    QState *sParseAnswer            = new QState();
    QState *sError                  = new QState();


    sInit->addTransition            (this, &DeviceSM::nextState, sWaitingNewData);
    sWaitingNewData->addTransition  (this, &DeviceSM::nextState, sSendData);
    sSendData->addTransition        (this, &DeviceSM::nextState, sParseAnswer);
    sParseAnswer->addTransition     (this, &DeviceSM::nextState, sWaitingNewData);
    sError->addTransition           (this, &DeviceSM::nextState, sInit);


    sInit->addTransition            (this, &DeviceSM::errorState, sError);
    sWaitingNewData->addTransition  (this, &DeviceSM::errorState, sError);
    sSendData->addTransition        (this, &DeviceSM::errorState, sError);
    sParseAnswer->addTransition     (this, &DeviceSM::errorState, sError);


    connect(sInit,              &QState::entered,  this, &DeviceSM::stInit);
    connect(sWaitingNewData,    &QState::entered,  this, &DeviceSM::stWaitingNewData);
    connect(sSendData,          &QState::entered,  this, &DeviceSM::stSendData);
    connect(sParseAnswer,       &QState::entered,  this, &DeviceSM::stParseAnswer);
    connect(sError,             &QState::entered,  this, &DeviceSM::stError);

    m_device.addState(sInit);
    m_device.addState(sWaitingNewData);
    m_device.addState(sSendData);
    m_device.addState(sParseAnswer);
    m_device.addState(sError);

    m_device.setInitialState(sInit);
    ////////////////

    //models
    m_actions["вЫключить реле 0"]    = '1';
    m_actions["включить реле 0"]     = '2';
    m_actions["вЫключить реле 1"]    = '3';
    m_actions["включить реле 1"]     = '4';
    m_actions["Переключить эффект"]  = '5';


    m_effects["Выключить светодиоды"]   = '0';
    m_effects["Восход"]                 = '1';
    m_effects["Закат"]                  = '2';
    m_effects["Включить светодиоды"]    = '3';
    m_effects["Вручную"]                = '4';

    m_timers = new TimersModelTable(this);
    m_timers->setActions(&m_actions);
    m_timers->setEffects(&m_effects);
    connect(m_timers, &TimersModelTable::sendTimerData, this, &DeviceSM::importTimer);

    sortedModel = new QSortFilterProxyModel();
    sortedModel->setSourceModel(m_timers);




    //timers
    m_periodAddElemToQtimer = new QTimer(this);
    m_periodAddElemToQtimer->callOnTimeout(this, QOverload<>::of(&DeviceSM::addNewData));
    m_periodAddElemToQtimer->setInterval(200);

    m_wDog = new QTimer(this);
    m_wDog->callOnTimeout(this, [&]{m_com->send(m_lastData);});
    m_wDog->setInterval(500);

    m_reloadTimers = new QTimer(this);
    m_reloadTimers->callOnTimeout(this, [&]{loadTimers();});
    m_reloadTimers->setSingleShot(true);
    m_reloadTimers->setInterval(200);


}

DeviceSM::~DeviceSM()
{
    emit errorState();
    m_periodAddElemToQtimer->stop();
    m_periodAddElemToQtimer->deleteLater();
    m_wDog->stop();
    m_wDog->deleteLater();
    sortedModel->deleteLater();
    m_timers->deleteLater();
}



QString DeviceSM::dateTime() const
{
    return m_DateTime;
}

QStringList DeviceSM::effectsName() const
{
    return m_effects.keys();
}

QString DeviceSM::effectName(char num) const
{
    return m_effects.key(num);
}

QString DeviceSM::curEffect() const
{
    return m_effects.key(m_effectIndex);
}

QStringList DeviceSM::actionsName() const
{
    return m_actions.keys();
}

QString DeviceSM::actionName(char num) const
{
    return m_actions.key(num);
}

QSortFilterProxyModel* DeviceSM::timersModel() const
{
    return sortedModel;
}

bool DeviceSM::exportTimers(QString fileName)
{
    return m_timers->exportTimers(fileName);
}

bool DeviceSM::importTimers(QString fileName)
{
    return m_timers->importTimers(fileName);
}

void DeviceSM::setDateTime()
{
    QString dt = "S" + QDateTime::currentDateTime().toString("HHmmssddMMyy") +
            QString::number(QDate::currentDate().dayOfWeek());

    m_qData.enqueue(dt);
}

void DeviceSM::setRelay(uint8_t num, bool state)
{
    m_qData.enqueue("X" + QString::number(num) + QString::number(state));
}

void DeviceSM::setEffect(QString effect)
{
    m_qData.enqueue("I" + QString(m_effects.value(effect)));
}

void DeviceSM::setOneLedColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    QString buf;
    buf.sprintf("M%03d%03d%03d%03d", n, r, g, b);
    m_qData.enqueue(buf);
}

void DeviceSM::setOneColor(uint8_t r, uint8_t g, uint8_t b)
{
    QString buf;
    buf.sprintf("N%03d%03d%03d", r, g, b);
    m_qData.enqueue(buf);
}

void DeviceSM::addTimer(QTime time, uint8_t dow, QString act, QString ef)
{
    if (dow == 0) return;

    QString str = "A";
    str.append(time.toString("HHmm"));
    str.append(dow);
    str.append(m_actions.value(act, '0'));
    str.append(m_effects.value(ef, '0'));

    m_qData.enqueue(str);
}

void DeviceSM::removeTimer(uint8_t n)
{
    QString buf;
    buf.sprintf("R%02d", n);
    m_qData.enqueue(buf);
}

void DeviceSM::editTimer(QTime time, uint8_t dow, QString act, QString ef, uint8_t n)
{
    if (dow == 0) return;

    QString buf;
    buf.sprintf("%02d", n);

    QString str = "C";
    str.append(time.toString("HHmm"));
    str.append(dow);
    str.append(m_actions.value(act, '0'));
    str.append(m_effects.value(ef, '0'));
    str.append(buf);

    m_qData.enqueue(str);
}

void DeviceSM::setEnTimers(uint8_t b)
{
    m_qData.enqueue("J" + QString::number(b).toLatin1());
}

void DeviceSM::loadTimers()
{
    m_timers->clear();
    m_qData.enqueue("T00");
}

bool DeviceSM::relay(uint8_t num) const
{
    return m_relay[num];
}

void DeviceSM::stInit()
{
    sendStatus("Подключение");

    m_com.reset(new Connector());
    if (m_com->connectToDevice()){
        connect(m_com.data(), &Connector::ready, this, &DeviceSM::nextState);
        connect(m_com.data(), &Connector::sendState, this, &DeviceSM::sendStatus);

        m_periodAddElemToQtimer->start();
        m_reloadTimers->start();
        emit nextState();
    }else{
        emit errorState();
    }
}



void DeviceSM::stWaitingNewData()
{
    while (m_qData.isEmpty())
        QCoreApplication::processEvents(QEventLoop::AllEvents);

    emit sendStatus("Длина очереди сообщений: " + QString::number(m_qData.count()), 1000);
    emit nextState();
}

void DeviceSM::stSendData()
{
    m_lastData = m_qData.dequeue();
    m_com->send(m_lastData);
    m_wDog->start();
}

void DeviceSM::stParseAnswer()
{
    m_wDog->stop();

    QString answer = QString(m_com->result());

    if (m_lastData.at(0) != answer.at(0) && answer.at(0) != 'E') m_qData.prepend(m_lastData);

    if (answer.at(0) == 'G'){
        m_DateTime = answer.remove('G');
    }

    if (answer.at(0) == 'Z'){ //вывод статуса контроллера
        m_enTimers = answer.at(1) == '0' ? false : true;
        m_relay[0] = answer.at(2) == '0' ? false : true;
        m_relay[1] = answer.at(3) == '0' ? false : true;
        m_effectIndex = answer.at(4).toLatin1();
    }

    if (answer[0] == 'T'){ //значение таймера
        m_timers->insertRow(QTime::fromString(answer.mid(1,4), "hhmm"),
                answer[5].toLatin1(),
                answer[6].toLatin1(),
                answer[7].toLatin1());
        QString buf;
        buf.sprintf("T%02d", m_timers->rowCount());
        m_qData.enqueue(buf);
    }

    if (answer.at(0) == 'R' || answer.at(0) == 'A' || answer.at(0) == 'C'){
        m_reloadTimers->start();
    }

    emit ready();
    emit nextState();
}

void DeviceSM::stError()
{
    m_periodAddElemToQtimer->stop();
    m_wDog->stop();
}

void DeviceSM::addNewData(QString str)
{
    m_qData.enqueue(str);
}

void DeviceSM::importTimer(QTime time, char dow, char act, char ef)
{
    QString str = "A";
    str.append(time.toString("HHmm"));
    str.append(dow);
    str.append(act);
    str.append(ef);

    m_qData.enqueue(str);
}

void DeviceSM::addNewData()
{
    if (m_device.isRunning()){
        m_qData.prepend("Z");
        m_qData.prepend("G");
    }
}

void DeviceSM::start()
{
    m_device.start();
}
