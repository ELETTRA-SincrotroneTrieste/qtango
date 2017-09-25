include(../test.pro)
SUBDIRS += src

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += QTANGO_PRINTINFO

TEMPLATE = app
CONFIG += debug 
	  
SOURCES += src/tplotlight.cpp \
           src/main.cpp

HEADERS += src/tplotlight.h

FORMS = src/tplotlight.ui
TARGET = bin/tplotlight



