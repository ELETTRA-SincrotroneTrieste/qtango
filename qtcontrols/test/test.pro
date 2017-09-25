# $Id: test.pro,v 1.17 2010-02-05 09:25:02 giacomo Exp $
include(../../qtango.pri)

TARGET       = test
TEMPLATE     = app

CONFIG       = qt thread warn_on debug

INCLUDEPATH += ../src
DEPENDPATH  += ../src

# remove lib dependency deriving from /runtime/elettra/..
unix:LIBS += -L$${LIB_DIR} $${QTANGOLIBS}

# the same goes for INCLUDEPATH
INCLUDEPATH -=  $${INC_DIR}

unix:LIBS   += -L.. -lqtcontrols

QMAKE_CLEAN += test

linux-g++:TMAKE_CXXFLAGS += -fno-exceptions 

HEADERS = test.h
SOURCES = test.cpp

