# $Id: plugins.pro,v 1.31 2014-08-28 13:23:45 giacomo Exp $
include(../../qtango.pri)

TEMPLATE = lib

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets x11extras printsupport
    QT += designer
} else {
    CONFIG += plugin
    CONFIG += designer
}

# DEFINES -= QT_NO_DEBUG_OUTPUT

UI_DIR = forms

FORMS = \
	forms/tdbbrowserdialog.ui \
	forms/editsourcedialog.ui \
	forms/editsourcetargetdialog.ui \
	forms/tlabelbooleditor.ui \
	forms/ttablebooleditor.ui \
	forms/tledbooleditor.ui 

RESOURCES = qtangoplugin.qrc

SOURCES	= \
	forms/tledbooleditor.cpp \
	forms/tlabelbooleditor.cpp \
	forms/ttablebooleditor.cpp \
	forms/edittargetdialog.cpp \ 
	forms/editsourcedialog.cpp \
        qtangoplugin.cpp \
        pointeditor.cpp

HEADERS	= qtangoplugin.h \
        pointeditor.h \
	forms/tledbooleditor.h \
	forms/tlabelbooleditor.h \
	forms/ttablebooleditor.h \
	forms/editsourcedialog.h \
	forms/edittargetdialog.h

TARGET  = qtango$${VER_MAJ}$${QTVER_SUFFIX}plugin

lessThan(QT_MAJOR_VERSION, 5) {
	        CONFIG += designer
}

# $${LIB_DIR} which is included in qmake.custom.4, is necessary for the panels, but not for the libraries
unix:LIBS -= -L$${LIB_DIR} $${QTANGOLIBS}
unix:LIBS += -L../ -L../../qtangocore -L../../qtcontrols -L../../dbus -L../../hdb $${QTANGOLIBS}

# the same goes for INCLUDEPATH
unix:INCLUDEPATH -= $${INC_DIR}
unix:INCLUDEPATH += ../src ../src/tplotlight ../src/treaderwriter \
    ../../qtcontrols/src ../../qtangocore/src ../../qtcontrols/src/eplotlight \
    ../../qtcontrols/src/eimage \
    ../../qtangocore/src/wrapper ./forms ../../qtangocore/src/qtango_wcommon \
    ../src/qtango_wcommon/ ../../common ../../dbus \
    ../utils

target.path = $${INSTALL_ROOT}/lib/plugins/designer

# include files for developing plugins
plugin_inc.files = qtangoplugin.h \
    qtangoplugin.cpp \
    qtangoplugin.qrc \
    forms/editsourcedialog.h \
    forms/edittargetdialog.h \
    forms/tlabelbooleditor.h \
    forms/tledbooleditor.h \
    forms/ttablebooleditor.h \
    forms/ui_ttablebooleditor.h \
    forms/ui_tledbooleditor.h \
    forms/ui_tlabelbooleditor.h

plugin_inc.path = $${INC_DIR}/plugins


INSTALLS += target plugin_inc
