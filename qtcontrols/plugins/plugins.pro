include(../../qtango.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets x11extras printsupport
    QT += designer
} else {
    CONFIG += plugin
    CONFIG += designer
}

lessThan(QT_MAJOR_VERSION, 5) {
	CONFIG += designer
}

SOURCES	+= qtcontrolsplugin.cpp
HEADERS	+= qtcontrolsplugin.h
RESOURCES += qtcontrolsplugin.qrc

TEMPLATE = lib

TARGET = qtcontrols$${VER_MAJ}$${QTVER_SUFFIX}plugin

QMAKE_CLEAN += designer/libqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}plugin*
CONFIG	+= qt thread warn_on

# $${LIB_DIR} which is included in qmake.custom.4, is necessary for the panels, but not for the libraries
unix:LIBS -= -L$${LIB_DIR} $${QTANGOLIBS}
unix:LIBS += -L.. -lqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}

# the same goes for INCLUDEPATH
INCLUDEPATH -= $${INC_DIR}
INCLUDEPATH += ../src ../src/eplotlight ../../dbus ../src/eimage

target.path = $${INSTALL_ROOT}/lib/plugins/designer

INSTALLS += target
