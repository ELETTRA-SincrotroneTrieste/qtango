#-------------------------------------------------
#
# Project created by QtCreator 2015-12-29T14:48:59
#
#-------------------------------------------------

include(/usr/local/qtango/include/qtango6/qtango.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/ex1
TEMPLATE = app


SOURCES += main.cpp\
        ex1.cpp \
    readlabel.cpp \
    led.cpp

HEADERS  += ex1.h \
    readlabel.h \
    led.h

FORMS    += ex1.ui
