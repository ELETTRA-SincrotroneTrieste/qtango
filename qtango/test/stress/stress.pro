# $Id: stress.pro,v 1.7 2009-12-07 08:56:26 giacomo Exp $
include(../test.pro)
TEMPLATE = app

message("")
message("-----------stress:---------------")
message("Occhio: i warning sono off")
CONFIG += warn_off
message("---------------------------------")
message("")

# Qui si potrebbe anche desiderare debug al posto di release
CONFIG += debug

# non abilitare QT_NO_DEBUG_OUTPUT perche` qDebug()
# viene utilizzata per stampare info di utilizzo.

DEFINES -= QT_NO_DEBUG_OUTPUT
	  
SOURCES += stress.cpp \
           main.cpp

HEADERS += stress.h

FORMS = stress.ui

TARGET = bin/qtango$${VER_MAJ}Stress


