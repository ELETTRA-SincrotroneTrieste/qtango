include(../test.pro)
SUBDIRS += src

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += QTANGO_PRINTINFO

TEMPLATE = app
CONFIG += debug 
	  
SOURCES += src/stacked.cpp \
           src/main.cpp

HEADERS += src/stacked.h

FORMS = src/stacked.ui
TARGET = bin/stacked



