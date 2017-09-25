from PyTango import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from PyQtControls import *
from pyttk import *

import time

class TDataManager:
	def getData(self):
		return None
	
	def setTarget(self, point, controller):
		controller.subscribe(self, point)

class TLabel(TDataViewer, ELabel):
	def __init__(self, parent = None):
		ELabel.__init__(self,parent)
		TDataViewer.__init__(self)
		self.quality = AttrQuality.ATTR_VALID;
		self.stateColors = list()
		for i in range(14):
			self.stateColors.append(Qt.yellow)
		self.stateColors[DevState.ON] = Qt.green
		self.stateColors[DevState.ALARM] = Qt.red
		self.stateColors[DevState.FAULT] = Qt.red
		self.setText("No Link")

	def setSource(self, point, data = None, mode = 'AUTO_REFRESH', period = 1000):
		try:
			TDataViewer.setSource(self, str(point), data, mode, period)
		except (DevFailed, Communication, ConnectionFailed), exc:
			self.setMessage((time.ctime(), exc))
	
	def setValue(self, val):
		if isinstance(val, list):
			return
		if isinstance(val, AttributeValue):
			self.quality = val.quality
			self.data = val.value
		else:
			self.data = val
		self.update()
			
	def getValue(self):
		return self.text()
	
	def setStateColors(self, colors):
		if isinstance(colors, list):
			if isinstance(colors[0], QColor):
				if len(colors) == 14:
					self.stateColors = colors
					self.update()
					return
		raise TypeError()
	
	def setStateColor(self, state, color):
		if isinstance(colors, QColor):
			if isinstance(state, DevState):
				self.stateColors[state] = color
				self.update()
				return
		raise TypeError()
	
	def paintEvent(self, ev):
		ELabel.paintEvent(self, ev)
		try:
			self.data
		except:
			return
		if isinstance(self.data, bool):
			ELabel.setValue(self, self.data, False)
		elif isinstance(self.data, DevState):
			self.setText(str(DevState.values[self.data]))
		else:
			try: 
				self.format.index('%')
				self.setText(self.format % self.data)
			except:
				self.setText(str(self.data))
				
		self.setToolTip(self.message)

		if self.quality == AttrQuality.ATTR_INVALID:
			self.setText(QString("######"))
			self.palette().setColor(QPalette.Background, Qt.white)
		
		if isinstance(self.data, DevState):
			self.palette().setColor(QPalette.Background, self.stateColors[self.data])
			return
		elif isinstance(self.data, bool):
			ELabel.display(self)
			return							        

		if self.quality == AttrQuality.ATTR_VALID:
			if not isinstance(self.data, DevState):
				self.palette().setColor(QPalette.Background, Qt.white)
		elif self.quality == AttrQuality.ATTR_ALARM:
			self.palette().setColor(QPalette.Background, Qt.red)
#		elif self.quality == AttrQuality.ATTR_WARNING or self.quality == AttrQuality.ATTR_CHANGING:
#			self.palette().setColor(QPalette.Background, Qt.yellow)

class TTable(TDataViewer, EFlag):
	def __init__(self, parent = None, classType=ELabel):
        	EFlag.__init__(self,parent,classType)
		TDataViewer.__init__(self)
		for c in self.cells:
			c.setText('No Link')
		self.quality = AttrQuality.ATTR_VALID;
	
	def setSource(self, point, data = None, mode = 'AUTO_REFRESH', period = 1000):
		try:
			TDataViewer.setSource(self, str(point), data, mode, period)
		except (DevFailed, Communication, ConnectionFailed), exc:
			self.setMessage((time.ctime(), exc))
	
	def setValue(self, val):
		if isinstance(val, AttributeValue):
			self.quality = val.quality
			val = val.value
		else:
			self.quality = AttrQuality.ATTR_VALID
		if not isinstance(val, list):
			print len(val)
			EFlag.setValue(self, val, False)
		else:
			self.data = val
		self.update()
		
	def paintEvent(self, ev):
		EFlag.paintEvent(self, ev)
		try:
			self.data
		except:
			return
	
		if isinstance(self.data[0], bool):
			EFlag.setValue(self, self.data, True)
		else:
			for i, cell in enumerate(self.cells):
				try:
					self.format.index('%')
					cell.setText(self.format % self.data[i])
				except ValueError:
					cell.setText(str(self.data[i]))
		
		self.setToolTip(self.message)
		
		if self.quality == AttrQuality.ATTR_INVALID:
			for cell in self.cells:
				cell.palette().setColor(QPalette.Background, Qt.white)
				cell.setText(QString("######"))
			return
		elif isinstance(self.data[0], bool):
			for c in self.cells:
				c.display()
			return
		
		if self.quality == AttrQuality.ATTR_VALID:
			col = Qt.white
		elif self.quality == AttrQuality.ATTR_ALARM:
			col = Qt.red
#		elif self.quality == AttrQuality.ATTR_WARNING:
#			col = Qt.yellow
#		elif self.quality == AttrQuality.ATTR_CHANGING:
#			col = Qt.yellow
		for cell in self.cells:
			cell.palette().setColor(QPalette.Background, col)

class AttrWriteDatum:
	def __init__(self):
		self.widget = None
		self.device = None
		self.attr = ''
		self.data = None
	
class CommWriteDatum:
	def __init__(self):
		self.widget = None
		self.device = None
		self.comm = ''
		self.data = None
	
class WritingThread(QThread):
	def __init__(self, p, attributes, commands):
		QThread.__init__(self)
		self.parent = p
		self.AttrWriteData = attributes
		self.CommWriteData = commands
	
	def run(self):
		self.parent.setDisabled(True)
		for attr in self.AttrWriteData:
			try:
				print "write_attribute ", attr.attr
				attr.device.write_attribute(attr.data)
			except (DevFailed, Communication, ConnectionFailed), exc:
				err = (time.ctime(), exc)
				TProxyManager().log(err)
				QMessageBox.critical(0, "Error", TProxyManager().formatMessage(err))
		
		for comm in self.CommWriteData:
			try:
				print "command_inout ", comm.comm
				comm.device.command_inout(comm.comm, comm.data)
			except (DevFailed, Communication, ConnectionFailed), exc:
				err = (time.ctime(), exc)
				TProxyManager().log(err)
				QMessageBox.critical(0, "Error", TProxyManager().formatMessage(err))
		self.parent.setDisabled(False)
															
class TDataController(QObject):
	def __init__(self, parent):
		QObject.__init__(self, parent)
		self.thread = None
		self.AttrWriteData = list()
		self.CommWriteData = list()
	
	def subscribe(self, manager, point):
		devname = ""
		attr_command = ""
		(mode, dev_name, attr_command) = TProxyManager().parse_name(point)
		try:
			if mode == 'COMMAND':
				dato = CommWriteDatum()
				dato.manager    = manager
				dato.device     = TProxyManager().findDeviceProxy(dev_name)
				dato.comm       = attr_command
				self.CommWriteData.append(dato)
			elif mode == 'ATTRIBUTE':
				dato = AttrWriteDatum()
				dato.manager    = manager
				dato.device     = TProxyManager().findDeviceProxy(dev_name)
				dato.attr       = attr_command
				dato.data	= AttributeValue()
				dato.data.name  = attr_command
				self.AttrWriteData.append(dato)
		except (DevFailed, Communication, ConnectionFailed), exc:
			raise(exc)
	
#	def unsubscribe(self, manager, point):
	def write(self, data = None):
		print 'write'
        	for attr in self.AttrWriteData:
                	attr.data.value = attr.manager.getData()
        	for comm in self.CommWriteData:
			comm.data = comm.manager.getData()

        	if self.thread is not None:
                	if self.thread.running():
                		print "thread running"
				return
                	elif self.thread.finished():
                		del self.thread
				self.thread = WritingThread(self.parent(), self.AttrWriteData, self.CommWriteData)
                else:
			self.thread = WritingThread(self.parent(), self.AttrWriteData, self.CommWriteData)
		self.thread.start()
	
class TPushButton(QPushButton, TDataManager):
	def __init__(self, parent):
		QPushButton.__init__(self, parent)
		self.setText('OK')
		self.controller = TDataController(self)
		self.connect(self, SIGNAL('clicked()'), self.controller.write)
	
	def setTarget(self, s):
		TDataManager.setTarget(self, s, self.controller)

# class TBooleanSpectrumController(TTable, TDataManager):
# 	def __init__(self, parent = None, name = None):
#         	TTable.__init__(self,parent,name,EToggleButton)
# 		self.controller = TDataController(self)
# 		for cell in self.cells:
# 			self.connect(cell, SIGNAL('clicked()'), self.controller.write)
# 			
# 	def setTarget(self, s):
# 		TDataManager.setTarget(self, s, self.controller)
# 		for cell in self.cells:
# 			self.connect(cell, SIGNAL('clicked()'), self.controller.write)
# 		
# 	def getData(self):
# 		print 'data to write', self.data
# 		return self.data
# 		
# 	def pippo(self, data = None):
# 		print 'pippo'
				
class TApplyButton(EApplyButton):
	def __init__(self, parent):
		EApplyButton.__init__(self, parent)
		self.controller = TDataController(self)
		self.connect(self, SIGNAL('clicked()'), self.controller.write)

class TNumeric(ENumeric, TDataManager):
	def __init__(self, parent = None, iD = 3, dD = 2):
                ENumeric.__init__(self, parent, iD, dD)
	
	def getData(self):
		print self.value()
		return self.value()

class TApplyNumeric(EApplyNumeric, TDataManager):
	def __init__(self, parent = None, iD = 3, dD = 2, pos = Qt.Horizontal):
		EApplyNumeric.__init__(self, parent, iD, dD, pos)
		self.controller = TDataController(self)
		self.connect(self, PYSIGNAL('clicked'), self.controller.write)
	
	def setTarget(self, s):
		TDataManager.setTarget(self, s, self.controller)
	
	def getData(self):
		print self.value()
		return self.value()

class Ui_LogDialog(object):
    def setupUi(self, LogDialog):
        LogDialog.setObjectName("LogDialog")
        LogDialog.resize(QSize(512, 320).expandedTo(LogDialog.minimumSizeHint()))
	LogDialog.setWindowTitle("Errors Log")

        self.vboxlayout = QVBoxLayout(LogDialog)
        self.vboxlayout.setObjectName("vboxlayout")

        self.listLog = QTreeWidget(LogDialog)
        self.listLog.setObjectName("listLog")
        self.vboxlayout.addWidget(self.listLog)

        self.hboxlayout = QHBoxLayout()
        self.hboxlayout.setObjectName("hboxlayout")

        spacerItem = QSpacerItem(131,31,QSizePolicy.Expanding,QSizePolicy.Minimum)
        self.hboxlayout.addItem(spacerItem)

	self.printButton = QPushButton(LogDialog)
        self.printButton.setText("Print")
	self.printButton.setObjectName("printButton")
	self.hboxlayout.addWidget(self.printButton)

        self.okButton = QPushButton(LogDialog)
        self.okButton.setText("OK")
        self.okButton.setObjectName("okButton")
        self.hboxlayout.addWidget(self.okButton)
        self.vboxlayout.addLayout(self.hboxlayout)

        QObject.connect(self.okButton,SIGNAL("clicked()"),LogDialog.accept)

    def tr(self, string):
        return QApplication.translate("LogDialog", string, None, QApplication.UnicodeUTF8)

class TLogDialog(QDialog):
    def __init__(self,parent = None):
        QDialog.__init__(self,parent)
	self.ui = Ui_LogDialog()
	self.ui.setupUi(self)
	labels = QStringList()
	labels.append("Origin")
	labels.append("Reason")
	labels.append("Severity")
	labels.append("Description")
	labels.append("Time")
	self.ui.listLog.setHeaderLabels(labels)
        self.connect(self.ui.printButton,SIGNAL("clicked()"),self.stampa)
	
    def showLogs(self):
	self.resize(600, 400)
	self.show()
	self.ui.listLog.clear()
    	errs = TProxyManager().getLogs()
        for err in errs:
        	base = QTreeWidgetItem(self.ui.listLog)
                for i, exc in enumerate(err[1]):
                        if i == 0:
                                temp = base
                        else:
                                der = QTreeWidgetItem()
                                base.insertChild(base.childCount(), der)
                                temp = der;
                        temp.setText(0, exc['origin'])
                        temp.setText(1, exc['reason'])
                        temp.setText(2, exc['severity'])
                        temp.setText(3, exc['desc'])
                        if i == 0:
                                temp.setText(4, err[0])
	for i in range(5):
		self.ui.listLog.resizeColumnToContents(i)
	self.show()
    
    def stampa(self):
	testo = QString(QApplication.argv()[0])
	testo.append(" Logs\n\n")
	errs = TProxyManager().getLogs()
	for err in errs:
		testo.append(QString(TProxyManager().formatMessage(err)))
		td = QTextDocument(testo, self)
	printer = QPrinter()
	dialog = QPrintDialog(printer, self)
	dialog.setWindowTitle("Print Document")
	if dialog.exec_() == QDialog.Accepted:
		td.print_(printer)
	self.close()
															

class TLogButton(QPushButton):
	def __init__(self, parent = None):
		QPushButton.__init__(self, parent)
		self.win = TLogDialog(self)
		self.setText('ShowLogs')
		self.connect(self, SIGNAL('clicked()'), self.win.showLogs)
