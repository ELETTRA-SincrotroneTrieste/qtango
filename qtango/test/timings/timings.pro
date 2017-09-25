include(../test.pro)
SUBDIRS += src

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += QTANGO_PRINTINFO

TEMPLATE = app
CONFIG += debug 
	  
SOURCES += src/timings.cpp \
           src/main.cpp \
   		src/timingwidget.cpp

HEADERS += src/timings.h \
   		src/timingwidget.h

TARGET = bin/timings


LIBS += -lproc


