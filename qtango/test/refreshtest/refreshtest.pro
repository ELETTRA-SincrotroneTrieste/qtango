include(../test.pro)
SUBDIRS += src

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += QTANGO_PRINTINFO

TEMPLATE = app
CONFIG += debug 
	  
SOURCES += src/refreshtest.cpp \
           src/main.cpp

HEADERS += src/refreshtest.h

FORMS = src/refreshtest.ui
TARGET = bin/refreshtest



