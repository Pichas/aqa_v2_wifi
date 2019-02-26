#include "driver.h"

driver::driver(QTableView *tview, QObject *parent) : QObject(parent)
{
    sender = new drvSender(this);
    connect(sender, &drvSender::sigSendStatus, [&](QString msg){emit sendMessage(msg);});
    connect(sender, &drvSender::sigSendResultData, this, &driver::getData);

    timersTable.reset(new timersModelTable());
    tview->setModel(timersTable.data());
    tview->setColumnWidth(0,40);
    tview->setColumnWidth(1,50);
    tview->setColumnWidth(2,150);
    tview->setColumnWidth(3,250);
    tview->setColumnWidth(4,10);
}

driver::~driver()
{

}

void driver::reconn()
{
    sender->restartSender();
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

        QMessageBox::information(nullptr, "Время в контроллере", "Установленное: " + msg + "\r\n"
                                 + "Текущее: " + QDateTime::currentDateTime().toString("dd.MM.yy HH:mm:ss ") + QString::number(QDate::currentDate().dayOfWeek()));
    }

    if (msg[0] == 'Z'){ //вывод статуса контроллера
        enTimers = msg[1] == '0' ? false : true;
        relay[0] = msg[2] == '0' ? false : true;
        relay[1] = msg[3] == '0' ? false : true;
        effectIndex = msg[4].toLatin1() - 0x30;
    }

    if (msg[0] == 'T'){ //значение таймера
        timersTable->insertRow(QTime((msg[1].toLatin1() - 0x30) * 10 + msg[2].toLatin1() - 0x30,
                            (msg[3].toLatin1() - 0x30) * 10 + msg[4].toLatin1() - 0x30),
                msg[5].toLatin1(),
                msg[6].toLatin1(),
                msg[7].toLatin1() - 0x30);

        QString buf;
        buf.sprintf("T%02d", timersTable->rowCount());
        sender->send(buf); //load next timer
    }
}


void driver::getStatus()
{
    sender->send("Z");
}



void driver::getTimeFromDevice()
{
    sender->send("G");
}

void driver::syncTimeToDevice()
{
    QString sync = "S" + QDateTime::currentDateTime().toString("HHmmssddMMyy") +
            QString::number(QDate::currentDate().dayOfWeek());

    sender->send(sync);
}

void driver::setRelayStatus(int n, int s)
{
    relay[n] = s;
    sender->send("X" + QString::number(n) + QString::number(s));
}

void driver::setEnTimers(int e)
{
    sender->send("J" + QString::number(e).toLatin1());
}

void driver::setEffect(int e)
{
    effectIndex = e;
    sender->send("E" + QString::number(e));
    emit sendEffectChanged();
}

void driver::setUserColor(int n, int r, int g, int b)
{
    QString buf;
    buf.sprintf("M%03d%03d%03d%03d", n, r, g, b);
    sender->send(buf);
}

void driver::addTimer(QString timerString)
{
    sender->send(timerString);
}


void driver::removeTimer(int n)
{
    QString buf;
    buf.sprintf("R%02d", n);
    sender->send(buf);
}

void driver::loadTimers()
{
    timersTable->clear();
    sender->send("T00"); //start loading
}



