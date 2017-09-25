#include "tspinbox.h"
#include <math.h>

TSpinBox::TSpinBox(QWidget *parent) : ESpinBox(parent), QTangoComProxyWriter(this), SimpleDataProxy(),  IntDelegate(),
  QTangoWidgetCommon(this)
{
	setAutoConfiguration(true);
///	setHideEventEnabled(false);
  connect(qtangoComHandle(), SIGNAL
  (attributeAutoConfigured(const TangoConfigurationParameters *)), this,
	   SLOT(configure(const TangoConfigurationParameters *)));
}

QString TSpinBox::getData()
{
    return QString::number(value());
}

void TSpinBox::configure(const TangoConfigurationParameters * cp)
{
    QString desc = "";
  if(!cp)
    perr("TSpinBox::configure():  TangoConfigurationParameters passed are NULL!");
  else
  {
    double min = -255, max = 255;
    if(cp->minIsSet() && cp->maxIsSet())
    {
    	min = cp->minValue();
   	max = cp->maxValue();
	setToolTip(QString("Type a value for \"%1\"\nbetween %2 and %3").arg(targets()).arg(min).arg(max));
	setMaximum(max);
    	setMinimum(min);
        desc.append("\n(min: "+ QString("%1").arg(cp->minValue()) + " max: "+ QString("%1").arg(cp->maxValue()) + ")");
    }
    else
    {
	   configureNumber(cp->format());
	   setMinimum(-(pow(10, integerDigits()) - 1));
	   setMaximum(pow(10, integerDigits()) - 1);
       pwarn("TSpinBox \"%s\", target \"%s\": "
		 "infering minimum and maximum on the basis of the format \"%s\" (%d integer digits)",
			qstoc(objectName()), qstoc(targets()),  qstoc(cp->format()),integerDigits());
    }
   
    if (cp->displayUnitIsSet())
      		setSuffix(" [" + cp->displayUnit() + "]");
    if (cp->currentValue().canConvertToDouble())
		setValue(cp->currentValue().toDouble(false));

    if (cp->descriptionIsSet()) {
        desc.prepend(cp->description());
    }
    setWhatsThis(desc);
  }
}


void TSpinBox::execute(int value)
{
  QTangoComProxyWriter::execute(value);
}

