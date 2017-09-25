/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	   *
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

/* $Id: subscriber_proxy.h,v 1.18 2013-07-03 13:54:42 giacomo Exp $ */

#ifndef SUBSCRIBER_PROXY_H
#define SUBSCRIBER_PROXY_H

#include <QObject>
#include <QString>

#include "taction.h"

class SubscriberAutoReconnectTimer;

class SubscriberProxy : public QObject
{
	
	Q_OBJECT
	public:
		SubscriberProxy(QString source,
			RefreshMode m = AUTO_REFRESH, 
   			bool blocking = false, 
            int period = 1000,
			const QString& assocObjectName = "assocObjectName unset");

		~SubscriberProxy();
		void setMode(RefreshMode m) { d_mode = m; }
		void setPeriod(int p) { d_period = p; }
		void setBlocking(bool b) { d_blocking = b; }
		void setParentDestroyed();
		/* If not empty, the subscriber proxy will ask TAction to obtain the attribute properties
		 * from the tango database during auto configuration.
		 */
		void setDesiredAttributeProperties(const QStringList &props) { d_desiredAttributeProperties = props; }
		
        void setAttributePropertyDbTimeoutMillis(int dbTimeoutMillis) { mAttrPropDbTimeoutMillis = dbTimeoutMillis; }

		QString name() { return d_name; }
		
		void stopAutoReconnectTimer();

        void deleteAutoReconnectTimer();
	
  public slots:
		TAction* subscribe();
	
	signals:
		void configured(TAction* a);
		void configurationErrorMessage(const QString&);
		void connectionOk(bool ok);
		void connectionFailed();
		void connectionError(const QString &msg);
	     
	protected:
		
		/* handle action creation events sending a signal with the new TAction and the 
		 * name of the source inside.
		 * The event is posted by the DeviceThread and so handling it inside the event
		 * handler makes things cleaner and allows to connect the configured() signal
		 * with a DIrectConnection (in the receiver, i.e. the communication handle).
		 */
		bool event(QEvent *e);
		
	private:
		QString d_name; /* the name of the source */
		QString d_assocObjName;
		int d_period;
		RefreshMode d_mode;
		bool d_blocking;
		bool d_parentDestroyed;
		QStringList d_desiredAttributeProperties;
		SubscriberAutoReconnectTimer *d_autoReconnectTimer;
        int mAttrPropDbTimeoutMillis;
};


#endif
