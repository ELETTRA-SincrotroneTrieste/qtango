# $Id: commands.pro,v 1.5 2009-09-14 15:02:12 giacomo Exp $
include(../test.pro)
TEMPLATE = app

CONFIG += release


DEFINES += QT_NO_DEBUG_OUTPUT
	  
SOURCES += commander.cpp \
           main.cpp \ 
	src/mylabel.cpp

HEADERS += commander.h \ 
	src/mylabel.h

FORMS = commander.ui

TARGET = bin/qtango$${VER_MAJ}Commander



