#include "myset.h"


mySet::mySet()
{
    sIni = new QSettings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat );
    sIni->setIniCodec("cp-1251");
}
