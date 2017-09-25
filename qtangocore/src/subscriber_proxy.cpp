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

/* $Id: subscriber_proxy.cpp,v 1.36 2013-07-03 13:54:42 giacomo Exp $ */

#include <QApplication>
#include "subscriber_proxy.h"
#include "subscriber_auto_reconnect_timer.h"
#include "action_configuration_failed_event.h"
#include "point_events.h"
#include "action_creation_event.h"
#include <tutil.h>
#include <devicethread.h>
#include <taction.h>
#include <QtDebug>
#include <device_thread_factory.h>

/* This is constructed by the communication handle and it is aimed at 
 * communicating with the device thread during the setup phase of the
 * tango connection.
 * Its life is independent from the life of its creator (the
 * QTangoCommunicationHandle): the QObject base class is initialized
 * with NULL so that QTangoCommunicationHandle destruction does not
 * cause the SubscriberProxy destruction. Indeed, there might be some
 * cases where the communication handle is destroyed but the SubscriberProxy
 * must stay alive, because the DeviceThread might post an event to the
 * subscriber while the handle has been destroyed in the main thread.
 */
SubscriberProxy::SubscriberProxy(QString s, RefreshMode m,
                                 bool blocking, int period, const QString& assocObjectName)
    : QObject(NULL) /* subscriber life is independent from parent (handle) */
{
    d_parentDestroyed = false;
    d_name = s;
    d_mode = m;
    d_blocking = blocking;
    d_period = period;
    d_assocObjName = assocObjectName;
    setObjectName(d_name);
    d_autoReconnectTimer = NULL;
    mAttrPropDbTimeoutMillis = -1;
    d_autoReconnectTimer = new SubscriberAutoReconnectTimer(this, d_name);
    connect(d_autoReconnectTimer, SIGNAL(retry()), this, SLOT(subscribe()));
}

SubscriberProxy::~SubscriberProxy()
{

}

void SubscriberProxy::setParentDestroyed()
{
    if(d_autoReconnectTimer)
    {
        d_autoReconnectTimer->unscheduleRetry();
        delete d_autoReconnectTimer;
        d_autoReconnectTimer = NULL;
    }
    d_parentDestroyed = true;
}

TAction* SubscriberProxy::subscribe()
{
    TAction *taction = NULL;
    QString devName, point;
    QStringList args;
    DeviceThread *th;
    bool db;

    qDebug() << this << d_name << d_period << d_mode;
    fflush(stdout);
    DataType type = TUtil::instance()->parseName(d_name, devName, point, args, db);

    /* returns NULL if the device thread does not exist - i.e. it was not registered into the DeviceThreadFactory.
     * this pointer is required when calling instance() the first time - DeviceThreadFactory needs a parent.
     * This is the first place where DeviceThreadFactory is instantiated and so call instance() with a valid parent - this -
     */
    th = DeviceThreadFactory::instance()->getDeviceThread(devName);
    if(!th) /* thread not already allocated: create a new one */
    {
        try{
            th = new DeviceThread(devName); /* will add itself to the device thread factory, _if  Tango device connection succeeds_! */
            th->start();
            /*  NOTE:
             * registration takes place in DeviceThread constructor, deregistration in DeviceThread destructor.
             */
        }
        catch(DevFailed &e)
        {
            delete th;
            /* do not TUtil::log() here: since we re-throw, let the last catcher log all */
            Except::re_throw_exception(e, "Error subscribing the source \"" + d_name.toStdString() + "\"",
                                       "Could not find nor create a device thread for \n\"" + devName.toStdString() + "\"",
                                       " SubscriberProxy::subscribe()", Tango::ERR);
            return NULL;
        }
    }
    /* the device thread will directly invoke configurationAvailable() when ready (through the this pointer) */
    PointEventData *pointEvent = new PointEventData(d_name, type, point,
                                                    args, d_desiredAttributeProperties,
                                                    mAttrPropDbTimeoutMillis,
                                                    d_mode, d_period, this);
    th->addPoint(pointEvent);

    if (d_blocking)
    {
        pinfo("blocking option provided: waiting for the new action to be available...");
        while (!ActionFactory::actionFactory()->actionForName(d_name))
            usleep(1000);
        taction = ActionFactory::actionFactory()->actionForName(d_name);
    }
    return taction;
}

bool SubscriberProxy::event(QEvent *e)
{
    if((e->type() == ACTION_CONFIGURATION_FAILED_EVENT || e->type() == ACTION_EVENT) && d_parentDestroyed)
    {
        pdelete("scheduling subscriber \"%s\"'s auto destruction: comm. handle no more alive...", qstoc(d_name));
        if(e->type() == ACTION_EVENT)
            ActionFactory::actionFactory()->destroy(static_cast<ActionCreationEvent *>(e)->action());
        deleteLater();
    }
    else if(e->type() == ACTION_CONFIGURATION_FAILED_EVENT)
    {
        ActionConfigurationFailedEvent * confFailedEv = static_cast<ActionConfigurationFailedEvent *>(e);
        pinfo("\e[0;31mconfiguration failed\e[0m for source \"%s\" - will start a \e[0;4mretry cycle\e[0m", qstoc(confFailedEv->sourceName()));
        if(d_autoReconnectTimer)
            d_autoReconnectTimer->scheduleRetry();
        emit configurationErrorMessage(QString("configuration of \"%1\" failed: connection retry cycle started: waiting for device...")
                                       .arg(confFailedEv->sourceName()));
        emit connectionFailed();
        emit connectionOk(false);
    }
    else if( e->type() == ACTION_EVENT)
    {
        if(d_autoReconnectTimer && d_autoReconnectTimer->isActive())
        {
            pok("reconnection \e[1;32msuccessful\e[0m for source \"%s\" after \e[1;36m%d reconnection attempts\e[0m",
                qstoc(d_name), d_autoReconnectTimer->attemptsNo());
            d_autoReconnectTimer->unscheduleRetry();
            delete d_autoReconnectTimer;
            d_autoReconnectTimer = NULL;
        }
        ActionCreationEvent *ace = static_cast<ActionCreationEvent *>(e);
        TAction *a = ace->action();
        /* each AddPointEvent sent to the DeviceThread contains the pointer to ourselves, see subscribe(), and
        * the DeviceThread will post an ActionCreationEvent to us according to that pointer. So each AddPointEvent
        * and each ActionCreationEvent are sent univoquely to us.
        */
        emit configured(a);
        // 		/* verify that the action we receive is the one we are wating for */
        // 		/* case 1: polled attributes or targets: desired mode, period and source must be the same */
        // 		if(a != NULL && a->desiredRefreshMode() == d_mode && a->period() == d_period && a->source() == d_name)
        // 		{
        // 			emit configured(s, a);
        // 		}
        // 		/* case 2: events: a->period() will be 0, so different from d_period.
        // 		 * Obviously, also in this case the d_name must be equal to a->source() but, in addition,
        // 		 * if the periods differ it must be dued to the events
        // 		 */
        // 		else if(a != NULL && a->desiredRefreshMode() == d_mode && a->period() == 0 && a->source() == d_name
        // 		       && d_mode != TARGET) /*  event driven */
        // 		{
        // 			emit configured(s, a);
        // 		}
        // 		else if(a != NULL)/* something is wrong if here */
        // 		{
        // 			perr("SubscriberProxy: error: cannot recognize an action as one of mine:");
        // 			perr("mine is: \"%s\", period: %d, mode: %s", qstoc(d_name), d_period,
        // 			qstoc(TUtil::instance()->modeRepresentation(d_mode)));
        // 			perr("other is: \"%s\", period: %d, mode: %s", qstoc(a->source()), a->period(),
        // 			qstoc(TUtil::instance()->modeRepresentation(a->mode())));
        // 			perr("The associated object info/name is \"%s\"", qstoc(d_assocObjName));
        // 			ActionFactory::actionFactory()->destroy(a);
        // 		}
        // 		else if(a == NULL) /* tell the communication handle that the action is NULL */
        // 			emit configured(s, a);
        return true;
    }
    return QObject::event(e);
}

void SubscriberProxy::deleteAutoReconnectTimer()
{
    if(d_autoReconnectTimer)
    {
        d_autoReconnectTimer->stop();
        delete d_autoReconnectTimer;
        d_autoReconnectTimer = NULL;
    }
}

void SubscriberProxy::stopAutoReconnectTimer()
{
    if(d_autoReconnectTimer && d_autoReconnectTimer->isActive())
        d_autoReconnectTimer->stop();
}


