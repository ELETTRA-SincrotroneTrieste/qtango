/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino, Vincenzo Forchi`		   *
*   giacomo.strangolino@elettra.trieste.it   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

/* $Id: tdoublespinbox.cpp,v 1.16 2012-07-13 13:17:52 giacomo Exp $ */

#include "tdoublespinbox.h"
#include <dec_int_from_format.h>
#include <math.h>



TDoubleSpinBox::TDoubleSpinBox(QWidget *parent) : 
  EDoubleSpinBox(parent),
  QTangoComProxyWriter(this),
  SimpleDataProxy(),
  FloatDelegate(),
  QTangoWidgetCommon(this)
{
	setAutoConfiguration(true);
///	setHideEventEnabled(false);
  connect(qtangoComHandle(), SIGNAL
  (attributeAutoConfigured(const TangoConfigurationParameters *)), this,
	   SLOT(autoConfigure(const TangoConfigurationParameters *)));
}

QString TDoubleSpinBox::getData()
{
    return QString::number(value());
}

void TDoubleSpinBox::autoConfigure(const TangoConfigurationParameters * cp)
{
    QString desc = "";
  	if(!cp)
    		return;

	/* first apply format, if - correctly - specified */
	if(cp->maxIsSet() && cp->minIsSet())
	{
		configureNumber(cp->minValue(), cp->maxValue(), cp->format());
		setDecimals(decimalDigits());
		setMaximum(cp->maxValue());
		setMinimum(cp->minValue());
        desc.append("\n(min: "+ QString("%1").arg(cp->minValue()) + " max: "+ QString("%1").arg(cp->maxValue()) + ")");
	}
	else /* set minimum and maximum with format information, which is available by default */
	{
	   configureNumber(cp->format());
	   setDecimals(decimalDigits());
	   pwarn("TDoubleSpinBox \"%s\", target \"%s\": "
		 "infering minimum and maximum on the basis of the format \"%s\" (%d integer digits)", 
			qstoc(objectName()), qstoc(targets()),qstoc(cp->format()), integerDigits());
#ifdef __FreeBSD__
           setMinimum(-(pow(10, integerDigits()) - 1));
           setMaximum(pow(10, integerDigits()) - 1);
#else
	   setMinimum(-(powl(10, integerDigits()) - 1));
	   setMaximum(powl(10, integerDigits()) - 1);
#endif
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

void TDoubleSpinBox::execute(double value)
{
  QTangoComProxyWriter::execute(value);
}


