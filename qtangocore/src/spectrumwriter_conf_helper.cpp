#include "spectrumwriter_conf_helper.h"
#include <auto_configuration.h>
#include <QVector>
#include <QObject>
#include <QtDebug>
#include <macros.h>
#include <dec_int_from_format.h>
#include <float_delegate.h>
#include <int_delegate.h>
#include <string_delegate.h>
#include <simple_dataproxy.h>
#include <QMetaObject>
#include <QMetaProperty>

ConfigHelper::ConfigHelper()
{
  
}

void ConfigHelper::updateInfo(const TangoConfigurationParameters *cp)
{
  QList<SimpleDataProxy *> plist = d_proxyList;
  int index = 0;
  foreach(SimpleDataProxy *sdp, plist)
  {
	QObject *o = dynamic_cast<QObject *>(sdp);
	if(o)
	{
	  int propIdx = o->metaObject()->indexOfProperty("additionalInfo");
	  if(propIdx > -1)
	  {
		QString additionalInfo = o->metaObject()->property(propIdx).read(o).toString();
		/* remove our previous information, if present */
		additionalInfo.remove(QRegExp("<div class=\"spectrumWriterAdditionalInfo\">.*</div>"));
		QString groupAdditionalInfo = QString("<div class=\"spectrumWriterAdditionalInfo\">"
		  "<h4>Element %1 of Spectrum Writer \"%2\"</h4><p>"
		  "<ul><li><em>target</em>: <cite>%3</cite>;</li>").arg(index).arg(objectName()).arg(d_attName);
		if(cp->minIsSet() && cp->maxIsSet())
		{
		  groupAdditionalInfo += QString("<li><em>min. value</em>: %1;</li>"
		  "<li><em>max. value</em>: %2;</li>").arg(cp->minValue()).arg(cp->maxValue());
		}
		groupAdditionalInfo += QString("</ul></p><p><em>Ctrl+Click</em> on the "
		  "<em>Apply</em> button for more information.</p></div>");
		o->metaObject()->property(propIdx).write(o, additionalInfo + groupAdditionalInfo);
	  }
	}
	index++;
  }
}

void ConfigHelper::configure(const TangoConfigurationParameters *cp)
{
	qprintf("\e[1;34mConfigHelper::configure() for \"%s\"\e[0m\n", qstoc(d_attName));
	setObjectName(QString("SpectrumWriter: \"%1\"").arg(d_attName));
	/* cp arrives not null */
	QString format = cp->format();
	TVariant v = cp->currentValue();
	
	/* 1. format. max and min: set it only if cp has them set */
	/* use a NumberDelegate to get max, min and number of int and decimal digits */
	if(cp->minIsSet() && cp->maxIsSet())
	{
		NumberDelegate nd;
		nd.configureNumber(cp->minValue(), cp->maxValue(), cp->format());
		foreach(SimpleDataProxy *sdp, d_proxyList)
		{
			FloatDelegate *fg;
			IntDelegate *ig = dynamic_cast<IntDelegate *>(sdp);
			if(ig)
			{
				ig->setMinimum((int) cp->minValue());
				ig->setMaximum((int ) cp->maxValue());		
			}
			else if((fg = dynamic_cast<FloatDelegate *>(sdp)))
			{
				fg->setMinimum(cp->minValue());
				fg->setMaximum(cp->maxValue());
				fg->setIntDigits(nd.integerDigits());
				fg->setDecDigits(nd.decimalDigits());
			}
		}
	}
	else
		pinfo("SpectrumWriter: maximum and minimum values not set into the tango database for the attribute \"%s\"", qstoc(d_attName));
		
	/* 2. current value */
	if((cp->dataType() == Tango::DEV_DOUBLE ||  cp->dataType() == Tango::DEV_FLOAT) && v.canConvertToDoubleVector())
	{
		qprintf("\e[1;34mConfigHelper::v.canConvertToDOUBLEVector()\e[0m\n");
		QVector<double> dv = v.toDoubleVector(false);
		for(int i = 0; i < dv.size() && i < d_proxyList.size(); i++)
		{
			FloatDelegate *fd = dynamic_cast<FloatDelegate *>(d_proxyList.at(i));
			if(fd)
			{
				fd->setValue(dv.at(i));
			}
            else if(dynamic_cast<StringDelegate *>(d_proxyList.at(i)))
            {
                dynamic_cast<StringDelegate *>(d_proxyList.at(i))->setValue(QString::number(dv.at(i)));
				perr("ConfigHelper::configure(): cannot cast to FloatDelegate");
            }
		}
	}
	else if(cp->dataType() != Tango::DEV_STRING && v.canConvertToIntVector())
	{
		qprintf("\e[1;34mConfigHelper::v.canConvertToIntVector()\e[0m\n");
		QVector<int> iv = v.toIntVector(false);
		for(int i = 0; i < iv.size() && i < d_proxyList.size(); i++)
		{
			IntDelegate *ig = dynamic_cast<IntDelegate *>(d_proxyList.at(i));
			if(ig)
				ig->setValue(iv.at(i));
			else
				perr("ConfigHelper::configure(): cannot cast to IntDelegate");
		}
	}
	else if(v.canConvertToStringVector())
	{
		QVector<QString> sv = v.toStringVector(false);
		for(int i = 0; i < sv.size() && i < d_proxyList.size(); i++)
		{
			StringDelegate *sd = dynamic_cast<StringDelegate *>(d_proxyList.at(i));
			if(sd)
				sd->setValue(sv.at(i));
			else 
				perr("ConfigHelper::configure(): cannot cast to StringDelegate");
		}
	}
	updateInfo(cp);
}
