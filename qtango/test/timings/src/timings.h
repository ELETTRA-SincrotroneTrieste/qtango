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


#ifndef timings_H
#define timings_H

#include <QWidget>

class CpuData
{
	public:
	 	CpuData();
		float percentage();
		unsigned int oldUtime, oldStime, utime, stime;
};

class TPlotLightMarker;

/**  A widget that demonstrates how the refresh of the QTango widgets can be managed and the 
  *  CPU usage of the interface deriving from your choice.
 */
class Timings: public QWidget
{
    Q_OBJECT

public:
    Timings(QWidget * =NULL);
    
    ~Timings();

private slots:
	void globalRefreshEnabled(bool);
	void refreshCpu();
	
private:
	TPlotLightMarker *d_cpuPlot;
	CpuData d_cpuData;
};


#endif
