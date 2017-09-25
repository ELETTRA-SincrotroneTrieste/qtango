include(../../../../qmake.custom.4)

TARGET = bin/statsAnalyzer

INCLUDEPATH += ../../src  ../../src/treaderwriter ../../src/tplotlight ../../../qtcontrols/src ../../../qtcontrols/src/eplotlight ../../../qtangocore/src/wrapper  ../../../qtangocore/src

# remove /runtime/elettra/lib to build the library
unix:LIBS -= -L$${LIB_DIR} $${QTANGOLIBS}

# the same goes for INCLUDEPATH
INCLUDEPATH -=  $${INC_DIR}

unix:LIBS += -L../.. -L../../../qtcontrols -L../../../qtangocore -L../../../qtango -lQTango -lqtcontrols -lQTangoCore



# Input
HEADERS += analyzer.h \
           read_data.h \
           data_loader.h \
           analyzer_plot.h \
           text_browser.h


SOURCES += analyzer.cpp \
           main.cpp \
           read_data.cpp \
           data_loader.cpp \
           analyzer_plot.cpp \
           text_browser.cpp
	   
inst.files = $${TARGET}
inst.path = $${INSTALL_ROOT}/fermi/panels/bin

INSTALLS += inst
