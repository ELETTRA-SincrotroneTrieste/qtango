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


#ifndef refreshtest_H
#define refreshtest_H

#include "ui_refreshtest.h"

class TPlotLight: public QWidget
{
    Q_OBJECT

public:
    TPlotLight(QWidget * =NULL);
    ~TPlotLight();
	
  protected slots:
	void refreshCurve();
	
	void switchMode(bool unused = false);
	void setPreloadPointsNumber(int);
	void setRefreshInterval(int);
	void setRefreshIntervalTimeMode(int);
	void setNumberOfPoints(int);

private:
    Ui::TPlotLight ui;
	int d_nPoints;

};


#endif
