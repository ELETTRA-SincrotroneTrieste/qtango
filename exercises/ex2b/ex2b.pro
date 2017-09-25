#-------------------------------------------------
#
# Project created by QtCreator 2015-12-31T10:55:23
#
#-------------------------------------------------

include(/runtime/include/qtango6/qtango.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/ex2b
TEMPLATE = app


SOURCES += main.cpp\
        ex2b.cpp \
    wslider.cpp \
    readlabelb.cpp

HEADERS  += ex2b.h \
    wslider.h \
    readlabelb.h

FORMS    += ex2.ui

bin.files = $${TARGET}

INSTALLS += bin
