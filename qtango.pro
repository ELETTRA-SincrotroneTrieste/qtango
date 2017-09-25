TEMPLATE = subdirs

dbus_lib.subdir = dbus
dbus_lib.target = sub-dbus

qtcontrols_lib.subdir = qtcontrols
qtcontrols_lib.target = sub-qtcontrols

qtangocore_lib.subdir = qtangocore
qtangocore_lib.target = sub-qtangocore
qtangocore_lib.depends = sub-qtcontrols sub-dbus

qtango_lib.subdir = qtango
qtango_lib.target = sub-qtango
qtango_lib.depends = sub-qtcontrols sub-qtangocore

qtcontrols_plugin.subdir = qtcontrols/plugins
qtcontrols_plugin.target = sub-qtcontrols-plugin
qtcontrols_plugin.depends = sub-qtcontrols

qtango_plugin.subdir = qtango/plugins
qtango_plugin.target = sub-qtango-plugin
qtango_plugin.depends = sub-qtango

qtango_projectwizard.subdir = tools/qtangoprojectwizard
qtango_projectwizard.target = sub-qtangoprojectwizard
qtango_projectwizard.depends = sub-qtango

SUBDIRS =  dbus_lib qtcontrols_lib qtangocore_lib qtango_lib qtcontrols_plugin qtango_plugin tools/qtangoprojectwizard

