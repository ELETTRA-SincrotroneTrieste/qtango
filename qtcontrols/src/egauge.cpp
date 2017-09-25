#include "egauge.h"
#include <QPainter>
#include <QPaintEvent>
#include <QConicalGradient>
#include <QRadialGradient>
#include <QPolygonF>
#include <QApplication>
#include <elettracolors.h>
#include <macros.h>
#include <eng_notation.h>
#include <math.h>

#include <QtDebug>

EAbstractGauge::EAbstractGauge(QWidget *parent) : QWidget(parent),
	m_colorMode(COLORBAR),
	m_referenceEnabled(true), m_scaleEnabled(true),
	m_numMajorTicks(LINEAR_MAJOR_TICKS), longestLabelIndex(-1),
	m_normalColor(EColor(Elettra::green)), m_warningColor(EColor(Elettra::orange)), m_errorColor(EColor(Elettra::red)),
	d_logarithmicScale(false)
{
	m_minValue = -15;
	m_maxValue = 15;
	m_lowError = -10;
	m_highError = 10;
	m_lowWarning = -5;
	m_highWarning = 5;
	m_reference = 3;
	m_value = 0;
	d_naturalMinV = m_minValue;
	d_naturalMaxV = m_maxValue;
	d_naturalLowWarn = m_lowWarning;
	d_naturalHighWarn = m_highWarning;

	d_naturalLowErr = m_lowError;
	d_naturalHighErr = m_highError;
	d_naturalValue = m_value;
	d_naturalReference = m_reference;
	
	m_valueFormat = "%.1f";
	
	configure();
}

double EAbstractGauge::logarithm(double v)
{
  double logar;
  if(v > 0)
	logar = log10(v);
  else 
  {
	perr("Gauge \"%s\": logarithmic scale but value %.2f <= 0", qstoc(objectName()), v);
	logar = 0;
  }
  return logar;
}

void EAbstractGauge::setLogarithmicScale(bool en)
{
  d_logarithmicScale = en;
  /* high and low warn */
  d_logarithmicScale ?  m_lowWarning = logarithm(d_naturalLowWarn) : m_lowWarning = d_naturalLowWarn;
  d_logarithmicScale ?  m_highWarning = logarithm(d_naturalHighWarn) : m_highWarning = d_naturalHighWarn;
  /* errors */
  d_logarithmicScale ?  m_lowError = logarithm(d_naturalLowErr) : m_lowError = d_naturalLowErr;
  d_logarithmicScale ?  m_highError = logarithm(d_naturalHighErr) : m_highError = d_naturalHighErr;
  /* min and max value */
  d_logarithmicScale ?  m_minValue = logarithm(d_naturalMinV) : m_minValue = d_naturalMinV;
  d_logarithmicScale ?  m_maxValue = logarithm(d_naturalMaxV) : m_maxValue = d_naturalMaxV;
  /* value */
  d_logarithmicScale ? m_value = logarithm(d_naturalValue) : m_value = d_naturalValue;
  /* reference */
  d_logarithmicScale ? m_reference = logarithm(d_naturalReference) : m_reference = d_naturalReference;
  configure();
  update();
}

void EAbstractGauge::setLowWarning(double w)
{
	d_naturalLowWarn = w;
	if(d_logarithmicScale)
	  m_lowWarning = logarithm(w);
	else
	  m_lowWarning = w; 
	configure();
	update(); 
}

void EAbstractGauge::setHighWarning(double w)
{
	d_naturalHighWarn = w;
	if(d_logarithmicScale)
	  m_highWarning = logarithm(w);
	else
	  m_highWarning = w; 

	configure();
	update(); 
}

void EAbstractGauge::setLowError(double e)
{ 
	d_naturalLowErr = e;
	if(d_logarithmicScale)
	  m_lowError = logarithm(e);
	else
	  m_lowError = e; 

	configure(); 
	update(); 
}

void EAbstractGauge::setHighError(double e)
{ 
	d_naturalHighErr = e;
	if(d_logarithmicScale)
	  m_highError = logarithm(e);
	else
	  m_highError = e; 

	configure(); 
	update(); 
}

void EAbstractGauge::setMinValue(double v)
{ 
  d_naturalMinV = v;
  if(d_logarithmicScale)
	m_minValue = logarithm(v);
  else
	m_minValue = v; 
  configure(); 
  update(); 
}

void EAbstractGauge::setMaxValue(double v)
{ 
  d_naturalMaxV = v;
   if(d_logarithmicScale)
	m_maxValue = logarithm(v);
  else
	m_maxValue = v; 
  configure(); 
  update(); 
}

void EAbstractGauge::setReference(double r)
{ 
	d_naturalReference = r;
	if(d_logarithmicScale)
	  m_reference = logarithm(r);
	else
	  m_reference = r; 
	update(); 
}

void EAbstractGauge::setScaleEnabled(bool b)
{ 
	m_scaleEnabled = b; 
	configure(); 
	update(); 
}

void EAbstractGauge::setReferenceEnabled(bool b)
{ 
	m_referenceEnabled = b; 
	configure(); 
	update(); 
}

void EAbstractGauge::setColorMode(ColorMode m)
{ 
	m_colorMode = m; 
	configure(); 
	update(); 
}

void EAbstractGauge::setNormalColor(QColor c)
{ 
	m_normalColor = c; 
	configure(); 
	update(); 
}

void EAbstractGauge::setWarningColor(QColor c)
{ 
	m_warningColor = c; 
	configure(); 
	update(); 
}

void EAbstractGauge::setErrorColor(QColor c)
{ 
	m_errorColor = c; 
	configure(); 
	update(); 
}

void EAbstractGauge::setExternalScale(bool b)
{ 
	m_externalScale = b; 
	configure(); 
	update(); 
}

void EAbstractGauge::setNumMajorTicks(int t)
{ 
	m_numMajorTicks = t; 
	configure(); 
}

void EAbstractGauge::setNumMinorTicks(int t)
{ 
	m_numMinorTicks = t;
	configure(); 
}

void EAbstractGauge::configure()
{ 
	qreal interval = m_maxValue - m_minValue;
	v_p.clear();
	v_c.clear();
	if (m_colorMode == GRADIENT)
	{
		v_p << m_minValue << (m_minValue+m_lowError)*.5;
		v_p << (m_lowError+m_lowWarning)*.5;
		v_p << (m_lowWarning+m_highWarning)*.5 << (m_highWarning+m_highError)*.5;
		v_p << (m_highError+m_maxValue)*.5 << m_maxValue;
		v_c << m_errorColor << m_errorColor << m_warningColor << m_normalColor;
		v_c << m_warningColor << m_errorColor << m_errorColor;
		if ((m_minValue == m_lowError) && (m_lowError == m_lowWarning))
			v_c[0] = v_c[1] = v_c[2] = m_normalColor;
		if ((m_maxValue == m_highError) && (m_highError == m_highWarning))
			v_c[4] = v_c[5] = v_c[6] = m_normalColor;
	}
	else if (m_colorMode == COLORBAR)
	{
		v_p << m_minValue << m_lowError << m_lowWarning;
		v_c << m_errorColor    << m_errorColor    << m_warningColor;
		v_p << m_highWarning << m_highError << m_maxValue;
		v_c << m_normalColor     << m_warningColor  << m_errorColor;
	}
	else
	{
		if (m_value < m_lowError)
			v_c << m_errorColor;
		else if (m_value < m_lowWarning)
			v_c << m_warningColor;
		else if (m_value < m_highWarning)
			v_c << m_normalColor;
		else if (m_value < m_highError)
			v_c << m_warningColor;
		else 
			v_c << m_errorColor;
	}

	for (int i = 0; i < v_p.size(); i++)
		v_p[i] = (v_p[i] - m_minValue) / interval;

	labels.clear();
	double val = m_minValue;
	double tick = (m_maxValue-m_minValue)/((m_numMajorTicks-1)*m_numMinorTicks);
	double step = qRound((m_maxValue-m_minValue)/(m_numMajorTicks-1)/tick)*tick;
	longestLabelIndex = -1;
	for (int i = 0; i < m_numMajorTicks; i++)
	{
		double representedValue = val;
#ifdef __FreeBSD__
		d_logarithmicScale ? representedValue  = pow(val, 10) : representedValue  = val;
#else
                d_logarithmicScale ? representedValue  = exp10(val) : representedValue  = val;
#endif
        EngString engString(QString().sprintf(qstoc(m_valueFormat), representedValue), m_valueFormat, representedValue);
// 		QString s = QString::number(representedValue);
		if ((longestLabelIndex == -1) || engString.length() > labels[longestLabelIndex].length())
			longestLabelIndex = i;
		labels << engString;
		val += step;
	}
}

void EAbstractGauge::setValue(double v)
{ 
  /* v is a natural value, not logarithmic */
	if(v == d_naturalValue)
		return;
	
	d_naturalValue = v;
	d_logarithmicScale ? m_value = logarithm(v) : m_value = v;
	
	m_value = qBound(m_minValue, m_value, m_maxValue); 
	
	if (m_colorMode == SINGLECOLOR)
	{
		v_c.clear();
		if (m_value < m_lowError)
			v_c << m_errorColor;
		else if (m_value < m_lowWarning)
			v_c << m_warningColor;
		else if (m_value < m_highWarning)
			v_c << m_normalColor;
		else if (m_value < m_highError)
			v_c << m_warningColor;
		else 
			v_c << m_errorColor;
	}
	update();
}

void EAbstractGauge::setValue(int v)
{
	setValue((double)v);
}

ELinearGauge::ELinearGauge(QWidget *parent, Qt::Orientation o) : EAbstractGauge(parent), m_orientation(o), m_fillMode(ALL)
{
	/* it is important that the font is installed since it has to scale well */
	QFont::insertSubstitutions("FreeSans", QStringList()  << "Verdana" << "Sans Serif");
	setNumMajorTicks(LINEAR_MAJOR_TICKS);
	setNumMinorTicks(NUM_MINOR_TICKS);
	QFont f = this->font();
	f.setPointSize(6);
	f.setFamily("FreeSans");
	this->setFont(f);
}

void ELinearGauge::configure()
{
	int shortSide, longSide;
	if (m_scaleEnabled)
	{
		longSide = 80;
		shortSide = 30;
	}
	else
	{
		longSide = 60;
		shortSide = 20;
	}
		

	if (m_orientation == Qt::Horizontal)
	{
		if (m_scaleEnabled)
			scalePos = 20;
		else
			scalePos = 0;
		labelsPos = 12;
		totalSize = 30;
		barSize = totalSize-scalePos;
		scaleSize = scalePos-labelsPos;
		labelsSize = labelsPos;
		setMinimumSize(longSide,shortSide);
	}
	else
	{
		if (m_scaleEnabled)
			scalePos = 10;
		else
			scalePos = 30;
		labelsPos = 18;
		totalSize = 30;
		barSize = scalePos;
		scaleSize = labelsPos-scalePos;
		labelsSize = totalSize-labelsPos;
		setMinimumSize(shortSide,longSide);
	}
	EAbstractGauge::configure();
}
	
void ELinearGauge::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	int size, w, h;
	QFontMetrics fm(painter.font());
	
	h = fm.height()+2;
	w = fm.width(labels[longestLabelIndex])+2;
	
	if (m_orientation == Qt::Horizontal)
		size = qMin((int)(width()*totalSize/100.0), height());
	else
		size = qMin((int)(height()*totalSize/100.0), width());
	
	painter.setRenderHint(QPainter::Antialiasing);
	
	if (m_orientation == Qt::Horizontal)
	{
		painter.setViewport((int)((width()-size*100.0/totalSize)*.5),(int)((height()-size)*.5), (int)(size*100.0/totalSize), size);
		painter.setWindow((int)(-w*.5), 0, 100+w, totalSize+2); /* border */
	}
	else
	{
		painter.setViewport((int)((width()-size)*.5),(int)((height()-size*100.0/totalSize)*.5), size, (int)(size*100.0/totalSize));
		painter.setWindow(-2, (int)(-h*.5), totalSize+2, 100+h); /* border */
	}

	drawColorBar(&painter);
	if (m_scaleEnabled)
	{
		drawScale(&painter);
		drawLabels(&painter);
	}
	if (isEnabled() && (m_fillMode == ALL))
		drawMarker(&painter, true);
	
	if (!isEnabled())
	{
		QColor c = palette().color(QPalette::Background);
		c.setAlpha(200);
		painter.fillRect(painter.window(), c);
	}
}

void ELinearGauge::drawColorBar(QPainter *p)
{
	if (m_colorMode == GRADIENT)
	{
		QPoint start, stop;
		QRect r;
		if (m_orientation == Qt::Horizontal)
		{
			start = QPoint(0,0);
			stop = QPoint(100,0);
			r = QRect(0, scalePos, 100, barSize);
		}
		else
		{
			start = QPoint(0,100);
			stop = QPoint(0,0);
			r = QRect(0, 0, barSize, 100);
		}
		QLinearGradient	grad(start, stop);
		for (int i = 0; i < v_p.size(); i++)
			grad.setColorAt(v_p[i], v_c[i]);
		p->setBrush(grad);
		p->drawRect(r);
	}
	else if (m_colorMode == COLORBAR)
	{
		p->setPen(Qt::NoPen);
		if (m_orientation == Qt::Horizontal)
		{
			QVector<qreal> widths;
			foreach(qreal w, v_p)
				widths << w * 100;
			for (int i = 1; i < v_p.size(); i++)
			{
				p->setBrush(v_c[i]);
				p->drawRect(QRectF(widths[i-1], scalePos, widths[i]-widths[i-1], barSize));
			}
			p->setBrush(Qt::NoBrush);
			p->setPen(Qt::black);
			p->drawRect(0, scalePos, 100, barSize);
		}
		else
		{
			QVector<qreal> heights;
			foreach(qreal h, v_p)
				heights << 100*(1-h);
			for (int i = 1; i < v_p.size(); i++)
			{
				p->setBrush(v_c[i]);
				p->drawRect(QRectF(0, heights[i], barSize, heights[i-1]-heights[i]));
			}
			p->setBrush(Qt::NoBrush);
			p->setPen(Qt::black);
			p->drawRect(0, 0, barSize, 100);
		}
	}
	else if (m_colorMode == SINGLECOLOR)
	{
		p->setBrush(v_c[0]);
		if (m_orientation == Qt::Horizontal)
			p->drawRect(0, scalePos, 100, barSize);
		else
			p->drawRect(0, 0, barSize, 100);
	}	

	if (m_fillMode != ALL)
	{
		float v1, v2, min = 0.0, max = 0.0;
		v1 = (m_value-m_minValue)/(m_maxValue-m_minValue);
	
		p->setPen(Qt::black);
		p->setBrush(palette().background());
		if (m_fillMode == FROM_MIN)
		{
			max = v1;
			min = 0.0;
		}
		else if (m_fillMode == FROM_ZERO)
		{
			v2  = (-m_minValue)/(m_maxValue-m_minValue);
			min = qMin(v1, v2);
			max = qMax(v1, v2);
		}
		else if (m_fillMode == FROM_REF)
		{
			v2  = (m_reference-m_minValue)/(m_maxValue-m_minValue);
			min = qMin(v1, v2);
			max = qMax(v1, v2);
		}
		
		if (m_orientation == Qt::Horizontal)
		{
			p->drawRect(QRectF(0, scalePos, 100.0*min, barSize));
			p->drawRect(QRectF(100.0*max, scalePos, 100.0*(1-max), barSize));
		}
		else
		{
			p->drawRect(QRectF(0.0, 100*(1-min), barSize, 100*min));
			p->drawRect(QRectF(0.0, 0.0, barSize, 100.0*(1-max)));
		}
	}
}

void ELinearGauge::drawScale(QPainter *p)
{
	QVector<QLineF> lines;

	if (m_orientation == Qt::Horizontal)
	{
		qreal x1=0, y1=scalePos, wM=scaleSize, wm=wM*.5;
		QLineF majorTickLine(x1,y1,x1,y1-wM);
		qreal interval = 100.0/(m_numMajorTicks-1)/(m_numMinorTicks); /* distance between 2 minor ticks */
	
		for (int i = 0; i < (m_numMajorTicks-1); i++)
		{
			lines << majorTickLine;
			QLineF minorTickLine(majorTickLine.x1(), majorTickLine.y1(), majorTickLine.x2(), majorTickLine.y1()-wm);
			for (int j = 0; j < (m_numMinorTicks-1); j++)
			{
				minorTickLine.translate(QPointF(interval,0));
				lines << minorTickLine;
			}
			majorTickLine.translate(QPointF(100.0/(m_numMajorTicks-1),0));
		}
		lines << majorTickLine;
	}
	else
	{
		qreal x1=scalePos, y1=0, wM=scaleSize, wm=wM*.5;
		QLineF majorTickLine(x1, y1, x1+wM, y1);
		qreal interval = 100.0/(m_numMajorTicks-1)/(m_numMinorTicks); /* distance between 2 minor ticks */
		for (int i = 0; i < (m_numMajorTicks-1); i++)
		{
			lines << majorTickLine;
			QLineF minorTickLine(majorTickLine.x1(), majorTickLine.y1(), majorTickLine.x1()+wm, majorTickLine.y1());
			for (int j = 0; j < (m_numMinorTicks-1); j++)
			{
				minorTickLine.translate(QPointF(0, interval));
				lines << minorTickLine;
			}
			majorTickLine.translate(QPointF(0, 100.0/(m_numMajorTicks-1)));
		}
		lines << majorTickLine;
	}
	p->drawLines(lines);
	if (m_referenceEnabled)
		drawMarker(p, false);
}

void ELinearGauge::drawMarker(QPainter *p, bool drawValue)
{
	QPolygonF triangolo;
	QColor c;
	QFont f, old;

	f = old = p->font();
	f.setPointSize(3);						
	p->setFont(f);
	p->setPen(EColor(Elettra::red));

	if (m_orientation == Qt::Horizontal)
	{
		if (drawValue)
		{
			QPointF	vertice(100*(m_value-m_minValue)/(m_maxValue-m_minValue), scalePos);
			triangolo << vertice << (vertice + QPointF(4,8)) << (vertice + QPointF(-4,8));
		}
		else
		{
			QPointF	vertice(100*(m_reference-m_minValue)/(m_maxValue-m_minValue), scalePos);
			triangolo << vertice << (vertice + QPointF(2,-4)) << (vertice + QPointF(-2,-4));
			p->drawText(vertice+QPoint(-4,-5), "REF");
		}
	}
	else
	{
		if (drawValue)
		{
			QPointF	vertice(scalePos, 100*(1-(m_value-m_minValue)/(m_maxValue-m_minValue)));
			triangolo << vertice << (vertice + QPointF(-8,-4)) << (vertice + QPointF(-8,4));
		}
		else
		{
			QPointF	vertice(scalePos, 100*(1-(m_reference-m_minValue)/(m_maxValue-m_minValue)));
			triangolo << vertice << (vertice + QPointF(4,-2)) << (vertice + QPointF(4,2));
			p->drawText(vertice+QPointF(5,1.5), "REF");
		}
	}
	p->setFont(old);
	p->setPen(Qt::black);

	if (drawValue)
		c = EColor(Elettra::darkBlue);
	else
		c = EColor(Elettra::red);
	c.setAlpha(100);
	p->setBrush(c);
	p->drawPolygon(triangolo);
}

void ELinearGauge::drawLabels(QPainter *p)
{
	double x, y, w, h;
	bool check = true;
	QFont f = p->font();
	QFontMetrics fm(f);
	h = fm.height();
	w = fm.width(labels[longestLabelIndex]);
			
	if (m_orientation == Qt::Horizontal)
        {
		y = 0;
		h = labelsSize;
		while (check)
		{
			if ((p->fontMetrics().width(labels[longestLabelIndex]) > (p->window().width()/m_numMajorTicks)))
			{
				f.setPointSize(f.pointSize()-1);
				p->setFont(f);
			}
			else
				check = false;
		}
		for (int i = 0; i < m_numMajorTicks; i++)
		{
			QRectF br(i*100.0/(m_numMajorTicks-1)-w*.5, y, w, h);
			p->drawText(br, Qt::AlignBottom | Qt::AlignHCenter | Qt::TextDontClip, labels[i]);
		}
	}
	else
	{
		x = labelsPos;
		w = labelsSize;
		while (check)
		{
			if (((p->fontMetrics().height()>(p->window().height()/m_numMajorTicks))||p->fontMetrics().width(labels[longestLabelIndex])>labelsSize)) 
			{
				f.setPointSize(f.pointSize()-1);
				p->setFont(f);
			}
			else
				check = false;
		}

		for (int i = 0; i < m_numMajorTicks; i++)
		{
			QRectF br(x, i*100.0/(m_numMajorTicks-1)-h*.5, w, h);
			p->drawText(br, Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip, labels[m_numMajorTicks-1-i]);
		}
	}
}

ECircularGauge::ECircularGauge(QWidget *parent) : EAbstractGauge(parent),
	m_startAngle(240), m_stopAngle(300), m_arcLength(300),
	m_innerRadius(25), m_outerRadius(30), m_labelRadius(45),
	m_label(""), m_valueDisplayed(true)
{
	setExternalScale(false);
	setNumMajorTicks(CIRCULAR_MAJOR_TICKS);
	setNumMinorTicks(NUM_MINOR_TICKS);
	setMinimumSize(70,70);

	/* it is important that the font is installed since it has to scale well */
	QFont::insertSubstitutions("FreeSans", QStringList()  << "Verdana" << "Sans Serif");
	
	QFont f = this->font();
	f.setFamily("FreeSans"); /* Free sans scales ugly */
// 	printf("Circular gauge font family %s\n", qstoc(f.family()));
	setFont(f);
}

void ECircularGauge::configure()
{
	if (m_scaleEnabled)
	{
		if (m_externalScale)
		{
			m_innerRadius = 25;
			m_outerRadius = 30;
			m_labelRadius = 46;
			setMinimumSize(80,80);
		}
		else
		{
			if (m_referenceEnabled)
			{
				m_innerRadius = 37;
				m_outerRadius = 46;
				m_labelRadius = 32;
			}
			else
			{
				m_innerRadius = 40;
				m_outerRadius = 49;
				m_labelRadius = 35;
			}
			setMinimumSize(70,70);
		}
	}
	else
	{
		m_innerRadius = 40;
		m_outerRadius = 49;
		m_labelRadius = 50;
		setMinimumSize(50,50);
	}
	EAbstractGauge::configure();
}
	
void ECircularGauge::paintEvent(QPaintEvent *)
{
	QPainter	painter(this);
	int size = qMin(height(), width());
	
	/* see http://doc.trolltech.com/4.4/coordsys.html#window-viewport-conversion for a analogous example */
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setViewport((int)((width()-size)*.5),(int)((height()-size)*.5),size,size);
	painter.setWindow(-50,-50,100,100);
		
	drawColorBar(&painter);
	
	if (isEnabled())
		drawNeedle(&painter);
	
	if (m_valueDisplayed)
		drawValue(&painter);
	
	if (!isEnabled())
	{
		QColor c = palette().color(QPalette::Background);
		c.setAlpha(200);
		painter.fillRect(painter.window(), c);
	}
	if (m_scaleEnabled)
	{
		drawScale(&painter);
		drawLabels(&painter);
	}
}

void ECircularGauge::drawColorBar(QPainter *p)
{
	p->setPen(Qt::NoPen);
	if (m_colorMode == GRADIENT)
	{
		QConicalGradient grad(0, 0, m_stopAngle);
		for (int i = 0; i < v_p.size(); i++)
			grad.setColorAt((1-v_p[i])*m_arcLength/360.0, v_c[i]);
		p->setBrush(grad);
		p->drawPie(-m_outerRadius,-m_outerRadius,m_outerRadius*2,m_outerRadius*2, (int)m_startAngle*16, (int)-m_arcLength*16);
	}
	else if (m_colorMode == COLORBAR)
	{
		for (int i = 1; i < v_p.size(); i++)
		{
			p->setBrush(v_c[i]);
			p->drawPie(-m_outerRadius,-m_outerRadius,m_outerRadius*2,m_outerRadius*2, (int)((m_startAngle-m_arcLength*v_p[i])*16), (int)((m_arcLength*v_p[i]-m_arcLength*v_p[i-1])*16));
		}
	}
	else
	{
		p->setBrush(v_c[0]);
		p->drawPie(-m_outerRadius,-m_outerRadius,m_outerRadius*2,m_outerRadius*2, (int)(m_startAngle*16), (int)-(m_arcLength*16));
	}
	p->setBrush(palette().background());
	p->drawEllipse(-m_innerRadius,-m_innerRadius,m_innerRadius*2,m_innerRadius*2);
}

void ECircularGauge::drawNeedle(QPainter *p)
{
	double angle = (m_startAngle-(m_value-m_minValue)/(m_maxValue-m_minValue)*m_arcLength)*3.1415927/180.0;

	QPolygonF tr1, tr2;
	QPointF longArm, shortArm, side1, side2;

	longArm = QPointF(m_outerRadius*cos(angle),-m_outerRadius*sin(angle));
	shortArm = QPointF(-2*cos(angle),2*sin(angle));
	side1 = QPointF(-2*sin(angle),-2*cos(angle));
	side2 = -side1;
	
	tr1 << longArm << side1 << shortArm;
	tr2 << longArm << side2 << shortArm;

	p->setPen(Qt::NoPen);
	QRadialGradient grad1(QPointF(0,0),m_outerRadius,side1*.5);
	grad1.setColorAt(0.0, palette().color(QPalette::Mid));
	grad1.setColorAt(1.0, palette().color(QPalette::Dark));
	QRadialGradient grad2(QPointF(0,0),m_outerRadius,side2*.5);
	grad2.setColorAt(0.0, palette().color(QPalette::Midlight));
	grad2.setColorAt(1.0, palette().color(QPalette::Dark));
	p->setBrush(grad1);
	p->drawPolygon(tr1);
	p->setBrush(grad2);
	p->drawPolygon(tr2);
	
	QPen pen(Qt::black);
	pen.setJoinStyle(Qt::RoundJoin);
	p->setPen(pen);
	p->drawLine(longArm,side1);
	p->drawLine(side1,shortArm);
	p->drawLine(shortArm,side2);
	p->drawLine(side2,longArm);

	p->setBrush(palette().color(QPalette::Dark));
	p->drawEllipse(QRectF(-.5,-.5,1,1));
}

void ECircularGauge::drawScale(QPainter *p)
{
	p->setPen(Qt::black);
	p->drawArc(-(m_outerRadius+0), -(m_outerRadius+0), 2*(m_outerRadius+0), 2*(m_outerRadius+0), (int)m_startAngle*16, (int)-m_stopAngle*16);

	qreal start = m_startAngle*3.1415927/180.0;
	QPointF startPoint = QPointF((m_outerRadius+0)*cos(start),-(m_outerRadius+0)*sin(start));
	QPointF stopPoint, stopPoint2;
	if (m_externalScale)
	{
		stopPoint = startPoint + QPointF(8*cos(start),-8*sin(start));
		stopPoint2 = startPoint + QPointF(4*cos(start),-4*sin(start));
	}
	else
	{
		stopPoint = startPoint - QPointF(8*cos(start),-8*sin(start));
		stopPoint2 = startPoint - QPointF(4*cos(start),-4*sin(start));
	}
	QLineF majorTickLine(startPoint, stopPoint);
	QLineF minorTickLine(startPoint, stopPoint2);

	p->save();
	for (int i = 0; i < m_numMajorTicks; i++)
	{
		p->drawLine(majorTickLine);
		if (i < (m_numMajorTicks-1))
		{
			for (int j = 0; j < (m_numMinorTicks-1); j++)
			{
				p->rotate(m_arcLength/(m_numMajorTicks-1)/m_numMinorTicks);
				p->drawLine(minorTickLine);
			}
			p->rotate(m_arcLength/(m_numMajorTicks-1)/m_numMinorTicks);
		}
	}
	p->restore();
	if (m_referenceEnabled)
	{
		double angle = (m_startAngle-(m_reference-m_minValue)/(m_maxValue-m_minValue)*m_arcLength)*3.1415927/180.0;
		double shift = 2.5*3.1415927/180.0;
		QPolygonF tr;
		QPointF p1, p2, p3;
		p1 = QPointF(m_outerRadius*cos(angle),-m_outerRadius*sin(angle));
		p2 = QPointF((m_outerRadius+3)*cos(angle+shift),-(m_outerRadius+3)*sin(angle+shift));
		p3 = QPointF((m_outerRadius+3)*cos(angle-shift),-(m_outerRadius+3)*sin(angle-shift));
		tr << p1 << p2 << p3;

		QColor c = EColor(Elettra::red);
		c.setAlpha(100);
		p->setBrush(c);
		p->setPen(Qt::black);
		p->drawPolygon(tr);
	}
}

void ECircularGauge::drawLabels(QPainter *p)
{
	double angle = m_startAngle*3.1415927/180.0;
	double step = m_arcLength/(m_numMajorTicks-1)*3.1415927/180.0;

	QFont f = p->font();
	if (m_externalScale)
		f.setPointSizeF(5.5);
	else
		f.setPointSizeF(5.5);
	p->setFont(f);
   for (int i = 0; i < m_numMajorTicks; i++)
    {
		QPointF vertice(m_labelRadius*cos(angle), -m_labelRadius*sin(angle));
		vertice -= QPointF(13,10);
		QRectF br(vertice, QSize(26, 20));
		br = QRectF(vertice, QSize(26, 20));
		p->drawText(br, Qt::AlignCenter, labels[i]);
		angle -= step;
	}
}

void ECircularGauge::drawValue(QPainter *p)
{
	int x, y, w, h;
	QString s;
	if (isEnabled())
	{
      EngString engString(QString().sprintf(qstoc(m_valueFormat), d_naturalValue), m_valueFormat, d_naturalValue);
	  s = engString;
	}
	else
		s = "####";
	QFont f = p->font();
	if (m_scaleEnabled)
	{
		f.setPointSize(7);
		x = -16; y = 5; w = 32; h = 12;
	}
	else
	{
		f.setPointSize(10);
		x = -20; y = 8; w = 40; h = 16;
	}
	p->setFont(f);
	bool check = true;
	while (check)
	{
		if ((p->fontMetrics().width(s) > (w-2)))
		{
			f.setPointSizeF(f.pointSizeF()-.5);
			p->setFont(f);
		}
		else
			check = false;
	}
	QRect textRect(x,y,w,h);
	p->setBrush(QColor(255,255,255,150));
	p->drawRect(textRect);
	p->drawText(x,y+1,w,h, Qt::AlignCenter|Qt::TextDontClip, s);
	
	f.setPointSize(8);
	p->setFont(f);
	check = true;
	while (check)
	{
		if ((p->fontMetrics().width(m_label) > (38)))
		{
			f.setPointSizeF(f.pointSizeF()-.5);
			p->setFont(f);
		}
		else
			check = false;
	}
	p->drawText(-20,35,40,10, Qt::AlignCenter|Qt::TextDontClip, m_label);
}
