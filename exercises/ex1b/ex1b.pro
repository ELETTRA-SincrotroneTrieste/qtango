#-------------------------------------------------
#
# Project created by QtCreator 2015-12-29T14:48:59
#
#-------------------------------------------------

include(/usr/local/qtango/include/qtango6/qtango.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/ex1b
TEMPLATE = app


SOURCES += main.cpp\
        ex1b.cpp \
    ledb.cpp \
    readlabelb.cpp

HEADERS  += ex1b.h \
    ledb.h \
    readlabelb.h

FORMS    += \
    ex1b.ui
