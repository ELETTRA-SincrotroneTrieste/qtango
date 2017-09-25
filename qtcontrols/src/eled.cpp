#include "eled.h"
#include "elettracolors.h"
#include <QPainter>
#include <QRadialGradient>
#include <QtDebug>
#include <math.h>

ELed::ELed(QWidget *parent) : QWidget(parent), ledColor(Qt::gray)
{
	d_width = d_height = 18;
	d_angleDeg = 0;
	d_rectangular = false;
	d_gradientEnabled = true;
	d_linearGradient = false;
	d_scaleContents = false;
	d_gradientStart = 0.10;
	d_gradientStop = 0.95;
	d_alphaChannel = 255;
	return;
}

void ELed::setColor(const QColor &c, bool up)
{
	if(c == ledColor)
		return;

	ledColor = c;
	ledColor.setAlpha(d_alphaChannel);
	if (up)
		update();
};

void ELed::paintEvent(QPaintEvent *)
{
	QPainter	painter(this);
	QMatrix m;
        int ledWidth, ledHeight;
        d_scaleContents ? ledWidth = width() - 2 : ledWidth = d_width;
        d_scaleContents ? ledHeight = height() - 2: ledHeight = d_height;
// 	painter.rotate(d_angleDeg);
	qreal		radius  = .45 * qMin(ledWidth, ledHeight);
	qreal x1, y1;
	x1 = width()/2.0 - ledWidth/2.0;
	y1 = height()/2.0 - ledHeight/2.0;
	
	QPointF  topLeft(x1, y1);
	QRect rect(x1, y1, ledWidth, ledHeight);
	QPointF center(rect.center());
	
	if(!d_linearGradient && d_gradientEnabled)
	{
	 QRadialGradient	gradient(.5 * width(), .5 * height(), radius, .5 * (width() - radius), .5 * (height() - radius));
// 	  QRadialGradient	gradient(center, qMin(ledWidth, ledHeight));
	  gradient.setColorAt(d_gradientStart, EColor(Elettra::white));
	  if (isEnabled())
		  gradient.setColorAt(d_gradientStop, ledColor);
	  else
		  gradient.setColorAt(d_gradientStop, EColor(Elettra::gray));
	  gradient.setColorAt(1, EColor(Elettra::black));
	  painter.setBrush(gradient);
	}
	else if(d_gradientEnabled && d_linearGradient) 
	{
	  QLinearGradient	gradient(rect.topLeft(), rect.bottomRight());
	  gradient.setColorAt(d_gradientStart, EColor(Elettra::white));
	  if (isEnabled())
		  gradient.setColorAt(d_gradientStop, ledColor);
	  else
		  gradient.setColorAt(d_gradientStop, EColor(Elettra::gray));
	  gradient.setColorAt(1, EColor(Elettra::black));
	  painter.setBrush(gradient);
	}
	else if(!d_gradientEnabled)
	{
	  QBrush b(ledColor);
	  painter.setBrush(b);
	}
	painter.setRenderHint(QPainter::Antialiasing);
	
	m.translate(center.x(), center.y());
	m.rotate(d_angleDeg);
	m.translate(-center.x(), -center.y());
	painter.setWorldMatrix(m, true);
	
	if(d_rectangular)
	  painter.drawRect(rect);
	else
	 painter.drawEllipse(rect);

}

void ELed::setAlphaChannel(int a)
{
  if(a < 0)
  {
    printf("\e[1;33m*\e[0m alpha channel %d is not valid: it must be an integer between 0 and 255. Setting to 0", a);
    d_alphaChannel = 0;
  }
  else if(a > 255)
  {
     printf("\e[1;33m*\e[0m alpha channel %d is not valid: it must be an integer between 0 and 255. Setting to 255", a);
    d_alphaChannel = 255;
  }
  d_alphaChannel = a;
  ledColor.setAlpha(a);
  update();
}

void ELed::setLinearGradient(bool l)
{
	d_linearGradient = l;
	update();
}

void ELed::setGradientEnabled(bool grad)
{
  d_gradientEnabled = grad;
  update();
}

void ELed::setAngle(int deg)
{
  d_angleDeg = deg;
  update();
}
	
void ELed::setLedWidth(int w)
{
//  setMinimumWidth(w + 2);
//  if(maximumWidth() < minimumWidth())
//  	setMaximumWidth(minimumWidth());
//  if(w < this->width())
    d_width = w;
//  else
//    d_width = this->width();
  update();
}
	
void ELed::setLedHeight(int h)
{
 //  setMinimumHeight(h + 2);
//   if(maximumHeight() < minimumHeight())
//   	setMaximumHeight(minimumHeight());
//  if(h < this->height())
    d_height = h;
//  else
//    d_height = this->height();
  update();
}

void ELed::setRectangular(bool re)
{
  d_rectangular = re;
  update();
}

void ELed::setScaleContents(bool s)
{
  d_scaleContents = s;
  update();
}

void ELed::setGradientStop(double val)
{
  d_gradientStop = val;
  update();
}

void ELed::setGradientStart(double val)
{
  d_gradientStart = val;
  update();
}

QSize ELed::sizeHint() const
{
  return QSize(d_width, d_height);
}

QSize ELed::minimumSizeHint() const
{
  return this->sizeHint();
}

	
