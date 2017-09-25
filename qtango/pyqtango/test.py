# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'form1.ui'
#
# Created: Thu Dec 9 13:23:02 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


import sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyTango import *
from PyQTango import *

class Form1(QDialog):
    def __init__(self,parent = None):
        QDialog.__init__(self,parent)

	self.layout = QVBoxLayout(self)

	self.ts = TLabel(self)
	self.layout.addWidget(self.ts)
	self.ts2 = TLabel(self)
	self.layout.addWidget(self.ts2)
	self.ts3 = TLabel(self)
	self.layout.addWidget(self.ts3)
	self.ts4 = TLabel(self)
	self.layout.addWidget(self.ts4)
	self.ts5 = TLabel(self)
	self.layout.addWidget(self.ts5)
	self.ts6 = TLabel(self)
	self.layout.addWidget(self.ts6)
	self.log = TLogButton(self)
	self.layout.addWidget(self.log)

	self.spec = TTable(self)
	self.spec.setNumRows(12)
	self.spec.setNumColumns(8)
	self.layout.addWidget(self.spec)

	#self.eap = TApplyNumeric(self,'eee')
	#self.eap.setIntDigits(4)
	#self.eap.setTarget('tangotest/1/1/short_scalar')
	#self.layout.addWidget(self.eap)

	self.ts.setSource('test/device/1/short_scalar')
	self.ts2.setSource('test/device/1/short_scalar')
	self.ts3.setSource('test/device/1/short_scalar')
	self.ts4.setSource('test/device/1/short_scalar')
	self.ts5.setSource('test/device/1/short_scalar')
#	self.ts6.setSource('test/device/1/short_slar')
	self.spec.setSource('sr/diagnostics/bpm_s/horpos')

#	self.spec.setSource('test/device/1/short_spectrum')
#	self.languageChange()

#        self.resize(QSize(600,480).expandedTo(self.minimumSizeHint()))
 #       self.clearWState(Qt.WState_Polished)

    def languageChange(self):
        self.setCaption(self.__tr("Form1"))


    def __tr(self,s,c = None):
        return qApp.translate("Form1",s,c)

if __name__ == "__main__":
	a = QApplication(sys.argv)
	#QObject.connect(a,SIGNAL("lastWindowClosed()"),a,SLOT("quit()"))
	w = Form1()
	w.show()
	sys.exit(w.exec_())
