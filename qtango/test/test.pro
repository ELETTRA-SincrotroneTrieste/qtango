include(../../qtango.pri)

INCLUDEPATH += ../../src ../../src/tplotlight  ../../src/treaderwriter ../../../qtcontrols/src ../../../qtcontrols/src/eplotlight ../../../qtangocore/src/wrapper  ../../../qtangocore/src ../../src/qtango_wcommon ../../../common ../../../qtangocore/src/tapplication ../../../dbus

MOC_DIR = moc
OBJECTS_DIR = obj

# the same goes for INCLUDEPATH
INCLUDEPATH -=  $${INC_DIR}

# remove /runtime/elettra/lib to build the library 
unix:LIBS -= -L$${LIB_DIR}

unix:LIBS += -L../.. -L../../../qtcontrols -L../../../qtangocore -L../../../qtango -L../../../dbus
