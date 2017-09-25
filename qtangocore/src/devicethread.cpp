#include <devicethread.h>
#include <tutil.h>
#include <subscriber_proxy.h>
#include <QCoreApplication>
#include "action_creation_event.h"
#include "action_configuration_failed_event.h"
#include "device_thread_factory.h"
#include "device_factory.h"
#include "qtangocore_read_stats.h"
#include <QtDebug>

/* connects to the device proxy with the given name. If connection succeeds, then the thread
 * registers itself  in the TUtil thread map.
 */
DeviceThread::DeviceThread(const QString &name) : d_devname(name), d_device(0), d_timer(0)
{
    setObjectName(QString("DeviceThread \"%1\"").arg(name));
    /* introduced in 4.2.1 to customize the stack size in particular cases where
         * it is necessary.
         */
    setStackSize(DeviceThreadFactory::instance()->stackSize());

    ADD_READ_STAT_MSG(QString("Dev.Thr. %1").arg(d_devname), "Creating", STATS_OBJINFO(this));
    d_timer = d_exitTimer = NULL;
    try
    {
        /* DeviceFactory::create() returns an already existing proxy or creates a new one.
                 * If `new DeviceProxy()' in create() throws an exception, the device pointer is deleted.
                 */
        d_device = DeviceFactory::instance()->create(d_devname);
        DeviceThreadFactory::instance()->registerThread(name, this);
    }
    catch(DevFailed &e)
    {
        /* do not TUtil::log() here: since we re-throw, let the last catcher log all */
        d_device = NULL;
        Except::re_throw_exception(e, "Error creating a new DeviceThread object",
                                   "Error creating a device thread for \n\"" + d_devname.toStdString() + "\"",
                                   "DeviceThread::DeviceThread(const QString &name)", Tango::ERR);
    }
}

/* 
 * DeviceThread is created without a QObject as parent. This means it has to be explicitly
 * deleted. Before deleting, associated actions must be destroyed. The DeviceThread destructor
 * does not delete its actions. Just removes itself from the DeviceThreadFactory.
 */
DeviceThread::~DeviceThread()
{
    qDebug() << QThread::currentThread() << ":" << this << "- ~DeviceThread() << ";
    qprintf("\e[0;33mDeviceThread DISTRUTTORE!\e[0m\n");
    DeviceThreadFactory::instance()->deregisterThread(this);
    if(d_timer)
        delete d_timer;
    if(d_exitTimer)
        delete d_exitTimer;
    ADD_READ_STAT_MSG(QString("Dev.Thr. %1").arg(d_devname), "Destructed", STATS_OBJINFO(this));
}

void DeviceThread::lastProcessAndExit()
{
    pstepl2("device thread is checking if other events are pending before exiting...");
    d_toBeAdded.clear(); /* do not want to add anything! if going to exit! */
    QList<TAction *> actions = ActionFactory::actionFactory()->actionsForDeviceProxy(d_device);
    qDebug() << QThread::currentThread() << ":" << this << "- lastProcessAndExit() -> DELETING ACTIONS <<";
    foreach(TAction* a, actions)
    {
        qprintf("\e[1;31mlastProcessAndExit: deleting %s %d %s\e[0m\n", qstoc(a->source()), a->period(), qstoc(TUtil::instance()->modeRepresentation(a->mode())));
        ActionFactory::actionFactory()->destroy(a);
    }
  //  process();
    quit();
}

void DeviceThread::run()
{
    omni_thread::ensure_self se;
    qDebug() << QThread::currentThread() << ":" << this << "- run() << ";
    if (d_device != NULL)
    {
        /* timer must be initialized with 0 as parent because cannot create children for a parent living in a different thread */
        d_timer = new QTimer(0); /* created in the thread - we are inside run() - */
        /* connect this and d_timer through QueuedConnection since d_timer has been created inside the thread - in run() - */
        connect(this, SIGNAL(modifiedPointList()), d_timer, SIGNAL(timeout()), Qt::QueuedConnection);
        /* d_timer lives in the thread DeviceThread (created in run() ), signals emitted by d_timer are directly connected because
         * process() is executed in d_timer thread - DeviceThread - */
        connect(d_timer, SIGNAL(timeout()), this, SLOT(process()), Qt::DirectConnection);

        /* create a timer living in the DeviceThread -  here inside  run() - */
        d_exitTimer = new QTimer(0);
        /* exitLoopThread is a SIGNAL emitted by `bool DeviceThread::exitLoop()' , called from the main thread
         * when TUtil is destroyed. So we need QueuedConnection here, since exitTimer lives in the DeviceThread.
         * The principle is the same as above: connect a signal from the main thread to a slot living in the device thread.
         */
        connect(this, SIGNAL(exitLoopThread()), d_exitTimer, SIGNAL(timeout()), Qt::QueuedConnection);
        /* now directly connect the exitTimer (living in DeviceThread) to the lastProcessAndExit slot */
        connect(d_exitTimer, SIGNAL(timeout()), this, SLOT(lastProcessAndExit()), Qt::DirectConnection);

        exec();
        d_timer->stop();
        d_exitTimer->stop();
        qDebug() << QThread::currentThread() << ":" << this << "- main loop finished";
    }
    qDebug() << QThread::currentThread() << ":" << this << "- run() >> ";
}


bool DeviceThread::exitLoop(ExitLoopEvent )
{
    emit exitLoopThread();
    return true;
}

bool DeviceThread::addPoint(PointEventData* e)
{
    /* creo il poller */
    qDebug() << QThread::currentThread() << ":" << this << "- addPoint() <<";
    d_pointEventMutex.lock();
    d_toBeAdded << e;
    while (d_timer == 0)
    {
        usleep(1000);
    }
    d_pointEventMutex.unlock();
    emit modifiedPointList();
    qDebug() << QThread::currentThread() << ":" << this << "- addPoint() >>";
    return true;
}

bool DeviceThread::removePoint(PointEventData *e)
{
    /* distruggo il poller */
    qDebug() << QThread::currentThread() << ":" << this << "- removePoint()";
    d_pointEventMutex.lock();
    d_toBeRemoved << e;
    d_pointEventMutex.unlock();
    while (d_timer == 0)
        usleep(1000);
    emit modifiedPointList();
    return true;
}

void DeviceThread::process()
{
    QList<PointEventData*> addPointListLocalCopy, removePointListLocalCopy;
    /* hold the lock as shortly as possible: make a local copy of the shared variables */
    d_pointEventMutex.lock();
    addPointListLocalCopy = d_toBeAdded;
    removePointListLocalCopy = d_toBeRemoved;
    d_toBeAdded.clear();
    d_toBeRemoved.clear();
    d_pointEventMutex.unlock();

    TAction *r = NULL;
    ActionFactory *factory;
    qDebug() << QThread::currentThread() << ":" << this << "- process() <<" << /*d_toBeAdded.size() <<*/ d_toBeRemoved.size();

    factory = ActionFactory::actionFactory();

    ///	foreach (PointEventData *ev, d_toBeAdded)
    foreach (PointEventData *ev, addPointListLocalCopy)
    {
        /* see if an already configured action exists in the factory */
        r = factory->actionConfiguredForPoint(ev);
        if(r == NULL && d_device != NULL) /* if not, create one */
        {
            r = factory->create(d_device, ev);
        }
        if(r != NULL) /* an action failed to configure itself and the factory did not store it */
        {
            r->addSubscriber(); /* the action, new or already existing, has one more subscriber */
            /* tell the subscriber proxy that a new action is available for the source .
          * The subscriber will then notify its communication handle that the action is ready to
          * provide data. Just keep in mind that we are in a different thread with respect to the
          * main thread that creates the communicationHandle and the SubscriberProxy.
          * So we choose to post an event , thread safe.
          */
            ActionCreationEvent *ace = new ActionCreationEvent(r);
            QCoreApplication::instance()->postEvent(ev->subscriber(), ace);
        }
        else
        {
            ActionConfigurationFailedEvent *aConfFailed = new ActionConfigurationFailedEvent(ev->source());
            QCoreApplication::instance()->postEvent(ev->subscriber(), aConfFailed);
        }
        delete ev;
        ev = NULL;
    }
    ///	d_toBeAdded.clear();

    ///	foreach (PointEventData *ev, d_toBeRemoved)
    foreach (PointEventData *ev, removePointListLocalCopy)
    {
        factory->destroy(ev);
        delete ev;
        ev = NULL;
    }
    ///	d_toBeRemoved.clear();
    qDebug() << QThread::currentThread() << ":" << this << "- process() >>";
    ///	d_pointEventMutex.unlock();
}
