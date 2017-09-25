include(../test.pro)

# include(/runtime/include/qtango4/qtango.pri)

SUBDIRS += src

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += QTANGO_PRINTINFO

TEMPLATE = app
CONFIG += debug 
	  
INCLUDEPATH += ../../utils

SOURCES += src/watcher.cpp \
           src/main.cpp 

HEADERS += src/watcher.h 

FORMS = src/watcher.ui

TARGET = bin/watcher




