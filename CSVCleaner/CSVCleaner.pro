QT       += core gui widgets

TARGET = CSVCleaner
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++14

SOURCES += \
        main.cpp \
        MainWindow.cpp \
        CleanWindow.cpp

HEADERS += \
        MainWindow.hpp \
        CleanWindow.hpp

INSTALLS += target
