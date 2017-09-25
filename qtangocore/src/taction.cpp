#include <taction.h>
#include <tutil.h>

#include <QtDebug>
#include <QCoreApplication>
#include <QMetaType>
#include <QReadLocker>
#include <QWriteLocker>
#include <qv2attribute.h>
#include <unsubscriber_proxy.h>
#include <subscriber_proxy.h>
#include <communication_handle.h>
#include <com_proxy_reader.h>
#include <device_factory.h>
#include <eventconsumer.h>
#include <QDateTime>
#include "taction_polling_period_change_event.h"
#include "action_factory.h"	/* for attribute configuration change event SIGNAL/SLOT connection */
#include "action_configuration_changed_event.h"
#include "action_polling_events.h"
#include "qtangocore_read_stats.h"
#include "action_timer.h"
#include "tlog.h"
#include "targetexecutionevent.h"

TAction::TAction(const QString &s,
                 DeviceProxy *dev,
                 DataType dt,
                 const QString & pt,
                 const QStringList & args,
                 const QStringList& attrProp, int attrPropDbTimeoutMillis,
                 RefreshMode m,
                 int p) :
    d_configured(false), d_type(dt), d_source(s),
    d_point(pt), d_args(args), d_desiredAttributeProperties(attrProp), d_device(dev),
    d_eventId(-1), d_mode(m), d_desiredMode(m), d_period(p),
    d_numSubscribers(0), d_triggeredRefresh(true)
{
    qDebug() << QThread::currentThread() << ":" << this << "- TAction::TAction() <<";
    setObjectName(QString("TAction \"%1\"").arg(d_source));
    if(m != TARGET)
        ADD_READ_STAT_MSG(d_source,"Creating TAction", STATS_OBJINFO(this));
    d_devName = QString::fromStdString(dev->dev_name());
    d_timer  = NULL;
    d_autoConfiguration = NULL;
    d_eventId = -1;
    d_refreshCount = 0;
    mDbTimeoutMillis = attrPropDbTimeoutMillis; /* before startConfiguration */
    startConfiguration();
    m_targetExecutionLogEnabled = ActionFactory::actionFactory()->targetExecutionLogEnabled();
}

TAction::~TAction()
{
    qprintf("XXX deleting action %s\n", qstoc(d_source));
    /* must acquire the lock because the distructor can be invoked indefinitely by
   * the device thread (and in such a case we would not need the lock) or by the
   * main thread (from DeviceThread::lastProcessAndExit(), at shutdown.
   * Thus, we must be sure that, when deleting the object, we are not inside the
   * execute() method, which is always called in the Device thread.
   */
    // /* debug locks */ printf("TAction::~TAction() - \e[0;33mLOCK exec \"%s\" %s\e[0m \e[1;36m%p\e[0m\n", qstoc(d_source),
    //         qstoc(TUtil::instance()->modeRepresentation(desiredRefreshMode())),  this);
    QMutexLocker muLock(&d_executionMutex);
    qDebug() << QThread::currentThread() << ":" << this << "- TAction::~TAction() <<";

    if(d_timer && d_timer->isActive())
    {
        d_timer->stop();
        delete d_timer;
        d_timer = NULL;
    }
    disconnectTango();
    if(d_mode != TARGET)
        ADD_READ_STAT_MSG(d_source,"TAction Destructed", STATS_OBJINFO(this));
    delete d_autoConfiguration;
    // /* debug locks */ printf("TAction::~TAction() - \e[0;36mUNLOCK exec\e[0m\n");
}

void TAction::setWaitRefresh(bool wait)
{
    d_waitRefresh = wait;
}

void TAction::disconnectTango()
{
    pok("disconnecting \"%s\" [desired mode: %s period: %d] from Tango layer", qstoc(d_source),
        qstoc(TUtil::instance()->modeRepresentation(desiredRefreshMode())), period());
    if (d_eventId != -1)
    {
        pstep("unsubscribing \e[1;37;42m\e[4mevent\e[0m for the source \"%s\"\e[0m", qstoc(source()));
        try{
            d_device->unsubscribe_event(d_eventId);
        }
        catch(DevFailed &e)
        {
            if(!ActionFactory::actionFactory()->loggingDisabled())
            {
                TLog failLog(e);
                TUtil::instance()->addLog(failLog.toTimeErrlistPair(), failLog.formatMessage());
            }
            perr("There was an error unsubscribing the event for the source \"%s\" [%s]",
                 qstoc(d_source), qstoc(TUtil::instance()->modeRepresentation(mode())));
        }
        d_eventId = -1;
    }
}

void TAction::startPoller()
{
    ////  qDebug() << QThread::currentThread() << "CREAZIONE TIMER :" << this << "- TAction::startPoller() <<";
    d_timer = new ActionTimer(this);
    d_timer->setObjectName(QString("timer refreshing %1 every %2ms").arg(source()).arg(period()));
    connect(d_timer, SIGNAL(timeout()), this, SLOT(execute()), Qt::DirectConnection);
    connect(this, SIGNAL(stopTimer()), d_timer, SLOT(stop()), Qt::QueuedConnection);
    if (d_mode != MANUAL_REFRESH)
    {
        d_mode = POLLED_REFRESH;
        d_timer->setInterval(d_period);
        d_timer->start();
    }
}

bool TAction::timerActive()
{
    if(d_timer)
        return d_timer->isActive();
    return false;
}

QString TAction::databaseHost() const
{
    return QString::fromStdString(d_device->get_db_host());
}

int TAction::databasePort() const
{
    return d_device->get_db_port_num();
}

/* Tries to register for events if the point is an attribute and depending on the REFRESH
 * mode. If the refresh mode is set to AUTO_REFRESH, then event subscription is tried first,
 * falling back to polling if event system fails. In case events are enabled successfully, an
 * additional execute() - i.e. read_attribute - is done just to be sure that the user of the TAction
 * has had enough time to setup and receive the newData(const TVariant&) signal emitted
 * by the push_event - thing that happens immediately at event subscription time.
 */
void TAction::startConfiguration()
{
    qDebug() << "TAction::startConfiguration() " << source() << QThread::currentThread() << ":" << this;
    d_stdPoint = d_point.toStdString();
    /* by default enable wait refresh */
    d_waitRefresh = true;
    d_data.setTangoPoint(d_source);
    d_autoConfiguration = new QTangoAutoConfiguration(this, d_device,
                                                      d_source,
                                                      d_desiredAttributeProperties,
                                                      mDbTimeoutMillis);
    d_autoConfiguration->setObjectName(QString("QTangoAutoConfiguration_%1").arg(d_source));

    if (d_type == COMMAND)
    {
        try{
            ////  qDebug() << QThread::currentThread() << ":" << this << "- command configured";
            d_cmdInfo	= d_device->command_query(d_stdPoint);
            d_configured = true;
            completeConfiguration();
        }
        catch(DevFailed &e)
        {
            TLog cmdQueryFailure(e);
            QString cmdQueryFailureMsg = cmdQueryFailure.formatMessage();
            if(!ActionFactory::actionFactory()->loggingDisabled())
                TUtil::instance()->addLog(cmdQueryFailure.toTimeErrlistPair(), cmdQueryFailureMsg);
            d_configurationError = QString("TAction: failed to get command configuration for \"%1\":"
                                           "this won't work\n%2").arg(d_source).arg(cmdQueryFailureMsg);
            perr("%s", qstoc(d_configurationError));
        }
    }
    else if (d_type == ATTRIBUTE)
    {
        try{
            qDebug() << "autoConfiguration->get() " << this << QThread::currentThread();
            d_autoConfiguration->get();
            qDebug() << "autoConfiguration->get COMPLETED() " << this << QThread::currentThread();

            d_configured = true;
            completeConfiguration();
            qDebug() << "autoConfiguration->completeConfiguration COMPLETED() " << this << QThread::currentThread();
        }
        catch(DevFailed &e)
        {
            TLog log(e);
            QString formattedMsg = log.formatMessage();
            if(!ActionFactory::actionFactory()->loggingDisabled())
                TUtil::instance()->addLog(log.toTimeErrlistPair(), formattedMsg);
            d_configurationError = QString("TAction: failed to get attribute configuration for \"%1\": "
                                           "this won't work\n%2").arg(d_source).arg(formattedMsg);
            perr("%s", qstoc(d_configurationError));
        }
    }
}

void TAction::completeConfiguration()
{
    qDebug() << "TAction::completeConfiguration() " << source() << QThread::currentThread() << ":" << this;
    if(d_type == COMMAND)
    {
        if(mode() != TARGET)
        {
            /* if we are reading (mode != TARGET), initialize first value calling execute()
       * and saving it into the auto configuration object, which will be extracted
       * right after the configuration. Do this if the argout type is not void.
       */
            if(d_cmdInfo.out_type != Tango::DEV_VOID)
                d_autoConfiguration->autoConfigurationData()->setCurrentValue(execute());
            /* then start the poller to peiodiacally read the value */
            startPoller();
        }
    }
    else
    {
        RefreshMode rMode = mode();

        /* This rMode variable was introduced in QTango 4.2. In auto refresh mode we give the
         * possibility to apply a custom default refresh mode when it is defined in the
         * DEFAULT_REFRESH_MODE environment variable.
         */
        if(rMode == AUTO_REFRESH)
            rMode = ActionFactory::actionFactory()->defaultRefreshMode();

        d_stdPoint = d_point.toStdString();
        vector<string> filters;
        try {
            ////  qDebug() << QThread::currentThread() << ":" << this << "- sottoscrivo evento...";
            switch(rMode)
            {
            case AUTO_REFRESH:
                d_eventId = 0;
                d_eventId = d_device->subscribe_event(d_stdPoint, CHANGE_EVENT, this, filters, false);
                d_mode = CHANGE_EVENT_REFRESH;
                break;
            case  USER_EVENT_REFRESH:
                d_eventId = 0;
                d_eventId = d_device->subscribe_event(d_stdPoint, USER_EVENT, this, filters,  false);
                break;
            case CHANGE_EVENT_REFRESH:
                d_eventId = 0;
                d_eventId = d_device->subscribe_event(d_stdPoint, CHANGE_EVENT, this,filters,  false);
                break;
            case ARCHIVE_EVENT_REFRESH:
                d_eventId = 0;
                d_eventId = d_device->subscribe_event(d_stdPoint, ARCHIVE_EVENT, this, filters,  false);
                break;
            case PERIODIC_EVENT_REFRESH:
                d_eventId = 0;
                d_eventId = d_device->subscribe_event(d_stdPoint, PERIODIC_EVENT, this, filters,  false);
                break;
            case POLLED_REFRESH:
                throw(string("start poller"));
                break;
            case TARGET:
                throw(string("target"));
                break;
            case MANUAL_REFRESH:
                /* need to initalize first data read */
                d_data = d_autoConfiguration->autoConfigurationData()->currentValue();
                break;
            }
            ////  qDebug() << QThread::currentThread() << ":" << this << "- evento sottoscritto " << d_eventId;
            d_period = 0;
            d_configured = d_eventDriven = true;
        }
        catch (string s)
        {
            DevFailed df;
            if(s == "target")
            {

            }
            else if(s == "start poller")
            {
                /* force immediately first execution */
                d_data = d_autoConfiguration->autoConfigurationData()->currentValue();
                startPoller();
            }
        }
        catch(EventSystemFailed &e)
        {
            bool noConsolePrint = true;
            if(!ActionFactory::actionFactory()->loggingDisabled())
            {
                TLog evSysFailLog(e);
                TUtil::instance()->addLog(evSysFailLog.toTimeErrlistPair(), evSysFailLog.formatMessage(),
                                          noConsolePrint);
            }
            pinfo("events not available for \"%s\": (event system failed) starting poller after a first,"
                  " not enqueued, execution", qstoc(d_source));
            d_data = d_autoConfiguration->autoConfigurationData()->currentValue();
            d_eventId = -1;
            startPoller();
        }
        catch(DevFailed &e)
        {
            bool noConsolePrint = true;
            if(!ActionFactory::actionFactory()->loggingDisabled())
            {
                TLog devFailLog(e);
                TUtil::instance()->addLog(devFailLog.toTimeErrlistPair(), devFailLog.formatMessage(), noConsolePrint);
            }
            pinfo("events not available for \"%s\": starting poller after a first, not enqueued, execution",
                  qstoc(d_source));
            d_data = d_autoConfiguration->autoConfigurationData()->currentValue();
            d_eventId = -1;
            startPoller();
        }
    }
    connect(TUtil::instance(), SIGNAL(refreshSignal()), this, SLOT(dequeue()));
}

bool TAction::event(QEvent *e)
{
    if(e->type() == ACTION_POLLING_PERIOD_CHANGE_EVENT && static_cast<PollingPeriodChangeEvent *>(e))
    {
        PollingPeriodChangeEvent *ppce = static_cast<PollingPeriodChangeEvent *>(e);
        if(d_mode == POLLED_REFRESH)
        {
            d_timer->setInterval(ppce->newPeriod());
            pok("changed period for source \"%s\" from %d to %dms", qstoc(d_source), d_period, ppce->newPeriod());
        }
        else
            pinfo("source \"%s\" has mode \"%s\": cannot change polling period", qstoc(d_source),
                  qstoc(TUtil::instance()->modeRepresentation(d_mode)));
        /* anyway, store new period, in case of new subscription */
        d_period = ppce->newPeriod();
        e->accept();
    }
    else if(e->type() == ACTION_STOP_POLLING_EVENT && static_cast<ActionStopPollingEvent *>(e))
    {
        if(d_mode == POLLED_REFRESH)
        {
            if(d_timer->isActive())
            {
                pstep("stopping poller for source \"%s\" [mode %s (desired %s) period: %dms]",
                      qstoc(d_source), qstoc(TUtil::instance()->modeRepresentation(d_mode)),
                      qstoc(TUtil::instance()->modeRepresentation( d_desiredMode)), d_period);
                d_timer->stop();
                emit connectionStatusChanged("connected. Poller stopped");
            }
            else
                pstep("poller for source \"%s\", period %d is already stopped", qstoc(d_source), d_period);
        }
        else
            perr("source \"%s\" is not in POLLED_REFRESH mode: should not have received stop polling event",
                 qstoc(d_source));
    }
    else if(e->type() == ACTION_RESTART_POLLING_EVENT && static_cast<ActionRestartPollingEvent *>(e))
    {
        if(d_mode == POLLED_REFRESH)
        {
            if(!d_timer->isActive())
            {
                pstep("restarting polling for source \"%s\" [mode %s (desired %s) period: %dms]",
                      qstoc(d_source), qstoc(TUtil::instance()->modeRepresentation(d_mode)),
                      qstoc(TUtil::instance()->modeRepresentation( d_desiredMode)), d_period);
                /* immediately read */
                execute();
                d_timer->start();
            }
            else
                pstep("poller for source \"%s\", period %d was already running", qstoc(d_source), d_period);
        }
        else
            perr("source \"%s\" is not in POLLED_REFRESH mode: should not have received restart polling event",
                 qstoc(d_source));
    }
    else if(e->type() == ACTION_CONFIGURATION_CHANGED)
    {
        ActionConfigurationChangedEvent *acce = static_cast<ActionConfigurationChangedEvent *>(e);
        const TangoConfigurationParameters *cp = acce->configurationParameters();
        emit notifyConfigurationChangedEvent(cp);
    }


    return QObject::event(e);
}

int TAction::refreshCount()
{
    // /* debug locks */ printf("TAction::refreshCount() - \e[0;33mLOCK exec\e[0m\n");
    QMutexLocker mutexLocker(&d_executionMutex);
    int refCnt = d_refreshCount;
    // /* debug locks */ printf("TAction::refreshCount() - \e[0;36mUNLOCK exec\e[0m\n");
    return refCnt;
}

TVariant& TAction::execute(QVariant v, bool push)
{
    // /* debug locks */ printf("TAction::execute() - \e[0;33mLOCK exec and data\e[0m\n");
    qDebug() << QThread::currentThread() << ":" << this << "- TAction::execute() <<";
    QMutexLocker muLocker(&d_executionMutex);
    QWriteLocker writeLocker(&d_dataRWLock);
    time_t timestamp;
    TargetExecutionEvent *targetExecutionEvent = NULL;
    d_refreshCount++; /* increment refresh count, even if execution fails */
    if (!d_configured)
    {
        /* start locked section */
        d_data.setMessage("TAction not configured");
        d_data.setQuality(ATTR_INVALID);
        // /* debug locks */ printf("TAction::execute() - \e[0;36mUNLOCK\e[0m\n");
        /* end locked section */
        return d_data;
    }
    emit startedCall();
    try
    {
        time(&timestamp);
        qDebug() << QThread::currentThread() << ":" << this << "-" << d_source;
        if (d_type == ATTRIBUTE)
        {
            DeviceAttribute	attrargin;
            if (mode() != TARGET && (d_args.isEmpty() || (!d_args.isEmpty() && d_args.at(0).isEmpty())))
            {
                DeviceAttribute devattr	= d_device->read_attribute(d_stdPoint);
                d_data.setDeviceAttribute(devattr, d_autoConfiguration->attributeInfoEx());
                d_data.setExecutionFailed(false);
                ADD_READ_STAT_MSG(d_source, (push ? "read_attribute (not enqueued)" : "read_attribute"), STATS_OBJINFO(this));
            }
            else if(v.isNull() && mode() == TARGET)
            {
                AttributeInfoEx aiex = d_autoConfiguration->attributeInfoEx();
                attrargin = TUtil::instance()->convertToDeviceAttribute(d_args, aiex);
                d_device->write_attribute(attrargin);
                d_data = TVariant(source());
                d_data.setExecutionFailed(false);
            }
            else if(!v.isNull() && mode() == TARGET)
            {
                QVariantConverter converter(point(), d_autoConfiguration->attributeInfoEx());
                attrargin = converter.convertToAttribute(v);
                if(!attrargin.is_empty())
                {
                    d_device->write_attribute(attrargin);
                    d_data = TVariant(source());
                    d_data.setExecutionFailed(false);
                }
                else
                    perr("attribute is empty! Cannot execute write_attribute");
            }
        }
        else if(d_type == COMMAND)
        {
            /* no need to lock d_cmdInfo, d_argin, d_stdPoint: they are not concurrently modified */
            if(v.isNull())
            {
                d_argin = TUtil::instance()->convertToDeviceData(d_args, d_cmdInfo);
            }
            else
            {
                QVariantConverter converter(point(), getCommandInfo());
                d_argin = converter.convertToDeviceData(v);
                /* Trattare il caso con device data in ingresso se QVariant non e` empty */
            }
            if(d_cmdInfo.out_type != Tango::DEV_VOID)
            {
                DeviceData ddout = d_device->command_inout(d_stdPoint, d_argin);
                d_data.setDeviceData(ddout, d_cmdInfo);
                d_data.setExecutionFailed(false);
                /* force direct emit in case of commands and writers, because write action might wait for immediate
             * readback of argout.
             */
                if(d_mode == TARGET)
                    push = true;
            }
            else
            {
                d_device->command_inout(d_stdPoint, d_argin);
                d_data.setExecutionFailed(false);
            }
        }
        if(d_mode != TARGET)
        {
            emit connectionStatusChanged(QString("connected: last read on %1 [%2ms] (tango timestamp) - polling mode").
                                         arg(QDateTime::fromTime_t(d_data.timeRead().tv_sec).toString()).arg(d_data.timeRead().tv_usec/1000));
        }
        else /* target */
        {
            emit connectionStatusChanged(QString("connected: last target execution on %1").arg(QDateTime::currentDateTime().toString()));
            if(m_targetExecutionLogEnabled) /* create an event to notify execution */
                targetExecutionEvent =
                        new TargetExecutionEvent(d_source, timestamp, true, QString(), QString());
        }

    }
    catch (DevFailed &e)
    {
        TLog tgtFailLog(e);
        QString tgtFailMsg = tgtFailLog.formatMessage();

        /* -- LOCK before writing on shared d_data */
        d_data.setMessage(e.errors);
        d_data.setQuality(ATTR_INVALID);
        d_data.setExecutionFailed(true);

        if(mode() == TARGET)
        {
            if(!ActionFactory::actionFactory()->loggingDisabled())
            {
                TUtil::instance()->addLog(tgtFailLog.toTimeErrlistPair(), tgtFailMsg);
                QShowLogsEvent *sle = new QShowLogsEvent(false);
                QCoreApplication::instance()->postEvent(TUtil::instance(), sle);
            }
            emit connectionStatusChanged(QString("execution failed on %1: %2").arg(QDateTime::currentDateTime().toString()).
                                         arg(tgtFailMsg));
            if(m_targetExecutionLogEnabled)
                targetExecutionEvent =
                        new TargetExecutionEvent(d_source, timestamp, false, tgtFailMsg, QString());

        }
        else
            emit connectionStatusChanged(QString("read failed on %1: %2").arg(QDateTime::currentDateTime().toString()).
                                         arg(tgtFailMsg));
    }
    emit finishedCall();
    if(targetExecutionEvent != NULL) /* post the event to the action factory */
        QCoreApplication::instance()->postEvent(ActionFactory::actionFactory(), targetExecutionEvent);

    /* in manual refresh mode we do not want to enqueue data or emit newData. The first value read
     * is however norified in QTangoCommunicationHandle::configureHandle.
     */
    if(d_waitRefresh && !TUtil::instance()->globalRefreshDisabled() && !push && d_mode != MANUAL_REFRESH)
    {
        // 		qprintf("accodo dati per \e[1;33m\"%s\"\e[0m : wait refresj azione: %d TUtil globale enabled: %d\n",
        // 			qstoc(d_source),  d_waitRefresh, !TUtil::instance()->globalRefreshDisabled());

        d_queueMutex.lock();
        dataQueue.enqueue(d_data);
        d_queueMutex.unlock();
    }
    else if(d_mode != MANUAL_REFRESH)
    {
        // 	  qprintf("\e[0;32mEmetto il segnale di newData \e[4mdirettamente\e[0m: \e[1;33m\"%s\"\e[0m: wait refresh azione: %d TUtil globale enabled: %d\n", qstoc(d_source), d_waitRefresh, !TUtil::instance()->globalRefreshDisabled());
        emit newData(d_data);
    }
    qDebug() << QThread::currentThread() << ":" << this << "- TAction::execute() >>";
    // /* debug locks */ printf("TAction::execute() - \e[0;36mUNLOCK\e[0m\n");
    return d_data;
}

TVariant TAction::data()
{
    return lastValueRead();
}

void TAction::dequeue()
{
    QMutexLocker locker(&d_queueMutex);
    //     ADD_READ_STAT_MSG(QString("%1").arg(d_source), "dequeue(): TAction emits newData()");
    while(!dataQueue.isEmpty())
        emit newData(dataQueue.dequeue());
}

TVariant TAction::lastValueRead()
{
    // /* debug locks */ printf("TAction::push_event() - \e[0;33mLOCK data\e[0m\n");
    QReadLocker rlocker(&d_dataRWLock);
    // /* debug locks */ printf("TAction::push_event() - \e[0;36mUNLOCK data\e[0m\n");
    return d_data;
}

void TAction::push_event(EventData* data)
{
    if(d_eventId < 0)
    {
        perr("TAction::push_event(): invoked when event id = %d! [%s]", d_eventId, qstoc(d_source));
        return;
    }
    // /* debug locks */ printf("TAction::push_event() - \e[0;33mLOCK exec\e[0m\n");
    QMutexLocker muLocker(&d_executionMutex);
    bool byEvent = true;
    /* increment refresh count for the action */
    d_refreshCount++;
    if(TUtil::instance()->shuttingDown())
        pinfo("TAction:: push_event: last events for %s", qstoc(d_source));

    //    qDebug() << QThread::currentThread() << ":" << this << "- push_event() <<" << QString::fromStdString(data->attr_name);
    if (!data->err && d_device)
    {
        data->attr_name.erase(0, d_device->dev_name().size()+1);
        d_data.setDeviceAttribute(*(data->attr_value), d_autoConfiguration->attributeInfoEx(), byEvent);
        ADD_READ_STAT_EVENT_MSG(d_source, "TAction push_event()", STATS_OBJINFO(this));

        /* the DeviceAttribute should be deleted if coming from events (manual)...
          * but not here...
          */
        emit connectionStatusChanged(QString("connected: last read on %1 [%2ms] (tango timestamp) - event no %3").
                                     arg(QDateTime::fromTime_t(d_data.timeRead().tv_sec).toString()).arg(d_data.timeRead().tv_usec/1000)
                                     .arg(d_refreshCount));
        d_data.setExecutionFailed(false);

    }
    else
    {
        d_data.setMessage(data->errors);
        d_data.setQuality(ATTR_INVALID);
        perr("error extracting data from the event. Source: \"%s\" [mode: %s]\n%s",
             qstoc(d_source), qstoc(TUtil::instance()->modeRepresentation(d_mode)),
             qstoc(d_data.message()));
        emit connectionStatusChanged(QString("event update failed on %1").arg(QDateTime::currentDateTime().toString()));
        d_data.setExecutionFailed(true);
    }
    /* directly emit(), do not enqueue with events */
    emit newData(d_data);
    ////  qDebug() << QThread::currentThread() << ":" << this << "- push_event() >>";
    // /* debug locks */ printf("TAction::push_event() - \e[0;36m UN LOCK exec\e[0m\n");
}

void TAction::executeAsynch(QVariant v)
{
    qRegisterMetaType<QVariant> ("QVariant");
    metaObject()->invokeMethod(this, "execute", Qt::QueuedConnection, Q_ARG(QVariant, v));
}

void TAction::addSubscriber()
{
    d_numSubscribers++;
    //printf("\e[1;33mincrementato num subscribers per %s: %d\e[0m\n", qstoc(d_source), d_numSubscribers);
    if (d_numSubscribers > 0 && d_timer != NULL && !d_timer->isActive() &&
            d_mode != MANUAL_REFRESH && d_mode != TARGET)
        d_timer->start();
}

void TAction::removeSubscriber()
{
    d_numSubscribers--;
    /* timer is active only if the RefreshMode is something different
     * from MANUAL_REFRESH or TARGET.
    */
    if (d_numSubscribers == 0 && d_timer != NULL && d_timer->isActive())
        d_timer->stop();
}

AttributeInfoEx TAction::getAttributeInfo()
{
    AttributeInfoEx aiex;
    if(d_autoConfiguration)
        aiex = d_autoConfiguration->attributeInfoEx();
    return aiex;
}

bool TAction::operator==(const TAction &other) const
{
    return source() == other.source() && mode() == other.mode() &&
            period() == other.period() && args() == other.args();
}

/** \brief returns true if the event consumer for our event id is managed by ZMQ, false otherwise
 *
 * @return true: the source is event driven and it is based on ZMQ
 * @return false: the source is polled, or tango version is less than 8 or the
 *         event system is based on Notifd.
 */
bool TAction::isZmq() const
{
#if TANGO_VERSION_MAJOR >= 8
    if(d_eventId != -1)
    {
        Tango::ZmqEventConsumer    *consumer = Tango::ApiUtil::instance()->get_zmq_event_consumer();
        return (consumer->get_event_system_for_event_id(d_eventId) == Tango::ZMQ);
    }
#endif
    return false;
}

TActionList::TActionList(const QStringList &ss, 
                         const QStringList& attributeProperties,
                         RefreshMode mode,
                         const QString& assocObjectName,
                         int period,
                         QObject *par) : QObject(par)
{
    d_assocObjName = assocObjectName;
    foreach (QString s, ss)
        my_sources << s;
    foreach (QString s, ss)
    {
        SubscriberProxy *subscriber = new SubscriberProxy(s, mode, false, period, d_assocObjName);
        if(attributeProperties.size())
            subscriber->setDesiredAttributeProperties(attributeProperties);

        d_subscriberProxiesMap.insert(s, subscriber);
        /* when a subscriber is destroyed, it will be removed from the map */
        connect(subscriber, SIGNAL(destroyed(QObject *)), this, SLOT(subscriberDestroyed(QObject *)));

        /*  subscribers that will succeed in subscribe() will send signals, the other that fail will never */
        connect(subscriber, SIGNAL(configured(TAction*)), this, SLOT(configure(TAction*)));
        connect(subscriber, SIGNAL(configurationErrorMessage(const QString&)), this, SIGNAL(connectionStatusChanged(const QString &)));
        connect(subscriber, SIGNAL(configurationErrorMessage(const QString&)), this, SIGNAL(connectionError(const QString &)));
        connect(subscriber, SIGNAL(connectionOk(bool)), this, SIGNAL(connectionOk(bool)));
        connect(subscriber, SIGNAL(connectionFailed()), this, SIGNAL(connectionFailed()));
        try{
            subscriber->subscribe();
        }
        catch(DevFailed &e)
        {
            perr("invalidating action for the source \"%s\" [mode %s period %d] for the action list \"%s\"",
                 qstoc(s), qstoc(TUtil::instance()->modeRepresentation(mode)),
                 period, objinfo(parent()));
            Except::re_throw_exception(e, "Error creating a new TActionList object",
                                       "Error creating TActionList for \n\"" + string(qstoc(ss.join(", "))) + "\" because one component,\n"
                                                                                                              "named \""+ string(qstoc(s)) + "\" failed to subscribe",
                                       "TActionList::TActionList() [subscriber->subscribe() failed]", Tango::ERR);
        }
    }
}

TActionList::~TActionList()
{
    pinfo("action list deletion: remember to call disconnectTango() on every element first!");
    /* we do not want any pending SIGNAL to be delivered to ourselves, since ActionList's actions are going to
     * be deleted in another thread. configured(TAction*) signals might be delivered after TAction is destroyed
     * in the device thread!
     */
    foreach(SubscriberProxy *sProxy, d_subscriberProxiesMap.values())
    {
        sProxy->disconnect();
        sProxy->stopAutoReconnectTimer();
    }
    foreach(TAction* a, d_actions)
    {
        if(ActionFactory::actionFactory()->actionAlive(a) && a->isConfigured())
        {
            a->disconnect();
            if(a != NULL && a->isConfigured())
            {
                qprintf("QTangoCommunicationHandle::unsetSources(): \e[0;32maction associated to object \"%s\" - target \"%s\"STILL alive\e[0m\n",  objinfo(parent()),
                        qstoc(a->source()));
                UnsubscriberProxy unsubscriber(a->source(),  a->desiredRefreshMode(), a->period());
                unsubscriber.unsubscribe();
            }
            else if(a != NULL)
            {
                perr("QTangoCommunicationHandle::unsetSources(): the action \"%s\"", qstoc(a->source()));
                perr("was not correctly configured. A possible cause could be a subscription for an");
                perr("attribute or a command not really exisiting with the name specified.");
                perr("Hint: restart the program and look for error messages during the subscription phase");
                perr("The object information is the following: {%s}.", objinfo(parent()));
            }
        }
        else
        {
            pinfononl("unset sources: action associated to object \"%s\" no more alive",  objinfo(parent()));
            qslisttoc(d_sources);
        }
    }
}

void TActionList::subscriberDestroyed(QObject *o)
{
    int nProxies = d_subscriberProxiesMap.remove(o->objectName());
    pdelete("deleting %d subscriber proxy for source \"%s\" in action list {\"%s\"}",
            nProxies, qstoc(o->objectName()), qstoc(my_sources.join("\",\"")));
}

void TActionList::configure(TAction *a)
{
    //       printf("TActionList::configure  ->%s:\n", qstoc(my_sources.join(",")));
    //      printf("\e[1;33m  TARGET: \e[0m \"%s\" pointer %p\n", qstoc(a->source()), a);

    if (ACTION_ALIVE(a) && d_sources.contains(a->source()))
        ;////  qDebug() << QThread::currentThread() << ":" << this << "- gia` configurato";
    else if(ACTION_ALIVE(a)  && !my_sources.contains(a->source()))
    {
        perr("\e[0;4m%s\e[0m\"\e[0m is not one of my targets or sources: not configuring",
             qstoc(a->source()));
    }
    else if(ACTION_ALIVE(a)  && a->isConfigured())
    {
        d_sources << a->source();
        d_actions << a;
        if(a == NULL)
            return;
        ////  qDebug() << "TActionList::configuring " << a->source() << QThread::currentThread() << ":" << this << "- connetto";
        connect(a, SIGNAL(newData(const TVariant&)), this, SLOT(newDataForAction(const TVariant&)), Qt::QueuedConnection);
        connect(a, SIGNAL(connectionStatusChanged(const QString&)), this, SIGNAL(connectionStatusChanged(const QString&)));

        /* configuration change event is valid only for the last target. Actually, it works only if one target
         * has been configured (and makes sense only in that case.
         */
        if(d_sources.size() == my_sources.size()) /* last target has been configured */
        {
            emit actionListConfigured(true);
            connect(a, SIGNAL(notifyConfigurationChangedEvent(const TangoConfigurationParameters *)),
                    this, SIGNAL(notifyLastAttributeConfigurationChangedEvent(const TangoConfigurationParameters *)));
        }

        emit connectionOk(true);
        emit connectionStatusChanged(QString("Targets \"%1\" correctly configured").arg(d_sources.join("; ")));
        /* connect() above is not called in time to receive the first signal */
        //     printf("\e[0;36mnotifying config for action %p target \"%s\e[0m\n", a, qstoc(a->source()));
        emit notifyLastAttributeConfigurationChangedEvent(a->autoConfiguration()->autoConfigurationData());
    }
    else if(ACTION_ALIVE(a) && !a->isConfigured())
    {
        QString error =  QString("TActionList: configuration failed for \"%1\", object \"%2\":\n%3").arg(a->source()).arg(objinfo(parent())).
                arg(a->configurationError());
        emit connectionOk(false);
        emit connectionFailed();
        emit connectionError(error);
        emit connectionStatusChanged(error);
    }

    if(qobject_cast<SubscriberProxy*>(sender()))
    {
        if(ACTION_ALIVE(a))
            pdelete("deleting (action list) subscriber proxy for %s", qstoc(a->source()));
        else
            pdelete("deleting subscriber for an action dead. Action list: %s", qstoc(d_sources.join(",")));

        sender()->deleteLater();
    }
    else
        qprintf("\e[1;31mx \e[0m TACtionList:: configure(): cannot cast subscriber proxy for %s\n", a->source().toStdString().c_str());
    //    if(d_sources.size() == my_sources.size())
    //        emit actionListConfigured(true);
}

QList<TVariant> TActionList::execute(QVariant v, bool dontStopOnErrors)
{
    ////  qDebug() << QThread::currentThread() << ":" << this << "- execute() <<" ;
    QList<TVariant> temp;
    int i = 0;
    emit started();
    foreach (TAction *a, d_actions)
    {
        TVariant ret;
        if(a != NULL)
        {
            ret = a->execute(v);
            temp << ret;
            if(ret.executionFailed() && !dontStopOnErrors)
            {
                pinfo("execution of targets \e[1;31mterminates\e[0m after the execution failure of target \"%s\"", qstoc(a->source()));
                emit finished(d_actions.size()); /* to restore style */
                break;
            }
            else if(ret.executionFailed() && dontStopOnErrors)
                pinfo("execution of targets \e[4mcontinues\e[0m after the execution \e[1;31mfailure\e[0m of target \"%s\"", qstoc(a->source()));
            emit finished(++i);
        }
        else
            perr("TActionList::execute(QVariant v): an action in the TAction list is null!");
    }
    if(!d_actions.size())
        perr("TActionList::execute(): no actions configured. Did not execute (\"%s\")", qstoc(d_sources.join(";")));
    emit finishedAll();
    ////  qDebug() << QThread::currentThread() << ":" << this << "- execute() >>";
    return temp;
}

void TActionList::executeAsynch(QVariant v)
{
    ////  qDebug() << QThread::currentThread() << ":" << this << "- executeAsynch() <<";
    /* do not emit started, finished, finishedAll signals here, since execution takes place
     * in another thread.
     */
    foreach (TAction *a, d_actions)
    {
        if(a != NULL)
        {
            a->executeAsynch(v);
        }
        else
            perr("TActionList::executeAsynch(): an action in the TAction list is null!");
    }
    ////  qDebug() << QThread::currentThread() << ":" << this << "- executeAsynch() >>";
}
