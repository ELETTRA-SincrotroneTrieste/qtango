/***************************************************************************
 *   Copyright (C) 2007 by Claudio Scafuri, Giacomo Strangolino   *
 *   claudio@hyo   *
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


#include "stacked.h"
#include <qtango.h>
#include <QLabel>
#include <QSpinBox>

Stacked::Stacked(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	int i = 0;
	QString objName;
	QGridLayout *lo = new QGridLayout(ui.gbPriority);
	//ui.eStackedWidgetContainer->addWidget(ui.tLed1, 2);
	//ui.eStackedWidgetContainer->addWidget(ui.tLed2, 4);
	ui.eStackedWidgetContainer->addWidget(ui.tLabel3, 5);
	//ui.eStackedWidgetContainer->addWidget(ui.tPlot, 1);
	//ui.eStackedWidgetContainer->addWidget(ui.CircularGauge, 8);
	ui.tTable->setEnumDisplay(0, 0, "zero", Qt::red);
	ui.tTable->setEnumDisplay(0, 1, "uno", Qt::green);
	ui.tTable->setEnumDisplay(0, 2, "due", Qt::blue);
	ui.tLabel3->setEnumDisplay(0, "label zero", Qt::red);
	ui.tLabel3->setEnumDisplay(1, "label uno", Qt::green);
	ui.tLabel3->setEnumDisplay(2, "label due", Qt::blue);
	ui.tLabel3->setEnumDisplay(3, "label tre", Qt::yellow);
	QList<QWidget *> widgets = findChildren<QWidget *>();
	foreach(QWidget *wi, widgets)
	{
		objName = wi->objectName();
		if(wi && ui.eStackedWidgetContainer->widgetPriority(wi) > -1)
		{
			if(dynamic_cast<QTangoComProxyReader *>(wi))
			{
				printf("-- disabling tango tooltips for \"%s\"\n", qstoc(objName));
				dynamic_cast<QTangoComProxyReader *>(wi)->setTangoToolTipsEnabled(false);
			}
			else
				printf("cannot dynamic cast \"%s\" to a reader\n", qstoc(objName));
			wi->setToolTip(QString("%1 - priority %2").arg(objName).arg(ui.eStackedWidgetContainer->widgetPriority(wi)));
			QLabel *label = new QLabel(ui.gbPriority);
			label->setText(QString("\"%1\" priority: ").arg(wi->objectName()));
			QSpinBox *sb = new QSpinBox(ui.gbPriority);
			sb->setObjectName(QString("sb%1").arg(objName));
			sb->setValue(ui.eStackedWidgetContainer->widgetPriority(wi));
			connect(sb, SIGNAL(valueChanged(int)), this, SLOT(changeWidgetPriority(int)));
			lo->addWidget(label, i, 0);
			lo->addWidget(sb, i, 1);
			i++;
		}
	}
	if(widgets.size())
	{
		QPushButton *pb = new QPushButton(ui.gbPriority);
		pb->setText("Update");
		foreach(QWidget *wi, widgets)
			connect(pb, SIGNAL(clicked()), wi, SLOT(update()));
		lo->addWidget(pb, i, 1);
	}
}

void Stacked::changeWidgetPriority(int p)
{
	QObject *s = sender();
	QString assocWidgetName = s->objectName().remove("sb");
	printf("assocWidgetName: %s\n", qstoc(assocWidgetName));
	int value;
	if(qobject_cast<QSpinBox *>(s))
		value  = qobject_cast<QSpinBox *>(s)->value();
	QWidget *sw = findChild<QWidget *>(assocWidgetName);
	if(sw)
	{
		printf("* changing \"%s\" priority from %d to %d\n", qstoc(sw->objectName()), ui.eStackedWidgetContainer->widgetPriority(sw),
			p);
		ui.eStackedWidgetContainer->setWidgetPriority(sw, value);
		sw->setToolTip(QString("%1 - priority %2").arg(sw->objectName()).arg(ui.eStackedWidgetContainer->widgetPriority(sw)));
	}
}

Stacked::~Stacked()
{

}    



