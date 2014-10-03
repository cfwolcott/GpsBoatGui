#-------------------------------------------------
#
# Project created by QtCreator 2014-09-29T22:12:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GpsBoatGui
TEMPLATE = app

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lwiringPi -lwiringPiDev -lpthread -lm

SOURCES += main.cpp\
        mainwindow.cpp \
    GpsBoatC/TinyGPS.cpp \
    GpsBoatC/Arduino.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
