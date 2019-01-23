QT       += core gui widgets

TARGET = CSVCleaner
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++14

SOURCES += \
        main.cpp \
        MainWindow.cpp

HEADERS += \
        MainWindow.hpp

INSTALLS += target
