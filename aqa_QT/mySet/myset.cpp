#include "myset.h"

QString mySet::progName = "";


QSettings* mySet::reg() const
{
    return sReg;
}

QSettings* mySet::ini() const
{
    return sIni;
}

mySet::mySet()
{
    sReg = new QSettings("UGRES_ASUTP", progName);
    sIni = new QSettings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat );
    sIni->setIniCodec("cp-1251");
}
