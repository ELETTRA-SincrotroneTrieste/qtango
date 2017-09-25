/***************************************************************************
*   Copyright (C) 2008 by  Vincenzo Forchi`, Giacomo Strangolino	  *
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

/* $Id: tlineargauge.cpp,v 1.13 2010-04-29 10:09:47 giacomo Exp $ */

#include <QEvent>
#include <QApplication>
#include <QMouseEvent>

#include "tlineargauge.h"

//#include <QtDebug>

TLinearGauge::TLinearGauge(QWidget *parent, Qt::Orientation o, bool mode) 
	: ELinearGauge(parent, o),
	QTangoComProxyReader(this),
	QTangoWidgetCommon(this)
{
	bool enableAttributeConfigurationChangeEvent = true;
	Config *cfg = Config::instance();
	m_errorColor = cfg->qualityColor(ATTR_ALARM);
	m_warningColor = cfg->qualityColor(ATTR_WARNING);
	m_normalColor = cfg->qualityColor((Tango::AttrQuality) ATTR_VALID_GREEN);
	setAutoConfiguration(mode, enableAttributeConfigurationChangeEvent);
	
	setViewTrendEnabled(true);
	setHideEventEnabled(true); /* generally enabled for readers */
	setHdbActionEnabled(true);
	
	setAttribute(Qt::WA_AlwaysShowToolTips);
	connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, 
		 SLOT(refresh(const TVariant&)),
		 Qt::DirectConnection);
	connect(qtangoComHandle(), 
		SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), 
		this, SLOT(init(const TangoConfigurationParameters *)));
}

TLinearGauge::~TLinearGauge()
{
      
}

void TLinearGauge::init(const TangoConfigurationParameters *cp)
{
	QString desc = "";
	if(!cp || !autoConfiguration())
		return;
	if(cp->maxIsSet())
	  setMaxValue(cp->maxValue());
	if(cp->minIsSet())
	  setMinValue(cp->minValue());
	
	if(cp->MErrIsSet())
	  setHighError(cp->maxError());
	else
	  setHighError(cp->maxValue());
	if(cp->mErrIsSet())
	  setLowError(cp->minError());
	else
	  setLowError(cp->maxValue());
	
	if(cp->MWarnIsSet())
	  setHighWarning(cp->maxWarning());
	else
	  setHighWarning(cp->maxValue());
	if(cp->mWarnIsSet())
	  setLowWarning(cp->minWarning());
	else
	  setLowWarning(cp->minValue());

    desc = "\n(Value   \tmin: "+ QString("%1").arg(minValue())   +" \tmax: "+  QString("%1").arg(maxValue())    +")"
         + "\n(Warning \tlow: "+ QString("%1").arg(lowWarning()) +" \thigh: "+ QString("%1").arg(highWarning()) +")"
         + "\n(Error   \tlow: "+ QString("%1").arg(lowError())   +" \thigh: "+ QString("%1").arg(highError())   +")";

    if (cp->descriptionIsSet()) {
        desc.prepend(cp->description());
    }
    setWhatsThis(desc);
}

void TLinearGauge::refresh(const TVariant& v)
{
	setReaderToolTip(v.message());
        if (v.quality() == ATTR_INVALID)
		setEnabled(false);
	else
	{
		if (v.canConvertToDouble())
		{
		     if(!isEnabled())
			setEnabled(true);
		      ELinearGauge::setValue(v.toDouble());
			if (isReferenceEnabled() && (v.getAttributeInfo().writable == READ_WRITE))
				setReference(v.toDouble(false)); /* true = WRITE VALUE */
		}
	        else
		{
	                setReaderToolTip("wrong data type");
                        setDisabled(communicationStatusChangesWidgetEnable());
		}
	}
	update();
}



