#-------------------------------------------------
#
# Project created by QtCreator 2016-01-05T14:42:07
#
#-------------------------------------------------

include(/usr/local/qtango/include/qtango6/qtango.pri)

QT       += core gui

DEFINES -= QT_NO_DEBUG_OUTPUT

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/ex3
TEMPLATE = app


SOURCES += main.cpp\
        ex3.cpp

HEADERS  += ex3.h

FORMS    += ex3.ui

INCLUDEPATH += /runtime/include/qgraphicsplot/ \
    /usr/local/qgraphicsplot/include/qgraphicsplot/

LIBS += -lQGraphicsPlot-qt5 -L/runtime/lib
