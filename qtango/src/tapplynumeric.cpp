/***************************************************************************
*   Copyright (C) 2008 by Vincenzo Forchi`, Giacomo Strangolino	   *
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

/* $Id: tapplynumeric.cpp,v 1.21 2010-07-22 11:49:33 giacomo Exp $ */
#include <QRegExp>
#include <QMessageBox>
#include <QString>
#include <QtDebug>
#include <QVariant>
#include <QtPlugin>
#include <dec_int_from_format.h>
#include "tapplynumeric.h"


TApplyNumeric::TApplyNumeric(QWidget *parent, int intDigits, int decDigits, Qt::Orientation pos) : EApplyNumeric(parent, intDigits, decDigits, pos), 
	QTangoComProxyWriter(this), QTangoWidgetCommon(this)
{
	connect(qtangoComHandle(), SIGNAL
		(attributeAutoConfigured(const TangoConfigurationParameters *)), this,
		  SLOT(configure(const TangoConfigurationParameters *)));

	connect(this, SIGNAL(clicked(double)), this, SLOT(confirmRequest(double)));
	
	connect(qtangoComHandle(), SIGNAL(progress(int, int)), this, 
		SLOT(stepsDone(int, int)), Qt::QueuedConnection);
		
	connect(qtangoComHandle(), SIGNAL(connectionFailed()), this, SLOT(disable()));
	
	/* when new data arrives, emit the signal containing the argout, if present */
	connect(qtangoComHandle(), SIGNAL(newData(const TVariant& )), this, 
		SIGNAL(newDataAvailable(const TVariant& )));
	
	m_confirmNeeded = false;
	setAutoConfiguration(true);
// 	setHideEventEnabled(false);
}

void TApplyNumeric::confirmRequest(double )
{
	bool launch = true;
	if (m_confirmNeeded)
	{
		int res = QMessageBox::question(this, "Confirm Request", 
			QString("Do you really want to execute ") + targets() + "?",
			QMessageBox::Yes | QMessageBox::No);
		if (res == QMessageBox::No)
			launch = false;
	}
	qDebug() << "lancio con valore" << value();
	if (launch)
	{
	  QList<TVariant> tl = execute(QVariant(value()));
          /* do what you want, if you want, with the list of TVariant */
	}
	emit applyClicked();
}

QString TApplyNumeric::getData()
{
	return QString::number(value());
}

void TApplyNumeric::setTarget(QString s)
{
	if (s.contains("("))
	{
		s = s.remove(")");
		s += ",&" + objectName() + ")";
	}
	else
		s += "(&" + objectName() + ")";
	QTangoComProxyWriter::setTargets(s);
}

// QVariant TApplyNumeric::getData()
// {
// 	return QVariant(value());
// }

void TApplyNumeric::configure(const TangoConfigurationParameters * cp)
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
		pinfo("TApplyNumeric: maximum and minimum values not set on the tango attribute \"%s\", object \"%s\": "
			"not setting format nor maximum/minimum", qstoc(targets()), qstoc(objectName()));
	/* can set current values instead */		
	if (cp->currentValue().canConvertToDouble())
	{
		qDebug() << "INIZIALIZZO A VALORE " << cp->currentValue().toDouble(false);
		setValue(cp->currentValue().toDouble(false));
		clearModified();
	}
    if (cp->descriptionIsSet()) {
        desc.prepend(cp->description());
    }
    setWhatsThis(desc);

}

void TApplyNumeric::stepsDone(int par, int total)
{
	qDebug() << "step " << par << "over total: " << total;
}

 
