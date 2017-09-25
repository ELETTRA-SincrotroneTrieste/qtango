# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'form1.ui'
#
# Created: Thu Dec 9 13:23:02 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


import sys
from PyQt4.QtGui import *
from pyqtcontrols import *


class Form1(QDialog):
    def __init__(self,parent = None):
        QDialog.__init__(self,parent)

	self.layout = QGridLayout(self)

#	self.eFlag = EFlag(self)
#	self.eFlag.setNumRows(2)
#	self.eFlag.setNumColumns(2)
#	self.eFlag.setDisplayMask([1,0,2,3])
#	self.eFlag.setValue([True,False,True,False])
#	self.eFlag.setValue(5)
#	self.layout.addWidget(self.eFlag,0,0)

 #       self.eNumeric1 = ENumeric(self)
#	self.layout.addWidget(self.eNumeric1,1,0)
        
#	self.eNumeric2 = EApplyNumeric(self, 3,2)
#	self.layout.addWidget(self.eNumeric2,2,0)
        
	self.ab = EApplyButton(self)
	self.ab.setText('pippo')
	self.ab.valueModified(1)
	self.layout.addWidget(self.ab,0,0)

	self.eLed = ELed(self)
	self.eLed.setColor(Qt.red)
	self.layout.addWidget(self.eLed, 1, 0)

	self.eLG = ELinearGauge(self)
#	self.eLG.setOrientation(Qt.Horizontal)
	self.layout.addWidget(self.eLG, 2, 0)

	self.eCG = ECircularGauge(self)
	self.layout.addWidget(self.eCG, 3, 0)
	
	self.languageChange()

        self.resize(QSize(600,480).expandedTo(self.minimumSizeHint()))

    def __tr(self,s,c = None):
        return qApp.translate("Form1",s,c)

if __name__ == "__main__":
    a = QApplication(sys.argv)
    QObject.connect(a,SIGNAL("lastWindowClosed()"),a,SLOT("quit()"))
    w = Form1()
    w.show()
    sys.exit(w.exec_())
