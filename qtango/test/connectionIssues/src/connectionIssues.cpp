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


#include "connectionIssues.h"
#include <qtango.h>
#include <QSettings>
#include <QMessageBox>
#include <EPlotCurve>
#include <QtDebug>

ConnectionIssues::ConnectionIssues(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.pbPlot, SIGNAL(clicked()), this, SLOT(setPlotSource()));
	connect(ui.pbLabel, SIGNAL(clicked()), this, SLOT(setLabelSource()));
	connect(ui.pbNumericTgt, SIGNAL(clicked()), this, SLOT(setNumericTarget()));
	connect(ui.pbUnsetSourcePlot, SIGNAL(clicked()), this, SLOT(unsetSourcePlot()));
	connect(ui.pbUnsetSourceLabel, SIGNAL(clicked()), this, SLOT(unsetSourceLabel()));
	connect(ui.pbClearTargets, SIGNAL(clicked()), this, SLOT(clearTargets()));
	connect(ui.sbPeriod, SIGNAL(valueChanged(int)), this, SLOT(changePlotPeriod(int)));
	
	foreach(QWidget *w, findChildren<QWidget *>())
	{
	  QTangoComProxyReader* reader = dynamic_cast<QTangoComProxyReader *> (w);
	  if(reader)
	  {
		printf("connecting %s\n", qstoc(w->objectName()));
		connect(reader->qtangoComHandle(), SIGNAL(connectionOk(bool)), this, SLOT(connectionOk(bool)));
		connect(reader->qtangoComHandle(), SIGNAL(connectionErrorMessage(const QString&)),
				this, SLOT(connectionErrorMessage(const QString&)));
	  }
	  
	}
	connect(ui.tNumeric->qtangoComHandle(), SIGNAL(connectionOk(bool)), this, SLOT(connectionOk(bool)));
	connect(ui.tNumeric->qtangoComHandle(), SIGNAL(connectionErrorMessage(const QString&)),
				this, SLOT(connectionErrorMessage(const QString&)));
	/* TPlotLight marker: since it manages multiple readers (multiple curves) it has 
	 * ad hoc connection status /error signals. The names are the same.
	 */
	connect(ui.tPlotLightMarker, SIGNAL(connectionOk(bool)), this, SLOT(connectionOk(bool)));
	connect(ui.tPlotLightMarker, SIGNAL(connectionErrorMessage(const QString&)),
				this, SLOT(connectionErrorMessage(const QString&)));
				
	//ui.tPlotLightMarker->setSources(ui.lePlotSrc->text());
	QTimer *timer = new QTimer(this);
	timer->setInterval(1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateHandleStatus()));
	timer->start();
	
	EPlotCurve *epc = new EPlotCurve(this, "TestCurve");
	ui.tPlotLightMarker->addCurve("TestCurve", epc);


	/* setup plot for test */
	m_testRefreshCnt = -150;
	ui.tPlotLightMarker->setXAxisAutoscaleEnabled(true);
	ui.tPlotLightMarker->setYAxisAutoscaleEnabled(true);
	ui.tPlotLightMarker->setTimeScaleDrawEnabled(false);
	
	QTimer *refTimer = new QTimer(this);
	refTimer->setInterval(800);
	connect(refTimer, SIGNAL(timeout()), this, SLOT(refresh()));
	//refTimer->start();
	
	setWindowTitle(QString("%1 %2").arg(qApp->applicationName()).arg(qApp->arguments().join(", ")));
}

void ConnectionIssues::refresh()
{
	double val = 255 * rand() / (double) RAND_MAX;
	ui.tPlotLightMarker->appendData("TestCurve", m_testRefreshCnt, val);
	ui.tPlotLightMarker->refresh();
	m_testRefreshCnt++;
}

void ConnectionIssues::clearTargets()
{
	ui.tNumeric->clearTargets();
}

void ConnectionIssues::unsetSourceLabel()
{
	ui.label->unsetSource();
}


void ConnectionIssues::unsetSourcePlot()
{
	ui.tPlotLightMarker->unsetSources();
}



void ConnectionIssues::changeSource()
{

}

ConnectionIssues::~ConnectionIssues()
{

}    

void ConnectionIssues::changePlotPeriod(int ms)
{
	ui.tPlotLightMarker->setPeriod(ms);
}

void ConnectionIssues::updateHandleStatus()
{
  /* for plots, do not call ui.tPlotLightMarker->qtangoComHandle()->configured(), but 
   * directly configured().
  */
  ui.tPlotLightMarker->configured() ? ui.ledPlot->setColor(KGREEN) : ui.ledPlot->setColor(KRED);
  ui.label->qtangoComHandle()->configured() ? ui.ledLabel->setColor(KGREEN) : ui.ledLabel->setColor(KRED);
  ui.tNumeric->qtangoComHandle()->configured() ? ui.ledNumeric->setColor(KGREEN) : ui.ledNumeric->setColor(KRED);
  
  QString txt = "<h4>Label</h4>\n";
  txt += QString("<p>%1</p>\n").arg(ui.label->qtangoComHandle()->connectionStatus());
  txt += "<h4>TNumeric</h4>\n";
  txt += QString("<p>%1</p>\n").arg(ui.tNumeric->qtangoComHandle()->connectionStatus());
  txt += "<h4>Plot</h4>\n";
  /* directly call connectionStatus on the plot to have full information about sources */
  txt += QString("<p>%1</p>\n").arg(ui.tPlotLightMarker->connectionStatus().replace("\n", "<br/>"));
  ui.textEdit->setHtml(txt);
  
}

void ConnectionIssues::connectionOk(bool ok)
{
  QTangoCommunicationHandle *h = dynamic_cast<QTangoCommunicationHandle *>(sender());
  //if(h && ok)
//	QMessageBox::information(this, "Connection Ok", "Connection ok for:\n" + h->objectName());
 /* else */ if(h&& !ok)
	QMessageBox::critical(this, "Connection Failed", "Connection failed for:\n\n" + h->objectName());
/*  else if(ok)
	QMessageBox::information(this, "Plot Connection Ok", "Connection ok for:\n" + sender()->objectName()); */
  else if(!ok)
	QMessageBox::critical(this, "Plot Connection failed", "Connection ok for:\n" + sender()->objectName());
	
  QList<QwtPlotCurve *> crvlist = ui.tPlotLightMarker->curves();
  foreach(QwtPlotCurve *c, crvlist)
  {
  	EPlotCurve *epc = static_cast<EPlotCurve *>(c);
	bool ok;
	if(!epc->vectorUpdateMode() && qApp->arguments().size() > 1 && qApp->arguments().at(1).toInt(&ok) > 0 && ok)
	{
		QDateTime dt = QDateTime::currentDateTime();
		time_t tt = dt.toTime_t();
		int i =  qApp->arguments().at(1).toInt(&ok); /* number of points to precharge */
		QVector<double> x, y;
		while(i > 0)
		{
			x << tt - i;
			y << (double) 255 * rand() / (double) RAND_MAX;
			i--;
		}
		qDebug() << "x vale" << x;
		qDebug() << " y vale " << y ;
		printf("precharging %d points to \"%s\"\n", x.size(), qstoc(epc->title().text()));
		epc->setData(x, y);
		
	}
  }
}

void ConnectionIssues::connectionErrorMessage(const QString& msg)
{
  QTangoCommunicationHandle *h = dynamic_cast<QTangoCommunicationHandle *>(sender());
  if(h)
	QMessageBox::information(this, "Connection message",  h->objectName() + "\nconnection message: " + msg);
  else /* plot */
	QMessageBox::information(this, "Plot connection message",  sender()->objectName() + "\nconnection message: " + msg);
}



void ConnectionIssues::setPlotSource()
{
  ui.tPlotLightMarker->setPeriod(ui.sbPeriod->value());
  ui.tPlotLightMarker->setSources(ui.lePlotSrc->text() + ";" + ui.lePlotSrc2->text());
} 
   
void ConnectionIssues::setLabelSource()
{
  ui.label->setRefreshMode(ARCHIVE_EVENT_REFRESH);
  ui.label->setSource(ui.leLabel->text());
  
}
  
void ConnectionIssues::setNumericTarget()
{
  ui.tNumeric->setTargets(ui.leApplyNumTgt->text());
}    

