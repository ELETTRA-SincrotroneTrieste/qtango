#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T14:42:39
#
#-------------------------------------------------

# for INSTALL_ROOT
include(../../qtango.pri)

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtangoprojectwizard

TEMPLATE = app

TEMPLATES_INSTALLDIR = $${SHAREDIR}/qtango/qtangoprojectwizard

DEFINES += TEMPLATES_PATH=\"\\\"$${TEMPLATES_INSTALLDIR}\\\"\"

DEFINES -= QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        qtangoprojectwizard.cpp

HEADERS  += qtangoprojectwizard.h

FORMS    += qtangoprojectwizard.ui

target.path = $${INSTALL_ROOT}/bin
target.files = $${TARGET}

templates.path = $${TEMPLATES_INSTALLDIR}
templates.files =  qtango_project_template/*

# remove linking information from qtango.pri. Not needed
LIBS =
unix.INCLUDEPATH =

INSTALLS = target templates
