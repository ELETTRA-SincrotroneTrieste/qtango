#include "tnumeric.h"
#include <QtDebug>
#include <dec_int_from_format.h>

TNumeric::TNumeric(QWidget *parent, int intDigits, int decDigits) : 
		ENumeric(parent, intDigits, decDigits),
		QTangoComProxyWriter(this)
		, SimpleDataProxy(),
		QTangoWidgetCommon(this)
{
	setAutoConfiguration(true);
	d_applyOnValueChanged = false;
// 	setHideEventEnabled(false);
	connect(qtangoComHandle(), SIGNAL
		(attributeAutoConfigured(const TangoConfigurationParameters *)), this,
		  SLOT(configure(const TangoConfigurationParameters *)));
        connect(qtangoComHandle(), SIGNAL(connectionOk(bool)), this, SLOT(setEnabled(bool)));
}

void TNumeric::configure(const TangoConfigurationParameters * cp)
{
  if(!autoConfiguration())
	return;

	QString desc = "";

	if(cp == NULL)
		return;
	
	/* first apply format, if - correctly - specified */
	if(cp->maxIsSet() && cp->minIsSet())
	{
		configureNumber(cp->minValue(), cp->maxValue(), cp->format());
		
		/* integerDigits() and decimalDigits() from NumberDelegate */
		setIntDigits(integerDigits());
		setDecDigits(decimalDigits());
		setMaximum(cp->maxValue());
		setMinimum(cp->minValue());
                desc = "\n(min: "+ QString("%1").arg(cp->minValue()) + " max: "+ QString("%1").arg(cp->maxValue()) + ")";
	}
	else
		pinfo("TNumeric: maximum and minimum values not set on the tango attribute \"%s\", object \"%s\":"
			" not setting format nor maximum/minimum", qstoc(targets()), qstoc(objectName()));
	/* can set current values instead */		
        if (cp->currentValue().canConvertToDouble())
            setValue(cp->currentValue().toDouble(false));

    if (cp->descriptionIsSet()) {
        desc.prepend(cp->description());
    }
    setWhatsThis(desc);
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged(double)));
}

QString TNumeric::getData()
{
	return QString::number(value());
}

void TNumeric::slotValueChanged(double )
{
  if(d_applyOnValueChanged)
        execute(QVariant(value()));
}



