# $Id: qtangocore.pro,v 1.77 2014-08-28 13:23:45 giacomo Exp $
# $Name $

include(../qtango.pri)

# widgets, qt5, for QDialog
QT += xml widgets printsupport

# DEFINES -= QT_NO_DEBUG_OUTPUT


FORMS = src/logdialog$${QTVER_SUFFIX}.ui

TEMPLATE = lib staticlib
SOURCES = src/tvariant.cpp \
    src/devicethread.cpp \
    src/taction.cpp \
    src/action_timer.cpp \ # src/textreader.cpp \
    src/tlogdialog.cpp \
    src/tutil.cpp \
    src/tlog.cpp \
    src/qv2attribute.cpp \
    src/subscriber_proxy.cpp \
    src/subscriber_auto_reconnect_timer.cpp \
    src/unsubscriber_proxy.cpp \ # src/tdevice_proxy.cpp \
    src/action_factory.cpp \
    src/device_thread_factory.cpp \
    src/device_factory.cpp \
    src/readersgroup.cpp \
    src/spectrumwriter.cpp \
    src/spectrumwriter_conf_helper.cpp \
    src/configuration.cpp \
    src/global_refresh_timer.cpp \
    src/qtangocore_read_stats.cpp \
    ../common/eng_notation.cpp \
    src/tapplication/tapplication.cpp \
    src/tapplication/tuniqueapplication.cpp \
    src/tactiondesc.cpp \
    src/targetexecutionevent.cpp \
    src/actionfactoryobjectfilter.cpp \
    utils/propertyreader.cpp \
    utils/propertyreaderprivate.cpp

HEADERS = src/qtangocore.h \
    src/qtangocore_common.h \
    src/qtangocore_macros.h \
    src/point_events.h \
    src/tvariant.h \ # src/tdevice_proxy.h \
    src/devicethread.h \
    src/taction.h \
    src/action_timer.h \ # src/textreader.h \
    src/tlogdialog.h \
    src/tutil.h \
    src/tlog.h \
    src/qv2attribute.h \
    src/subscriber_proxy.h \
    src/unsubscriber_proxy.h \
    src/subscriber_auto_reconnect_timer.h \
    src/action_factory.h \
    src/device_thread_factory.h \
    src/device_factory.h \
    src/action_creation_event.h \
    src/action_configuration_failed_event.h \
    src/readersgroup.h \
    src/spectrumwriter.h \
    src/spectrumwriter_conf_helper.h \
    src/configuration.h \
    src/taction_polling_period_change_event.h \
    src/action_polling_events.h \
    src/global_refresh_timer.h \
    src/qtangocore_read_stats.h \
    ../common/eng_notation.h \
    src/tapplication/tapplication.h \
    src/tapplication/tuniqueapplication.h \
    src/tactiondesc.h \
    src/targetexecutionevent.h \
    src/actionfactoryobjectfilter.h \
    utils/propertyreader.h \
    src/timagedata.h \
    src/timagedata_impl.h

# wrapper
HEADERS += src/wrapper/auto_configuration.h \
    src/wrapper/com_proxy_reader.h \
    src/wrapper/com_proxy_writer.h \
    src/wrapper/communication_handle.h \
    src/wrapper/simple_dataproxy.h \
    src/wrapper/source_validator.h \ # src/wrapper/attributes_configuration_manager.h \
    src/wrapper/proxy_interface.h \
    src/wrapper/enum2qstring.h

PRIVATE_HEADERS = \
    src/tapplication/private/tapplication_p.h \
    utils/propertyreaderprivate.h

PRIVATE_SOURCES = \
    src/tapplication/private/tapplication_p.cpp

SOURCES += src/wrapper/auto_configuration.cpp \
    src/wrapper/com_proxy_reader.cpp \
    src/wrapper/com_proxy_writer.cpp \
    src/wrapper/communication_handle.cpp \
    src/wrapper/source_validator.cpp \ # src/wrapper/attributes_configuration_manager.cpp \
    src/wrapper/proxy_interface.cpp \
    src/wrapper/enum2qstring.cpp

# qtango common functionalities for the widgets: v. 4.x.x
TARGET = QTangoCore$${VER_MAJ}$${QTVER_SUFFIX}
QMAKE_CLEAN += libQTangoCore$${VER_MAJ}$${QTVER_SUFFIX}*
DEFINES += QTANGO_PRINTINFO

# $${LIB_DIR} which is included in qmake.custom.4, is necessary for the panels, but not for the libraries
unix:LIBS -= -L$${LIB_DIR} \
    $${QTANGOLIBS}

unix:LIBS += -L../qtcontrols \
    -L ../dbus \
    -lqtcontrols$${VER_MAJ}$${QTVER_SUFFIX} \
    -lQTangoDBus$${VER_MAJ}$${QTVER_SUFFIX}

# the same goes for INCLUDEPATH
unix:INCLUDEPATH -= $${INC_DIR}
unix:INCLUDEPATH += src \
    ../qtcontrols/src \
    src/wrapper \
    src/qtango_wcommon \
    ../common \
    src/tapplication \
    ../hdb ../hdb/queries \
    ..

# doc generation and installation has been moved to qtango/qtango.pro since version 4.x
# doc removed also from INSTALLS += directive below.
DOC_DIR = $${SHAREDIR}/qtangocore
doc.commands = \
    doxygen \
    Doxyfile;
doc.files = doc/
doc.path = $${DOC_DIR}
lib.path = $${LIB_DIR}
lib.files = libQTangoCore$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VERSION}
lib.commands = ln \
    -sf \
    libQTangoCore$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VERSION} \
    $${LIB_DIR}/libQTangoCore$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VER_MAJ} \
    && \
    ln \
    -sf \
    libQTangoCore$${VER_MAJ}$${QTVER_SUFFIX}.so.$${VER_MAJ} \
    $${LIB_DIR}/libQTangoCore$${VER_MAJ}$${QTVER_SUFFIX}.so

inc.files = $${HEADERS} \
    src/TLogDialog \
    src/ui_logdialog$${QTVER_SUFFIX}.h \
    src/DeviceFactory \
    src/SpectrumWriter \
    src/ReadersGroup \
    src/tapplication/TApplication \
    src/TLog \
    src/tapplication/TUniqueApplication
inc.path = $${INC_DIR}
inc.commands = cp src/ui_logdialog$${QTVER_SUFFIX}.h $${INC_DIR}/

# QTango installs project include file into $${INC_DIR} on behalf of the applications that want to compile against the library
pri.files = ../qtango.pri
pri.path = $${INC_DIR}
styles.files = stylesheets/tinfo.css
styles.path = $${INSTALL_ROOT}/share/qtango/stylesheets

INSTALLS += lib inc styles  pri doc

QMAKE_EXTRA_TARGETS += plugin

# Private headers are added after inc.files has been defined, so that the private
# headers are not installed int the inc.path
HEADERS += $${PRIVATE_HEADERS}
SOURCES += $${PRIVATE_SOURCES}
