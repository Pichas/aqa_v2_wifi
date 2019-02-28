#ifndef DRIVER_H
#define DRIVER_H

#include <QObject>
#include <QMessageBox>
#include <QDateTime>
#include <QStateMachine>
#include <QMessageBox>
#include <QTableView>
#include <QInputDialog>
#include <QQueue>



#include "timersmodeltable.h"
#include "addtimerwin.h"
#include "connector.h"
#include "mySet/myset.h"

class driver : public QObject
{
    Q_OBJECT
public:
    explicit driver(QTableView *tview, QObject *parent = nullptr);
    ~driver();

    void conn();

    void getStatus();

    void getTimeFromDevice();
    void syncTimeToDevice();
    void setRelayStatus(int n, int s);
    void setEnTimers(int e);
    void setEffect(int e);
    void setUserColorQueue(int n, int r, int g, int b);
    void sendSingleLedColor(int n, int r, int g, int b);
    void sendLedsArray();


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
    QQueue<QString> ledsArray;

    QScopedPointer<connector> cntr;
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
