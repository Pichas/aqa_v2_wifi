#ifndef TIMERSMODELTABLE_H
#define TIMERSMODELTABLE_H

#include <QObject>
#include <QAbstractTableModel>
#include <QTime>
#include <QSize>
#include <QtDebug>

class timersModelTable : public QAbstractTableModel
{
    Q_OBJECT

    struct timer
    {
        timer (QTime startTime_, char daysOfWeek_, char type_, int effNumber_)
            : startTime(startTime_), daysOfWeek(daysOfWeek_), type(type_), effNumber(effNumber_) {}
        QTime startTime;
        char daysOfWeek;
        char type;
        int effNumber;
    };

public:
    timersModelTable(QObject *parent = nullptr);


    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    void insertRow(QTime startTime, char daysOfWeek, char type, int effNumber);
    void clear();

private:
    int rows;
    int columns;


    QList<timer*> allTimers;


signals:
    void insertedRow();



};

#endif // TIMERSMODELTABLE_H
