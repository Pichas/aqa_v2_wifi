#ifndef TIMERSMODELTABLE_H
#define TIMERSMODELTABLE_H

#include <QObject>
#include <QAbstractTableModel>
#include <QTime>
#include <QSize>
#include <QtDebug>
#include <QFile>
#include <QDataStream>


class TimersModelTable : public QAbstractTableModel
{
    Q_OBJECT

    struct timer
    {
        timer (QTime startTime_, int daysOfWeek_, int type_, int effNumber_)
            : startTime(startTime_), daysOfWeek(daysOfWeek_), type(type_), effNumber(effNumber_) {}
        QTime startTime;
        int daysOfWeek;
        int type;
        int effNumber;
    };

public:
    TimersModelTable(QObject *parent = nullptr);

    void setActions(QMap<QString, char> *actMap) {m_actions = actMap;}
    void setEffects(QMap<QString, char> *effMap) {m_effects = effMap;}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override  ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;


    void insertRow(QTime startTime, int daysOfWeek, int type, int effNumber);
    void clear();
    bool exportTimers(QString fileName);
    bool importTimers(QString fileName);

private:
    int columns;

    QList<timer*> allTimers;
    QMap<QString, char> *m_actions;
    QMap<QString, char> *m_effects;

signals:
    void insertedRow();
    void sendTimerData(QTime time, char dow, char act, char ef);



};

#endif // TIMERSMODELTABLE_H
