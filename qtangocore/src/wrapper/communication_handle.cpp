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

/* $Id: communication_handle.cpp,v 1.110 2013-07-03 13:54:43 giacomo Exp $ */


#include <QtDebug>
#include <QMutex>
#include <QWidget>
/* to enable/disable widgets after configuration */
#include <QMetaProperty> 
#include <action_factory.h>
#include <QCoreApplication>
#include "communication_handle.h"
#include "source_validator.h"
#include "taction_polling_period_change_event.h"
#include "action_polling_events.h"
#include "qtangocore_read_stats.h"
#include <tlog.h>


QTangoCommunicationHandle::QTangoCommunicationHandle(QObject *parent, RefreshMode mode, int period) :
	QObject(parent)
{
    Q_ASSERT(parent != NULL);
    d_source =  QString();
    d_sources = QStringList();

    d_handleConfigured = false;
    d_action = NULL;
    d_actionList = NULL;
    d_refreshMode = mode;
    d_period = period;
    d_actionWaitRefresh = true;
    d_designerMode = false;
    d_toolTipsEnabled = true;
    d_confChangeEventEnabled = true;
    d_executionMode = Synchronous;
    d_subscriberProxy = NULL;
    mAttrPropDbTimeoutMillis = -1;

    /* set the ancestor of the TUtil, looking for the ancestor in the objects */
    QObject *temp = dynamic_cast<QObject*>(parent), *a = NULL;
    while (temp != NULL)
    {
        a = temp;
        temp = temp->parent();
    }
    TUtil::instance()->setAncestor(a);
}

/* the communication handle destructor is responsible to unsetSource() on the configured source.
 *  Unsubscription is made by unsetSource throughout an UnsubscriberProxy.
 * In turn, the unsubscriber proxy retrieves the DeviceThread linked to the source and triggers a 
 * remove point event on it (see unsubscriber_proxy.cpp).
 */
QTangoCommunicationHandle::~QTangoCommunicationHandle()
{
    qprintf("\e[0;31mQTangoCommunicationHandle::~QTangoCommunicationHandle():\e[0m will call unset source on \"%s\" - handle configured %d\n",
            qstoc(d_source), d_handleConfigured);
    if (!d_source.isEmpty()) /* d_handleConfigured is set to false in unsetSource() */
        unsetSource();
    if (d_actionList)
        unsetSources();
    if(d_subscriberProxy)
        d_subscriberProxy->setParentDestroyed();
        
}

// bool QTangoCommunicationHandle::event(QEvent *e)
// {
// 	if(e->type() == ATTRIBUTE_CONFIGURATION_QEVENT && static_cast<AttributeConfigurationEvent *>(e))
// 	{
// 		AttributeConfigurationEvent *ace = static_cast<AttributeConfigurationEvent *>(e);
// 		/* extract the configuration data from the event */
// 		const TangoConfigurationParameters *cp = ace->configurationParameters();
// 		if(cp != NULL)
// 		{
// 			if(d_sources.size())
// 				pok("auto configuration possible for \"%s\"", qstoc(d_sources.first()));
// 			else 
// 				pok("auto configuration possible for \"%s\"", qstoc(d_source));
// 
// 			emit attributeAutoConfigured(cp);
// 			/* save the attribute configuration received */
// 			d_tangoConfigurationParameters = cp;
// 			return true;
// 		}
// 		else /* should not get here */
// 		{
// 			pwrn("could not setup auto configuration for \"%s\" (should not have arrived here!)", qstoc(d_source));
// 			return false;
// 		}
// 
// 	}
// 	else
// 		return QObject::event(e);
// }

/** \brief slot connected to the subscriber proxy "configured" signal.
 *
 * The subscriber proxy emits the configured() signal inside its event() method.
 * This means after an ActionCreationEvent is posted to the subscriber proxy by the DeviceThread::process()
 * method, after the successful creation of the TAction.
 */
void QTangoCommunicationHandle::configureHandle(TAction* r) 
{
    qprintf("configureHandle %p \"\e[1;34m%s\e[0m\"\n", r, qstoc(this->source()));
    if(!ACTION_ALIVE(r))
        return;

    TVariant firstValueRead;
    if(r->source() != d_source)
    {
        /* The action we received does not have to do with ourselves (any more). Probably source has changed while waiting for
	   * the first source configured() signal. 
	   * Two setSource() very close to each other might cause this.
	   */
        pinfo("configureHandle: received a \e[1;31;4mnon pertinent action\e[0m \"%s\". My current source is \"%s\", unsubscribing old action",
              qstoc(r->source()), qstoc(d_source));
        UnsubscriberProxy unsubscriber(r->source(), r->desiredRefreshMode(), r->period());
        unsubscriber.unsubscribe(); /* will delete r because it is no more needed */
        r = NULL;
    }
    else if(r->source() == d_source && d_handleConfigured)
    {
        pinfo("configureHandle: new source configuration detected: \e[1;31;4mdeleting old action\e[0m \"%s\"", qstoc(d_action->source()));
        disconnect(d_action, 0, this ,0); /* no more signals from the old actin */
        /* The handle was already configured with an action connected to tango and a new action has been configured.
	   * This happens when two setSources() are called on the same handle, without calling unsetSource()
	   * between the two, and the first source succeeded configuring. The two setSources() have not
	   * been called so close to each other. Thus, the configuration of the first was able to complete
	   * configuring the handle before the arrive of the second configuration signal.
	   */
        UnsubscriberProxy unsubscriber(d_action->source(), d_action->desiredRefreshMode(), d_action->period());
        /* TAction pointed by previously configured action will be deleted by the unsubscriber.
	   */
        unsubscriber.unsubscribe();
    }

    if(r->source() == d_source && r->isConfigured())
    {
        d_handleConfigured = true;
        d_action = r;
        r->setWaitRefresh(d_actionWaitRefresh);

        /* TAction r created and living in another thread */
        connect(r, SIGNAL(newData(const TVariant&)), this, SLOT(newDataAvailable(const TVariant&)),
                Qt::QueuedConnection);
        /* messages from TAction */
        connect(r, SIGNAL(connectionStatusChanged(const QString&)), this,
                SLOT(updateConnectionStatus(const QString &)), Qt::QueuedConnection);
        /* Auto configuration available from TAction, from attribute configuration change event */
        connect(r, SIGNAL(notifyConfigurationChangedEvent(const TangoConfigurationParameters *)), this,
                SLOT(slotAttributeAutoConfigured(const TangoConfigurationParameters *)), Qt::QueuedConnection);

        /* get the first execute() return value. Cannot call TAction::dequeue() here because it is not thread safe */
        firstValueRead = r->lastValueRead();

        /* the user called setPeriod() between setSource() and the configuration of the TAction
	   * (which takes place in another thread). In this case, we force another setPeriod() to
	   * correctly (re)configure the period.
	   */
        if(r->period() && r->period() != d_period)
        {
            /* r->period() was the period at the moment of the setSource(), that created the TAction with that period.
		 * d_period might be different if the programmer called setPeriod after setSource() and before this slot
		 * was invoked.
		 */
            pwarn("comm.handle: \"%s\" period changed between setSource() and source configuration from %d to %d",
                  qstoc(source()), r->period(), d_period);
            pwarn("to improve performance, please call setPeriod() before setSource().");
            setPeriod(period());
        }
        if(r->period())
        {
            pok("subscribed and configured \"%s\" [desired mode:%s|real:%s|period:%dms] obj: \"%s\"",
                qstoc(r->source()), qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())),
                qstoc(TUtil::instance()->modeRepresentation(r->mode())), d_period, objinfo(parent()));
            /* d_period and not r->period(), because the setPeriod() might have been called above and it
		   * posts an event to r, and that event might not have been delivered yet.
		   */
            TUtil::instance()->adjustGlobalRefreshIntervalIfNeeded(d_period); /* d_period! */
        }
        else
            pok("subscribed and configured \"%s\" [desired mode:%s|real:%s] obj: \"%s\"",
                qstoc(r->source()), qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())),
                qstoc(TUtil::instance()->modeRepresentation(r->mode())), objinfo(parent()));

        /* connection with tango was successful: now see if this handle represents a widget. If yes, if
	   * the widget is hidden and hide/show events are enabled, we'll stop the poller.
	   * Wait let's say three times the polling period, so the poller refreshes two or three times and
	   * the widget (if it is a widget, but d_stopPollingIfWidgetHidden() will test ) becomes visible
	   * or it is (more or less surely) visible.
	   */
        if(r->mode() == POLLED_REFRESH)
            QTimer::singleShot(d_period * 3, this, SLOT(d_stopPollingIfWidgetHidden()));

        /* When done configuring, send necessary signals and notifications. From this point on, it
         * is possible that unsetSource() is called or a new source is set, and so the action be deleted.
         * For this, emit configuration SIGNALS after the action (r) is used.
         */
        /* 1. fist send auto configuration and notify connection ok, if first read is not invalid */
        connectionOkNotification(true);
        d_connectionStatus = firstValueRead.message(); /* update connection status */
        slotAttributeAutoConfigured(r->autoConfiguration()->autoConfigurationData());
        /* 2. then send first data read */
        newDataAvailable(firstValueRead);
    } /* if: r != NULL && r->source() == d_source && r->isConfigured() */
    /* let the sender(), i.e. the SubscriberProxy() leave the QObject::event() before deleting. */
    if(qobject_cast<SubscriberProxy*>(sender()))
    {
        sender()->deleteLater(); /* delete later ... */
        d_subscriberProxy = NULL; /* ... but immediately mark d_subscriberProxy NULL */
    }
}

void QTangoCommunicationHandle::d_stopPollingIfWidgetHidden()
{
    QWidget *w = qobject_cast<QWidget *>(parent());
    if(w && d_hideEventEnabled && !w->isVisible())
    {
        pstep("widget \"%s\" connected to \"%s\" is not visible", qstoc(w->objectName()), qstoc(d_source));
        stopPoller();
    }
}

void QTangoCommunicationHandle::subscriberDestroyed(QObject *obj)
{
    if(obj == d_subscriberProxy)
        pdelete("deleting %s's subscriber proxy %p [%s]", qstoc(obj->objectName()), d_subscriberProxy, qstoc(d_source));
    d_subscriberProxy = NULL;
}

void QTangoCommunicationHandle::connectSource()
{
    if(d_period < TUtil::instance()->globalRefreshInterval())
    {
        pinfo("period for \"%s\" [%dms] is less than the global refresh interval [%dms]: automatically",
              objinfo(parent()), d_period, TUtil::instance()->globalRefreshInterval());
        pinfo("increasing its value to %dms.", d_period);
        TUtil::instance()->setGlobalRefreshInterval(d_period);
    }
    /* if the subscriber proxy is still alive, delete it here: it might have survived through subsequent setSource()s
	 * an still be trying to reconnect to a unreachable source.
	 * Stop the retry timer, but do not delete it: it might be still waiting for ActionConfigurationEvent.
	 */
    if(d_subscriberProxy != NULL)
    {
        d_subscriberProxy->disconnect();
        d_subscriberProxy->deleteAutoReconnectTimer();
    }

    /* d_subscriberProxy is created here */
    if(parent() != NULL)
        d_subscriberProxy = new SubscriberProxy(d_source, d_refreshMode, false, d_period, parent()->objectName());
    else
        d_subscriberProxy = new SubscriberProxy(d_source, d_refreshMode, false, d_period);

    if(d_desiredAttributeProperties.size())
        d_subscriberProxy->setDesiredAttributeProperties(d_desiredAttributeProperties);

    if(mAttrPropDbTimeoutMillis > -1)
        d_subscriberProxy->setAttributePropertyDbTimeoutMillis(mAttrPropDbTimeoutMillis);

    /* The subscriber  receives an event from the device thread through postEvent().
	 * Inside the SubscriberProxy::event() handler this signal is emitted. Hence, the 
	 * configured(QString,TAction*) signal comes from the same thread as handle's.
	 * It is safe to use DirectConnection here.
	 * The subscriber proxy emits configured() only if the DeviceThread posts an ActionCreationEvent.
	 * So we will enter configureHandle() if and only if the TAction created for us is alive.
	 */
    connect(d_subscriberProxy, SIGNAL(configured(TAction*)), this, SLOT(configureHandle(TAction*)), Qt::DirectConnection);
    connect(d_subscriberProxy, SIGNAL(destroyed(QObject *)), this, SLOT(subscriberDestroyed(QObject *)));
    connect(d_subscriberProxy, SIGNAL(configurationErrorMessage(const QString&)), this,
            SLOT(connectionErrorMessageNotification(const QString &)));
    connect(d_subscriberProxy, SIGNAL(connectionFailed()), this, SLOT(connectionFailedNotification()));
    connect(d_subscriberProxy, SIGNAL(connectionOk(bool)), this, SLOT(connectionOkNotification(bool)));
    pok("subscribing source \"%s\" {%s}", qstoc(d_source), objinfo(parent()));
    try{
        d_subscriberProxy->subscribe();
        d_connectionStatus = QString("Subscriber proxy activated for source \"%1\"... waiting for configuration...").arg(d_source);
    }
    catch(DevFailed &e)
    {
        perr("invalidating handle for the source \"%s\" [mode %s period %d] for the object \"%s\"",
             qstoc(d_source), qstoc(TUtil::instance()->modeRepresentation(d_refreshMode)),
             d_period, objinfo(parent()));
        d_handleConfigured = false;
        d_action = NULL;
        // // // 		d_source = d_oldSource = s; /* save anyway in old source the desired string value */
        log(e);
        TLog dFailLog(e);
        d_connectionErrorMessage = QString("Caught exception while subscribing source.\nError :%1").arg(dFailLog.formatMessage());
        connectionOkNotification(false);
        connectionFailedNotification();
        connectionErrorMessageNotification(d_connectionErrorMessage);
        TVariant v(d_source);
        v.setMessage(e.errors);
        emit newData(v);
        d_connectionStatus = d_connectionErrorMessage;
    }
}


void QTangoCommunicationHandle::startPoller()
{
    bool action_alive = ActionFactory::actionFactory()->actionAlive(d_source, d_action);
    if(action_alive && d_handleConfigured && !d_source.isEmpty())
    {
        /* d_action->mode() is the real mode, not the desired one. Restart poller only
         * for POLLED_REFRESH sources.
         */
        if((!d_action->timerActive()) &&
           ((d_action->mode() == POLLED_REFRESH)))
        {
            ActionRestartPollingEvent *aStartPolling = new ActionRestartPollingEvent();
            QCoreApplication::instance()->postEvent(d_action, aStartPolling);
        }
        else
            pinfo("not restarting poller for source \"%s\", \"%s\", [%s]", qstoc(d_source), objinfo(parent()),
                  qstoc(TUtil::instance()->modeRepresentation(d_action->mode())));
    }
}


void QTangoCommunicationHandle::setSource(QString s)
{
    int pos = 0;
    QTangoSourceValidator validator(this);
    /* we need a sourceCopy to pass to replaceWildcards(). replaceWildcards() operates by
	 * reference and so we do not always want to lose the passed string. For instance, when
	 * replace wildcards produces a not valid string (designer when used with $ )
	 */
    QString sourceCopy = s;

    /* case 0: s not empty and different from a previously _configured_ source:
	 * must unset source to remove the old source
	 */  
    if(s != d_source && !d_source.isEmpty())
    {
        pinfo("unsetting source \"%s\" before updating to \"%s\".\e[0m\n", qstoc(d_source), qstoc(s));
        unsetSource();
    }
    /* case 1:
	 * a. handle configured 
	 * b. mode not manual nor polled
	 * c. argument `s' equal to d_source 
	 * => source s has the same name of a previously event-driven configured source: let it be
	 */
    if((s == d_source) && d_handleConfigured && ActionFactory::actionFactory()->actionAlive(d_action) &&
       (d_action->mode() != MANUAL_REFRESH) && (d_action->mode() != POLLED_REFRESH) )
    {
        pinfo("source already configured in %s: source running in event mode. Doing nothing.",
              qstoc(TUtil::instance()->modeRepresentation(d_action->mode())));
        return;
    }
    /* case 2: source empty: refuse to configure */
    if(s.isEmpty())
    {
        pwarn("source is empty: cannot configure it {%s} (this is normal in first show event)", objinfo(parent()));
        return;
    }

    /* case 3: leave this after case 1 to let case 1 print its information about event driven actions
	 * a. `s' equal to source previously set
	 * b. handle already configured: setSource() called twice (actually unsetSource() sets handle not configured )
	 */
    if(s == d_source && d_handleConfigured)
    {
        pwarn("source already configured with the same name: %s {%s}", s.toStdString().c_str(), objinfo(parent()));
        return;
    }
    else if(s.contains("$") && validator.validate(s, pos) == QValidator::Acceptable)
    {
        pok("replacing wildcards for source \"%s\" {%s}", qstoc(s), objinfo(parent()));
        TUtil::replaceWildcards(sourceCopy, QCoreApplication::instance()->arguments());
    }
    /* after replaceWildcards the name won't have to contain the $ any more: validate it a bit more strictly */
    validator.setRegexp2();
    if(validator.validate(sourceCopy, pos) != QValidator::Acceptable)
    {
        d_connectionErrorMessage = QString("Cannot configure the source \"%1\" for the object \"%2\". Syntax error?").arg( s).arg( objinfo(parent()));
        perr("%s", qstoc(d_connectionErrorMessage));
        // // 		d_source = d_oldSource = s; /* save anyway in old source the desired string value */
        d_source = s;
        connectionOkNotification(false);
        connectionFailedNotification();
        connectionErrorMessageNotification(d_connectionErrorMessage);
        d_connectionStatus = d_connectionErrorMessage;
        return;
    }
    else
        s = sourceCopy; /* replace wildcard operated well or replaceWildcard not needed and source ok */

    d_source = s;

    setObjectName(QString("%1_comHandle[%2]").arg(parent() ? parent()->objectName() : "NULLparent").arg(d_source));

    /* always connect all sources, also for hidden widgets... then if the widget is hidden, the poller
	 * will be stopped.
         */
    ADD_READ_STAT_MSG(d_source, "setSource", STATS_OBJINFO(this));
    connectSource();
}

void QTangoCommunicationHandle::stopPoller()
{
    bool action_alive = ActionFactory::actionFactory()->actionAlive(d_source, d_action);
    if(action_alive && d_handleConfigured && !d_source.isEmpty())
    {
        /* d_action->mode() returns the real mode, not the desired one.
         * Stop the poller only when POLLED_REFRESH mode.
         */
        if((d_action->mode() == POLLED_REFRESH))
            {
            if(d_action->numSubscribers() == 1)
            {
                ActionStopPollingEvent *aStopPolling = new ActionStopPollingEvent();
                QCoreApplication::instance()->postEvent(d_action, aStopPolling);
            }
            else
                pinfo("not stopping poller for source \"%s\", \"%s\", [%s] because the number of subscribers is %d",
                      qstoc(d_source), objinfo(parent()),
                      qstoc(TUtil::instance()->modeRepresentation(d_action->mode())), d_action->numSubscribers());
        }
        else
            pinfo("not stopping poller for source \"%s\", \"%s\", [%s]", qstoc(d_source), objinfo(parent()),
                  qstoc(TUtil::instance()->modeRepresentation(d_action->mode())));
    }
    else
        qprintf("\e[1;36m not stopping poller: %d %d %d\e[0m\n", action_alive, d_handleConfigured, !d_source.isEmpty());
}

/* remember to check that the action pointer, d_action, represents a valid action alive in the ActionFactory
 * before using it.
 */
void QTangoCommunicationHandle::unsetSource(bool by_hideEvent)
{
    Q_UNUSED(by_hideEvent);
    if(d_subscriberProxy)
    {
        d_subscriberProxy->deleteAutoReconnectTimer();
        d_subscriberProxy->disconnect();
    }

    /* If handle is not configured, then unsetSource() needs only set d_source variable to an empty string.
   * A subsequent setSource will trigger the whole configuration process.
   * A special case is represented by the following three invokations, in the exact order:
   *
   * setSource("a/very/simple/source");
   * unsetSource(); (d_handleConfigured is false because configureHandle() still hasn't been invoked)
   * configureHandle() slot invokation.
   * In that case configureHandle will receive an action connected to "a/very/simple/source", but will 
   * find d_source empty: it will simply discard the action received and unsubscribe it.
   *
   */
    if(d_handleConfigured) /* means that configureHandle() was called with a connected action */
    {
        ADD_READ_STAT_MSG(d_source, "unsetSource", STATS_OBJINFO(this));
        if(ACTION_ALIVE(d_action))
        {
            disconnect(d_action, 0, this ,0); /* disconnect `this' specific receiver */
            UnsubscriberProxy unsubscriber(d_source, d_action->desiredRefreshMode(), d_action->period());
            unsubscriber.unsubscribe();
        }
        d_handleConfigured = false;
        d_connectionStatus = "disconnected by \"unset source\"";
        d_action = NULL;
    }
    d_source = QString();
}

/* set sources manages the targets with TActionList */
void QTangoCommunicationHandle::setSources(QStringList sourcesList)
{
    QString element;
    int i, pos = 0;
    QStringList savedSourcesList = sourcesList; /* to use in case of validation failure of an element */
    if(sourcesList.size() == 0)
    {
        pwarn("no sources/targets provided! Cannot setup object %s", objinfo(parent()));
        return;
    }
    /* check if some source (really target) has a wild card inside and replace it */
    for(i = 0; i < sourcesList.size(); i++)
    {
        QString source = sourcesList[i];
        QTangoSourceValidator validator(this);
        if(source.isEmpty())
        {
            pwarn("the first element of the sources list for the object \"%s\" is empty. Cannot configure it.", objinfo(parent()));
        }
        else if(source.contains("$") && validator.validate(source, pos) == QValidator::Acceptable )
        {
            pok("the source \"%s\" in the \e[4maction list\e[0m contains a wildcard: processing it",
                qstoc(sourcesList[i]));
            TUtil::replaceWildcards(source, QCoreApplication::instance()->arguments());
        }
        /* after replaceWildcards the name won't have to contain the $ any more: validate it a bit more strictly */
        validator.setRegexp2();
        if( validator.validate(source, pos) != QValidator::Acceptable)
        {
            d_connectionStatus = d_connectionErrorMessage = QString("the source \"%1\" assigned to the object \"%2\" is not syntactically valid! Cannot configure it.").
                                                            arg(source).arg(objinfo(parent()));
            perr("%s", qstoc(d_connectionErrorMessage));
            perr("Leaving all the list not configured for \"%s\".", objinfo(parent()));
            // // // 			  d_sources = d_oldSources = savedSourcesList;
            d_sources = savedSourcesList;
            /* in designer mode, one is normally not interested in error messages. Even worst
			 * would be to have the widgets automatically disabled.
			 */
            connectionOkNotification(false);
            connectionFailedNotification();
            connectionErrorMessageNotification(d_connectionErrorMessage);

            return;
        }
        else
            sourcesList[i] = source; /* perhaps modified by replaceWildcards */
    }
    if(sourcesList == d_sources && d_handleConfigured)
    {
        pwarn("sources already configured with the names: {%s}", objinfo(parent()));
        qslisttoc(d_sources);
        return;
    }
    /* use TActionList for setSources (really it is setTargets ) */
    if(d_actionList != NULL/* && d_sources.size() > 0 && d_handleConfigured*/)
    {
        // 	  printf("\e[1;31munset Source prima delle nuove sorgenti: vecchie %s nuove %s\n", qstoc(d_actionList->sources().join(", ")),
        // 			 qstoc(d_sources.join(", ")));
        pinfo("unsetting sources before setting new ones {%s}", objinfo(parent()));
        unsetSources();
    }
    else if(d_actionList != NULL && d_sources.size())
    {
        pinfo("targets changed before handle configuration: please avoid this (\"%s\")", qstoc(d_sources.join(";")));
    }
    pok("subscribing sources [mode: %s] for each of the following entries: {%s}",
        qstoc(TUtil::instance()->modeRepresentation(d_refreshMode)), objinfo(parent()));
    qslisttoc(sourcesList);

    setObjectName(QString("%1_comHandle[%2]").arg(parent() ? parent()->objectName() : "NULL parent").arg(sourcesList.join(";")));

    try{
        /* will be set to true if TActionList emits the actionListConfigured signal.
		 */
        d_handleConfigured = false;
        d_actionList = NULL;
        /* temporary pointer: avoids assigning to d_actionList a not completely allocated
		 * TActionList and leaves d_actionList null if an exception is caught
		 */
        if(parent() != NULL)
            d_actionList = new TActionList(sourcesList, d_desiredAttributeProperties, d_refreshMode, parent()->objectName());
        else
            d_actionList = new TActionList(sourcesList, d_desiredAttributeProperties, d_refreshMode);
        /* no exception caught during new TActionList: we can go on and assign a valid pointer to d_actionList */
        d_sources = sourcesList;
        d_connectionStatus = QString("Action list created for targets \"%1\". Wating for configuration complete")
                             .arg(d_sources.join("\","));
    }
    catch(DevFailed &e)
    {
        qslisttoc(sourcesList);
        d_handleConfigured = false;
        // // // 		d_sources = d_oldSources = savedSourcesList;
        d_sources = savedSourcesList;
        log(e);
        TLog failLog(e);
        d_connectionStatus = failLog.formatMessage();
    }
    if(d_actionList)
    {
        connect(d_actionList, SIGNAL(newData(const TVariant&)), this, SLOT(newDataAvailable(const TVariant&)));

        connect(d_actionList, SIGNAL(notifyLastAttributeConfigurationChangedEvent(const TangoConfigurationParameters *)),
                this, SLOT(slotAttributeAutoConfigured(const TangoConfigurationParameters *)));

        connect(d_actionList, SIGNAL(finishedAll()), this, SLOT(actionListFinished()));
        connect(d_actionList, SIGNAL(finished(int)), this, SLOT(stepAccomplished(int)));
        connect(d_actionList, SIGNAL(connectionOk(bool)), this, SLOT(connectionOkNotification(bool)));
        connect(d_actionList, SIGNAL(connectionFailed()), this, SLOT(connectionFailedNotification()));
        connect(d_actionList, SIGNAL(connectionError(const QString &)), this, SLOT(connectionErrorMessageNotification(const QString &)));
        connect(d_actionList, SIGNAL(connectionStatusChanged(const QString&)), this, SLOT(updateConnectionStatus(const QString &)));
        /* the d_actionList SIGNAL actionListConfigured is emitted when the desired sources size equals the configured sources
	   * size. The slot will mark this handle as configured.
	   */
        connect(d_actionList, SIGNAL(actionListConfigured(bool)), this, SLOT(actionListConfigured(bool)));
    }
}

void QTangoCommunicationHandle::actionListConfigured(bool c)
{
    d_handleConfigured = c;
}

void QTangoCommunicationHandle::unsetSources()
{
    if (d_actionList)
    {
        d_actionList->disconnect();
        pok("analyzing \e[4maction list\e[0m and unsubscribing for the following sources {%s}:", objinfo(parent()));
        qslisttoc(d_sources);
        delete d_actionList;
        d_actionList = NULL;
        d_handleConfigured = false; /* set deconfiguration, step 2 */
    }
    d_sources.clear();
}

void QTangoCommunicationHandle::setPeriod(int p)
{
    PollingPeriodChangeEvent *ppce = new PollingPeriodChangeEvent(p);
    if(ActionFactory::actionFactory()->actionAlive(d_action))
    {
        QCoreApplication::instance()->postEvent(d_action, ppce);
    }
    d_period = p;
    TUtil::instance()->adjustGlobalRefreshIntervalIfNeeded(p);
}

QString QTangoCommunicationHandle::sourcesAsSingleString()
{
    QString ret;
    ret = d_sources.join(";");
    return ret;
}

QList<TVariant> QTangoCommunicationHandle::executeAll(QVariant v)
{
    QList<TVariant>  ret;
    if(d_actionList && d_handleConfigured)
    {
        if(d_executionMode == Synchronous)
            ret = d_actionList->execute(v);
        else if(d_executionMode == DontStopOnErrors)
            ret = d_actionList->execute(v, true);
        else if(d_executionMode == Asynchronous)
            d_actionList->executeAsynch(v); /* returns void */
    }
    else
    {
                        printf("\e[1;31m* \e[0maction list not executed: action list is NULL!\e[0m\n");
        // 		QMessageBoxEvent *mev = new QMessageBoxEvent(QString("The action list was not executed.\n"
        // 					"This might be dued  to the QTangoCommunicationHandle that refused to configure\n"
        // 					"the source you provided. In most cases, there was a syntax error on the source name\n"
        // 					"rather than a tango concerned issue.\n"
        // 					"The object involved is \"%1\".\nCheck the source or target syntax in setSource() or setTargets()").
        // 				arg(objinfo(parent())));
        // 		QCoreApplication::postEvent(TUtil::instance(), mev);
    }
    return ret;
}

void QTangoCommunicationHandle::executeAsynch(QVariant v)
{
    if(ACTION_ALIVE(d_action))
	d_action->executeAsynch(v);
}

TVariant  QTangoCommunicationHandle::execute()
{
    TVariant t;
    if(ACTION_ALIVE(d_action))
        t = d_action->execute();
    return t;
}

int QTangoCommunicationHandle::actionsCount()
{ 
    if(d_actionList)
        return d_actionList->actions().size();
    else
        return ACTION_ALIVE(d_action) ? 1 : 0;
}

void QTangoCommunicationHandle::setActionWaitRefresh(bool enable)
{
    if(ACTION_ALIVE(d_action))
        d_action->setWaitRefresh(enable);
    d_actionWaitRefresh = enable;
}

bool QTangoCommunicationHandle::actionWaitRefresh()
{
    if(ACTION_ALIVE(d_action))
        return d_action->waitRefresh();
    else
        return d_actionWaitRefresh;
}

void QTangoCommunicationHandle::setAutoConfiguration(bool en, bool confChangeEventEnabled)
{ 
    d_autoconfEnabled = en; d_confChangeEventEnabled = confChangeEventEnabled;
}

void QTangoCommunicationHandle::slotAttributeAutoConfigured(const TangoConfigurationParameters *cp)
{
    emit attributeAutoConfigured(cp);
}

void QTangoCommunicationHandle::setHideEventEnabled(bool e)
{
    if(e != d_hideEventEnabled)
    {
        // 		/* maybe we called setHideEventEnabled _after_ setSource() and setSource() was not called
        // 		 * on show event or connectSource() was not called by setSource() because the widget was
        // 		 * hidden. Moreover, the widget might have been disabled and hidden when setSource() was called,
        // 		 * and then disabled widgets do not receive showEvents.
        // 		 */
        // 		if(e && !d_hideEventEnabled && !configured() && qobject_cast<QWidget *>(parent()))
        // 		{
        // 			pok("source \"%s\" was not previously configured and show/hide events have been enabled: setting source now",
        // 			    qstoc(d_source));
        // 			setSource(d_source);
        // 		}
        // 		/* disabled hide events but at setSource() time it was enabled: if the widget was not visible, then connectSource()
        // 		 * wasn't called.
        // 		*/
        if(!e && d_hideEventEnabled && qobject_cast<QWidget *>(parent()))
        {
            pinfo("show/hide events enabled for \"%s\" (%s): starting polling, if necessary",
                  qstoc(d_source), objinfo(parent()));
            startPoller();
        }
    }
    d_hideEventEnabled = e;
}

const TangoConfigurationParameters *QTangoCommunicationHandle::tangoConfigurationParameters() const
{
    const TangoConfigurationParameters *cp = NULL;
    /* !d_source.isEmpty() means the handle is configured for an attribute */
    if(!d_source.isEmpty() && ACTION_ALIVE(d_action))
	cp = d_action->autoConfiguration()->autoConfigurationData();
    return cp;
}

void QTangoCommunicationHandle::newDataAvailable(const TVariant& v)
{
    emit newData(v);
    d_lastValueRead = v;
    /* unused yet */
    // 	emit newData(v.tangoPoint(), v);
}

void QTangoCommunicationHandle::stepAccomplished(int s) 
{
    int total;
    if(d_actionList)
    {
        total = actionsCount();
        emit progress(s, total);
    }
}

int QTangoCommunicationHandle::period()
{
    return d_period;
}

void QTangoCommunicationHandle::setRefreshMode(RefreshMode mode)
{
    if(mode != d_refreshMode && d_handleConfigured)
    {
        d_refreshMode = mode;
        unsetSource();
        setSource(d_source);
    }
    else if(!d_handleConfigured && mode != d_refreshMode && !d_source.isEmpty())
    {
        perr("communication handle: cannot call setRefreshMode right after setSource (\"%s\").", qstoc(source()));
        perr("communication handle: please call setRefreshMode \e[4mbefore setSource\e[0m");
        perr("communication handle: alternatively, use setSource(const QString &source, int period, RefreshMode refreshMode)");
        perr("instead of setSource(const QString&).");
    }
    else
        d_refreshMode = mode;
}

RefreshMode QTangoCommunicationHandle::refreshMode()
{
    if(ActionFactory::actionFactory()->actionAlive(d_source, d_action))
        return d_action->mode();
    else
    {
        pwarn("action associated to \"%s\" object \"%s\" is not alive", qstoc(d_source), objinfo(parent()));
        return d_refreshMode;
    }
}

/** \brief returns true if the source is event driven and the underlying event system
  * is based on ZMQ, false otherwise.
  *
  * @return true:  the event system is based on ZMQ
  * @return false: the event system is based on notifd, or the attribute is polled, or
  *                the tango version is less than 8.
  *
  */
bool QTangoCommunicationHandle::isEventZmq() const
{
    if(ActionFactory::actionFactory()->actionAlive(d_source, d_action))
        return d_action->isZmq();
    return false;
}

int QTangoCommunicationHandle::readerActionRefreshCount() const
{
    /* check whether we are a reader and whether the action is alive */
    if(!d_source.isEmpty() && ACTION_ALIVE(d_action))
	return d_action->refreshCount();
    return 0;
}

void QTangoCommunicationHandle::connectionOkNotification(bool ok)
{
    emit connectionOk(ok);
    enableIfQTangoWidget(ok);
}

void QTangoCommunicationHandle::connectionFailedNotification()
{
    emit connectionFailed();
}

void QTangoCommunicationHandle::connectionErrorMessageNotification(const QString& msg)
{
    d_connectionStatus = msg;
    emit connectionErrorMessage(msg);
}

void QTangoCommunicationHandle::updateConnectionStatus(const QString &s)
{
    d_connectionStatus = s;
}

bool QTangoCommunicationHandle::log(Tango::DevFailed &e)
{
    if(!ActionFactory::actionFactory()->loggingDisabled())
    {
        TLog failLog(e);
        TUtil::instance()->addLog(failLog.toTimeErrlistPair(), failLog.formatMessage());
        QShowLogsEvent *sle = new QShowLogsEvent(true);
        QCoreApplication::postEvent(TUtil::instance(), sle);
    }
    enableIfQTangoWidget(false);
    return true;
}

void QTangoCommunicationHandle::enableIfQTangoWidget(bool en)
{
    /* since in principle we do not know anything about QTangoWidgetCommon interface, and we don't really want to know
     * anything of such interface, let's benefit from QMetaObject's QMetaProperties
     */
    int commStatusChangesEnablePropIndex, widgetEnablePropertyIndex;
    const QMetaObject *metaObject = parent()->metaObject();
    commStatusChangesEnablePropIndex = metaObject->indexOfProperty("communicationStatusChangesWidgetEnable");
    widgetEnablePropertyIndex = metaObject->indexOfProperty("enabled");
    if(commStatusChangesEnablePropIndex >= 0 && widgetEnablePropertyIndex >= 0)
    {
        QMetaProperty metaProperty = metaObject->property(commStatusChangesEnablePropIndex);
        bool commStatusChangesEnable = metaProperty.read(parent()).toBool();
	QMetaProperty enableMetaProperty = metaObject->property(widgetEnablePropertyIndex);
        if(commStatusChangesEnable)
	{
            if(!enableMetaProperty.write(parent(), en)) /* disable/enable widget */
		perr("QTangoCommunicationHandle: failed to disable widget via the \"disable\" property (\"%s\", \"%s\")",
                     qstoc(parent()->objectName()), qstoc(source()));
	}
    }
}

