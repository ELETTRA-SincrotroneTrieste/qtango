# $Id: qtango.pro,v 1.115 2014-08-28 13:23:45 giacomo Exp $
include(../qtango.pri)

QT += xml widgets x11extras
TEMPLATE = lib staticlib

DEFINES += QT_NO_DEBUG_OUTPUT
RESOURCES = src/treaderwriter/treaderwriter.qrc \
    src/tdbbrowser.qrc \
    src/qtango_wcommon/qtango_wcommon.qrc

# qtango common functionalities for the widgets: v. 4.x.x
FORMS += src/tplotlight/rtconfwidget.ui

SOURCES += src/qtango_wcommon/qtango_wcommon.cpp \
    src/qtango_wcommon/drag_drop_event.cpp \
    src/qtango_wcommon/source_metadata.cpp \
    src/qtango_wcommon/right_click_event.cpp \
    src/qtango_wcommon/helper_application.cpp \
    src/qtango_wcommon/copypaste.cpp \
    src/qtango_wcommon/copypaste_xml.cpp \
    src/qtango_wcommon/show_event.cpp \
    src/qtango_wcommon/hide_event.cpp \
    src/qtango_wcommon/showhide.cpp \
    src/qtango_wcommon/left_click_event.cpp \
    src/qtango_wcommon/tinfo_textbrowser.cpp \
    src/qtango_wcommon/info_notification_center.cpp \
    src/qtango_wcommon/html_notification_center.cpp \
    src/qtango_wcommon/enter_leave_event.cpp \
    src/qtango_wcommon/infowidget.cpp \
    src/qtango_wcommon/qtangohdb.cpp \
    src/qtango_wcommon/whatsthis.cpp \
    utils/qtangoizerprivate.cpp \
    utils/qtangoizer.cpp \
    utils/qtwriter.cpp \
    utils/tvaluefilter.cpp \
    src/plotadapter/tdbplotadapter.cpp \
    src/plotadapter/hdbplotadapter.cpp \
    src/plotadapter/dbplotbackground.cpp \
    src/plotadapter/dbplotadapterinterfaceprivate.cpp \
    src/plotadapter/dbplotadapterinterface.cpp \
    utils/ttextdbproperty.cpp \
    utils/tpropertylabel.cpp \
    src/plotadapter/dbcurveinterpolator.cpp \
    src/twidget.cpp \
    src/timage/timagewidget.cpp


HEADERS += src/qtango_wcommon/qtango_wcommon.h \
    src/qtango_wcommon/drag_drop_event.h \
    src/qtango_wcommon/source_metadata.h \
    src/qtango_wcommon/right_click_event.h \
    src/qtango_wcommon/helper_application.h \
    src/qtango_wcommon/copypaste.h \
    src/qtango_wcommon/copypaste_xml.h \
    src/qtango_wcommon/show_event.h \
    src/qtango_wcommon/hide_event.h \
    src/qtango_wcommon/showhide.h \
    src/qtango_wcommon/left_click_event.h \
    src/qtango_wcommon/tinfo_textbrowser.h \
    src/qtango_wcommon/info_notification_center.h \
    src/qtango_wcommon/html_notification_center.h \
    src/qtango_wcommon/enter_leave_event.h \
    src/qtango_wcommon/infowidget.h \
    src/qtango_wcommon/qtangohdb.h \
    src/qtango_wcommon/whatsthis.h \
    utils/qtangoizerprivate.h \
    utils/qtangoizer.h \
    utils/qtwriter.h \
    utils/tvaluefilter.h \
    src/plotadapter/tdbplotadapter.h \
    src/plotadapter/hdbplotadapter.h \
    src/plotadapter/dbplotbackground.h \
    src/plotadapter/dbplotadapterinterfaceprivate.h \
    src/plotadapter/dbplotadapterinterface.h \
    utils/ttextdbproperty.h \
    utils/tpropertylabel.h \
    src/plotadapter/dbcurveinterpolator.h \
    src/twidget.h \
    src/timage/timagewidget.h

HEADERS += src/qtango.h \
    src/QTango \
    src/view_trend.h \
    src/tdbbrowser.h \
    src/tlabel.h \
    src/tsimplelabel.h \
    src/tpushbutton.h \
    src/tapplynumeric.h \
    src/tnumeric.h \
    src/ttable.h \
    src/tlogbutton.h \
    src/tled.h \
    src/tlineargauge.h \
    src/tcirculargauge.h \
    src/tpixmap.h \ # src/twidgetgroup.h \
    src/tcheckbox.h \
    src/tcombobox.h \
    src/tlineedit.h \
    src/tspinbox.h \
    src/tdoublespinbox.h \
    src/twidgetgroup.h \
    src/tplotlight/plot_com_proxy_reader.h \
    src/tplotlight/background.h \
    src/tplotlight/tplotlight_marker.h \
    src/tplotlight/tplot_refresh_connector.h \
    src/tplotlight/trendplot.h \
    src/tplotlight/realtimeplot.h \
    src/tplotlight/rtconfwidget.h \
    src/tplotlight/rtconfdialog.h \
    src/treaderwriter/treaderpushbutton.h \
    src/treaderwriter/treaderwriter.h \
    src/qtangowidget.h \
    src/qtangodialog.h \
    src/qtangomainwindow.h \
    src/qtango_info_textbrowser.h \
    src/tspectrumbutton.h \
    src/ttextbrowser.h \
    src/widget_cleaner.h \
    src/tsplashscreen.h
SOURCES += src/view_trend.cpp \
    src/tdbbrowser.cpp \
    src/tlabel.cpp \
    src/tsimplelabel.cpp \
    src/tapplynumeric.cpp \
    src/tnumeric.cpp \
    src/tpushbutton.cpp \
    src/ttable.cpp \
    src/tlogbutton.cpp \
    src/tled.cpp \
    src/tlineargauge.cpp \
    src/tcirculargauge.cpp \
    src/tpixmap.cpp \ # src/twidgetgroup.cpp \
    src/tcheckbox.cpp \
    src/tcombobox.cpp \
    src/tlineedit.cpp \
    src/tspinbox.cpp \
    src/tdoublespinbox.cpp \
    src/twidgetgroup.cpp \
    src/tplotlight/plot_com_proxy_reader.cpp \
    src/tplotlight/background.cpp \
    src/tplotlight/tplotlight_marker.cpp \
    src/tplotlight/tplot_refresh_connector.cpp \
    src/tplotlight/trendplot.cpp \
    src/tplotlight/realtimeplot.cpp \
    src/tplotlight/rtconfwidget.cpp \
    src/tplotlight/rtconfdialog.cpp \
    src/treaderwriter/treaderpushbutton.cpp \
    src/treaderwriter/treaderwriter.cpp \
    src/qtangowidget.cpp \
    src/qtangodialog.cpp \
    src/qtangomainwindow.cpp \
    src/widget_cleaner.cpp \
    src/qtango_info_textbrowser.cpp \
    src/tspectrumbutton.cpp \
    src/ttextbrowser.cpp \
    src/tsplashscreen.cpp

# TImage START

#SOURCES +=        src/timagesnapshot.cpp \
#    src/timage.cpp

#HEADERS += \
#    src/TImageSnapshot \
#    src/timagesnapshot.h \
#    src/TImage \
#    src/timage.h

# TImage END

HEADERS += utils/qtwatcher.h
SOURCES += utils/qtwatcher.cpp
TARGET = QTango$${VER_MAJ}$${QTVER_SUFFIX}

# $${LIB_DIR} which is included in qtango.pri, is necessary for the panels, but not for the libraries
unix:LIBS -= -L$${LIB_DIR} \
    $${QTANGOLIBS}

unix:LIBS += -L../qtcontrols \
    -L../qtangocore \
    -L../hdb \
    -lqtcontrols$${VER_MAJ}$${QTVER_SUFFIX} \
    -lQTangoCore$${VER_MAJ}$${QTVER_SUFFIX}

# the same goes for INCLUDEPATH
unix:INCLUDEPATH -= $${INC_DIR}
unix:INCLUDEPATH += ../qtcontrols/src \
    ../ \
    ../qtcontrols/src/eplotlight \
    ../qtcontrols/src/eimage \
    ../qtangocore/src \
    ../qtangocore/src/wrapper \
    ../qtangocore/utils \
    src/qtango_wcommon \
    src/tplotlight \
    ../common \
    ../dbus \
    ../hdb ../hdb/queries ../hdb/widgets

DOC_DIR = $${SHAREDIR}/qtango
PY_DIR = $${LIB_DIR}/python2.5/site-packages/
QMAKE_CLEAN += libQTango$${VER_MAJ}$${QTVER_SUFFIX}*
QMAKE_EXTRA_TARGETS += plugin
lib.path = $${LIB_DIR}
lib.files = libQTango$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VERSION}
lib.commands = ln \
    -sf \
    libQTango$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VERSION} \
    $${LIB_DIR}/libQTango$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VER_MAJ} \
    && \
    ln \
    -sf \
    libQTango$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VER_MAJ} \
    $${LIB_DIR}/libQTango$${VER_MAJ}$${QTVER_SUFFIX}.so
inc.files = $${HEADERS} \
    src/ui_logdialog.h \
    src/TApplyNumeric \
    src/TNumeric \
    src/TGauge \
    src/TLabel \
    src/TLed \
    src/TLogButton \
    src/TNumeric \
    src/TPlot \
    src/TPushButton \
    src/TScalarControllers \
    src/TTable \
    src/TWidgetGroup \
    src/TCheckBox \
    src/TLinearGauge \
    src/TCircularGauge \
    src/TLineEdit \
    src/TSpinBox \
    src/TDoubleSpinBox \
    src/TDbBrowser \
    src/TComboBox \
    src/TPixmap \
    src/tplotlight/TPlotLightMarker \
    src/treaderwriter/TReaderWriter \
    src/QTangoInfoTextBrowser \
    src/TSimpleLabel \
    src/TSpectrumButton \
    src/TWidgetGroup \
    src/tplotlight/TRealtimePlot \
    src/qtango_wcommon/CopyPasteXml \
    src/TTextBrowser \
    src/TSplashScreen \
    src/plotadapter/DbCurveInterpolator \
    src/plotadapter/HdbPlotAdapter \
    src/plotadapter/TdbPlotAdapter \
    src/plotadapter/DbPlotAdapterInterface \
    utils/QTWatcher \
    utils/QTWriter \
    utils/QTangoizer \
    utils/TPropertyLabel \
    src/QTangoMainWindow \
    src/timage/TImageWidget

inc.path = $${INC_DIR}


py.files = pyqtango/PyQTango$${VER_MAJ}.py
py.path = $${PY_DIR}
doc.commands = \
    doxygen \
    Doxyfile;

doc.files = doc/
doc.path = $${DOC_DIR}

styles.files = stylesheets/qtango.css
styles.path = $${INSTALL_ROOT}/share/qtango/stylesheets

INSTALLS += lib \
    inc \
    doc \
    styles
