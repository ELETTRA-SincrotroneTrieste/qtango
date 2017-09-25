DEFINES += QTANGOWIDGET_LIBRARY

# QTangoWidget files

SOURCES += qtangowidgetplugin.cpp

HEADERS += qtangowidgetplugin.h \
        qtangowidget_global.h \
        qtangowidgetconstants.h

# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/var/tmp/portage/dev-qt/qt-creator-4.0.0/work/qt-creator-opensource-src-4.0.0

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/home/giacomo/devel/utils/svn/qtango/trunk/qtango/qtcreator/qtcreator-build

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
# USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = QTangoWidget
QTC_LIB_DEPENDS += \
    # nothing here at this time

QTC_PLUGIN_DEPENDS += \
    coreplugin

QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

LIBS += -L/var/tmp/portage/dev-qt/qt-creator-4.0.0/work/qt-creator-opensource-src-4.0.0/lib64/qtcreator

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
