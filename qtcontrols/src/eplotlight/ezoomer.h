#ifndef EZOOMER_H
#define EZOOMER_H

/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	   			  *
*   delleceste@gmail.com		   				  *
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


#include <scrollzoomer.h>
#include <scrollbar.h>
class QWidget;

class Zoomer: public ScrollZoomer
{
  Q_OBJECT
  public:
    Zoomer(QWidget *plot);
    
    
    virtual void rescale();
    
    virtual QwtText trackerText(const QPoint &pos) const;
    
    bool trackerTextEnabled() { return d_trackerTextEnabled; }
    void enableTrackerText(bool en) { d_trackerTextEnabled = en; }
    
    bool xAsDate() { return d_xAsDate; }
    void setXAsDate(bool s) { d_xAsDate = s; }
    
  signals:
	void zoomHint();
	void removeZoomHint();
	
  protected:
	virtual void widgetMouseMoveEvent(QMouseEvent *e);
	virtual void widgetMouseReleaseEvent(QMouseEvent *e);
	
  private:
    bool d_trackerTextEnabled, d_xAsDate;
   
};


#endif
