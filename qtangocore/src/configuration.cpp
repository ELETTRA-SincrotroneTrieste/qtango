
#include "configuration.h"
#include <elettracolors.h>


Config* Config::_instance = NULL;

Config::Config()
{
        initStateColorsAndStrings();
		initQualityColorsAndStrings();
		d_valueAttrPropName = "values";
}


Config* Config::instance()
{
  if(_instance == NULL)
    _instance = new Config();
  
  return _instance;
}

void Config::initStateColorsAndStrings()
{
        d_stateStrings[Tango::ON] = "ON";
        d_stateStrings[Tango::OFF] = "OFF";
        d_stateStrings[Tango::CLOSE] = "CLOSED";
        d_stateStrings[Tango::OPEN] = "OPENED";
        d_stateStrings[Tango::INSERT]     = "INSERT";
        d_stateStrings[Tango::EXTRACT]    = "EXTRACT";
        d_stateStrings[Tango::MOVING]     = "MOVING";
        d_stateStrings[Tango::STANDBY]    = "STANDBY";
        d_stateStrings[Tango::FAULT]      = "FAULT";
        d_stateStrings[Tango::INIT]       = "INIT";
        d_stateStrings[Tango::RUNNING]    = "RUNNING";
        d_stateStrings[Tango::ALARM]      = "ALARM";
        d_stateStrings[Tango::DISABLE]    = "DISABLE";
        d_stateStrings[Tango::UNKNOWN]    = "UNKNOWN";

        d_stateColors[Tango::ON]          = EColor(Elettra::green);
		d_stateColors[Tango::OFF]         = Qt::white;
        d_stateColors[Tango::CLOSE]       = Qt::white;
        d_stateColors[Tango::OPEN]        = Qt::white;
        d_stateColors[Tango::INSERT]      = Qt::white;
        d_stateColors[Tango::EXTRACT]     = Qt::white;
        d_stateColors[Tango::MOVING]      = EColor(Elettra::blue);
        d_stateColors[Tango::STANDBY]     = EColor(Elettra::yellow);
        d_stateColors[Tango::FAULT]       = EColor(Elettra::red);
        d_stateColors[Tango::INIT]        = EColor(Elettra::blue);
        d_stateColors[Tango::RUNNING]     = EColor(Elettra::green);
		d_stateColors[Tango::ALARM]       = EColor(Elettra::orange);
		d_stateColors[Tango::DISABLE]     = EColor(Elettra::gray);
        d_stateColors[Tango::UNKNOWN]     = EColor(Elettra::darkGray);
		
		
}

void Config::initQualityColorsAndStrings()
{
  d_qualityStrings[Tango::ATTR_VALID] = "ATTR_VALID";
  d_qualityStrings[Tango::ATTR_INVALID] = "ATTR_INVALID";
  d_qualityStrings[Tango::ATTR_ALARM] = "ATTR_ALARM";
  d_qualityStrings[Tango::ATTR_CHANGING] = "ATTR_CHANGING";
  d_qualityStrings[Tango::ATTR_WARNING] = "ATTR_WARNING";
  d_qualityColors[Tango::ATTR_VALID] = Qt::white;
  d_qualityColors[(Tango::AttrQuality) ATTR_VALID_GREEN] = EColor(Elettra::green);
  d_qualityColors[Tango::ATTR_INVALID] = EColor(Elettra::gray);
  d_qualityColors[Tango::ATTR_ALARM] = EColor(Elettra::red);
  d_qualityColors[Tango::ATTR_CHANGING] = EColor(Elettra::blue);
  d_qualityColors[Tango::ATTR_WARNING] = EColor(Elettra::orange);
}

void Config::setStateColors(QVector<QColor> v)
{
  for(int i = 0; i < v.size(); i++)
  {
    Tango::DevState ds = (Tango::DevState)(i);
    d_stateColors[ds] = v[i];
  }
}

const QColor Config::qualityColor(Tango::AttrQuality q) const
{
  return d_qualityColors.value(q);
}

void Config::setQualityColor(Tango::AttrQuality q, QColor c)
{
  d_qualityColors[q] = c;
}

void Config::setQualityString(Tango::AttrQuality q, QString s)
{
  d_qualityStrings[q] = s;
}

QString Config::qualityString(Tango::AttrQuality q) const
{
  return d_qualityStrings.value(q);
}

void Config::setStateColor(Tango::DevState s, QColor c)
{
  d_stateColors[s] = c;
}
	
void Config::setStateString(Tango::DevState s, QString str)
{
  d_stateStrings[s] = str;
}
	
const QString Config::stateString(Tango::DevState s) const
{
  return d_stateStrings.value(s);
}

const QColor  Config::stateColor(Tango::DevState s) const
{
  return d_stateColors[s];
}






