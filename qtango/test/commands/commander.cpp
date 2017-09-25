/***************************************************************************
*   Copyright (C) 2007 by Giacomo Strangolino   *
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


#include <qtango.h>
#include <QTimer>
#include "commander.h"


Commander::Commander(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	/* disabilito hide/show events per far lavorare i widget anche quando 
	 * la finestra e` nascosta.
	 */
	QTimer::singleShot(3000, this, SLOT(disableHideEvents()));
	/* connect the TPushButton (handle) newData signal to the TLineEdit we want to refresh
	 * after a successful command
	 */
	connect(ui.tPushButton->qtangoComHandle(), SIGNAL(newData(const TVariant&)), ui.lEString1,
		 SLOT(refresh(const TVariant&)));
	connect(ui.tPushButton_3->qtangoComHandle(), SIGNAL(newData(const TVariant&)), ui.lEDevLong,
		SLOT(refresh(const TVariant&)));
	connect(ui.tPushButton_3->qtangoComHandle(), SIGNAL(newData(const TVariant&)), ui.LEDevShort,
		SLOT(refresh(const TVariant&)));	
	connect(ui.tApplyNumeric_2->qtangoComHandle(), SIGNAL(newData(const TVariant&)), ui.LEDevShort,
		SLOT(refresh(const TVariant&)));	
	connect(ui.tApplyNumeric_2->qtangoComHandle(), SIGNAL(newData(const TVariant&)), ui.lEDevLong,
		SLOT(refresh(const TVariant&)));	
	connect(ui.tPushButton_4->qtangoComHandle(), SIGNAL(newData(const TVariant&)), ui.lEDevDouble,
		SLOT(refresh(const TVariant&)));
	connect(ui.tApplyNumeric->qtangoComHandle(), SIGNAL(newData(const TVariant&)), ui.lEDevDouble,
		SLOT(refresh(const TVariant&)));
	connect(ui.tPushButton_2->qtangoComHandle(), SIGNAL(newData(const TVariant&)), ui.lEString2,
		SLOT(refresh(const TVariant&)));
	connect(ui.tPushButton_5->qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(updateCmdArray(const TVariant&)));
	connect(ui.pbApplyTarget, SIGNAL(clicked()), this, SLOT(changeDevVarDoubleArrayTarget()));
				  
}	

void Commander::updateCmdArray(const TVariant& v)
{
	QString s;
	if(v.canConvertToDoubleVector())
	{
		QVector<double> vd = v.toDoubleVector();
		foreach(double d, vd)
			s += QString("%1, ").arg(d);
		ui.lineEdit->setText(s);
	}
	else
		ui.lineEdit->setText("Conversion error");
}

void Commander::disableHideEvents()
{
	QList<QWidget *> l = findChildren<QWidget *>();
	for(int i = 0; i < l.size(); i++)
	{
		if(dynamic_cast<QTangoWidgetCommon *>(l[i]))
		{
			printf("\e[0;35mDisabilito hide event per %s\e[0m\n", l[i]->objectName().toStdString().c_str());
			dynamic_cast<QTangoWidgetCommon *>(l[i])->setHideEventEnabled(false);
		}
	}
	printf("\e[1;32mNOTA\e[0m: Ho disabilitato gli show/hide events per garantire la attivita` di QTangoCore anche quando\n"
		"  nascondiate questa mega finestra!\n");
}

void Commander::changeDevVarDoubleArrayTarget()
{
	ui.tPushButton_5->setTargets(ui.leDevVarDoubleArrayTarget->text());
}

Commander::~Commander()
{

}    



