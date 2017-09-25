# ***************************************************************************
# *   $Id: ETest.py,v 1.3 2010-01-11 11:19:16 fabio Exp $
# *                                                                         *
# *   Copyright (C) 2010 by Fabio Asnicar  fabio.asnicar@elettra.trieste.it *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# ***************************************************************************

import sys
from PyQt4 import QtCore, QtGui
from qtcontrols import ECircularGauge, ELinearGauge, ELabel, ELed, EFlag, EApplyNumeric

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(QtCore.QSize(QtCore.QRect(0,0,570,490).size()).expandedTo(MainWindow.minimumSizeHint()))

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)

        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")

        #
        # qtcontrols widget samples
        #
        self.groupBox_qtcontrols = QtGui.QGroupBox(self.centralwidget)
        self.groupBox_qtcontrols.setGeometry(QtCore.QRect(21,9,259,461))

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_qtcontrols.sizePolicy().hasHeightForWidth())
        self.groupBox_qtcontrols.setSizePolicy(sizePolicy)
        self.groupBox_qtcontrols.setObjectName("qtcontrols")

        self.eLed = ELed(self.groupBox_qtcontrols)
        self.eLed.setGeometry(QtCore.QRect(20,30,30,30))
        self.eLed.setColor(QtCore.Qt.red)

        self.eLedR = ELed(self.groupBox_qtcontrols)
        self.eLedR.setGeometry(QtCore.QRect(60,30,40,30))
        self.eLedR.setColor(QtCore.Qt.red)
        self.eLedR.setAngle(25)
        self.eLedR.setRectangular(1)
        self.eLedR.setGradientEnabled(1);
        self.eLedR.setLinearGradient(1);
        self.eLedR.setLedWidth(30)
        self.eLedR.setLedHeight(10)

        self.eLedO = ELed(self.groupBox_qtcontrols)
        self.eLedO.setGeometry(QtCore.QRect(100,30,30,30))
        self.eLedO.setColor(QtCore.Qt.red)
        self.eLedO.setAngle(25)
        self.eLedO.setRectangular(0)
        self.eLedO.setGradientEnabled(0);
        self.eLedO.setLinearGradient(1);

        self.eLedO.setLedWidth(15)
        self.eLedO.setLedHeight(30)

        self.eLabel = ELabel(self.groupBox_qtcontrols)
        self.eLabel.setGeometry(QtCore.QRect(20,70,87,28))
        self.eLabel.setTrueString("ENABLED")
        self.eLabel.setFalseString("DISABLED")
        self.eLabel.setTrueColor(QtCore.Qt.green)
        self.eLabel.setFalseColor(QtCore.Qt.lightGray)
        self.eLabel.setText(QtCore.QString("  ###  "))

        self.eFlag = EFlag(self.groupBox_qtcontrols)
        self.eFlag.setGeometry(QtCore.QRect(20,120,140,50))
        self.eFlag.setNumRows(2)
        self.eFlag.setNumColumns(3)
        self.eFlag.setBooleanDisplay(0, "0", "1", QtCore.Qt.cyan, QtCore.Qt.yellow);
        self.eFlag.setBooleanDisplay(1, "0", "1", QtCore.Qt.cyan, QtCore.Qt.yellow);
        self.eFlag.setBooleanDisplay(2, "0", "1", QtCore.Qt.cyan, QtCore.Qt.yellow);
        self.eFlag.setDisplayMask("0,1,2,4,5,6")
        self.eFlag.setValue(QtCore.QVariant(1))

        self.eLinearGauge = ELinearGauge(self.groupBox_qtcontrols)
        self.eLinearGauge.setGeometry(QtCore.QRect(20,190,50,120))

        self.eCircularGauge = ECircularGauge(self.groupBox_qtcontrols)
        self.eCircularGauge.setGeometry(QtCore.QRect(120,240,130,130))

        self.eApplyNumeric = EApplyNumeric(self.groupBox_qtcontrols)
        self.eApplyNumeric.setGeometry(QtCore.QRect(20,400,130,40))
        self.eApplyNumeric.setMaximum(555.5)
        self.eApplyNumeric.setMinimum(-333.33)
        self.eApplyNumeric.setIntDigits(3);
        self.eApplyNumeric.setDecDigits(2)

        #
        # In Out values for testing purpose
        #
        self.groupBox_InOutValues = QtGui.QGroupBox(self.centralwidget)
        self.groupBox_InOutValues.setGeometry(QtCore.QRect(291,9,259,461))

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_InOutValues.sizePolicy().hasHeightForWidth())
        self.groupBox_InOutValues.setSizePolicy(sizePolicy)

        self.checkBox_eLedValue = QtGui.QCheckBox(self.groupBox_InOutValues)
        self.checkBox_eLedValue.setGeometry(QtCore.QRect(20,30,100,22))

        self.comboBox_eFlagValue = QtGui.QComboBox(self.groupBox_InOutValues)
        self.comboBox_eFlagValue.setGeometry(QtCore.QRect(20,120,80,22))
        self.comboBox_eFlagValue.addItem("1")
        self.comboBox_eFlagValue.addItem("2")
        self.comboBox_eFlagValue.addItem("4")
        self.comboBox_eFlagValue.addItem("8")
        self.comboBox_eFlagValue.addItem("16")
        self.comboBox_eFlagValue.addItem("32")
        self.comboBox_eFlagValue.addItem("64")

        self.horizontalSlider_eLinearGaugeValue = QtGui.QSlider(self.groupBox_InOutValues)
        self.horizontalSlider_eLinearGaugeValue.setGeometry(QtCore.QRect(30,200,200,16))
        self.horizontalSlider_eLinearGaugeValue.setMinimum(-15)
        self.horizontalSlider_eLinearGaugeValue.setMaximum(15)
        self.horizontalSlider_eLinearGaugeValue.setProperty("value",QtCore.QVariant(5))
        self.horizontalSlider_eLinearGaugeValue.setOrientation(QtCore.Qt.Horizontal)

        self.horizontalSlider_eCircularGaugeValue = QtGui.QSlider(self.groupBox_InOutValues)
        self.horizontalSlider_eCircularGaugeValue.setGeometry(QtCore.QRect(30,280,200,16))
        self.horizontalSlider_eCircularGaugeValue.setMinimum(-15)
        self.horizontalSlider_eCircularGaugeValue.setMaximum(15)
        self.horizontalSlider_eCircularGaugeValue.setProperty("value",QtCore.QVariant(5))
        self.horizontalSlider_eCircularGaugeValue.setOrientation(QtCore.Qt.Horizontal)

        self.label_eApplyNumericValue = QtGui.QLabel(self.groupBox_InOutValues)
        self.label_eApplyNumericValue.setGeometry(QtCore.QRect(70,390,130,30))
        self.label_eApplyNumericValue.setFrameShape(QtGui.QFrame.StyledPanel)
        self.label_eApplyNumericValue.setFrameShadow(QtGui.QFrame.Sunken)
        self.label_eApplyNumericValue.setAlignment(QtCore.Qt.AlignCenter)

        self.checkBox_eLabelValue = QtGui.QCheckBox(self.groupBox_InOutValues)
        self.checkBox_eLabelValue.setGeometry(QtCore.QRect(20,70,110,22))

        MainWindow.setCentralWidget(self.centralwidget)

        self.label_1 = QtGui.QLabel(self.groupBox_qtcontrols)
        self.label_1.setGeometry(QtCore.QRect(160,30,57,30))
        self.label_2 = QtGui.QLabel(self.groupBox_qtcontrols)
        self.label_2.setGeometry(QtCore.QRect(130,70,57,30))
        self.label_3 = QtGui.QLabel(self.groupBox_qtcontrols)
        self.label_3.setGeometry(QtCore.QRect(180,130,57,30))
        self.label_4 = QtGui.QLabel(self.groupBox_qtcontrols)
        self.label_4.setGeometry(QtCore.QRect(80,200,90,30))
        self.label_5 = QtGui.QLabel(self.groupBox_qtcontrols)
        self.label_5.setGeometry(QtCore.QRect(20,330,100,30))
        self.label_6 = QtGui.QLabel(self.groupBox_qtcontrols)
        self.label_6.setGeometry(QtCore.QRect(160,400,100,40))
        self.label_7 = QtGui.QLabel(self.groupBox_InOutValues)
        self.label_7.setGeometry(QtCore.QRect(70,210,150,30))
        self.label_8 = QtGui.QLabel(self.groupBox_InOutValues)
        self.label_8.setGeometry(QtCore.QRect(70,290,170,30))
        self.label_9 = QtGui.QLabel(self.groupBox_InOutValues)
        self.label_9.setGeometry(QtCore.QRect(70,410,130,40))
        self.label_10 = QtGui.QLabel(self.groupBox_InOutValues)
        self.label_10.setGeometry(QtCore.QRect(20,150,200,22))

        #self.menubar = QtGui.QMenuBar(MainWindow)
        #self.menubar.setGeometry(QtCore.QRect(0,0,576,29))
        #MainWindow.setMenuBar(self.menubar)

        #self.statusbar = QtGui.QStatusBar(MainWindow)
        #MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QObject.connect(self.checkBox_eLedValue,QtCore.SIGNAL("toggled(bool)"),self.updateELed)
        QtCore.QObject.connect(self.checkBox_eLabelValue,QtCore.SIGNAL("toggled(bool)"),self.updateELabel)
        QtCore.QObject.connect(self.comboBox_eFlagValue,QtCore.SIGNAL("currentIndexChanged(QString)"),self.updateEFlag)
        QtCore.QObject.connect(self.eApplyNumeric,QtCore.SIGNAL("clicked(double)"),self.label_eApplyNumericValue.setNum)
        QtCore.QObject.connect(self.horizontalSlider_eLinearGaugeValue,QtCore.SIGNAL("valueChanged(int)"),self.eLinearGauge.setValue)
        QtCore.QObject.connect(self.horizontalSlider_eCircularGaugeValue,QtCore.SIGNAL("valueChanged(int)"),self.eCircularGauge.setValue)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "MainWindow", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_qtcontrols.setTitle(QtGui.QApplication.translate("MainWindow", "qtcontrols", None, QtGui.QApplication.UnicodeUTF8))
        self.label_1.setText(QtGui.QApplication.translate("MainWindow", "ELed", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("MainWindow", "ELabel", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("MainWindow", "EFlag", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("MainWindow", "ELinearGauge", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("MainWindow", "ECircularGauge", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("MainWindow", "EApplyNumeric", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_InOutValues.setTitle(QtGui.QApplication.translate("MainWindow", "for testing", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox_eLedValue.setText(QtGui.QApplication.translate("MainWindow", "ELed value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("MainWindow", "ELinearGauge value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("MainWindow", "ECircularGauge value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("MainWindow", "EApplyNumeric value", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("MainWindow", "Display mask: 0,1,2, 4,5,6", None, QtGui.QApplication.UnicodeUTF8))
        self.label_eApplyNumericValue.setText(QtGui.QApplication.translate("MainWindow", "---", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBox_eLabelValue.setText(QtGui.QApplication.translate("MainWindow", "ELabel value", None, QtGui.QApplication.UnicodeUTF8))

    def updateELed(self, v):
        if v:
               self.eLed.setColor(QtCore.Qt.green)
               self.eLedR.setColor(QtCore.Qt.green)
               self.eLedO.setColor(QtCore.Qt.green)
        else:
               self.eLed.setColor(QtCore.Qt.red)
               self.eLedR.setColor(QtCore.Qt.red)
               self.eLedO.setColor(QtCore.Qt.red)

    def updateELabel(self, v):
        self.eLabel.setValue(QtCore.QVariant(v))

    def updateEFlag(self, v):
        self.eFlag.setValue(QtCore.QVariant(v))


if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    MainWindow = QtGui.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
