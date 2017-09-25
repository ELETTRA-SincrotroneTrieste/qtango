include(../test.pro)
SUBDIRS += src

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += QTANGO_PRINTINFO

TEMPLATE = app
CONFIG += debug 
	  
SOURCES += src/connectionIssues.cpp \
           src/main.cpp 

HEADERS += src/connectionIssues.h 

FORMS = src/connectionIssues.ui

TARGET = bin/connectionIssues




