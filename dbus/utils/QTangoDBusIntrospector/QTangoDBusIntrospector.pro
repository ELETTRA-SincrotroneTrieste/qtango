include (/usr/local/qtango/include/qtango6/qtango.pri)

# -------------------------------------------------
# Project created by QtCreator 2010-08-10T15:41:05
# -------------------------------------------------
QT += dbus \
    xml
CONFIG += debug
TARGET = bin/qtangodbusintrospector
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    appslistreader.cpp \
    appinfo.cpp \
    appdbusinspector.cpp \
    appinfolist.cpp \
    src/appslisttreehelper.cpp \
    src/tactionlisthelper.cpp
HEADERS += mainwindow.h \
    appslistreader.h \
    appinfo.h \
    appdbusinspector.h \
    appinfolist.h \
    src/appslisttreehelper.h \
    src/tactionlisthelper.h
FORMS += mainwindow.ui
DEFINES -= QT_NO_DEBUG_OUTPUT
UI_DIR = src
inst.files = $${TARGET}
inst.path = $${INSTALL_ROOT}/bin
INSTALLS += inst

# for development with qt creator
