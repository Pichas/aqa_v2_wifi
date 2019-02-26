#ifndef DRIVER_H
#define DRIVER_H

#include <QObject>
#include <QMessageBox>
#include <QDateTime>
#include <QStateMachine>
#include <QMessageBox>
#include <QTableView>
#include <QInputDialog>



#include "addtimerwin.h"
#include "timersmodeltable.h"
#include "mySet/myset.h"
#include "drvSender.h"

class driver : public QObject
{
    Q_OBJECT
public:
    explicit driver(QTableView *tview, QObject *parent = nullptr);
    ~driver();

    void reconn();

    void getStatus();

    void getTimeFromDevice();
    void syncTimeToDevice();
    void setRelayStatus(int n, int s);
    void setEnTimers(int e);
    void setEffect(int e);
    void setUserColor(int n, int r, int g, int b);



    bool isEnTimers(){return enTimers;}
    bool isRelay(int n){return relay[n];}

    int effect() {return effectIndex;}

    void addTimer(QString timerString);
    void removeTimer(int n);
    void loadTimers();


private:
    bool enTimers;
    bool relay[2];
    int effectIndex;

    drvSender* sender;

    QScopedPointer<timersModelTable> timersTable;

signals:
    void sendMessage(QString message, int timeout = 0);
    void sendEffectChanged();
    void sendResult(QString data);

public slots:

private slots:
    void getData(QString msg);

};

#endif // DRIVER_H
