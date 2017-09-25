include(../test.pro)
SUBDIRS += src

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += QTANGO_PRINTINFO

TEMPLATE = app
CONFIG += debug 
	  
SOURCES += src/showhide.cpp \
           src/main.cpp 

HEADERS += src/showhide.h 

FORMS = src/showhide.ui

TARGET = bin/showhide




