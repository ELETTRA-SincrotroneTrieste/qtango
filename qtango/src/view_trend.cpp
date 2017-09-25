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

/* $Id: view_trend.cpp,v 1.4 2010-01-07 16:11:44 giacomo Exp $ */

#include <proxy_interface.h>
#include "view_trend.h"
#include <QMessageBox>
#include <macros.h>
#include <action_factory.h>
#include <trendplot.h>

using namespace Tango;

QTangoViewTrend::QTangoViewTrend(QObject* parent)
{
	plot = NULL;
	d_parentObject = parent;
	d_colouredBackground = true;
}

bool QTangoViewTrend::canPlot()
{
  TAction* action;
  QTangoComProxyReader *proxyReader = dynamic_cast<QTangoComProxyReader *>(d_parentObject);
  if(proxyReader && proxyReader->configured())
  {
	action = ActionFactory::actionFactory()->actionForName(proxyReader->realSource());
	if(action)
	{
	  switch(action->data().type())
	  {
		case DEV_STATE:
        case DEV_BOOLEAN:
        case DEV_STRING:
        case CONST_DEV_STRING:
        case DEV_UCHAR:
		return false;
      default:
          break;
	  }
	  return true;
	}
	else
	  perr("QTangoViewTrend::canPlot(): no action for source \"%s\"", qstoc(proxyReader->realSource()));
  }
  else
	pinfo("QTangoViewTrend::canPlot(): source not configured yet: cannot create trend plot");
  return false;
}

void QTangoViewTrend::createTrendPlot()
{
	int period;
	QTangoComProxyReader *proxyReader = NULL;
	QString source;
	proxyReader = dynamic_cast<QTangoComProxyReader *>(d_parentObject);
	
	if(plot != NULL && proxyReader && plot->sources() != proxyReader->realSource()) /* source of our widget has changed */
	{
	  delete plot;
	  plot = NULL;
	}
	
	/* if canPlot() returns true, then we are sure that proxyReader != NULL */
	if(canPlot() && !plot)
	{
	  source = proxyReader->realSource();
	  plot = new TrendPlot(qobject_cast<QWidget *>(d_parentObject));
	  plot->setWindowFlags(Qt::Window);
	  plot->setColouredBackground(d_colouredBackground);
	  /* disable enter/leave events on the view trend.
	   * Otherwise the user might be confused when hovering the trend plot.
	   */
	  plot->setEnterLeaveEventsEnabled(false);
	  plot->setAttribute(Qt::WA_QuitOnClose, false);
	  plot->resize(400,300);
	  plot->show();
	  plot->setDataBufferSize(600);
	  period = proxyReader->period();
		
	 if(period == 0)
		period = 1000;
	 else
		plot->setPeriod(period);
	  printf("\e[1;33mazione period: %d\e[0m\n", period);
	  plot->setSources(source);
	  plot->setWindowTitle("Trend - " + source + " -");
	  plot->setTitleOnCanvasEnabled(true);
	}
	else if(plot)
	{
		if (plot->isMinimized())
			plot->showNormal();
		if (plot->isHidden())
			plot->show();
		plot->raise();
	}
}



