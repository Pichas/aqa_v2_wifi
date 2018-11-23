#ifndef TIMERS_H
#define TIMERS_H

#include <QObject>
#include <QStateMachine>
#include <QTimer>
#include "connector.h"
#include "timersmodeltable.h"

class timers : public QObject
{
    Q_OBJECT
public:
    explicit timers(connector *con, timersModelTable* model, QObject *parent = nullptr);
    ~timers();

    void startLoading();


private:

    QStateMachine timerMachine;
    connector *con;

    int curTimer;
    timersModelTable* model;


signals:

    void nextStep();
    void errorTimerNum();



public slots:
    void getData(QString msg);

    void initState();
    void queryTimer();
    void errorTimer();


};

#endif // TIMERS_H
