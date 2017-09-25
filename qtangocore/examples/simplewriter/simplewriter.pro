# $Id: simplewriter.pro,v 1.10 2010-01-29 13:50:52 giacomo Exp $
include(../../../../qmake.custom.4)
TEMPLATE = app


message("")
message("----------------qtangocore simple reader: ----------")
message("Occhio: i warning sono off")
CONFIG += warn_off
message("----------------------------------------------------")
message("")


CONFIG += debug
DEFINES += QT_NO_DEBUG_OUTPUT
	  
SOURCES += simplewriter.cpp

HEADERS += simplewriter.h


TARGET = bin/simplewriter

MOC_DIR = moc
OBJECTS_DIR = obj

INCLUDEPATH += ../../src ../../../qtcontrols/src  \
  ../../../qtangocore/src/qtango_wcommon  ../../../qtangocore/src/wrapper  ../../../qtangocore/src  ../../../common


# /runtime/elettra/lib, which is included in qmake.custom.4, is necessary for the panels, but not for the libraries
unix:LIBS -= -L$${LIB_DIR} $${QTANGOLIBS}
# the same goes for INCLUDEPATH
INCLUDEPATH -=  $${INC_DIR}

LIBS +=   -L.. -L../../../qtangocore -L../../../qtcontrols -lQTangoCore -lqtcontrols


