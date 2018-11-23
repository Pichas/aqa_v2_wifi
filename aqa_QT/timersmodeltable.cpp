#include "timersmodeltable.h"

timersModelTable::timersModelTable(QObject *parent) : QAbstractTableModel(parent)
{
    rows = 0;
    columns = 5;

}

int timersModelTable::rowCount(const QModelIndex &/*parent*/) const
{
    return rows;
}

int timersModelTable::columnCount(const QModelIndex &/*parent*/) const
{
    return columns;
}

QVariant timersModelTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("№пп");
            case 1:
                return QString("Время");
            case 2:
                return QString("День недели");
            case 3:
                return QString("Действие");
            case 4:
                return QString("№ эффекта");
            }
        }
        if (orientation == Qt::Vertical) {
            return section;
        }
    }
    return QVariant();
}


QVariant timersModelTable::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();


    if (role == Qt::DisplayRole)
    {
        switch (column) {
        case 0:
            return row;

        case 1:
            return allTimers.at(row)->startTime;

        case 2:{
            QString days;
            if (allTimers.at(row)->daysOfWeek & (1 << 0)) days.append(" ПН ");
            if (allTimers.at(row)->daysOfWeek & (1 << 1)) days.append(" ВТ ");
            if (allTimers.at(row)->daysOfWeek & (1 << 2)) days.append(" СР ");
            if (allTimers.at(row)->daysOfWeek & (1 << 3)) days.append(" ЧТ ");
            if (allTimers.at(row)->daysOfWeek & (1 << 4)) days.append(" ПТ ");
            if (allTimers.at(row)->daysOfWeek & (1 << 5)) days.append(" СБ ");
            if (allTimers.at(row)->daysOfWeek & (1 << 6)) days.append(" ВС ");
            return days;
        }
        case 3:{
            QString str;
            if (allTimers.at(row)->type == '1') str = "ВЫключить реле 0";
            if (allTimers.at(row)->type == '2') str = "Включить реле 0";
            if (allTimers.at(row)->type == '3') str = "ВЫключить реле 1";
            if (allTimers.at(row)->type == '4') str = "Включить реле 1";
            if (allTimers.at(row)->type == '5') str = "Переключить эффект";
            return str;
        }
        case 4:
            return allTimers.at(row)->effNumber;
        }
    }
    return QVariant();
}

void timersModelTable::insertRow(QTime startTime, char daysOfWeek, char type, int effNumber)
{
    beginResetModel();
    rows++;
    timer* t = new timer(startTime, daysOfWeek, type, effNumber);
    allTimers.append(t);
    endResetModel();
}

void timersModelTable::clear()
{
    beginResetModel();
    allTimers.clear();
    rows = 0;
    endResetModel();
}
