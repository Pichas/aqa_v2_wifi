#ifndef MYSET_H
#define MYSET_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>

class mySet;
#define SETTING mySet::instance()
#define SETTING_REG mySet::instance()->reg()
#define SETTING_INI mySet::instance()->ini()
#define SETTING_SET_PROGNAME mySet::progName


class mySet : public QObject
{
    Q_OBJECT

private:
    mySet();
    ~mySet(){sIni->deleteLater();}

    mySet(const mySet& Src);
    mySet(mySet&& Src);
    mySet& operator=(const mySet& Src);

    QSettings* sIni;

public:
    QSettings* ini() const {return sIni;}

    static mySet* instance(){
        static mySet* theSingleInstance = new mySet();
        return theSingleInstance;
    }
};

#endif // MYSET_H
