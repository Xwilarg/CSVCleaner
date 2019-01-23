#-------------------------------------------------
#
# Project created by QtCreator 2019-01-23T00:24:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CSVCleaner
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++14

SOURCES += \
        main.cpp \
        MainWindow.cpp

HEADERS += \
        MainWindow.hpp

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
