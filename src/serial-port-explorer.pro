#-------------------------------------------------
#
# Project created by QtCreator 2013-09-27T00:09:06
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serial-port-explorer
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    serialportworker.cpp \
    updateserialportinfo.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    serialportworker.h \
    updateserialportinfo.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui

RESOURCES += \
    res.qrc
