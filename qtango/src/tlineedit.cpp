/***************************************************************************
*   Copyright (C) 2008 by  	Giacomo Strangolino	  *
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

/* $Id: tlineedit.cpp,v 1.12 2010-10-08 13:06:03 giacomo Exp $ */

#include "tlineedit.h"
#include <QDoubleValidator>
#include <QIntValidator>

TLineEdit::TLineEdit(QWidget *parent) : ELineEdit(parent), QTangoComProxyWriter(this), SimpleDataProxy(), StringDelegate(),
  QTangoWidgetCommon(this)
{
  connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), this,
	SLOT(configure(const TangoConfigurationParameters *)));
  setAutoConfiguration(true, true);
  QTangoWidgetCommon::setDragEnabled(false);
///  setHideEventEnabled(false);
}

void TLineEdit::refresh(const TVariant &v)
{
     if(v.canConvertToString())
	  setText(v.toString());
}

QString TLineEdit::getData()
{
    return text();
}

void TLineEdit::setTargets(QString s)
{
	QTangoComProxyWriter::setTargets(s);
}

void TLineEdit::configure(const TangoConfigurationParameters * cp)
{
	if(!cp)
		return;
	
    double min, max;
	int mini, maxi;
	TVariant currentVal;
	QIntValidator* intV;
	QDoubleValidator* doubleV;
	currentVal = cp->currentValue();
	if((cp->dataType() != DEV_STRING && cp->minIsSet() && cp->maxIsSet()) || (cp->dataType() == DEV_STRING))
	{
	  switch(cp->dataType())
	  {
		  case Tango::DEV_USHORT:
		  case Tango::DEV_ULONG:
		  case Tango::DEV_SHORT:
		  case Tango::DEV_LONG:
			  if(validator())
				  delete validator();
			  mini = cp->minValue();
			  maxi = cp->maxValue();
			  pinfo("setting int validator: min %d max %d\n", mini, maxi);
			  intV = new QIntValidator(mini, maxi, parent());
			  QLineEdit::setValidator(intV);
			  if(currentVal.canConvertToInt())
				  setText(QString("%1").arg(currentVal.toInt(false)));
		  break;
		  case Tango::DEV_FLOAT:
		  case Tango::DEV_DOUBLE:
			  if(validator())
				  delete validator();
			  min = cp->minValue();
			  max = cp->maxValue();
			  doubleV = new QDoubleValidator(parent());
			  doubleV->setBottom(min);
			  doubleV->setTop(max);
			  doubleV->setDecimals(10);
			  pinfo("setting double validator: %.2f %.2f\n", min, max);
			  QLineEdit::setValidator(doubleV);
			  if(currentVal.canConvertToDouble())
				  setText(QString("%1").arg(currentVal.toDouble(false)));
			  break;
		  case Tango::DEV_STRING:
				  if(cp->currentValue().canConvertToString())
					  setText(cp->currentValue().toString());
				  break;
	  }
	}

    if (cp->descriptionIsSet()) {
        setWhatsThis(cp->description());
    }
}
