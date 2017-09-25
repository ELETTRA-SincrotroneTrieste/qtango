/***************************************************************************
*   Copyright (C) 2008 by  Giacomo Strangolino   			   *
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

#include "action_factory.h"
#include "tactiondesc.h"
#include <TApplication>
#include <QList>
#include <QMetaType>
#include "qtangocore_macros.h"
#include "taction_polling_period_change_event.h"
#include "action_configuration_changed_event.h"
#include "actionfactoryobjectfilter.h"
#include <dbus/ActionFactoryAdaptor.h>
#include "tutil.h"
#include <QtDebug>

#include <QReadLocker>
#include <QWriteLocker>

ActionFactory * ActionFactory::__actionFactory = NULL;


ActionFactory *::ActionFactory::actionFactory()
{
    if(!__actionFactory)
        __actionFactory = new ActionFactory();
    return __actionFactory;
}

/* The constructor, since version 3.1, does not accept a parent object to 
 * initialize the QObject base class. This allows the class to stay alive
 * as long as the application closes completely, and not be directly des-
 * troyed by QApplication's destructor.
 */
ActionFactory::ActionFactory() : QObject(NULL)
{
    /* Get TARGET_EXECUTION_LOG_DISABLED environment variable to disable target
     * execution log.
     */
    char *targetExecutionDisabled = getenv("TARGET_EXECUTION_LOG_DISABLED");
    if(targetExecutionDisabled != NULL && (!strcmp(targetExecutionDisabled, "1") ||
          !strcmp(targetExecutionDisabled, "true")))
        m_targetExecutionLogEnabled = false;
    else
        m_targetExecutionLogEnabled = true;

    /* Get DEFAULT_REFRESH_MODE environment variable, if set */
    char *defaultRefreshMode = getenv("DEFAULT_REFRESH_MODE");
    if(defaultRefreshMode == NULL)
        m_defaultRefreshMode = AUTO_REFRESH;
    else
    {
        if(!strcmp(defaultRefreshMode, "POLLED_REFRESH"))
            m_defaultRefreshMode = POLLED_REFRESH;
        else if(!strcmp(defaultRefreshMode, "AUTO_REFRESH"))
            m_defaultRefreshMode = AUTO_REFRESH;
        else if(!strcmp(defaultRefreshMode, "USER_EVENT_REFRESH"))
            m_defaultRefreshMode = USER_EVENT_REFRESH;
        else if(!strcmp(defaultRefreshMode, "CHANGE_EVENT_REFRESH"))
            m_defaultRefreshMode = CHANGE_EVENT_REFRESH;
        else if(!strcmp(defaultRefreshMode, "PERIODIC_EVENT_REFRESH"))
            m_defaultRefreshMode = PERIODIC_EVENT_REFRESH;
        else if(!strcmp(defaultRefreshMode, "ARCHIVE_EVENT_REFRESH"))
            m_defaultRefreshMode = ARCHIVE_EVENT_REFRESH;
        else if(!strcmp(defaultRefreshMode, "MANUAL_REFRESH"))
            m_defaultRefreshMode = MANUAL_REFRESH;
        else
            defaultRefreshMode = NULL; /* mark an error in value */
        if(!defaultRefreshMode)
            perr("ActionFactory: \"DEFAULT_REFRESH_MODE\" environment variable value \"%s\" is "
                 "an invalid refresh mode.", defaultRefreshMode);
        else
            pinfo("ActionFactory: default refresh mode for readers set to \"%s\"", defaultRefreshMode);
    }

    m_initDBus();

    if(m_targetExecutionLogEnabled)
    {
        ActionFactoryObjectFilter *actionFactoryObjectFilter = new ActionFactoryObjectFilter(this);
        connect(actionFactoryObjectFilter,
                SIGNAL(targetExecuted(const QString&, int, bool, const QString&, const QString&)),
                this,
                SIGNAL(targetExecuted(const QString&, int, bool, const QString&, const QString&)));

        installEventFilter(actionFactoryObjectFilter);
    }
    /* initialize logging disabled to false: logging is enabled by default */
    m_loggingDisabled = false;
}

void ActionFactory::m_initDBus()
{
    TApplication* tApp = qobject_cast<TApplication *>(qApp);
    if(tApp)
    {
        ActionFactoryAdaptor *actionFactoryAdaptor = new ActionFactoryAdaptor(this);
        actionFactoryAdaptor->setObjectName("ActionFactoryAdaptor");
        QDBusConnection connection = QDBusConnection::sessionBus();
        QString dbusPath = "/ActionFactory";
        if(!connection.registerObject(dbusPath, this))
        {
            perr("ActionFactory: failed to register object with path \"%s\":", qstoc(dbusPath));
            perr("ActionFactory: %s: %s", qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));
        }
    }
}

void ActionFactory::cleanup()
{
    QWriteLocker rLocker(&d_actionsRWLock);
    if(d_actions.size())
    {
        pinfo("action factory closing: %d actions still alive...", d_actions.size());
        foreach(TAction *a, d_actions.values())
        {
            /* post a delete event rather than directly deleting an action from another thread */
            pinfo("\e[1;31m NOT DELETING NO MORE deleting \e[0m action for %s from cleanup() in ActionFactory:\n", qstoc(a->source()));
            qDebug() << QThread::currentThread() << " <-- cleanup(): calling delete on action";
          //  delete a;
        }
    }
    else
        pok("action factory: empty, good.");
    d_actions.clear(); 
}

ActionFactory::~ActionFactory()
{
    qprintf("\e[1;31maction factory destroyed\e[0m\n");
    __actionFactory = NULL;
}

TAction *ActionFactory::actionForName(const QString &name)
{
    /* this is a read only method */
    QReadLocker rLocker(&d_actionsRWLock);
    QList<TAction* > actionsForSource = d_actions.values(name);
    TAction* retAct = NULL;
    if(actionsForSource.size() > 0)
    {
        pwarn("action factory: actionForName() returns the first action found in the list!");
        retAct = actionsForSource.first();
    }
    return retAct;
}

TAction* ActionFactory::actionConfiguredForPoint(const PointEventData* ev)
{
    /* this is a read only method */
    QReadLocker rLocker(&d_actionsRWLock); /* automatically released when out of scope */
    ////	 ////  () << QThread::currentThread() << " ActionFactory::actionConfiguredForPoint(PointEventData *a) LOCK";
    TAction *r;
    if (d_actions.contains(ev->source()))
    {
        QList<TAction* > actionsForSource = d_actions.values(ev->source());
        for(int i = 0; i < actionsForSource.size(); i++)
        {
            r = actionsForSource[i];
            if((r->desiredRefreshMode() == ev->mode()) && (r->args() == ev->args()) && (r->mode() == POLLED_REFRESH)
                && r->d_desiredAttributeProperties == ev->attributeProperties())
            {
                /* a. action in the same desired mode and in POLLED_REFRESH already configured, but with a
		    *    different polling period.
			*/
                if(r->period() != ev->period())
                {
                    if(ev->period() < r->period()) /* needs faster refresh */
                    {
                        pinfo("action factory: found an action already configured in polled mode with period %d for \"%s\"\n"
                              "  adapting it to refresh every %dms instead and reusing it", r->period(), qstoc(r->source()),
                              ev->period());
                        PollingPeriodChangeEvent *ppce = new PollingPeriodChangeEvent(ev->period());
                        qApp->postEvent(r, ppce);
                        TUtil::instance()->adjustGlobalRefreshIntervalIfNeeded(ev->period());
                    }
                    else
                        pinfo("action factory: found an action already configured in polled mode with period %d for \"%s\" \n"
                              "  (desired period: %d)", r->period(), qstoc(r->source()), ev->period());
                }
                pinfo("action factory: found an action already configured for \"%s\"  [mode: %s period %d]",
                      qstoc(r->source()), qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())), r->period());
                ////  () << QThread::currentThread() << " ActionFactory::actionConfiguredForPoint(PointEventData *a) UNLOCK - found action polled";
                 /// qprintf("\e[0;35maction found 0x%p\e[0m\n", r);
                /* unlock the mutex before returning */
                return r;
            }
            /* events: their actions have period set to 0 */
            else if((r->period() == 0) && (r->desiredRefreshMode() == AUTO_REFRESH && r->mode() == CHANGE_EVENT_REFRESH)
                && (ev->mode() == r->desiredRefreshMode()) && (r->args() == ev->args())
                     && r->d_desiredAttributeProperties == ev->attributeProperties())
                {
                pinfo("action factory: found an action already configured for \"%s\"  [desired mode: %s real: %s]",
                      qstoc(r->source()), qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())), qstoc(TUtil::instance()->modeRepresentation(r->mode())));
               /// qprintf("\e[0;35maction found 0x%p\e[0m\n", r);
                ////  () << QThread::currentThread() << " ActionFactory::actionConfiguredForPoint(PointEventData *a) UNLOCK - found action event";
                return r;
            }
            else if(r->period() == 0 && (r->desiredRefreshMode() == r->mode()) && (r->args() == ev->args()) &&
                    (ev->mode() == r->desiredRefreshMode())  && r->d_desiredAttributeProperties == ev->attributeProperties())
            {
                pinfo("action factory: found an action already configured by events for \"%s\", with the desired mode [%s]",
                      qstoc(r->source()), qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())));
                return r;
            }
            else if((r->desiredRefreshMode() == r->mode()) && (r->args() == ev->args()) &&
                    (ev->mode() == r->desiredRefreshMode()) && (ev->mode() == TARGET) &&
                    r->d_desiredAttributeProperties == ev->attributeProperties() )
            {
                pinfo("action factory: found an action already configured for the target \"%s\".", qstoc(r->source()));
                return r;
            }
        } /* for */
    }
    ////  () << QThread::currentThread() << " ActionFactory::actionConfiguredForPoint(PointEventData *a) UNLOCK - NOT found";
    return NULL; /* ReadLocker will unlock */
}

TAction* ActionFactory::create(DeviceProxy* device, const PointEventData *ev)
{
    TAction* r;
    r = new TAction(ev->source(), device, ev->type(), ev->point(), ev->args(),
                    ev->attributeProperties(), ev->attrPropDbTimeoutMillis(),
                    ev->mode(), ev->period());
    if(r->isConfigured())
    {
        /* lock for write */
        d_actionsRWLock.lockForWrite();
        ////  () << QThread::currentThread() << " ActionFactory::create(TAction *a) LOCK";
        d_actions.insert(ev->source(), r);
        d_devActions.insert(device, r);
        d_actionsRWLock.unlock();
        ////  () << QThread::currentThread() << " ActionFactory::create(TAction *a) UNLOCK";

        if(r->period())
            pok("action factory: new action created for \"%s\" [mode: desired %s | real:%s | period %d]",
                qstoc(r->source()), qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())),
                qstoc(TUtil::instance()->modeRepresentation(r->mode())), r->period());
        else
            pok("action factory: new \e[4mevent driven\e[0m action created for \"%s\" [mode: desired %s | real:%s]",
                qstoc(r->source()), qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())),
                qstoc(TUtil::instance()->modeRepresentation(r->mode())));
    }
    else
    {
        pdelete("action factory: deleting a \e[0;4mnot configured\e[0m action associated with \"%s\" [%s,%dms]", qstoc(r->source()),
                qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())), r->period());
        delete r; /* ok delete in the same thread, r created in this method */
        r = NULL;
    }
   /// qprintf("\e[0;35maction created 0x%p\e[0m\n", r);
    return r;
}

/* NOTE: be careful that this can be invoked both inside the main thread and the 
 * device thread.
 * Invoked by 
 * a. SubscriberProxy::event() when the comm. handle is destroyed before configuration event from
 *    the device thread (version 3.1);
 * b. DeviceThread destructor.
 */
void ActionFactory::destroy(TAction *act)
{
    /* actionAlive() acquires a read lock on the d_actionsRWLock */
    if(actionAlive(act->source(), act))
    {
        /* actions can be shared by readers or writers, but shared actions live in the same thread,
         * because they pertain to the same device thread.
         */
        QWriteLocker wLocker(&d_actionsRWLock);
        act->removeSubscriber();
        if (!act->numSubscribers())
        {
            pstep("action factory: removing action for source \"%s\" [desired mode: %s period: %d]", qstoc(act->source()),
                  qstoc(TUtil::instance()->modeRepresentation(act->desiredRefreshMode())), act->period());
            d_actions.remove(act->source(), act);
            d_devActions.remove(act->d_device);

            /* be careful in TAction's destructor: Device or main thread?
             * We must pay attention not to delete  the action while the
             * device thread is inside the execute() method (execute() is always
             * invoked by the device thread, while deletion, at this point, might
             * both happen in main or device thred
             */
            act->deleteLater();
            qprintf("\e[1;35mremoved action 0x%p\e[0m\n", act);
        }
        else
            qprintf("action factory: \e[0;36m%d subscribers still remaining cannot delete action for source \"%s\"\e[0m\n", act->numSubscribers(),
                    qstoc(act->source()));
    }
}

/* NOTE: be careful that this can be invoked both inside the main thread and the 
 * device thread. Currently (version 3.1) invoked by the DeviceThread.
 */
void ActionFactory::destroy(const PointEventData *ev)
{
    QWriteLocker wLocker(&d_actionsRWLock);

    QString s = ev->source();
    qprintf("\e[0;31mActionFactory::destroy(RemovePointEvent ev): source \"%s\"\e[0m\n", qstoc(s) );
    if (d_actions.contains(s))
    {
        // 			TAction *r = d_actions[s];
        TAction *r = NULL;
        QList<TAction* > actionsForSource = d_actions.values(s);
        qDebug() << QThread::currentThread() << ":" << this << "- ActionFactory::destroy(RemovePointEvent ev) <<";

        for(int i = 0; i < actionsForSource.size(); i++)
        {
            r = actionsForSource[i];
            if((r->period() == ev->period()) && (r->desiredRefreshMode() == ev->mode()) &&
               (r->args() == ev->args())) /* found an action doing this! */
            {
                r->removeSubscriber();
                if (!r->numSubscribers())
                {
                    /* remove action from the map before deleting */
                    d_actions.remove(ev->source(), r);
                    d_devActions.remove(r->d_device);
                    pstep("action factory: removed action for source \"%s\" [desired mode: %s period: %d] [actions still alive %d]", qstoc(r->source()),
			  qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())), r->period(), d_actions.count());
                    /// qprintf("\e[1;35mremoved action %p\e[0m\n", r);
                    r->deleteLater();
                }
                else
                    qprintf("\e[1;31maction factory: num subscribers: %d for source %s\e[0m\n", r->numSubscribers(), qstoc(ev->source()));
            }
            else
                qprintf("\e[1;31maction factory: REQUIRED TO REMOVE (%s,%d,%s): NOT removed action for source \"%s\" [desired mode: %s period: %d] [actions still alive %d]",
                      qstoc(ev->source()), ev->period(), qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())),
                      qstoc(r->source()),
          qstoc(TUtil::instance()->modeRepresentation(r->desiredRefreshMode())), r->period(), d_actions.count());
        }
    }
    /* QWriteLocker released when out of scope */
}

QList<TAction *> ActionFactory::actionsForDeviceProxy(Tango::DeviceProxy *dev)
{
    QReadLocker rLocker(&d_actionsRWLock);
    ////  () << QThread::currentThread() << " ActionFactory::actionsForDeviceProxy(TAction *a) LOCK";
    QList<TAction *>actions;
    if(d_devActions.contains(dev))
    {
        actions = d_devActions.values(dev);
    }
    ////  () << QThread::currentThread() << " ActionFactory::actionsForDeviceProxy(TAction *a) UNLOCK";
    return actions;
}

bool ActionFactory::actionAlive(TAction *a)
{
    bool b;
    if(!a)
        return false;
    QReadLocker rLocker(&d_actionsRWLock);
    ////  () << QThread::currentThread() << " ActionFactory::actionAlive(TAction *a) LOCK";
    b = d_actions.values().contains(a);
    ////  () << QThread::currentThread() << " ActionFactory::actionAlive(TAction *a) UNLOCK";
    return b;
}

bool ActionFactory::actionAlive(const QString& source, TAction *a )
{
    bool b;
    QReadLocker rLocker(&d_actionsRWLock);
    ////  () << QThread::currentThread() << " ActionFactory::actionAlive(QString& TAction *a) LOCK";
    b = d_actions.contains(source, a);
    ////  () << QThread::currentThread() << " ActionFactory::actionAlive(QString& TAction *a) UNLOCK";
    return b;
}

bool ActionFactory::event(QEvent *e)
{
    if(e->type() == ACTION_CONFIGURATION_CHANGED)
    {
	QReadLocker rLocker(&d_actionsRWLock);
	ActionConfigurationChangedEvent * acce = static_cast<ActionConfigurationChangedEvent *>(e);
	const TangoConfigurationParameters *cp = acce->configurationParameters();
	QList<TAction* > actionsForSource = d_actions.values(acce->source());
	foreach(TAction *a, actionsForSource)
	{
            QCoreApplication::postEvent(a, new ActionConfigurationChangedEvent(a, cp));
            pinfo("ActionFactory: distributing configuration changed to \"%s\" [%s]", qstoc(a->source()),
                  qstoc(TUtil::instance()->modeRepresentation(a->mode())));
	}
    }
    return QObject::event(e);
}

QString ActionFactory::actionAsXml(const QString& point, const QStringList& args, int desiredRefreshMode)
{
    QReadLocker wLocker(&d_actionsRWLock); /* lock for reading */
    if (d_actions.contains(point))
    {
        TAction *r = NULL;
        QList<TAction* > actionsForSource = d_actions.values(point);
        for(int i = 0; i < actionsForSource.size(); i++)
        {
            r = actionsForSource[i];
            /* actions with same refresh mode and different periods are reused:  see actionConfiguredForPoint().
             * Thus it is not necessary to look for the period.
             */
            if((r->desiredRefreshMode() == desiredRefreshMode) && (r->args() == args)) /* found! */
            {
                TActionDesc aDesc(r);
                return aDesc.actionDescription();
            }
        }
    }
    return QString();
}

QStringList ActionFactory::actionsListAsXml() const
{
    QStringList aclist;
    foreach(TAction *a, d_actions.values())
    {
        TActionDesc aDesc(a);
        aclist << aDesc.toXml();
    }
    return aclist;
}

