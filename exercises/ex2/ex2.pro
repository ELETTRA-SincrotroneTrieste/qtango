#-------------------------------------------------
#
# Project created by QtCreator 2015-12-31T10:55:23
#
#-------------------------------------------------

include(/usr/local/qtango/include/qtango6/qtango.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/ex2
TEMPLATE = app


SOURCES += main.cpp\
        ex2.cpp \
    wslider.cpp \
    readlabelb.cpp

HEADERS  += ex2.h \
    wslider.h \
    readlabelb.h

FORMS    += ex2.ui
