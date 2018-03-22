# $Id: qtango.pri,v 1.56 2014-08-28 13:23:45 giacomo Exp $

# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
# Qwt libraries (>= 6.1.2) are installed here:
#   QWT_HOME =
#
exists(/usr/local/qwt-6.1.4) {
    QWT_HOME = /usr/local/qwt-6.1.4
}
exists(/usr/local/qwt-6.1.3) {
    QWT_HOME = /usr/local/qwt-6.1.3
}
exists(/usr/local/qwt-6.1.2) {
    QWT_HOME = /usr/local/qwt-6.1.2
}

QWT_LIB = qwt

QWT_INCLUDES=$${QWT_HOME}/include

QWT_HOME_USR = /usr
QWT_INCLUDES_USR = $${QWT_HOME_USR}/include/qwt


#
# since 6.4.0: use pkgconfig for tango dependency
#
CONFIG+=link_pkgconfig
PKGCONFIG += tango x11

QWT_PKGCONFIG = Qt5Qwt6

packagesExist($${QWT_PKGCONFIG}){
    PKGCONFIG += $${QWT_PKGCONFIG}
    message("Qwt: using pkg-config to configure qwt includes and libraries")
} else {
    warning("Qwt: no pkg-config file found")
    warning("Qwt: export PKG_CONFIG_PATH=/usr/path/to/qwt/lib/pkgconfig if you want to enable pkg-config for qwt")
    warning("Qwt: if you build and install qwt from sources, be sure to uncomment/enable ")
    warning("Qwt: QWT_CONFIG     += QwtPkgConfig in qwtconfig.pri qwt project configuration file")
}


#
# Here qtango will be installed
    INSTALL_ROOT = /usr/local/qtango
#
# End customization
#
# + ----------------------------------------------------------------- +
#

greaterThan(QT_MAJOR_VERSION, 4) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
    QT += widgets x11extras printsupport
} else {
    QTVER_SUFFIX =
}


DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QTANGO_PRINTINFO
CONFIG +=  dbus debug

QT += dbus

VERSION_HEX = 0x060405
VERSION = 6.4.5
VER_MAJ = 6
VER_MIN = 4
VER_FIX = 5

DEFINES += QTANGO_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    QTANGO_VERSION=$${VERSION_HEX} \
    VER_MAJ=$${VER_MAJ} \
    VER_MIN=$${VER_MIN} \
    VER_FIX=$${VER_FIX}

unix:INCLUDEPATH += \
    $${QWT_HOME}/include \
    $${QWT_HOME_USR}/include/qwt

QMAKE_CXXFLAGS -= -std=gnu++0x
QMAKE_CXXFLAGS = -std=c++11 -Wall

freebsd-g++ {
QMAKE_CXXFLAGS += -std=c++11 -Wall
}

freebsd-clang{
    message( )
    message( *)
    message( * Compiling under FreeBSD)
    message( * :-P)
    message( )

    OMNI_HOME = /usr/local

    unix:LIBS -= -ldl
}

MOC_DIR = moc
OBJECTS_DIR = objects
FORMS_DIR = ui
LANGUAGE = C++
TEMPLATE = app
UI_DIR = src
QMAKE_DEL_FILE = rm \
    -rf
QMAKE_CLEAN = moc \
    obj \
    Makefile \
    *.tag

QMAKE_EXTRA_TARGETS += docs
LIB_DIR = $${INSTALL_ROOT}/lib
QTANGOLIBS = -lQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX} \
    -lQTango$${VER_MAJ}$${QTVER_SUFFIX} \
    -lQTangoCore$${VER_MAJ}$${QTVER_SUFFIX} \
    -lqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}

INC_DIR = $${INSTALL_ROOT}/include/qtango6

SHAREDIR = $${INSTALL_ROOT}/share

docs.commands = doxygen \
    Doxyfile;

unix:INCLUDEPATH += $${INC_DIR} \
    $${QWT_INCLUDES} \
    $${QWT_INCLUDES_USR}

unix:LIBS += -L$${LIB_DIR} \
    $${QTANGOLIBS}

!packagesExist($${QWT_PKGCONFIG}){
    unix:LIBS += -L$${QWT_HOME_USR}/lib -l$${QWT_LIB}$${QTVER_SUFFIX}
}

DISTFILES += \
    $$PWD/messages.pri
