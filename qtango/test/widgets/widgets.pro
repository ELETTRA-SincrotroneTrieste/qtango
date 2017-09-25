# $Id: widgets.pro,v 1.3 2009-09-14 15:02:12 giacomo Exp $
include(../test.pro)
TEMPLATE = app


message("")
message("---------------- test: ----------")
message("Occhio: i warning sono off")
CONFIG += warn_off
message("---------------------------------")
message("")


CONFIG += release
DEFINES += QT_NO_DEBUG_OUTPUT
	  
SOURCES += test.cpp \
           main.cpp

HEADERS += test.h

FORMS = test.ui

TARGET = bin/testQTango$${VER_MAJ}



