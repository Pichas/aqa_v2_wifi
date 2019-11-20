#ifndef DEVICESM_H
#define DEVICESM_H

#include <QObject>
#include <QStateMachine>
#include <QQueue>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QSortFilterProxyModel>


#include "connector.h"
#include "timersmodeltable.h"

class DeviceSM : public QObject
{
    Q_OBJECT

public:
    explicit DeviceSM(QObject *parent = nullptr);
    ~DeviceSM();

    void setDateTime();
    void setRelay(uint8_t num, bool state);
    void setEffect(QString effect);
    void setOneLedColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setOneColor(uint8_t r, uint8_t g, uint8_t b);
    void addTimer(QTime time, uint8_t dow, QString act, QString ef);
    void removeTimer(uint8_t n);
    void editTimer(QTime time, uint8_t dow, QString act, QString ef, uint8_t n);
    void setEnTimers(uint8_t b);

    bool relay(uint8_t num) const;
    QString dateTime() const;
    QStringList effectsName() const;
    QString effectName(char num) const;
    QString curEffect() const;
    QStringList actionsName() const;
    QString actionName(char num) const;
    void loadTimers();
    bool isEnTimers() const {return m_enTimers;}
    QSortFilterProxyModel* timersModel() const;

    bool exportTimers(QString fileName);
    bool importTimers(QString fileName);



private:
    QScopedPointer<Connector> m_com;
    QTimer* m_periodAddElemToQtimer;
    QTimer* m_wDog;
    QTimer* m_reloadTimers;

    QQueue<QString> m_qData;
    QStateMachine m_device;
    QString m_lastData;


    bool m_enTimers;
    bool m_relay[2];
    char m_effectIndex;
    QString m_DateTime;
    TimersModelTable *m_timers;
    QSortFilterProxyModel *sortedModel;


    QMap<QString, char> m_actions;
    QMap<QString, char> m_effects;

signals:
    void nextState();
    void errorState();
    void ready();
    void sendStatus(QString status, int time = 0);


public slots:
    void start();


private slots:

    void stInit();
    void stWaitingNewData();
    void stSendData();
    void stParseAnswer();
    void stError();

    void addNewData();
    void addNewData(QString str);
    void importTimer(QTime time, char dow, char act, char ef);




};

#endif // DEVICESM_H
