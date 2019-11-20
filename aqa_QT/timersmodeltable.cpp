#include "timersmodeltable.h"

TimersModelTable::TimersModelTable(QObject *parent) : QAbstractTableModel(parent)
{
    columns = 5;
}

int TimersModelTable::rowCount(const QModelIndex &/*parent*/) const
{
    return allTimers.count();
}

int TimersModelTable::columnCount(const QModelIndex &/*parent*/) const
{
    return columns;
}

QVariant TimersModelTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("№");
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


QVariant TimersModelTable::data(const QModelIndex &index, int role) const
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
            QString days; //TODO переделатьт на дни недели
            if (allTimers.at(row)->daysOfWeek & (1 << 0)) days.append(" ПН ");
            if (allTimers.at(row)->daysOfWeek & (1 << 1)) days.append(" ВТ ");
            if (allTimers.at(row)->daysOfWeek & (1 << 2)) days.append(" СР ");
            if (allTimers.at(row)->daysOfWeek & (1 << 3)) days.append(" ЧТ ");
            if (allTimers.at(row)->daysOfWeek & (1 << 4)) days.append(" ПТ ");
            if (allTimers.at(row)->daysOfWeek & (1 << 5)) days.append(" СБ ");
            if (allTimers.at(row)->daysOfWeek & (1 << 6)) days.append(" ВС ");
            if (allTimers.at(row)->daysOfWeek == 0) days.append(" - ");
            return days;
        }
        case 3:{
            return m_actions->key(allTimers.at(row)->type);
        }
        case 4:
            if (allTimers.at(row)->type == '5')
                return m_effects->key(allTimers.at(row)->effNumber);
            else
                return "";
        }
    }
    return QVariant();
}

void TimersModelTable::insertRow(QTime startTime, int daysOfWeek, int type, int effNumber)
{
    beginResetModel();
    allTimers.append(new timer(startTime, daysOfWeek, type, effNumber));
    endResetModel();
}

void TimersModelTable::clear()
{
    beginResetModel();

    for(timer* t : allTimers)
        delete t;

    allTimers.clear();
    endResetModel();
}

bool TimersModelTable::exportTimers(QString fileName)
{
    if (fileName.isEmpty()) return false;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) return false;


    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_4_2);

    stream << QString("Аквариумные таймеры");
    stream << allTimers.count(); //записать количество записей

    for (int i = 0 ; i < allTimers.count() ; i++){
        stream << allTimers.at(i)->startTime;
        stream << allTimers.at(i)->daysOfWeek;
        stream << allTimers.at(i)->type;
        stream << allTimers.at(i)->effNumber;
    }

    file.close();

    if(stream.status() != QDataStream::Ok) return true;
    return false;

}

bool TimersModelTable::importTimers(QString fileName)
{
    if (fileName.isEmpty()) return false;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) return false;

    QDataStream stream(&file);
    stream.setVersion (QDataStream::Qt_4_2) ;

    QString check;
    stream >> check;
    if (check != "Аквариумные таймеры") return false;

    int dataCount;
    stream >> dataCount;

    for (int i = 0; i < dataCount ; i++){
        QTime time;
        int dow;
        int act;
        int eff;

        stream >> time;
        stream >> dow;
        stream >> act;
        stream >> eff;

        emit sendTimerData(time, dow, act, eff);
    }

    file.close();

    if(stream.status() != QDataStream::Ok) return true;
    return false;
}
