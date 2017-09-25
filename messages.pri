# This file is included by by qtango/qtango.pro qtcontrols/qtcontrols.pro
# qtangocore/qtangocore.pro. It's not included by qtango.pri and it is not
# part of the installed files.
#
#
message("")
message("QTango: **  + ----------------------   qmake options   ------------------------------+")
message("            |                                                                        |")
message("QTango: *** | call qmake with the following options to indicate where necessary      |")
message("QTango: *** | libraries are installed:                                               |")
message("QTango: *** | - TANGO_HOME=/usr/local/tango-9.1.0 - or some other path/to/tango -    |")
message("QTango: *** | - OMNI_HOME=/usr/local/omniorb-4.1.6 - or some other path/to/omniorb - |")
message("QTango: *** | - ZMQ_HOME=/usr/local/zeromq-4.0.7 - or some other path/to/zeromq -    |")
message("QTango: *** | - QWT_HOME=/usr/local/qwt-6.1.2  - or some other path/to/qwt -         |")
message("QTango: *** | - prefix=/usr/local/qtango - or some other qtango/install/dir -        |")
message("QTango: **  + -----------------------------------------------------------------------+")
message("")

message("QTango: *** TANGO_HOME is \"$${TANGO_HOME}\", call qmake TANGO_HOME=path/to/tango to change it")

# Omniorb installation dir
message("QTango: *** OMNI_HOME is \"$${OMNI_HOME}\", call qmake OMNI_HOME=path/to/omniorb to change it")

message("QTango: *** ZMQ_HOME is \"$${ZMQ_HOME}\", call qmake ZMQ_HOME=path/to/zeromq to change it")

message("QTango: *** QWT_HOME is \"$${QWT_HOME}\" call qmake QWT_HOME=path/to/qwt to change it")
message("")

message("QTango: *** qtango will be installed in \"$${INSTALL_ROOT}\"")
message("QTango: *** call qmake prefix=/usr/local/qtango (or something else) to change installation prefix.")
message("")

message("QTango: --> installation dir: \"$${INSTALL_ROOT}\" <-- .")
message("")
