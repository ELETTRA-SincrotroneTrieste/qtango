/***************************************************************************
*   Copyright (C) 2008 by  Giacomo Strangolino   			   *
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


// #include <qtango.h>
#include <QTimer>
#include "test.h"
#include <qtango_wcommon.h>
#include <readersgroup.h>


Test::Test(QWidget *parent) : QWidget(parent)
{
 	
	Tango::DevState st = (Tango::DevState)10;
        Config::instance()->setStateColor(Tango::ON, Qt::blue);
	Config::instance()->setStateColor(Tango::ALARM, Qt::blue);
	Config::instance()->setStateColor(Tango::DISABLE, Qt::blue);
	 Config::instance()->setStateColor(Tango::OFF, Qt::blue);
	Config::instance()->setStateColor(Tango::UNKNOWN, Qt::blue);
	Config::instance()->setStateColor(Tango::STANDBY, Qt::blue);
	 Config::instance()->setStateColor(Tango::INIT, Qt::blue);
	Config::instance()->setStateColor(Tango::MOVING, Qt::blue);
	Config::instance()->setStateColor(Tango::EXTRACT, Qt::blue);
	Config::instance()->setStateColor(Tango::OPEN, Qt::blue);
	Config::instance()->setStateColor(Tango::CLOSE, Qt::blue);
	Config::instance()->setStateColor(Tango::INSERT, Qt::blue);
	Config::instance()->setStateColor(Tango::FAULT, Qt::darkRed);
	Config::instance()->setStateColor(st, Qt::cyan);
	
       	
	ui.setupUi(this);
	
	/* set some labels to refresh asinchronously */
//	ui.tCircularGauge->setWaitRefresh(false);
//	ui.tCircularGauge_4->setWaitRefresh(false);
	
	if(!qApp->arguments().contains("--enable-hide-show-events"))
	{
		printf("\e[1;32;4m%s\e[0m: if you want to enable hide/show events, pass also\n"
			"\"--enable-hide-show-events\" as command line argument.", qApp->arguments().at(0).
	      toStdString().c_str());
		/* disabilito hide/show events per far lavorare i widget anche quando 
	 	* la finestra e` nascosta.
	 	*/
		QTimer::singleShot(3000, this, SLOT(disableHideEvents()));
	}
	else
		QTimer::singleShot(3000, this, SLOT(warnHideShowEventsEnabled()));
	
	QTimer* timer = new QTimer(this);
	timer->setInterval(1000);
	timer->setSingleShot(false);
	connect(timer, SIGNAL(timeout()), this, SLOT(refreshLabels()));
// 	timer->start();
	pippo = 0;
	setupReadersGroups();
	// ui.tPixmap->setDefaultPixmap("/runtime/elettra/share/icons/lock.png");
	// ui.tPixmap->setStatePixmap(0, "/runtime/elettra/share/icons/bell_disabled.png");
	// ui.tPixmap->setStatePixmap(1, "/runtime/elettra/share/icons/cancel.png");
	// ui.tPixmap->setStatePixmap(2, "/runtime/elettra/share/icons/choose.png");
	// ui.tPixmap->setStatePixmap(3, "/runtime/elettra/share/icons/clanbomber.png");
	// ui.tPixmap->setStatePixmap(4, "/runtime/elettra/share/icons/graph1.png");
	// ui.tPixmap->setStatePixmap(5, "/runtime/elettra/share/icons/graph2.png");
	// ui.tPixmap->setStatePixmap(6, "/runtime/elettra/share/icons/graph3.png");
	// ui.tPixmap->setStatePixmap(7, "/runtime/elettra/share/icons/ksnapshot.png");

}	

void Test::disableHideEvents()
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
	printf("\e[1;32;4m%s\e[0m: if you want to enable hide/show events, pass also\n"
			"\"--enable-hide-show-events\" as command line argument.\n\n", qApp->arguments().at(0).
	      toStdString().c_str());
	printf("\e[1;32mNOTA\e[0m: Ho disabilitato gli show/hide events per garantire la attivita` di QTangoCore anche quando\n"
		"  nascondiate questa mega finestra!\n");
}


void Test::warnHideShowEventsEnabled()
{
	printf("\n   \e[1;32;4m%s\e[0m: \e[4mNOTE\e[0m: show/hide events are enabled. This is the default\n"
		"   behaviour of the QTango3 widgets: hide events will unset sources and disconnect\n"
		"   from the tango layer, while show events will reconnect again.\n"
		"   This is aimed at optimizing the communication load between Tango and your panels.\n"
		"   In special cases, it is possible to disable this feature. To see how, just read\n"
		"   the void Test::disableHideEvents() in test.cpp file, in the test directory.\n\n"
			, qApp->arguments().at(0).
	      toStdString().c_str());
}
		
void Test::refreshLabels()
{
     pippo++;
     QString txt = QString().number(pippo);
     QList<TLabel *> l = findChildren<TLabel *>();
     QList<TCircularGauge *>cgl = findChildren<TCircularGauge *>();
     
     printf("Refresh n. %d\n", pippo);
     TVariant tv;
     for(int i = 0; i < l.size(); i++)
     {
	  if(qobject_cast<TLabel *>(l[i]))
	  {
	       TVariant currentValue = qobject_cast<TLabel *>(l[i])->currentValue();
	       if(currentValue.canConvertToDouble())
		       printf("Letto il valore di \"%s\" e vale %.2f [label]\n", qstoc(qobject_cast<TLabel *>(l[i])->objectName()),
			   currentValue.toDouble());
	  }
     }
     for(int i = 0; i < cgl.size(); i++)
     {
	  if(qobject_cast<TCircularGauge *>(cgl[i]))
	  {
	       TVariant currentValue = qobject_cast<TCircularGauge *>(cgl[i])->currentValue();
	       if(currentValue.canConvertToDouble())
		       printf("Letto il valore di \"%s\" e vale %.2f [circular gauge]\n", 
			   qstoc(qobject_cast<TCircularGauge *>(cgl[i])->objectName()),
			   currentValue.toDouble());
	  }
     }
     printf("Letto da %d tlabel e %d tcirculargauge\n", l.size(), cgl.size());
}

void Test::setupReadersGroups()
{
	int i;
	ReadersGroup *gshort, *gdouble, *glong, *gstring, *gushort, *gbool;
	/* 1. create readers group */
	gshort = new ReadersGroup(this);
		
	/* 2. add a number of widgets, possibly reflecting the exact number of elements of the spectrum
	 *    that will be set as source.
	 */
	for(i = 0; i < 12; i++) /* short */
	{
		QObject *reader = findChild<QObject *>(QString("element_%1").arg(i));
		gshort->addReader(reader, i);
	}
	
	gdouble = new ReadersGroup(this);
	
	for(i = 12; i < 23; i++) /* double */
	{
		QObject *reader = findChild<QObject *>(QString("element_%1").arg(i));
		gdouble->addReader(reader, i - 12);
	}
	
	glong = new ReadersGroup(this);
	for(i = 23; i < 34; i++) /* long */
	{
		QObject *reader = findChild<QObject *>(QString("element_%1").arg(i));
		glong->addReader(reader, i - 23);
	}
	
	gstring = new ReadersGroup(this);
	for(i = 34; i < 45; i++) /* long */
	{
		QObject *reader = findChild<QObject *>(QString("element_%1").arg(i));
		gstring->addReader(reader, i - 34);
	}
	
	gushort = new ReadersGroup(this);
	for(i = 45; i < 56; i++) /* long */
	{
		QObject *reader = findChild<QObject *>(QString("element_%1").arg(i));
		gushort->addReader(reader, i - 45);
	}
	
	gbool = new ReadersGroup(this);
	for(i = 56; i < 67; i++) /* long */
	{
		QObject *reader = findChild<QObject *>(QString("element_%1").arg(i));
		gbool->addReader(reader, i - 56);
	}
	
	/* sources */
	if(qApp->arguments().size() > 1)
	{
		QString dev = qApp->arguments().at(1);
		gshort->setSource(dev + "/short_spectrum_ro");
		gdouble->setSource(dev + "/double_spectrum_ro");
		glong->setSource(dev + "/long_spectrum_ro");
		gstring->setSource(dev + "/string_spectrum_ro");
		gushort->setSource(dev + "/ushort_spectrum_ro");
		gbool->setSource(dev + "/boolean_spectrum_ro");
	}
}

Test::~Test()
{

}    



