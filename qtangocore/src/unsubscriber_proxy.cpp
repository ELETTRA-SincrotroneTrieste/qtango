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

/* $Id: unsubscriber_proxy.cpp,v 1.11 2013-07-03 13:54:42 giacomo Exp $ */


#include "unsubscriber_proxy.h"
#include <tutil.h>
#include <devicethread.h>
#include <device_thread_factory.h>
#include <QStringList>
#include <QtDebug>
#include <QCoreApplication>

UnsubscriberProxy::UnsubscriberProxy(
	QString s, 
	RefreshMode desiredMode,
      	int period)
{
	d_source = s;
	d_period = period;
	d_desiredMode = desiredMode;
}

void UnsubscriberProxy::unsubscribe()
{
	bool db;
	DeviceThread *th = NULL;
	QString devName, point;
	QStringList args;
	DataType type = TUtil::instance()->parseName(d_source, devName, point, args, db);
	qprintf("\e[0;35munsubscribe() dell unsubscriber per %s\n", qstoc(d_source));
	try
	{
		th = DeviceThreadFactory::instance()->getDeviceThread(devName);
		if(th)
		{
		   /* QStringList() param is to use the constructor of PointEventData, which takes the attribute property
		    * list as 5th argument. It will be ignored in this case by the device thread.
            * -1 is for Database get_device_attribute_property timeout, ignored in this case.
			*/
           PointEventData *removePointEvent = new PointEventData(d_source, type, point, args, QStringList(), -1, d_desiredMode, d_period);
		   th->removePoint(removePointEvent);
		}
		else
			pinfo("unsubscribing \"%s\": device thread associated no more available - \e[0;37mI think it's already been deleted\e[0m - ",
			    qstoc(d_source));
	}
	catch(DevFailed &e)
	{
        TLog log(e);
        TUtil::instance()->addLog(log);
	}
}




