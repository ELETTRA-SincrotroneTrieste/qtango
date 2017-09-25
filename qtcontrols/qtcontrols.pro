# $Id: qtcontrols.pro,v 1.105 2014-08-28 13:23:45 giacomo Exp $
include(../qtango.pri)

QT += widgets  x11extras printsupport opengl

RESOURCES = qtcontrols.qrc src/eimage/confdialog.qrc

DEFINES += QT_NO_DEBUG_OUTPUT

# if you want to enable by default scalable fonts on widgets that support them

# message("* comment or uncomment the line ")
# message("* DEFINES += SCALABLE_FONTS_ENABLED_BY_DEFAULT")
# message("* in qtcontrols.pro to have automatically disabled/enabled")
# message("* widgets' font scaling. Since version 4.x this directive is defined.")
# message("* If you comment out it, then you must explicitly")
# message("* enable font scaling on each widget supporting this feature.")
# message("* On the other hand, if the directive is defined, all widgets supporting")
# message("* font scaling feature are constructed with such property enabled")

# DEFINES += SCALABLE_FONTS_ENABLED_BY_DEFAULT

DEFINES += SHAREDIR=\"\\\"$${SHAREDIR}\\\"\"

SOURCES	+= \
	src/enumeric.cpp \
	src/eflag.cpp \
	src/eapplybutton.cpp \
	src/euniqueapplication.cpp \
	src/euniqueapplication_adapter.cpp \
	src/euniqueapplication_interface.cpp \
	src/eapplynumeric.cpp \
	src/leftclick_with_modifiers_eater.cpp \
	src/elabel.cpp \
	src/eled.cpp \
	src/egauge.cpp \
	src/eapplicationlauncher.cpp \
	src/eapplicationbutton.cpp \
	src/scrollzoomer.cpp \
	src/scrollbar.cpp \
	src/eplotlight/ezoomer.cpp \
	src/elettracolors.cpp \
	src/earrow.cpp \
	src/eplotlight/eplotlight_base.cpp \
	src/eplotlight/eplotcurve.cpp \
	src/eplotlight/ecurvedata.cpp \
	src/eplotlight/eplotlight.cpp \
	src/eplotlight/eplotlight_marker.cpp \
	src/eplotlight/snapshot.cpp \
	src/eplotlight/plotsaver.cpp \
	src/estackedwidget_container.cpp \
	src/dec_int_from_format.cpp \
	src/number_delegate.cpp \
    src/econtexthelp.cpp \
	src/estringlisteditor.cpp \
	src/eplotlight/eplot_configuration_widget.cpp   \
	src/fontscalingwidget.cpp \
#	src/simplefontscalingwidget.cpp \
	src/fontscaling_eventfilter.cpp \
	src/epushbutton.cpp \
	src/espinboxes.cpp \
	src/elineedit.cpp \
        src/esimplelabel.cpp \
	src/ewidget.cpp \
        src/estyleloader.cpp \
        ../common/eng_notation.cpp \
    src/eimage/eimagegl.cpp \
    src/eimage/eimagemouseeventinterface.cpp \
    src/eimage/eimagewidgetbase.cpp \
    src/eimage/confdialog.cpp \
    src/eimage/colortablemap.cpp \
    src/eimage/eimagewidget.cpp

HEADERS	+= \
	src/econstants.h \
	src/macros.h \
        src/enumeric.h \
	src/leftclick_with_modifiers_eater.h \
	src/eflag.h \
	src/eapplybutton.h \
	src/euniqueapplication.h \
	src/euniqueapplication_adapter.h \
	src/euniqueapplication_interface.h \
	src/eapplynumeric.h \
	src/elabel.h \
	src/eled.h \
	src/egauge.h \
	src/eapplicationlauncher.h \
	src/eapplicationbutton.h \
	src/scrollzoomer.h \
	src/scrollbar.h \
	src/eplotlight/ezoomer.h \
	src/elettracolors.h \
	src/earrow.h \
	src/eplotlight/eplotlight_base.h \
	src/eplotlight/eplotcurve.h \
	src/eplotlight/ecurvedata.h \
	src/eplotlight/eplotlight.h \
	src/eplotlight/eplotlight_marker.h \
	src/eplotlight/snapshot.h \
	src/eplotlight/plotsaver.h \
	src/estackedwidget_container.h \
	src/dec_int_from_format.h \
	src/number_delegate.h \
	src/int_delegate.h \
	src/float_delegate.h \
	src/string_delegate.h \
    src/econtexthelp.h \
	src/right_click_menuable.h \
	src/estringlisteditor.h \
	src/eplotlight/eplot_configuration_widget.h   \
	src/fontscalingwidget.h \
#	src/simplefontscalingwidget.h \
	src/fontscaling_eventfilter.h \
	src/epushbutton.h \
	src/espinboxes.h \
	src/elineedit.h \
	src/esimplelabel.h \
        ../common/eng_notation.h \
	src/ewidget.h \
        src/estyleloader.h \
    src/eimage/eimagegl.h \
    src/eimage/eimagemouseeventinterface.h \
    src/eimage/eimagewidgetbase.h \
    src/eimage/confdialog.h \
    src/eimage/colortablemap.h \
    src/eimage/eimagewidget.h

# TImage Start

#SOURCES +=     src/imagepicturemode.cpp \
#    src/imagemarker.cpp \
#    src/imagemanipulator.cpp \
#    src/imagehistogram.cpp \
#    src/eimagescreen.cpp \
#    src/eimagebase.cpp

#HEADERS +=  \
#    src/imagepicturemode.h \
#    src/imagemarker.h \
#    src/imagemanipulator.h \
#    src/imagehistogram.h \
#    src/eimagescreen.h \
#    src/eimagebase.h

# TImage end

FORMS += src/estringlisteditor.ui src/eplotlight/configuration.ui src/eimage/confdialog.ui

TEMPLATE = lib staticlib

TARGET = qtcontrols$${VER_MAJ}$${QTVER_SUFFIX}

# $${LIB_DIR} which is included in qtango.pri, is necessary for the panels, but not for the libraries
unix:LIBS -= -L$${LIB_DIR} $${QTANGOLIBS}
unix:LIBS += -L../dbus -L/usr/X11R6/lib -lQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX} -lXmu -lX11

# the same goes for INCLUDEPATH
unix:INCLUDEPATH -= $${INC_DIR}
unix:INCLUDEPATH += /usr/include/X11 ../common/ .. src ../dbus src/eimage src/eplotlight


DOC_DIR = $${SHAREDIR}/qtcontrols
PY_DIR  = $${LIB_DIR}/python2.3/site-packages/

QMAKE_CLEAN += libqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}*
QMAKE_EXTRA_TARGETS += plugin

data.path       = $${SHAREDIR}/qtcontrols/data
data.files = src/eplotlight/saveDataFormatHelp.html

lib.path	= $${LIB_DIR}
lib.files	= libqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VERSION}
lib.commands	= ln -sf libqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VERSION} $${LIB_DIR}/libqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VER_MAJ} && ln -sf libqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VER_MAJ} $${LIB_DIR}/libqtcontrols$${VER_MAJ}$${QTVER_SUFFIX}.so

inc.files  = $${HEADERS} src/EApplyButton src/EApplyNumeric src/EFlag src/EGauge src/ELabel src/ELed src/EPlot
inc.files += src/EApplicationButton src/EApplicationLauncher src/ENumeric src/QtControls src/EArrow src/EContextHelp
inc.files += src/eplotlight/EPlotLight src/eplotlight/EPlotLightBase src/eplotlight/EPlotLightMarker src/ESimpleLabel
inc.files += src/EWidget src/EStyleLoader src/ui_configuration.h
inc.path = $${INC_DIR}
inc.commands = cp src/ui_estringlisteditor.h src/ui_configuration.h $${INC_DIR}/

py.files = pyqtcontrols/PyQtControls$${VER_MAJ}$${QTVER_SUFFIX}.py
py.path = $${PY_DIR}

# doc generation and installation has been moved to qtango/qtango.pro since version 4.x
# doc removed also from INSTALLS += directive below.

doc.commands = doxygen Doxyfile;
doc.files = doc/
doc.path = $${DOC_DIR}


INSTALLS += lib inc doc data

PRIVATE_HEADERS =
PRIVATE_SOURCES =

# Private headers are added after inc.files has been defined, so that the private
# headers are not installed int the inc.path
HEADERS += $${PRIVATE_HEADERS}
SOURCES += $${PRIVATE_SOURCES}
