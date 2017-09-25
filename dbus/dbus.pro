# $Id: dbus.pro,v 1.9 2014-08-28 13:23:45 giacomo Exp $
# $Name $

include(../qtango.pri)
include(../messages.pri)

TEMPLATE = lib staticlib

# CONFIG += qdbus
# CONFIG -= warn_off

QT += dbus xml

TARGET = QTangoDBus$${VER_MAJ}$${QTVER_SUFFIX}

QMAKE_CLEAN += libQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX}*

DEFINES += QTANGO_PRINTINFO
DEPENDPATH += . \
              private

INCLUDEPATH += . \
               private

# Input
HEADERS += ActionFactoryAdaptor.h \
           ActionFactoryInterface.h \
           DeviceThreadFactoryAdaptor.h \
           DeviceThreadFactoryInterface.h \
           secondinstancehandler.h \
           tappinfo.h \
           tappinfolist.h \
           TApplicationAdaptor.h \
           TApplicationInterface.h \
           tapplicationspoolproxy.h

SOURCES += ActionFactoryAdaptor.cpp \
           ActionFactoryInterface.cpp \
           DeviceThreadFactoryAdaptor.cpp \
           DeviceThreadFactoryInterface.cpp \
           secondinstancehandler.cpp \
           tappinfo.cpp \
           tappinfolist.cpp \
           TApplicationAdaptor.cpp \
           TApplicationInterface.cpp \
           tapplicationspoolproxy.cpp \
           private/tapplicationspoolproxy_p.cpp

# unix:LIBS += -lQtDBus

# 4.2.1 remove from unix:LIBS $${QTANGOLIBS} defined in qtango.pri
unix:LIBS -= -lQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX} $${QTANGOLIBS}


# source documentation. Mainpage has been inserted into tapplicationspoolproxy.h

DOC_DIR = $${SHAREDIR}/qtangodbus
doc.commands = \
    doxygen \
    Doxyfile;
doc.files = doc/
doc.path = $${DOC_DIR}

# lib

lib.path = $${LIB_DIR}
lib.files = libQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VERSION}
lib.commands = ln \
    -sf \
    libQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VERSION} \
    $${LIB_DIR}/libQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VER_MAJ} \
    && \
    ln \
    -sf \
    libQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VER_MAJ} \
    $${LIB_DIR}/libQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX}.so

inc.files = \
    TApplicationAdaptor \
    TApplicationInterface \
    TApplicationsPoolProxy \
    TAppInfo \
    TAppInfoList \
    DeviceThreadFactoryAdaptor \
    DeviceThreadFactoryInterface

inc.files += $${HEADERS}

inc.path = $${INC_DIR}

# installation

INSTALLS += lib \
    inc \
    doc
