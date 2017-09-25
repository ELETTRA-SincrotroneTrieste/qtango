# $Id: spectrumReadersWriters.pro,v 1.1 2009-09-22 14:44:50 giacomo Exp $
include(../test.pro)
TEMPLATE = app

message("")
message("---------sprw test: --------")
message("Occhio: i warning sono off")
CONFIG += warn_off
message("---------------------------------")
message("")

CONFIG += release


DEFINES += QT_NO_DEBUG_OUTPUT
	  
SOURCES += sprw.cpp \
           main.cpp

HEADERS += sprw.h

FORMS = sprw.ui

TARGET = bin/qtango$${VER_MAJ}SpRW



