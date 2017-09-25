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


#include "refreshtest.h"
#include <qtango.h>
#include <QLabel>
#include <QSpinBox>

TPlotLight::TPlotLight(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	if(qApp->arguments().size() == 2)
	{
		ui.groupBox->setVisible(false);
		QString arg = qApp->arguments().at(1);
		if(arg.count("/") == 3 || arg.count("->") == 1)
			ui.tRealtimePlot->setSources(arg);
	}
	else if(qApp->arguments().size() == 3)
	{
	  ui.groupBox->setVisible(true);
	  
	  connect(ui.rbVectorMode, SIGNAL(toggled(bool)), this, SLOT(switchMode(bool)));
	  connect(ui.rbTimeMode, SIGNAL(toggled(bool)), this, SLOT(switchMode(bool)));
	  connect(ui.sbNPoints, SIGNAL(valueChanged(int)), this, SLOT(setNumberOfPoints(int)));
	  connect(ui.sbRefreshInterval, SIGNAL(valueChanged(int)), this, SLOT(setRefreshInterval(int)));
	  connect(ui.sbNPointsPreload, SIGNAL(valueChanged(int)), this, SLOT(setPreloadPointsNumber(int)));
	  connect(ui.sbRefreshIntervalTimeMode, SIGNAL(valueChanged(int)), this, SLOT(setRefreshIntervalTimeMode(int)));
	  
	  ui.rbVectorMode->setChecked(true);
	  ui.sbNPoints->setValue(qApp->arguments().at(2).toInt());
	  ui.sbRefreshInterval->setValue( qApp->arguments().at(1).toInt());
	  QTimer *timer= new QTimer(this);
	  int interval = qApp->arguments().at(1).toInt();
	  timer->setInterval(interval);
	  timer->setSingleShot(false);
	  connect(timer, SIGNAL(timeout()), this, SLOT(refreshCurve()));
	  
	  ui.tRealtimePlot->createAndAddCurve(QString("Curve"));
	  timer->start();
	  switchMode();
	}
}

void TPlotLight::refreshCurve()
{
  QVector<double> xdata, ydata;
  if(ui.rbVectorMode->isChecked())
  {
	d_nPoints = ui.sbRefreshInterval->value();
	for(int i = 0; i < d_nPoints; i++)
	{
	  ydata << 2.0 * ( rand() / ((double)RAND_MAX + 1) ) - 1.0;
	  xdata << i;
	//  printf("adding couple %.2f %.2f (nPoints %d)\n", xdata.last(), ydata.last(), d_nPoints);
	}
	// printf("setting data\n");
	ui.tRealtimePlot->setData("Curve", xdata, ydata);
  }
  else
  {
	QDateTime dt = QDateTime::currentDateTime();
	ui.tRealtimePlot->appendData("Curve", (double) dt.toTime_t(), 2.0 * ( rand() / ((double)RAND_MAX + 1) ) - 1.0);
  }
  ui.tRealtimePlot->refresh();
}

TPlotLight::~TPlotLight()
{

}    

void TPlotLight::switchMode(bool unused)
{
  Q_UNUSED(unused);
  /* switch tab widget */
  ui.stackedWidget->setCurrentIndex(ui.rbVectorMode->isChecked() ? 0 : 1);
  /* Time scale when not in vector mode (append mode) */
  ui.tRealtimePlot->setTimeScaleDrawEnabled(!ui.rbVectorMode->isChecked());
  
  /* clear the current curve and recreate a new one */
  ui.tRealtimePlot->clearPlot();
  ui.tRealtimePlot->createAndAddCurve(QString("Curve"));
  
  if(ui.rbVectorMode->isChecked())
  {
	ui.tRealtimePlot->setXAxisAutoscaleEnabled(false);
	ui.tRealtimePlot->setYAxisAutoscaleEnabled(false);
	ui.tRealtimePlot->setYUpperBound(1.02);
	ui.tRealtimePlot->setYLowerBound(-1.02);
	ui.tRealtimePlot->setXUpperBound(ui.sbRefreshInterval->value());
	ui.tRealtimePlot->setXLowerBound(0);
  }
  else /* time mode */
  {
	ui.tRealtimePlot->setYAxisAutoscaleEnabled(true);
	ui.tRealtimePlot->setXAxisAutoscaleEnabled(true);
	/* preload a certain number of elements in the curve */
	setPreloadPointsNumber(ui.sbNPointsPreload->value());
  }
}

void TPlotLight::setPreloadPointsNumber(int v)
{
  ui.tRealtimePlot->removeCurve("Curve");
  ui.tRealtimePlot->createAndAddCurve(QString("Curve"));
  QDateTime dt = QDateTime::currentDateTime();
  dt = dt.addSecs(-v);
  for(int i = 0; i < v; i++)
  {
	dt = dt.addSecs(1);
	printf("data %d a \"%s\"\n", i, qstoc(dt.toString()));
	/* append some data */
	ui.tRealtimePlot->appendData("Curve", (double) dt.toTime_t(), 2.0 * ( rand() / ((double)RAND_MAX + 1) ) - 1.0);
  }
}

void TPlotLight::setRefreshInterval(int v)
{
  findChild<QTimer* >()->setInterval(v);
}

void TPlotLight::setRefreshIntervalTimeMode(int v)
{
  setRefreshInterval(v);
}

void TPlotLight::setNumberOfPoints(int v)
{
  d_nPoints = v;
  ui.tRealtimePlot->setXUpperBound(d_nPoints);
  ui.tRealtimePlot->setXLowerBound(0);
}


