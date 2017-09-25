#e This program is free software you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from math import sin, cos

MIN_SIZE = 9
MARGIN = 0.2
MIN_MARGIN = 2

class ELabel(QLabel):
      	def __init__(self, parent = None):
		QLabel.__init__(self, parent)

		self._falseColor = QColor(Qt.red)
		self._trueColor = QColor(Qt.green)
		self._falseString = 'Zero'
		self._trueString = 'One'
		self.val = 'No Data'
		self.setText('No Data')
		self.setAlignment(Qt.AlignCenter)
		self._pal = self.palette()
		self._pal.setColor(QPalette.Background, Qt.white)
		self.setPalette(self._pal)
		self.setFrameShape(QFrame.WinPanel)
		self.setFrameShadow(QFrame.Sunken)
		self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.setMinimumHeight(20)
		self.setMinimumWidth(30)
		
	def setValue(self, v, update = True):
		self.val = v
		if update:
			self.display()
			
        def value(self):
		return self.val

	def setBooleanDisplay(self, fS, tS, fC, tC):
		self._falseString = fS
		self._trueString = tS
		self._falseColor = fC
		self._trueColor = tC
		self.display()
	
	def display(self):
		if type(self.val) == type(bool()):
			if self.val:
				self._pal.setColor(QPalette.Background, self._trueColor)
				self.setText(self._trueString)
			else:
				self._pal.setColor(QPalette.Background, self._falseColor)
				self.setText(self._falseString)
		else:
			self._pal.setColor(QPalette.Background, Qt.white)
			self.setText(QString(self.val.__str__()))
		self.setPalette(self._pal)

class EFlag(QWidget):
	def __init__(self, parent = None, classType = ELabel):
		QWidget.__init__(self, parent)

		self._numRows = 3
		self._numColumns = 3
		
		self._trueStrings = list()
		self._falseStrings = list()
		self._trueColors = list()
		self._falseColors = list()
		
		self.mask = range(self._numRows*self._numColumns)
		self.cells = list()
		self.grid = QGridLayout(self)
		self.grid.setSpacing(0)
		self.grid.setMargin(0)
		self.classType = classType
		self.arrangeCells()
	
	def setValue(self, v, update = True):
		if isinstance(v, list):
			self.data = list()
			try:
				for ind in self.mask:
					self.data.append(v[ind])
			except:
				print "1 data range error"
		else:
			self.data = list()
			for ind in self.mask:
				self.data.append(False)
			try:
				for ind in range(len(self.mask)):
					if v & 0x1:
						self.data[self.mask.index(ind)] = True
					else:
						self.data[self.mask.index(ind)] = False
					v = v >> 1
			except:
				print "2 data range error"

		try:
			for i,c in enumerate(self.cells()):
				c.setValue(self.data[i], update)
		except:
			print "3 data range error"
		
	def readValue(self):
		return self.data
	
	def setDisplayMask(self, m):
		self.mask = m
	
	def displayMask(self): 
		return self.mask

	def setNumRows(self, r):
		if r < 1:
			return
		self._numRows = r
		self.arrangeCells()
	
	def setNumColumns(self, c):
		if c < 1:
			return
		self._numColumns = c
		self.arrangeCells()
	
	def setBooleanDisplay(self, i, fS, tS, fC, tC):
		self.cells[i].setBooleanDisplay(fS, tS, fC, tC)
		self._trueStrings[i] = tS
		self._falseStrings[i] = fS
		self._trueColors[i] = QVariant(tC)
		self._falseColors[i] = QVariant(fC)
		
	def arrangeCells(self):
		for i in range(self.grid.count()):
			self.grid.removeWidget(self.cells[i])
			self.cells[i].deleteLater()
		self.data = list()
		for i in range(self._numRows*self._numColumns):
			self.data.append(False)

		self.cells = list()	
		self._trueStrings = list()
		self._falseString = list()
		self._trueColors = list()
		self._falseColors = list()
		
		for j in range(self._numColumns):
	        	for i in range(self._numRows):
				temp = self.classType(self)
				self.grid.addWidget(temp, i, j)
				self.cells.append(temp)
				self.mask.append(i*self._numColumns+j)
				self._trueStrings.append(QString("One"))
				self._falseStrings.append(QString("Zero"))
				self._trueColors.append(QVariant(QColor(Qt.green)))
				self._falseColors.append(QVariant(QColor(Qt.red)))
				temp.show()
		self.setMinimumHeight(20*self._numRows)
		self.setMinimumWidth(50*self._numColumns)
		self.configureCells()
		self.update()

	def configureCells(self):
		for i in range(len(self.cells)):
			self.setBooleanDisplay(i, self._falseStrings[i], self._trueStrings[i], QColor(self._falseColors[i]), QColor(self._trueColors[i]))

	def setTrueStrings(self, s):
		self._trueStrings = s.split(';')
		self.configureCells()
	
	def trueStrings(self, s):
		return self._trueStrings.join(';')

	def setFalseStrings(self, s):
		self._falseStrings = s.split(';')
		self.configureCells()
	
	def falseStrings(self, s):
		return self._falseStrings.join(';')

	def setTrueColors(self, s):
		self._trueColors = list()
		l = s.split(';', QString.SkipEmptyParts)
		for el in l:
			self._trueColors.append(QVariant(QColor(el.toUInt())))
		self.configureCells()

	def trueColors(self):
		temp = list()
		for var in self._trueColors:
			list.append(QString().setNum(QColor(var).rgba()))
		return temp.join(';')
	
	def setFalseColors(self, s):
		self._falseColors = list()
		l = s.split(';', QString.SkipEmptyParts)
		for el in l:
			self._falseColors.append(QVariant(QColor(el.toUInt())))
		self.configureCells()

	def falseColors(self):
		temp = list()
		for var in self._falseColors:
			list.append(QString().setNum(QColor(var).rgba()))
		return temp.join(';')
	
class EApplyButton(QPushButton):
	def __init__(self, parent):
		QPushButton.__init__(self,parent)
		self.setText(QString("APPLY"))
		self.setMinimumWidth(30)
		self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
		self.connect(self, SIGNAL('clicked()'), self.clearModified)
		self.defaultTextColor = self.palette().color(QPalette.ButtonText)
	
	def valueModified(self, v):
		if self.isEnabled():
			self.palette().setColor(QPalette.ButtonText, QColor(Qt.red))
			self.update()
					
	def clearModified(self):
		if self.isEnabled():
			self.palette().setColor(QPalette.ButtonText, self.defaultTextColor)
			self.update()

class ENumeric(QWidget):
	def __init__(self, parent = None, iD = 3, dD = 2):
		QWidget.__init__(self,parent)
		self.intDig = iD
		self.decDig = dD
		self.digits = iD + dD
		self.data = 0
		self.minVal = -pow(10.0, self.digits) + 1
		self.maxVal = pow(10.0, self.digits) - 1
		self.labels = list()
		self.grid = QGridLayout(self)
		self.grid.setSpacing(0)
		self.grid.setMargin(0)
		self.init()
		
	def init(self):
		self.bup = QButtonGroup(self)
		self.bdown = QButtonGroup(self)
		self.labels = list()
		self.text = None
	
		self.signLabel = QLabel(self)
		self.signLabel.setAlignment(Qt.AlignCenter)
		self.signLabel.setObjectName('layoutmember+')
		self.grid.addWidget(self.signLabel, 1, 0)
	
		for i in range(self.digits):
			if i == self.intDig:
				pointLabel = QLabel('.', self)
				pointLabel.setAlignment(Qt.AlignCenter)
				pointLabel.setObjectName("layoutmember.")
				self.grid.addWidget(pointLabel, 1, i + 1)
				
			temp = QPushButton(self)
			temp.setObjectName(QString('layoutmember') + QString().setNum(i))
			self.bup.addButton(temp)
			
			l = QLabel(self)
			self.setObjectName(QString('layoutmember') + QString().setNum(i))
			self.labels.append(l)
			
			temp2 = QPushButton(self)
			temp2.setObjectName(QString('layoutmember') + QString().setNum(i))
			self.bdown.addButton(temp2)
			
			self.formatDigit(temp, l, temp2)

			if i < self.intDig:
				self.grid.addWidget(temp, 0, i + 1)
				self.grid.addWidget(l, 1, i + 1)
				self.grid.addWidget(temp2, 2, i + 1)
			elif i >= self.intDig:
				self.grid.addWidget(temp, 0, i + 2)
				self.grid.addWidget(l, 1, i + 2)
				self.grid.addWidget(temp2, 2, i + 2)
		
		for i in range(self.grid.rowCount()):
			self.grid.setRowStretch(i, 1)
		for i in range(self.grid.columnCount()):
			self.grid.setColumnStretch(i, 1)
		
		self.connect(self.bup, SIGNAL('buttonClicked(QAbstractButton*)'), self.upData)
		self.connect(self.bdown, SIGNAL('buttonClicked(QAbstractButton*)'), self.downData)
		self.showData()

	def clearContainers(self):
		for w in self.children():
			if isinstance(w, QWidget):
				w.deleteLater()
	
	def setValue(self, v):
		temp = int(v * pow(10.0, self.decDig))
		if temp >= self.minVal and temp <= self.maxVal:
			if self.data != temp:
				self.emit(SIGNAL('valueChanged'),(temp*pow(10.0, -self.decDig),))
			self.data = temp
			self.showData()
			
	def value(self):
		return self.data*pow(10.0, -self.decDig)
	
	def setMax(self, M):
		self.setMaxValue(M)
	
	def setMaxValue(self, M):
		if M >= self.minVal:
			self.maxVal = int(M * pow(10.0, self.decDig))
	
	def maxValue(self):
		return self.maxVal*pow(10.0, -self.decDig)

	def setMin(self, m):
		self.setMinValue(m)

	def setMinValue(self, m):
		if m <= self.maxVal:
			self.minVal = int(m * pow(10.0, self.decDig))
	
	def minValue(self):
		return self.minVal*pow(10.0, -self.decDig)

	def setIntegerDigits(self, i):
		self.setIntDigits(i)
		
	def setIntDigits(self, i):
		if i < 1:
			return
		self.intDig = i
		self.digits = self.intDig + self.decDig
		self.clearContainers()
		self.init()
			
	def intDigits(self):
		return self.intDig

	def setDecimalDigits(self, d):
		self.setDecDigits(d)
		
	def setDecDigits(self, d):
		if d < 0:
			return
		self.data = int(self.data * pow(10.0, d - self.decDig))
		self.maxVal = int(self.maxVal * pow(10.0, d - self.decDig))
		self.minVal = int(self.minVal * pow(10.0, d - self.decDig))
		self.decDig = d
		self.digits = self.intDig + self.decDig
		self.clearContainers()
		self.init()
	
	def decDigits(self):
		return self.decDig

	def upData(self, b):
		id = b.objectName().remove("layoutmember").toInt()[0]
		if (self.data + pow(10.0, self.digits-id-1)) <= self.maxVal:
			self.data += int(pow(10.0, self.digits-id-1))
			self.emit(SIGNAL('valueChanged'), (self.data * pow(10.0, -self.decDig),))
			self.showData()
		if self.text is not None:
			self.text.hide()
	
	def downData(self, b):
		id = b.objectName().remove("layoutmember").toInt()[0]
		if (self.data - pow(10.0, self.digits-id-1)) >= self.minVal:
			self.data -= int(pow(10.0, self.digits-id-1))
			self.emit(SIGNAL('valueChanged'), (self.data * pow(10.0, -self.decDig),))
			self.showData()
		if self.text is not None:
			self.text.hide()
	
	def showData(self):
		temp = self.data
		if self.data < 0:
			self.signLabel.setText('-')
		else:
			self.signLabel.setText('+')
		for i in range(self.digits):
			num = int(temp / pow(10.0, self.digits-i-1))
			temp -= num * pow(10.0, self.digits-i-1)
			self.labels[i].setText(str(abs(num)))
		
	def dataInput(self):
		(val, ok) = self.text.text().toDouble()
		if ok:
			self.setValue(val)
		self.text.hide()
	
	def mouseDoubleClickEvent(self, ev):
		if self.text is None:
			self.text = QLineEdit(self)
			self.connect(self.text, SIGNAL('returnPressed()'), self.dataInput)
			self.connect(self.text, SIGNAL('lostFocus()'), self.text.hide)
		self.text.setGeometry(QRect(self.grid.cellRect(1, 0).topLeft(), self.grid.cellRect(1, self.grid.columnCount() - 1).bottomRight()))
		self.text.setFont(self.signLabel.font())
		self.text.setAlignment(Qt.AlignRight)
		self.text.setMaxLength(self.digits+2)
		self.text.setText("")
		self.text.setFocus()
		self.text.show()
				
	def resizeEvent(self, ev):
		temp = self.bup.buttons()[0]
		pix = QPixmap(temp.size())
		pix.fill(self.palette().color(QPalette.Background))
		p = QPainter(pix)
		p.setRenderHint(QPainter.Antialiasing)
		
		hmargin = pix.width() * MARGIN
		vmargin = pix.height() * MARGIN
		if hmargin < MIN_MARGIN:
			hmargin = MIN_MARGIN
		if vmargin < MIN_MARGIN:
			vmargin = MIN_MARGIN
		h = pix.height()
		w = pix.width()

		poly = QPolygon(3)
		poly.setPoint(0, w*.5, vmargin)
		poly.setPoint(1, w - hmargin, h - vmargin)
		poly.setPoint(2, hmargin, h - vmargin)
		pen = QPen()
		pen.setColor(self.palette().color(QPalette.Foreground))
		p.setPen(pen)
		linearGradient = QLinearGradient(0, 0, w, h)
		linearGradient.setColorAt(0.0, self.palette().color(QPalette.Light))
		linearGradient.setColorAt(1.0, self.palette().color(QPalette.Dark))
		p.setBrush(QBrush(linearGradient))
		p.drawConvexPolygon(poly)
		p.end()

		for temp in self.bup.buttons():
			temp.setIconSize(pix.size())
			temp.setIcon(QIcon(pix))
				
		pix2 = pix.transformed(QMatrix().rotate(-180))
		for temp in self.bdown.buttons():
			temp.setIconSize(pix2.size())
			temp.setIcon(QIcon(pix2))

		if self.text is not None:
			self.text.setGeometry(QRect(self.grid.cellRect(1, 0).topLeft(), self.grid.cellRect(1, self.grid.columnCount() - 1).bottomRight()))

	def formatDigit(self, up, l, down):
		up.setAutoRepeat(True)
		up.setMinimumSize(QSize(MIN_SIZE,MIN_SIZE))
		up.setFlat(True)
		up.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		up.setFocusPolicy(Qt.NoFocus)
	
		l.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		l.setAlignment(Qt.AlignHCenter|Qt.AlignVCenter)
		l.setMinimumSize(QSize(MIN_SIZE,2*MIN_SIZE))
	
		down.setAutoRepeat(True)
		down.setMinimumSize(QSize(MIN_SIZE, MIN_SIZE))
		down.setFlat(True)
		down.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		down.setFocusPolicy(Qt.NoFocus)
		
	def setFont(self, f):
		QWidget.setFont(f)
		self.draw()

	def setEnabled(self, b):
		QWidget.setEnabled(b)
		self.draw()

	def setDisabled(self, b):
		QWidget.setDisabled(b)
		self.draw()
		
class EApplyNumeric(QWidget):
	def __init__(self, parent = None, i = 3, d = 2, pos = Qt.Horizontal):
		QWidget.__init__(self, parent)
		self.data = None
		self.button = None
		self.buttonPos = pos
		self.intDig = i
		self.decDig = d
		self.init()

	def init(self):
		if self.buttonPos == Qt.Horizontal:
			self.box2 = QHBoxLayout()
		elif self.buttonPos == Qt.Vertical:
			self.box2 = QVBoxLayout()
		self.setLayout(self.box2)
		
		if self.data is None:
			self.data = ENumeric(self, self.intDig, self.decDig)
		if self.button is None: 
			self.button = EApplyButton(self)
		self.box2.addWidget(self.data, self.data.intDigits() + self.data.decDigits())
		self.box2.addWidget(self.button, 3)
		
		self.connect(self.data, SIGNAL('valueChanged'), self.button.valueModified)
		self.connect(self.button, SIGNAL('clicked()'), self.applyValue)
	
	def setFont(self, f):
		QWidget.setFont(f)
		self.button.setFont(f)

	def setButtonPosition(self, pos):
		self.buttonPos = pos
		self.init()
	
	def buttonPosition(self):
		return self.buttonPos
	
	def setValue(self, v):
		self.data.setValue(v)
	
	def value(self):
		return self.data.value()
		
	def setMax(self, v):
		self.data.setMaxValue(v)
		
	def setMaxValue(self, v):
		self.data.setMaxValue(v)
		
	def maxValue(self):
		return self.data.maxValue()
		
	def setMin(self, v):
		self.data.setMinValue(v)
		
	def setMinValue(self, v):
		self.data.setMinValue(v)
		
	def minValue(self):
		return self.data.minValue()
		
	def setIntegerDigits(self, i):
		self.data.setIntDigits(i)
		
	def setIntDigits(self, i):
		self.data.setIntDigits(i)
		
	def intDigits(self):
		return self.data.intDigits()
		
	def setDecimalDigits(self, d):
		self.data.setDecDigits(d)
		
	def setDecDigits(self, d):
		self.data.setDecDigits(d)
		
	def decDigits(self):
		return self.data.decDigits()
		
	def setButtonText(self, s):
		self.button.setText(s)
		
	def buttonText(self):
		return self.button.text()
		
	def numericFont(self):
		return self.data.font()
		
	def setNumericFont(self, f):
		self.data.setFont(f)
	
	def applyValue(self):
		self.emit(SIGNAL('clicked'), (self.data.value(),))
		
	def clearModified(self):
		self.button.clearModified()

	def setOrientation(self, pos):
		self.buttonPos = pos
		self.init()
	
class ELed(QWidget):
	def __init__(self, parent):
		QWidget.__init__(self, parent)
		self.ledColor = QColor(Qt.gray)
		self.setMinimumHeight(30)
	        self.setMinimumWidth(30)

	def setColor(self, c, up=True):
		self.ledColor = QColor(c)
		if up:
			self.update()
	
	def paintEvent(self, ev):
		painter = QPainter(self)
		radius  = .45 * min(self.width(), self.height())
		gradient = QRadialGradient(.5 * self.width(), .5 * self.height(), radius, .5 * (self.width() - radius), .5 * (self.height() - radius))
		gradient.setColorAt(0.2, QColor(Qt.white))
		if self.isEnabled():
			gradient.setColorAt(0.8, self.ledColor)
		else:
			gradient.setColorAt(0.8, Qt.gray)
		gradient.setColorAt(1, QColor(Qt.black))
		painter.setRenderHint(QPainter.Antialiasing)
		painter.setBrush(QBrush(gradient))
		painter.drawEllipse(.5 * self.width() - radius, .5 * self.height() - radius, radius * 2, radius * 2)
		
LINEAR_MAJOR_TICKS = 6
CIRCULAR_MAJOR_TICKS = 11
NUM_MINOR_TICKS = 5

class EAbstractGauge(QWidget):
	GRADIENT=0
	COLORBAR=1
	SINGLECOLOR=2
	
	def __init__(self, parent=None):
		QWidget.__init__(self, parent)
		self._lowWarning = -5
		self._highWarning = 5
		self._lowError = -10
		self._highError = 10
		self._minValue = -15
		self._maxValue = 15
		self._value = 5
		self._reference = 3
		self._colorMode = EAbstractGauge.COLORBAR
		self._referenceEnabled = True
		self._scaleEnabled = True
		self._numMajorTicks = LINEAR_MAJOR_TICKS
		self._longestLabelIndex = -1
		self._normalColor = Qt.green
		self._warningColor = Qt.yellow
		self._errorColor = Qt.red
		self._externalScale = False
		self.labels = list()
		self.configure()
	
	def configure(self):
		self.v_p = list()
		self.v_c = list()
		interval = self._maxValue - self._minValue
		if self._colorMode == EAbstractGauge.GRADIENT:
			self.v_p = [self._minValue, (self._minValue+self._lowError)*.5, (self._lowError+self._lowWarning)*.5, (self._lowWarning+self._highWarning)*.5, (self._highWarning+self._highError)*.5, (self._highError+self._maxValue)*.5, self._maxValue]
			self.v_c = [self._errorColor, self._errorColor, self._warningColor, self._normalColor, self._warningColor, self._errorColor, self._errorColor]
			if self._minValue == self._lowError and self._lowError == self._lowWarning:
				self.v_c[0] = self.v_c[1] = self.v_c[2] = self._normalColor
			if self._maxValue == self._highError and self._highError == self._highWarning:
				self.v_c[4] = self.v_c[5] = self.v_c[6] = self._normalColor
		elif self._colorMode == EAbstractGauge.COLORBAR:
			self.v_p = [self._minValue, self._lowError, self._lowWarning, self._highWarning, self._highError, self._maxValue]
			self.v_c = [self._errorColor, self._errorColor, self._warningColor, self._normalColor, self._warningColor, self._errorColor]
		else:
			if self._value < self._lowError:
				self.v_c.append(self._errorColor)
			elif self._value < self._lowWarning:
				self.v_c.append(self._warningColor)
			elif self._value < self._highWarning:
				self.v_c.append(self._normalColor)
			elif self._value < self._highError:
				self.v_c.append(self._warningColor)
			else:
				self.v_c.append(self._errorColor)

		for i in range(len(self.v_p)):
			self.v_p[i] = float(self.v_p[i] - self._minValue) / interval

		self.labels = list()
		val = self._minValue
		tick = (self._maxValue-self._minValue)/100.0
		step = round((self._maxValue-self._minValue)/(self._numMajorTicks-1)/tick)*tick
		self.longestLabelIndex = -1
		for i in range(self._numMajorTicks):
			s = QString.number(val)
			if self.longestLabelIndex == -1 or s.length() > self.labels[self.longestLabelIndex].length():
				self.longestLabelIndex = i
			self.labels.append(s)
			val = val + step
	
	def setLowWarning(self, w):
		self._lowWarning = w
		self.configure()
		self.update()

	def lowWarning(self):
		return self._lowWarning
	
	def setHighWarning(self, w):
		self._highWarning = w
		self.configure()
		self.update()
	
	def highWarning(self):
		return self._highWarning

	def setLowError(self, e):
		self._lowError = e
		self.configure()
		self.update()

	def lowError(self):
		return self._lowError
	
	def setHighError(self, e):
		self._highError = e
		self.configure()
		self.update()
	
	def highError(self):
		return self._highError

	def setMinValue(self, v):
		self._minValue = v
		self.configure()
		self.update()
	
	def minValue(self):
		return self._minValue

	def setMaxValue(self, v):
		self._maxValue = v
		self.configure()
		self.update()
	
	def maxValue(self):
		return self._maxValue

	def setNumericValue(self, v):
		self._value = v
		self.configure()
		self.update()
	
	def value(self):
		return self._value

	def setReference(self, r):
		self._reference = r
		self.update()

	def reference(self):
		return self._reference
	
	def setScaleEnabled(self, b):
		self._scaleEnabled = b
		self.configure()
		self.update()

	def isScaleEnabled(self):
		return self._scaleEnabled

	def setReferenceEnabled(self, b):
		self._referenceEnabled = b
		self.configure()
		self.update()
	
	def isReferenceEnabled(self):
		return self._referenceEnabled

	def setColorMode(self, m):
		self._colorMode = m
		self.configure()
		self.update()

	def colorMode(self):
		return self._colorMode

	def setNormalColor(self, c):
		self._normalColor = c
		self.configure()
		self.update()
	
	def normalColor(self):
		return self._normalColor
	
	def setWarningColor(self, c):
		self._warningColor = c
		self.configure()
		self.update()
		
	def warningColor(self):
		return self._warningColor

	def setErrorColor(self, c):
		self._errorColor = c
		self.configure()
		self.update()
		
	def errorColor(self):
		return self._errorColor

	def setExternalScale(self, b):
		self._externalScale = b
		self.configure()
		self.update()
		
	def externalScale(self):
		return self._externalScale

	def setNumMajorTicks(self, t):
		self._numMajorTicks = t
		self.configure()
		
	def numMajorTicks(self):
		return self._numMajorTicks
	
	def setNumMinorTicks(self, t):
		self._numMinorTicks = t
		self.configure()
		
	def numMinorTicks(self):
		return self._numMinorTicks
	
class ELinearGauge(EAbstractGauge):
	ALL=0
	FROM_MIN=1
	FROM_ZERO=2
	FROM_REF=3

	def __init__(self, parent=None, o=Qt.Vertical):
		self._orientation = o
		self._fillMode = ELinearGauge.ALL
		EAbstractGauge.__init__(self, parent)
		self.setNumMajorTicks(LINEAR_MAJOR_TICKS)
		self.setNumMinorTicks(NUM_MINOR_TICKS)
		f = self.font()
		f.setPointSize(6)
		self.setFont(f)

	def configure(self):
		EAbstractGauge.configure(self)
		if self._scaleEnabled:
			longSide = 80
			shortSide = 30
		else:
			longSide = 60
			shortSide = 20
		
		if self._orientation == Qt.Horizontal:
			if self._scaleEnabled:
				self.scalePos = 20
			else:
				self.scalePos = 0
			self.labelsPos = 12
			self.totalSize = 30
			self.barSize = self.totalSize-self.scalePos
			self.scaleSize = self.scalePos-self.labelsPos
			self.labelsSize = self.labelsPos
			self.setMinimumSize(longSide,shortSide)
		else:
			if self._scaleEnabled:
				self.scalePos = 10
			else:
				self.scalePos = 30
			self.labelsPos = 18
			self.totalSize = 30
			self.barSize = self.scalePos
			self.scaleSize = self.labelsPos-self.scalePos
			self.labelsSize = self.totalSize-self.labelsPos
			self.setMinimumSize(shortSide,longSide)
	
	def paintEvent(self, ev):
		painter = QPainter(self)
		fm = QFontMetrics(painter.font())
		h = fm.height()+2
		try:
			w = fm.width(self.labels[self._longestLabelIndex])+2
		except:
			return
		if self._orientation == Qt.Horizontal:
			size = min(self.width()*self.totalSize/100.0, self.height())
		else:
			size = min(self.height()*self.totalSize/100.0, self.width())
		painter.setRenderHint(QPainter.Antialiasing)
		if self._orientation == Qt.Horizontal:
			painter.setViewport((self.width()-size*100.0/self.totalSize)*.5,(self.height()-size)*.5, size*100.0/self.totalSize, size)
			painter.setWindow(-w*.5, 0, 100+w, self.totalSize+2) #border
		else:
			painter.setViewport((self.width()-size)*.5,(self.height()-size*100.0/self.totalSize)*.5, size, size*100.0/self.totalSize)
			painter.setWindow(-2, -h*.5, self.totalSize+2, 100+h) #border
		self.drawColorBar(painter)
		
		if self._scaleEnabled:
			self.drawScale(painter)
			self.drawLabels(painter)
		if self.isEnabled() and self._fillMode == ELinearGauge.ALL:
			self.drawMarker(painter, True)
		if not self.isEnabled():
			c = self.palette().color(QPalette.Background)
			c.setAlpha(200)
			painter.fillRect(painter.window(), c)

	def drawColorBar(self, p):
		if self._colorMode == EAbstractGauge.GRADIENT:
			if self._orientation == Qt.Horizontal:
				start = QPoint(0,0)
				stop = QPoint(100,0)
				r = QRect(0, self.scalePos, 100, self.barSize)
			else:
				start = QPoint(0,100)
				stop = QPoint(0,0)
				r = QRect(0, 0, self.barSize, 100)
			grad = QLinearGradient(start, stop)
			for i in range(len(v_p)):
				grad.setColorAt(v_p[i], v_c[i])
			p.setBrush(QBrush(grad))
			p.drawRect(r)
		elif self._colorMode == EAbstractGauge.COLORBAR:
			p.setPen(Qt.NoPen)
			if self._orientation == Qt.Horizontal:
				widths = list()
				for i in range(len(self.v_p)):
					widths.append(self.v_p[i] * 100)
				for i in range(1, len(self.v_p)):
					p.setBrush(QBrush(self.v_c[i]))
					p.drawRect(QRectF(widths[i-1], self.scalePos, widths[i]-widths[i-1], self.barSize))
				p.setBrush(QBrush(Qt.NoBrush))
				p.setPen(QPen(Qt.black))
				p.drawRect(0, self.scalePos, 100, self.barSize)
			else:
				heights = list()
				for i in range(len(self.v_p)):
					heights.append(100*(1-self.v_p[i]))
				for i in range(1, len(self.v_p)):
					p.setBrush(QBrush(self.v_c[i]))
					p.drawRect(QRectF(0, heights[i], self.barSize, heights[i-1]-heights[i]))
				p.setBrush(QBrush(Qt.NoBrush))
				p.setPen(QPen(Qt.black))
				p.drawRect(0, 0, self.barSize, 100)
		elif self._colorMode == EAbstractGauge.SINGLECOLOR:
			p.setBrush(QBrush(self.v_c[0]))
			if self._orientation == Qt.Horizontal:
				p.drawRect(0, self.scalePos, 100, self.barSize)
			else:
				p.drawRect(0, 0, self.barSize, 100)
		
		if self._fillMode != ELinearGauge.ALL:
			min = 0.0
			max = 0.0
			v1 = (self._value-self._minValue)/(self._maxValue-self._minValue)
			p.setPen(QPen(Qt.black))
	                p.setBrush(QBrush(palette().background()))
	                if self._fillMode == ELinearGauge.FROM_MIN:
	                        max = v1
	                        min = 0.0
	                elif self._fillMode == ELinearGauge.FROM_ZERO:
	                        v2  = -self._minValue/(self._maxValue-self._minValue)
        	                min = min(v1, v2)
				max = max(v1, v2)
	                elif self._fillMode == ELinearGauge.FROM_REF:
	                        v2  = (self._reference-self._minValue)/(self._maxValue-self._minValue)
	                        min = min(v1, v2)
	                        max = max(v1, v2)
	
	                if self._orientation == Qt.Horizontal:
	                        p.drawRect(QRectF(0, self.scalePos, 100.0*min, self.barSize))
	                        p.drawRect(QRectF(100.0*max, self.scalePos, 100.0*(1-max), self.barSize))
	                else:
	                        p.drawRect(QRectF(0.0, 100*(1-min), self.barSize, 100*min))
	                        p.drawRect(QRectF(0.0, 0.0, self.barSize, 100.0*(1-max)))
	
	def drawScale(self, p):
		lines = list()
		
		if self._orientation == Qt.Horizontal:
			x1 = 0
			y1 = self.scalePos
			wM = self.scaleSize
			wm = wM*.5
			majorTickLine = QLineF(x1,y1,x1,y1-wM)
			interval = 100.0/(self._numMajorTicks-1)/self._numMinorTicks
			for i in range(self._numMajorTicks-1):
				lines.append(majorTickLine)
				minorTickLine = QLineF(majorTickLine.x1(), majorTickLine.y1(), majorTickLine.x2(), majorTickLine.y1()-wm)
				for j in range(self._numMinorTicks-1):
					#minorTickLine.translate(QPointF(interval,0))
					minorTickLine = QLineF(majorTickLine.x1()+(j+1)*interval, majorTickLine.y1(), majorTickLine.x2()+(j+1)*interval, majorTickLine.y1()-wm)
					lines.append(minorTickLine)
#                        	majorTickLine.translate(QPointF(100.0/(self._numMajorTicks-1),0))
				majorTickLine = QLineF(x1+(i+1)*100.0/(self._numMajorTicks-1), y1, x1+(i+1)*100.0/(self._numMajorTicks-1), y1-wM)
			lines.append(majorTickLine)
		else:
			x1 = self.scalePos
			y1 = 0
			wM = self.scaleSize
			wm = wM*.5
			majorTickLine = QLineF(x1, y1, x1+wM, y1)
			interval = 100.0/(self._numMajorTicks-1)/self._numMinorTicks
			for i in range(self._numMajorTicks-1):
				lines.append(majorTickLine)
				minorTickLine = QLineF(majorTickLine.x1(), majorTickLine.y1(), majorTickLine.x1()+wm, majorTickLine.y1())
				for j in range(self._numMinorTicks-1):
					#minorTickLine.translate(QPointF(0, interval))
					minorTickLine = QLineF(majorTickLine.x1(), majorTickLine.y1()+(j+1)*interval, majorTickLine.x1()+wm, majorTickLine.y1()+(j+1)*interval)
					lines.append(minorTickLine)
				#majorTickLine.translate(QPointF(0, 100.0/(self._numMajorTicks-1)))
				majorTickLine = QLineF(x1, y1+(i+1)*100.0/(self._numMajorTicks-1), x1+wM, y1+(i+1)*100.0/(self._numMajorTicks-1))
			lines.append(majorTickLine)
		p.drawLines(lines)
	        if self._referenceEnabled:
	                self.drawMarker(p, False)

	def drawMarker(self, p, drawValue):
		old = p.font()
		f = old
		f.setPointSize(3)
		p.setFont(f)
		p.setPen(QPen(Qt.red))

		if self._orientation == Qt.Horizontal:
        	        if drawValue:
	                        vertice = QPointF(100.0*float(self._value-self._minValue)/(self._maxValue-self._minValue), self.scalePos)
        	                triangolo = [vertice, vertice + QPointF(4,8), vertice + QPointF(-4,8)]
			else:
				vertice = QPointF(100.0*float(self._reference-self._minValue)/(self._maxValue-self._minValue), self.scalePos)
				triangolo = [vertice, vertice + QPointF(2,-4), vertice + QPointF(-2,-4)]
				p.drawText(vertice+QPointF(-4,-5), "REF")
		else:
			if (drawValue):
				vertice = QPointF(self.scalePos, 100.0*(1-float(self._value-self._minValue)/(self._maxValue-self._minValue)))
				triangolo = [vertice, vertice + QPointF(-8,-4), vertice + QPointF(-8,4)]
			else:
				vertice = QPointF(self.scalePos, 100.0*(1-float(self._reference-self._minValue)/(self._maxValue-self._minValue)))
				triangolo = [vertice, vertice + QPointF(4,-2), vertice + QPointF(4,2)]
				p.drawText(vertice+QPointF(5,1.5), "REF")
		p.setFont(old)
		p.setPen(QPen(Qt.black))
		
		if (drawValue):
			c = QColor('blue')
		else:
			c = QColor('red')
		c.setAlpha(100)
		p.setBrush(QBrush(c))
		p.drawPolygon(QPolygonF(triangolo))

	def drawLabels(self, p):
		check = True
		f = p.font()
		fm = QFontMetrics(f)
		h = fm.height()
		w = fm.width(self.labels[self.longestLabelIndex])
		if self._orientation == Qt.Horizontal:
			y = 0
			h = self.labelsSize
			while check:
				if p.fontMetrics().width(self.labels[self.longestLabelIndex]) > p.window().width()/self._numMajorTicks:
					f.setPointSize(f.pointSize()-1)
					p.setFont(f)
				else:
					check = False
			for i in range(self._numMajorTicks):
				br = QRectF(i*100.0/(self._numMajorTicks-1)-w*.5, y, w, h)
				p.drawText(br, Qt.AlignBottom | Qt.AlignHCenter, self.labels[i])
		else:
	                x = self.labelsPos
			w = self.labelsSize
			while check:
				if p.fontMetrics().height() > p.window().height()/self._numMajorTicks or p.fontMetrics().width(self.labels[self.longestLabelIndex]) > self.labelsSize:
					f.setPointSize(f.pointSize()-1)
					p.setFont(f)
				else:
					check = False
			for i in range(self._numMajorTicks):
				br = QRectF(x, i*100.0/(self._numMajorTicks-1)-h*.5, w, h)
				p.drawText(br, Qt.AlignRight | Qt.AlignVCenter, self.labels[self._numMajorTicks-1-i])
				#p.drawText(QRect(0,0,10,10), Qt.AlignCenter, QString("pippo"))

	def setOrientation(self, o):
		self._orientation = o
		self.configure()
		self.update()

	def orientation(self):
		return self._orientation

	def setFillMode(self, m):
		self._fillMode = m
		self.configure()
		self.update()
	
	def fillMode(self):
		return self._fillMode

class ECircularGauge(EAbstractGauge):
	def __init__(self, parent=None):
		EAbstractGauge.__init__(self, parent)
		self._startAngle = 240
		self._stopAngle = 300
		self._arcLength = 300
		self._innerRadius = 25
		self._outerRadius = 30
		self._labelRadius = 45
		self._valueDisplayed = True
		self._valueFormat = QString()

		self.setExternalScale(False)
		self.setNumMajorTicks(CIRCULAR_MAJOR_TICKS)
		self.setNumMinorTicks(NUM_MINOR_TICKS)
		self.setMinimumSize(70,70)

	def configure(self):
		if self._scaleEnabled:
			if self._externalScale:
				self._innerRadius = 25
				self._outerRadius = 30
				self._labelRadius = 46
				self.setMinimumSize(80,80)
			else:
				if self._referenceEnabled:
					self._innerRadius = 37
					self._outerRadius = 46
					self._labelRadius = 32
				else:
					self._innerRadius = 40
					self._outerRadius = 49
					self._labelRadius = 35
				self.setMinimumSize(70,70)
		else:
			self._innerRadius = 40
			self._outerRadius = 49
			self._labelRadius = 50
			self.setMinimumSize(50,50)
		EAbstractGauge.configure(self)

	def paintEvent(self, ev):
		painter = QPainter(self)
		size = min(self.height(), self.width())

		painter.setRenderHint(QPainter.Antialiasing)
		painter.setViewport((self.width()-size)*.5,(self.height()-size)*.5,size,size)
		painter.setWindow(-50,-50,100,100)

		self.drawColorBar(painter)
		if self._scaleEnabled:
			self.drawScale(painter)
			self.drawLabels(painter)

		if self.isEnabled():
			self.drawNeedle(painter)

		if self._valueDisplayed:
			self.drawValue(painter)

		if not self.isEnabled():
			c = self.palette().color(QPalette.Background)
			c.setAlpha(200)
			painter.fillRect(painter.window(), c)

	def drawColorBar(self, p):
		p.setPen(Qt.NoPen)
		if self._colorMode == EAbstractGauge.GRADIENT:
			grad = QConicalGradient(0, 0, self._startAngle)
			for i in range(len(self.v_p)):
				grad.setColorAt(self.v_p[i]*self._arcLength/360.0, self.v_c[i])
				p.setBrush(QBrush(grad))
			p.drawPie(-self._outerRadius,-self._outerRadius,self._outerRadius*2,self._outerRadius*2, self._startAngle*16, -self._arcLength*16)
		elif self._colorMode == EAbstractGauge.COLORBAR:
			for i in range(1, len(self.v_p)):
				p.setBrush(QBrush(self.v_c[i]))
				p.drawPie(-self._outerRadius,-self._outerRadius,self._outerRadius*2,self._outerRadius*2, (self._startAngle-self._arcLength*self.v_p[i])*16, (self._arcLength*self.v_p[i]-self._arcLength*self.v_p[i-1])*16)
		else:
			p.setBrush(QBrush(self.v_c[0]))
			p.drawPie(-self._outerRadius,-self._outerRadius,self._outerRadius*2,self._outerRadius*2, self._startAngle*16, -self._arcLength*16)
		p.setBrush(QBrush(self.palette().background()))
		p.drawEllipse(-self._innerRadius,-self._innerRadius,self._innerRadius*2,self._innerRadius*2)

	def drawNeedle(self, p):
		angle = (self._startAngle-float(self._value-self._minValue)/(self._maxValue-self._minValue)*self._arcLength)*3.1415927/180.0

		longArm = QPointF(self._outerRadius*cos(angle),-self._outerRadius*sin(angle))
		shortArm = QPointF(-2*cos(angle),2*sin(angle))
		side1 = QPointF(-2*sin(angle),-2*cos(angle))
		side2 = -side1

		tr1 = QPolygonF([longArm, side1, shortArm])
		tr2 = QPolygonF([longArm, side2, shortArm])

		p.setPen(Qt.NoPen)
		grad1 = QRadialGradient(QPointF(0,0),self._outerRadius,side1*.5)
		grad1.setColorAt(0.0, self.palette().color(QPalette.Mid))
		grad1.setColorAt(1.0, self.palette().color(QPalette.Dark))
		grad2 = QRadialGradient(QPointF(0,0),self._outerRadius,side2*.5)
		grad2.setColorAt(0.0, self.palette().color(QPalette.Midlight))
		grad2.setColorAt(1.0, self.palette().color(QPalette.Dark))
		p.setBrush(QBrush(grad1))
		p.drawPolygon(tr1)
		p.setBrush(QBrush(grad2))
		p.drawPolygon(tr2)

		pen = QPen(Qt.black)
		pen.setJoinStyle(Qt.RoundJoin)
		p.setPen(QPen(pen))
		p.drawLine(longArm,side1)
		p.drawLine(side1,shortArm)
		p.drawLine(shortArm,side2)
		p.drawLine(side2,longArm)

		p.setBrush(QBrush(self.palette().color(QPalette.Dark)))
		p.drawEllipse(QRectF(-.5,-.5,1,1))

	def drawScale(self, p):
		p.setPen(QPen(Qt.black))
		p.drawArc(-(self._outerRadius+0), -(self._outerRadius+0), 2*(self._outerRadius+0), 2*(self._outerRadius+0), self._startAngle*16, -self._stopAngle*16)

		start = self._startAngle*3.1415927/180.0
		startPoint = QPointF((self._outerRadius+0)*cos(start),-(self._outerRadius+0)*sin(start))
		if self._externalScale:
			stopPoint = startPoint + QPointF(8*cos(start),-8*sin(start))
			stopPoint2 = startPoint + QPointF(4*cos(start),-4*sin(start))
		else:
			stopPoint = startPoint - QPointF(8*cos(start),-8*sin(start))
			stopPoint2 = startPoint - QPointF(4*cos(start),-4*sin(start))
		majorTickLine = QLineF(startPoint, stopPoint)
		minorTickLine = QLineF(startPoint, stopPoint2)

		p.save()
		for i in range(self._numMajorTicks):
			p.drawLine(majorTickLine)
			if i < 10:
				for j in range(4):
					p.rotate(6)
					p.drawLine(minorTickLine)
				p.rotate(6)
		p.restore()
		
		if self._referenceEnabled:
			angle = (self._startAngle-float(self._reference-self._minValue)/(self._maxValue-self._minValue)*self._arcLength)*3.1415927/180.0
			shift = 2.5*3.1415927/180.0
			p1 = QPointF(self._outerRadius*cos(angle),-self._outerRadius*sin(angle))
			p2 = QPointF((self._outerRadius+3)*cos(angle+shift),-(self._outerRadius+3)*sin(angle+shift))
			p3 = QPointF((self._outerRadius+3)*cos(angle-shift),-(self._outerRadius+3)*sin(angle-shift))
			tr = QPolygonF([p1, p2, p3])

			c = QColor('red')
			c.setAlpha(100)
			p.setBrush(QBrush(c))
			p.setPen(QPen(Qt.black))
			p.drawPolygon(tr)

	def drawLabels(self, p):
		angle = self._startAngle*3.1415927/180.0
		step = self._arcLength/(self._numMajorTicks-1)*3.1415927/180.0

		f = p.font()
		if self._externalScale:
			f.setPointSizeF(4.5)
		else:
			f.setPointSizeF(4.5)
		p.setFont(f)

		for i in range(self._numMajorTicks):
			vertice = QPointF(self._labelRadius*cos(angle), -self._labelRadius*sin(angle))
			vertice -= QPointF(13,10)
			br = QRectF(vertice, QSizeF(26, 20))
			p.drawText(br, Qt.AlignCenter, self.labels[i])
			angle -= step

	def drawValue(self, p):
		if self.isEnabled():
			if self._valueFormat.contains('%'):
				s = QString().sprintf(self._valueFormat.toAscii(), self._value)
			else:
				s = QString.number(self._value)
		else:
			s = "####"

		f = p.font()
		if self._scaleEnabled:
			f.setPointSize(8)
			x = -16 
			y = 5 
			w = 32 
			h = 12
		else:
			f.setPointSize(14)
			x = -24 
			y = 8 
			w = 48 
			h = 20
		p.setFont(f)

		check = True
		while check:
			if (p.fontMetrics().width(s) > (w-2)):
				f.setPointSize(f.pointSize()-1)
				p.setFont(f)
			else:
				check = False
		textRect = QRect(x,y,w,h)
		p.setBrush(QBrush(QColor(255,255,255,150)))
		p.drawRect(textRect)
		#p.drawText(textRect, Qt.AlignCenter, s)
		p.drawText(x,y+1,w,h, Qt.AlignCenter|Qt.TextDontClip, s)
	
	def setValueDisplayed(self, b):
		self._valueDisplayed = b
		self.update()
	
	def isValueDisplayed(self):
		return self._valueDisplayed

	def setValueFormat(self, s):
		self._valueFormat = s
		self.configure()
		self.update()
	
	def valueFormat(self):
		return self._valueFormat
	
