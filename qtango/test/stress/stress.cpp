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


#include <qtango.h>
#include <QTimer>
#include "stress.h"
#include <TLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtDebug>
#include <QStringList>


Stress::Stress(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	/* disabilito hide/show events per far lavorare i widget anche quando 
	 * la finestra e` nascosta.
	 */
	if(!qApp->arguments().contains("--enable-show-hide-events"))
		QTimer::singleShot(3000, this, SLOT(disableHideEvents()));
	parseCmdLine();
	buildTable();
	populateTable();
}	

void Stress::disableHideEvents()
{
     #ifdef QTANGOCORE_VERSION_3
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
     #else
	  printf("QTango version < 3. Cannot disable hide events in this version.\n");
	  printf("If you are doing comparison stresss between QTango 2 and 3 remember that\n");
	  printf("QTango version 2 disables refreshes on hide events.\n");
     #endif
}


Stress::~Stress()
{

}    


void Stress::populateTable()
{
     int i, j, k = 0;
     int step = 0;
     QString dev, att, source;
     int numWidgetTypes = widgetTypes.size() + 1;
     int totElems = ui.table->columnCount() * ui.table->rowCount();

     for(i = 0; i < ui.table->rowCount(); i++)
     {
	  dev = ui.table->verticalHeaderItem(i)->text();
	  for(j = 0; j < ui.table->columnCount(); j++)
	  {
	     
	      att = ui.table->horizontalHeaderItem(j)->text();
	      source = dev + "/" + att;
//	      qDebug() << "source " << k+1 << source << "(row " << i << " col " << j <<").";
	      if(numWidgetTypes == 2 && widgetTypes.contains(QString("tlineargauge")))
	      {
		   if(k < totElems /2)
		   {
			TLinearGauge *lg = new TLinearGauge(this);
			lg->setOrientation(Qt::Horizontal);
			lg->setObjectName(QString("Linear Gauge table[%1,%2]: source \"%3\"").arg(j).arg(i).arg(source));
			ui.table->setCellWidget(i, j, lg);
			lg->setSource(source);
		   }
	      }
	      else if(numWidgetTypes == 2 && widgetTypes.contains(QString("tcirculargauge")))
	      {
		   if(k < totElems /2)
		   {
			TCircularGauge *cg = new TCircularGauge(this);
			cg->setObjectName(QString("Circular Gauge table[%1,%2]: source \"%3\"").arg(j).arg(i).arg(source));
			ui.table->setCellWidget(i, j, cg);
			cg->setSource(source);
			ui.table->setRowHeight(j, 100);
			ui.table->setColumnWidth(i, 100);
		   }
	      }
	      if(numWidgetTypes == 2)
	      {
		   if(k >= totElems/2)
		   {
			TLabel *label = new TLabel(this);
			label->setObjectName(QString("Label table[%1,%2]: source \"%3\"").arg(j).arg(i).arg(source));
			ui.table->setCellWidget(i, j, label);
			label->setSource(source);
		   }
	      }
	      else
	      {
		   if(k <= totElems /numWidgetTypes)
		   {
			TLabel *label = new TLabel(this);
			label->setObjectName(QString("Label table[%1,%2]: source \"%3\"").arg(j).arg(i).arg(source));
			ui.table->setCellWidget(i, j, label);
			label->setSource(source);
		   }
		   else if(k < totElems *2/numWidgetTypes)
		   {
			TCircularGauge *cg = new TCircularGauge(this);
			cg->setObjectName(QString("Circular Gauge table[%1,%2]: source \"%3\"").arg(j).arg(i).arg(source));
			ui.table->setCellWidget(i, j, cg);
			cg->setSource(source);
			ui.table->setRowHeight(j, 100);
			ui.table->setColumnWidth(i, 100);
		   }
		   else
		   {
			TLinearGauge *lg = new TLinearGauge(this);
			lg->setOrientation(Qt::Horizontal);
			 lg->setObjectName(QString("Linear Gauge table[%1,%2]: source \"%3\"").arg(j).arg(i).arg(source));
			 ui.table->setCellWidget(i, j, lg);
			 lg->setSource(source);
		   }
			
	      }
	      k++;
	  }
     }
}

void Stress::parseCmdLine()
{
     int i;
     bool ok;
     QStringList args = qApp->arguments();
     args.pop_front();
     if(args.size() < 5 )
     {
	  qDebug() << "Usage:\n" << qApp->arguments().at(0) << 
	       "device/stress/1 [... device/stress/N] --numattrs N --attlist att1 [... attN]";
	  qDebug() << "An example is:";
	  qDebug() <<  qApp->arguments().at(0) <<
	       "stress/device/1 stress/device/2 stress/device/3 --numatts 100 --attlist "
		"double_scalar short_scalar long_scalar";
	  qDebug() << "The program normally uses TLineEdit objects, but you can specify\n"
	  "--add-widget-types with tlineargauge or tcirculargauge (or both) as arguments.";
	  qDebug() << "\nThe stress will display a table with the devices as columns and\n"
	       "the attributes in the rows, with a number of rows * columns that\n"
	       "approximates N\n";
	  exit(EXIT_FAILURE);
     }
     foreach(QString a, args)
     {
	  if(a.count('/') == 2) /* a device */
		    devices << a;
     }
     
     for(i = 1; i < args.size(); i++)
     {
	  if(args.at(i) == "--numattrs" && args.size() > i+1)
	  {
	       numAtt = args.at(i + 1).toUInt();
	       break;
	  }
	  else if(args.at(i) == "--numattrs" && args.size() <= i+1)
	  {
	       qDebug() << "--numattrs requires an integer ";
	       exit(EXIT_FAILURE);
	  }
     }
     if(i == args.size())
     {
	  qDebug() << "You must provide --numattrs followed by an integer";
	  exit(EXIT_FAILURE);
     }   
     for(i = 1; i < args.size(); i++)
     {
	  if(args.at(i) == "--attlist" && args.size() > i+1)
	  {
	       for(int j = i+1; j < args.size(); j++)
	       {
		    if(args.at(j).contains("--") || args.at(j).contains("-") || args.at(j).contains('/') || 
			 atoi(args.at(j).toStdString().c_str()) != 0)
			 break;
		    else
			 attributes << args.at(j);
	       }
	       break;
	  }
	  else if(args.at(i) == "--attlist" && args.size() <= i+1)
	  {
	       qDebug() << "--attlist requires an integer ";
	       exit(EXIT_FAILURE);
	  }  
     }
     if(i == args.size())
     {
	  qDebug() << "You must provide --attlist followed by a list of attributes";
	  qDebug() << "such as short_scalar double_scalar";
	  exit(EXIT_FAILURE);
     }   
     
     for(i = 1; i < args.size(); i++)
     {
	  if(args.at(i) == "--add-widget-types" && args.size() > i+1)
	  {
	       for(int j = i+1; j < args.size(); j++)
	       {
		    if(args.at(j).contains("--") || args.at(j).contains("-") || args.at(j).contains('/') || 
			 atoi(args.at(j).toStdString().c_str()) != 0)
			 break;
		    else
			 widgetTypes << args.at(j);
	       }
	       break;
	  }
	  else if(args.at(i) == "--add-widget-types" && args.size() <= i+1)
	  {
	       qDebug() << "--add-widget-types requires a list of widget types: allowed types are: ";
	       qDebug() << "tlineargauge tcirculargauge";
	       exit(EXIT_FAILURE);
	  }  
     }
     for(i = 0; i < args.size(); i++)
     {
     	if(args.at(i) == "--test-on-light-devices"  && args.size() >= i+1)
	{
		int devicesNo = args.at(i + 1).toInt();
		qDebug() << "number of devices test/LightTest/: " << devicesNo;
		for(int devno = 1; devno < devicesNo; devno++)
			devices << QString("test/LightTest/") + QString("%1").arg(devno);
		break;
	}
	else if(args.at(i) == "--test-on-light-devices" && args.size() < i+1)
	{
		qDebug() << "--test-on-light-devices connects to a number of devices named test/LightTest/N";
		qDebug() << "where N is a number that must follow the argument \"--test-on-light-devices\".";
		exit(EXIT_FAILURE);
	}
	else
		qDebug() << "arg " << args.at(i) << " different from " << "--test-on-light-devices";

     }	
     if(devices.size() == 0)
     {
	  qDebug() << "You must provide at least one device, for example:";
	  qDebug() << "stress/device/1";
	  exit(EXIT_FAILURE);
     }
     if(widgetTypes.size() > 0)
	  qDebug() << "Additional widgets: " << widgetTypes;

}

void Stress::buildTable()
{
     /* the vertical header will contain the device names */
     int devs = devices.size();
     QStringList devList;

     devList = devices;
 
     int colFactor = 1;
     if(numAtt % attributes.size() == 0)
	colFactor = numAtt / attributes.size();

     ui.table->setRowCount(devices.size());
     ui.table->setVerticalHeaderLabels(devList);

     int numCols = attributes.size() * colFactor;
     qDebug() << "Devices: " << devices.size() << "attributes: " << attributes.size() <<
	  "Rows : " << numRows;
     ui.table->setColumnCount(numCols);
     
     
     QStringList horLabels;
     for(int i = 0; i < numCols; i++)
	  horLabels << attributes.at(i % attributes.size());
     ui.table->setHorizontalHeaderLabels(horLabels);
     
     setWindowTitle(QString("%1 - [%2 rows x %3 cols]").arg(qApp->arguments().first()).arg(ui.table->rowCount()).
	  arg(ui.table->columnCount()));
}

void Stress::resizeEvent(QResizeEvent *e)
{
     for(int i = 0; i < ui.table->columnCount(); i++)
	  ui.table->setColumnWidth(i, e->size().width()/ui.table->columnCount());
     QWidget::resizeEvent(e);
}


     


