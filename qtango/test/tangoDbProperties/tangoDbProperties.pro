#-------------------------------------------------
#
# Project created by QtCreator 2012-06-01T10:15:49
#
#-------------------------------------------------

QT       += core gui

unix:INCLUDEPATH += ../../src ../../

TARGET = tangoDbProperties
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

LIBS += -L../../ -lQTango
