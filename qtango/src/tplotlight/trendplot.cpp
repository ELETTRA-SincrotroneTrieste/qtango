/***************************************************************************
*   Copyright (C) 2008 by  Vincenzo Forchi`, Giacomo Strangolino   *
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

/* $Id: trendplot.cpp,v 1.4 2011-03-28 10:04:00 giacomo Exp $ */

#include "trendplot.h"
#include <action_factory.h>
#include <QApplication>
#include <QPainter>
#include <device_factory.h>


TrendPlot::TrendPlot(QWidget *parent) : TPlotLightMarker(parent)
{
	setHideEventEnabled(false);
	setAutoConfiguration(true);
	setFillFromHistoryEnabled(true);
	setTimeScaleDrawEnabled(true);
	setXAxisAutoscaleEnabled(true);
	/* let trend plots show the set point by default */
        setSetPointEnabled(true);
}

TrendPlot::~TrendPlot()
{
      
}




