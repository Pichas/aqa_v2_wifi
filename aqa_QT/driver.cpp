#include "driver.h"

driver::driver(QObject *parent) : QObject(parent)
{
    cntr.reset(new connector());
    timersTable.reset(new timersModelTable());
}

driver::~driver()
{

}

void driver::conn()
{
    cntr.reset(new connector());
    if (cntr->conn()){
        connect(cntr.data(), &connector::sendResult, this, &driver::getData);
        sendMessage("Соединение установлено");
    }else{
        sendMessage("Ошибка соединения");
    }
}


void driver::getData(QString msg)
{
    if (msg[0] == 'G'){ //вывод даты времени

        msg.remove('G');
        msg.insert(12, ' ');
        msg.insert(10, '.');
        msg.insert(8, '.');
        msg.insert(6, ' ');
        msg.insert(4, ':');
        msg.insert(2, ':');

        QMessageBox::information(nullptr, "Время в контроллере", msg);
    }

    if (msg[0] == 'Z'){ //вывод статуса контроллера
        enTimers = msg[1] == '0' ? false : true;
        relay[0] = msg[2] == '0' ? false : true;
        relay[1] = msg[3] == '0' ? false : true;
        effectIndex = msg[4].toLatin1() - 0x30;
    }
}





void driver::getStatus()
{
    cntr->send("Z");
}



void driver::getTimeFromDevice()
{
    cntr->send("G");
}

void driver::syncTimeToDevice()
{
    QString sync = "S" + QDateTime::currentDateTime().toString("HHmmssddMMyy") +
            QString::number(QDate::currentDate().dayOfWeek());

    cntr->send(sync);
}

void driver::setRelayStatus(int n, int s)
{
    relay[n] = s;
    cntr->send("X" + QString::number(n) + QString::number(s));
}

void driver::setEnTimers(int e)
{
    cntr->send("J" + QString::number(e).toLatin1());
}

void driver::setEffect(int e)
{
    effectIndex = e;
    cntr->send("E" + QString::number(e));
    emit sendEffectChanged();
}

void driver::setUserColor(int n, int r, int g, int b)
{
    QString buf;
    buf.sprintf("M%03d%03d%03d%03d", n, r, g, b);
    cntr->send(buf);
}

void driver::addTimer(QString timerString)
{
    cntr->send(timerString);
}





void driver::removeTimer(int n)
{
    QString buf;
    buf.sprintf("R%02d", n);
    cntr->send(buf);
}

void driver::loadTimers(QTableView *tview)
{
    tview->setModel(timersTable.data());
    tview->setColumnWidth(0,40);
    tview->setColumnWidth(1,50);
    tview->setColumnWidth(2,150);
    tview->setColumnWidth(3,250);
    tview->setColumnWidth(4,10);

    timers* timersLoader = new timers(cntr.data(), timersTable.data(), nullptr);
    timersLoader->startLoading();
}



