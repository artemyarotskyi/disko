#-------------------------------------------------
#
# Project created by QtCreator 2017-06-20T14:21:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Disko
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    CommonUiControllers/LampLight.cpp \
    CommonUiControllers/CornerGrabber.cpp \
    CommonUiControllers/Lamp.cpp

HEADERS  += MainWindow.h \
    CommonUiControllers/LampLight.h \
    CommonUiControllers/CornerGrabber.h \
    CommonUiControllers/Lamp.h

FORMS    += MainWindow.ui
