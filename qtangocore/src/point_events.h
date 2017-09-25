/***************************************************************************
*   Copyright (C) 2008 by  Giacomo Strangolino	  *
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

/* $Id: point_events.h,v 1.12 2013-07-03 13:54:42 giacomo Exp $ */

#ifndef QTANGO_POINT_EVENTS_H
#define QTANGO_POINT_EVENTS_H

#include <qtangocore_common.h>
#include <QStringList>
#include <QEvent>

class SubscriberProxy;

class ExitLoopEvent : public QEvent
{
	public: 
		ExitLoopEvent(Type t) : QEvent(t) {};
};

class PointEventData
{
   public:
   PointEventData(const QString& s, 
	  const DataType dt, const QString& pt, const QStringList& args, 
	  const  QStringList& attributeProperties, 
      int attrPropDbTimeoutMillis,
	  RefreshMode m, int p,
      SubscriberProxy *subscriber = NULL) : 
		d_source(s),
		d_type(dt),  
		d_point(pt), 
		d_args(args), 
		d_attProp(attributeProperties),
		d_mode(m),
		d_period(p),  
		d_subscriber(subscriber)
     {
       mAttrPropDbTimeoutMillis = attrPropDbTimeoutMillis;
   };

   ~PointEventData() { }
      const QString		source() const { return d_source; };
      DataType		type()  const { return d_type; };
      const QString		point() const { return d_point; };
      const QStringList	args() const  { return d_args; };
      RefreshMode		mode() const  { return d_mode; };
      int			period() const  { return d_period; };
	  const QStringList attributeProperties() const { return d_attProp; }
      int attrPropDbTimeoutMillis() const { return mAttrPropDbTimeoutMillis; }
      SubscriberProxy *		subscriber() { return d_subscriber; }

   private:
      QString		d_source;
      DataType	d_type;
      QString		d_point;
      QStringList	d_args, d_attProp;
      RefreshMode	d_mode;
      int		d_period;
      SubscriberProxy *d_subscriber;
      int mAttrPropDbTimeoutMillis;
};

#endif
