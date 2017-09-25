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


#ifndef connectionIssues_H
#define connectionIssues_H

#include "ui_connectionIssues.h"

class ConnectionIssues: public QWidget
{
    Q_OBJECT

public:
    ConnectionIssues(QWidget * =NULL);
    ~ConnectionIssues();

   private slots:

	void clearTargets();
	void unsetSourceLabel();
	void unsetSourcePlot();

   void changeSource();
   void changePlotPeriod(int millis);
   
   void refresh();

   void setPlotSource();
   void setLabelSource();
   void setNumericTarget();
   
   void connectionOk(bool);
   void connectionErrorMessage(const QString&);
   
   void updateHandleStatus();
   
private:
    Ui::ConnectionIssues ui;
    int m_testRefreshCnt;

};


#endif
