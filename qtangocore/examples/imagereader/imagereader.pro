# $Id: imagereader.pro,v 1.9 2010-01-29 13:50:52 giacomo Exp $
include(../../../qtango.pri)
TEMPLATE = app


message("")
message("----------------qtangocore simple reader: ----------")
message("Occhio: i warning sono off")
CONFIG += warn_off
message("----------------------------------------------------")
message("")


CONFIG += release
DEFINES -= QT_NO_DEBUG_OUTPUT
	  
SOURCES += imagereader.cpp

HEADERS += imagereader.h


TARGET = bin/imagereader

MOC_DIR = moc
OBJECTS_DIR = obj

LIBS += 


