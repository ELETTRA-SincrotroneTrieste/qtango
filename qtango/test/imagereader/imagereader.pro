#-------------------------------------------------
#
# Project created by QtCreator 2016-09-30T14:57:19
#
#-------------------------------------------------
include(/runtime/include/qtango6/qtango.pri)

QT       += core gui

CONFIG += debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES -= QT_NO_DEBUG_OUTPUT

TARGET = imagereader
TEMPLATE = app

INCLUDEPATH += src

SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui
