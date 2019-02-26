#-------------------------------------------------
#
# Project created by QtCreator 2018-11-20T11:34:52
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aqa

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

ICON = ico/main.ico
win32:RC_ICONS += ico/main.ico

VERSION = 0.1.0.0
QMAKE_TARGET_COMPANY = home
QMAKE_TARGET_PRODUCT = aqa
QMAKE_TARGET_DESCRIPTION = no
QMAKE_TARGET_COPYRIGHT = no

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    driver.cpp \
    connector.cpp \
    timersmodeltable.cpp \
    addtimerwin.cpp \
    mySet/myset.cpp \
    drvSender.cpp

HEADERS += \
        mainwindow.h \
    driver.h \
    connector.h \
    timersmodeltable.h \
    addtimerwin.h \
    mySet/myset.h \
    drvSender.h

FORMS += \
        mainwindow.ui \
    addtimerwin.ui



