/* $Id: tutil.cpp,v 1.87 2014-08-26 14:55:11 giacomo Exp $ */
#include <tutil.h>
#include <tlogdialog.h>
#include <tapplication.h>

#include <QVariant>
#include <QStringList>
#include <QObjectList>
#include <QApplication>
#include <QMessageBox>
#include <QMutexLocker>
#include <QFile>
#include <QKeyEvent>
#include <QSplashScreen>
#include <tvariant.h>
#include <devicethread.h>
#include <taction.h>
#include <subscriber_proxy.h>
#include <simple_dataproxy.h>
#include <device_thread_factory.h>
#include <device_factory.h>
#include <global_refresh_timer.h>
#include <qtangocore_read_stats.h>
#include <QtDebug>
#include <QDateTime>
#include <QSettings>

/* warn just once inside old replaceWIldCards */
bool warnOldReplaceWildcards; /// to remove

TUtil::TUtil() : QObject(qApp), d_ancestor(0)
{
    setObjectName("TUtil");
    thread()->setObjectName("Main Thread");
    /* instantiate statistic infrastructure */
    INIT_STATS();
    ADD_READ_STAT_MSG("TUtil", "Constructor", STATS_OBJINFO(this));
    int globalTriggerStartDelay = 0, globalRefreshInterval;
    QStringList args;
    logDialog = NULL;

    /* warn if we are still using QApplication instead of TApplication */
    if(!dynamic_cast<TApplication *>(qApp))
    {
        printf("\e[1;33m*\e[0m \e[4mwarning\e[0m: you are still using "
               "\e[4mQApplication\e[0m: please upgrade to \e[4mTApplication\e[0m instead.\n"
               "           It is enough to substitute QApplication with TApplication in main.cpp\n");
    }
    /* allocate action Factory
 */
    if(getenv("QTANGO_PRINT") != NULL)
    {
        pok("\e[1;4;32mwelcome\e[0m! This is qtango version %s. mailto:giacomo.strangolino@elettra.trieste.it", QTANGO_VERSION_STR);
        pinfo("QTANGO_PRINT environment variable is set: printing qtango3 coloured messages...");
        pstep("should you like to disable this feature, simply remove \"QTANGO_PRINT\" environment variable before"
              "\n   executing your qtango3 application.");
    }

    /* QCoreApplication initialization with organization and application name */
    args = qApp->arguments();
    if(QCoreApplication::organizationName() == QString())
        QCoreApplication::setOrganizationName("Elettra");
    if(QCoreApplication::applicationName() == QString())
    {
        if(args.size() && args.at(0).contains('/') && args.at(0).split("/", QString::SkipEmptyParts).size() > 1)
            QCoreApplication::setApplicationName(args.at(0).split("/", QString::SkipEmptyParts).last());
        else if(args.size())
            QCoreApplication::setApplicationName(args.at(0));
    }

    QSettings s;
    globalTriggerStartDelay = s.value("GLOBAL_REFRESH_TIMER_START_DELAY", 0).toInt();
    if(args.contains("--set-trigger-start-delay") && args.size() > args.indexOf("--set-trigger-start-delay") + 1)
    {
        globalTriggerStartDelay = args.at(args.indexOf("--set-trigger-start-delay") + 1).toInt();
        pok("setting and saving global trigger start delay to %dms", globalTriggerStartDelay);
        s.setValue("GLOBAL_REFRESH_TIMER_START_DELAY", globalTriggerStartDelay);
    }
    else if(args.contains("--set-trigger-start-delay") && args.size() <= args.indexOf("--set-trigger-start-delay") + 1)
    {
        perr("command line argument \"--set-trigger-start-delay\" requires an integer (milliseconds)");
        perr("not setting/changing the global trigger start delay, leaving it to %d ms", globalTriggerStartDelay);
    }

    char *globalRefreshDisabledByEnv = getenv("QTANGO_GLOBAL_REFRESH_DISABLED");
    char *globalRefreshSuggestedInterval = getenv("QTANGO_GLOBAL_REFRESH_SUGGESTED_INTERVAL");

    if(globalRefreshSuggestedInterval != NULL && atoi(globalRefreshSuggestedInterval) > 10)
    {
        globalRefreshInterval = atoi(globalRefreshSuggestedInterval) * GLOBAL_REFRESH_FACTOR_ADJUST;
        pinfo("tutil: global refresh interval has been set to %dms according to the environment variable",
              atoi(globalRefreshSuggestedInterval));
        pinfo("\"QTANGO_GLOBAL_REFRESH_SUGGESTED_INTERVAL\"");
    }
    else if(globalRefreshSuggestedInterval != NULL)
    {
        perr("tutil: invalid value for the environment variable \"QTANGO_GLOBAL_REFRESH_SUGGESTED_INTERVAL\": %s",
             globalRefreshSuggestedInterval);
        perr("accepted values must greater than 10 (milliseconds)");
    }
    else
        globalRefreshInterval = 1000 * GLOBAL_REFRESH_FACTOR_ADJUST;

    if(globalRefreshDisabledByEnv)
    {
        d_globalRefreshDisabled = true;
        pinfo("tutil: global refresh has been disabled by the \"QTANGO_GLOBAL_REFRESH_DISABLED\" environment variable");
    }
    else
        d_globalRefreshDisabled = false;

    d_shuttingDown = false;
    // 	QTimer *d_refreshTimer = new QTimer();
    // 	d_refreshTimer->setInterval(globalRefreshInterval);
    // 	connect(d_refreshTimer, SIGNAL(timeout()), this, SLOT(triggerRefresh()), Qt::DirectConnection);
    // 	d_refreshTimer->start();

    qDebug() << "TUTIL creo GlobalRefreshExternalTimer " << this << QThread::currentThread();
    /* create external refresh trigger and connect its timeout() signal go my triggerRefresh method */
    d_externalTimer = new GlobalRefreshExternalTimer(this, globalRefreshInterval);
    connect(d_externalTimer, SIGNAL(timeout()), this, SLOT(triggerRefresh()), Qt::QueuedConnection);
    /* external refresh trigger might start with a delay determined by QSettings saved GLOBAL_REFRESH_TIMER_START_DELAY */
    QTimer *globalTriggerStartTimer = new QTimer(this);
    globalTriggerStartTimer->setSingleShot(true);
    globalTriggerStartTimer->setInterval(globalTriggerStartDelay);
    connect(globalTriggerStartTimer, SIGNAL(timeout()), d_externalTimer, SLOT(start()));
    ADD_READ_STAT_MSG("TUtil", "timer startup", STATS_OBJINFO(this));

    if(!d_globalRefreshDisabled)
        globalTriggerStartTimer->start();

    /* create here action and thread factories, so they are deleted inside TUtil destructor */
    ActionFactory::actionFactory();
    DeviceThreadFactory::instance();
    DeviceFactory::instance(this);

    qRegisterMetaType<TVariant>("TVariant");
    qRegisterMetaType<RefreshMode>("RefreshMode");

    char *loc;
    loc = setlocale(LC_ALL, "");
    if (!loc) printf("setlocale(LC_ALL) failed.\n");
    warnOldReplaceWildcards = true;
    /* when the QCoreApplication is about to quit, do some cleanup on the device threads */
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanup()));
}

/* this is invoked when the QCoreApplication is about to quit. The slot performs last second cleanup.
 * It includes suggesting to each device thread to exit from their event loop. The TUtil destructor, then,
 * will wait for each thread and delete it.
 */
void TUtil::cleanup()
{
    QList<DeviceThread *>threads = DeviceThreadFactory::instance()->threadList() ;
    if(!threads.size())
    {
        pok("no threads to wait for remaining, cleanup not necessary");
    }
    else
    {
        // 		QPixmap pix(350,120);
        // 		pix.fill(qApp->palette().color(QPalette::Window));
        // 		QSplashScreen splash(pix);
        // 		QFont f = splash.font();
        // 		f.setPointSize(12);
        // 		splash.setFont(f);
        // 		splash.showMessage("Performing cleanup operations,\nplease wait...", Qt::AlignCenter);
        // 		splash.show();
        // 		qApp->processEvents();
        pinfo("performing cleanup operations on threads...");
        /* request the list of threads to the DeviceThreadFactory */

        foreach(DeviceThread *thread, threads)
        {
            if(thread)
            {
                ExitLoopEvent ele(EXITLOOPEVENT);
                // 				splash.showMessage(QString("waiting for device thread representing\n\"%1\"...").arg(thread->devName()), Qt::AlignCenter);
                // 				qApp->processEvents();
                pstep("waiting for device thread representing \"%s\"...", qstoc(thread->devName()));
                thread->exitLoop(ele);
                if(!thread->wait(/*6000*/))
                {
                    perr("the thread associated to the device \"%s\" did not exit in time [in 6 seconds]", qstoc(thread->devName()));
                    perr("this might be dued to slow responding servers, especially if the \e[1;4msetup phase\e[0m");
                    perr("has not been \e[1;4mcompleted\e[0m and lead to \e[4;31mshutdown crashes\e[0m.");
                    /* yes, QMessageBox this! */
                    // 					QMessageBox::critical(0, "Error - thread did not exit",
                    // 						QString("The thread associated to the device \"%1\" did not exit in time [in three seconds]").arg(thread->devName()));
                }
                delete thread;
            }
        }
    }
}

/* wait for all threads to complete their execution */
TUtil::~TUtil() 
{
    ADD_READ_STAT_MSG("TUtil", "destruction begins: waiting for threads", STATS_OBJINFO(this));
    pinfo("stopping external timer....");
    d_externalTimer->quit();
    d_externalTimer->wait();
    pinfo("wating for device threads");
    /* try to correctly leave */
    d_shuttingDown = true;
    // 	printf("\e[1;32mShutting down the ORB (to clean up event channel)\n\e[0m");
    // 	client_leavefunc();
    ADD_READ_STAT_MSG("TUtil", "destruction completed",  STATS_OBJINFO(this));
    SAVE_STATS(); /* if enabled, this will save the statistics gathered during execution */
    /* then destruction will follow for the devices in the device factory */
}

void TUtil::setLoggingTarget(QString target)
{
    char *env = getenv("QTANGO_LOG");
    if (env == NULL)
    {
        cout << "QTANGO_LOG not set, defaulting to stdout" << endl;
        loggingTarget = "";
    }
    else
    {
#ifdef WIN32
        loggingTarget = QString(env) + "\\" + target + ".log";
#else
        if(target.contains("/") && target.split("/", QString::SkipEmptyParts).size() > 1)
            loggingTarget = target.split("/", QString::SkipEmptyParts).last();
        printf("Logging target \"%s\"\n", qstoc(loggingTarget));
        loggingTarget = QString(env) + "/" + loggingTarget + ".log";
        printf("Logging target \"%s\"\n", qstoc(loggingTarget));
#endif
    }
    printf("Logging target \"%s\"\n", qstoc(loggingTarget));
    qDebug() << loggingTarget;
}

TUtil* TUtil::_instance = 0;

void TUtil::addLog(const QPair<time_t, DevErrorList> &logInfo, const QString &formattedMessage, bool noConsole)
{
    /* protect agains concurrent threads trying to log */
    d_logMutex.lock();
    logs.push_back(logInfo);

    if (logs.size() == MAX_LOGS)
        logs.pop_front();

    if (loggingTarget == "" && !noConsole)
        cout << formattedMessage.toStdString() << endl;
    else if(loggingTarget != "")
    {
        TLog log;
        QFile logFile(loggingTarget);
        logFile.open(QIODevice::Append);
        logFile.write(QByteArray().append(log.formatMessage(logInfo) + "\n"));
        logFile.close();
    }
    /* release log mutex */
    d_logMutex.unlock();
}

void TUtil::addLog(TLog& log)
{
    QPair<time_t, DevErrorList> timeListPair = log.toTimeErrlistPair();
    QString formattedMessage = log.formatMessage();
    addLog(timeListPair, formattedMessage);
}

void TUtil::clearLogs()
{
    /* protect against changes coming from concurrent threads while parsing the list */
    d_logMutex.lock();
    logs.clear();
    d_logMutex.unlock();
}

TUtil* TUtil::instance()
{
    if (_instance == NULL)
        _instance = new TUtil();
    return _instance;
}

DataType TUtil::parseName(const QString &fullName, QString &devName, QString &point, QStringList &args, bool &dbase)
{
    bool database	= true;

    //QString word("[^ \\t\\n\\r\\f\\v]+");
    QString word("[a-zA-Z0-9_,.&\\-]+");
    /* argWord is a regexp pattern that accepts also `/', to match tango device names inside () arguments.
         * This has been added in Release_3_0_1 to allow configuration of sources/targets of type
         * `test/device/1->Command(test/device/2,tComboBoxFoo)'
         */
    QString argWord("[a-zA-Z0-9_,.&/\\-]+");
    QRegExp re("((" + word + ":[0-9]+/)?" + word + "/" + word + "/" + word + ")(/|->)(" + argWord + ")(#dbase=(no|yes))?(\\(" + argWord + "\\))?");
    // // 	printf("regexp per parseName: %s stringa \"%s\"\n", qstoc(re.pattern()), qstoc(fullName));
    if (re.exactMatch(fullName))
    {
        devName	= re.capturedTexts()[1].toLower();
        point	= re.capturedTexts()[4].toLower();
        if (re.capturedTexts()[5].contains("#"))
        {
            if (re.capturedTexts()[6] == "no")
                database = false;
        }
        args = re.capturedTexts()[7].remove("(").remove(")").split(",", QString::SkipEmptyParts);
        // 	printf("\e[1;31mdevName %s | point %s\n", qstoc(devName), qstoc(point));
        // 	qslisttoc(args);
        // 	printf("\e[0m------------------------------------\n");
    }
    //     else
    // 	printf("\e[1;31m REGEXP UNMATC!!!!!!!!!!!!\e[0m\n");

    dbase = database;
    qDebug() << QThread::currentThread() << ":" << "TUtil::parseName() - " << fullName << devName << point << args;
    if (re.capturedTexts()[3] == "->")
        return COMMAND;
    else if (re.capturedTexts()[3] == "/")
        return ATTRIBUTE;
    else
        return INVALID;
}

QObject* TUtil::findObject(const QString &name, QObject *par)
{
    if (par != 0)
    {
        pstep("Tutil::findObject: parent not null: objectName: %s", qstoc(par->objectName()));
        while (par->parent() != 0)
        {
            pstep("going up through parents: current object name is: \"%s\"", qstoc(par->parent()->objectName()));
            par = par->parent();
        }
        pstep("finally looking for children of \"%s\"", qstoc(par->objectName()));
        d_objects[name] = par->findChild<QObject*>(name);
    }
    else if (d_ancestor != 0)
    {
        pstep("par passed is null, but ancestor is not null and has name \"%s\"", qstoc(d_ancestor->objectName()));
        d_objects[name] = d_ancestor->findChild<QObject*>(name);
    }
    if(!d_objects[name]) /* ! try to find child starting from qApplication! */
    {
        pwarn("object \"%s\" not found in ancestor: looking for a child of active window:", qstoc(name));
        d_objects[name] = qApp->activeWindow()->findChild<QObject*>(name);
    }

    qDebug() << "findObject con " << name << "ritorna: " << d_objects[name];
    pinfo("findObject looked for name \"%s\" and found oject \"0x%p\"", qstoc(name), d_objects[name]);
    if(d_objects[name])
        pok("found object with name \"%s\"", qstoc(d_objects[name]->objectName()));
    else
        pwarn("no object found with search name \"%s\"", qstoc(name));
    return d_objects[name];
}

QString TUtil::findHelperApplication(QString device)
{
    return findHelperApplication(device, QString(), -1);
}

QString TUtil::findHelperApplication(QString dev, const QString& database, const int port)
{
    qDebug() << "TUtil::findHelperApplication <<";
    string app = "", device = dev.toStdString();
    DeviceProxy* dev_proxy = NULL;
    Database *db;
    try {
        /* use specific host and port if provided */
        if(database != QString() && port > 0)
        {
            std::string dbHostName = database.toStdString();
            db = new Database(dbHostName, port);
        }
        else
            db = new Database();


        string dev_class;
        try {
            DbData  data;
            data.push_back(DbDatum("helperApplication"));
            db->get_device_property(device, data);
            if (!data[0].is_empty())
            {
                data[0] >> app;
            }
            else
            {
                dev_proxy =  DeviceFactory::instance()->getDeviceProxy(dev);
                if(dev_proxy != NULL)
                {
                    dev_class = DeviceFactory::instance()->getDeviceProxy(dev)->info().dev_class;
                    DbData  data;
                    data.push_back(DbDatum("helperApplication"));
                    db->get_class_property(dev_class, data);
                    if (!data[0].is_empty())
                    {
                        data[0] >> app;
                    }
                }
                else
                    qprintf("\e[1;31m* \e[0mNo device proxy with name \"%s\"\n\e[0m",
                            device.c_str());
            }
            delete db;
        }
        catch (DevFailed &e)
        {
            TLog log(e);
            TUtil::instance()->addLog(log);
        }
    }
    catch (DevFailed &e)
    {
        TLog log(e);
        TUtil::instance()->addLog(log);
    }

    qDebug() << "TUtil::findHelperApplication >>" << QString::fromStdString(app);
    return QString::fromStdString(app);
}



void TUtil::replaceWildcards(QString &s, QStringList devices)
{
    int i;
    devices.removeAll(QApplication::instance()->arguments()[0]);
    QList<unsigned int> daTogliere;
    for (int i = 0; i < devices.size(); i++)
    {
        if(devices[i].startsWith("//"))
        {
            devices[i].remove("//");
        }
        else if(!QRegExp("*/*/*", Qt::CaseSensitive, QRegExp::Wildcard).exactMatch(devices[i]))
            daTogliere << i;
    }
    foreach (unsigned int i, daTogliere)
        devices.removeAt(i);

    if (!s.isEmpty() && (s.contains("$")))
    {
        for (i = devices.size() - 1; i >= 0; i--)
        {
            s.replace(QString("$%1").arg(i+1), devices[i]);
        }
        /* for the designer: read the devices from the QTANGO_DEVICES environment variable! */
        if( (!devices.size()) && (getenv("QTANGO_DEVICES") != NULL))
        {
            i = 0;
            pinfo("parsing QTANGO_DEVICES environment variable");
            char* devlist = getenv("QTANGO_DEVICES");
            qprintf("device list from env: \"%s\"\n", devlist);
            QString envDevices(devlist);
            QStringList envDevicesList;
            if(envDevices.contains(QRegExp("\\s+")))
                envDevicesList = envDevices.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            else
                envDevicesList << envDevices;

            foreach(QString dev, envDevicesList)
            {
                if(QRegExp("*/*/*", Qt::CaseSensitive, QRegExp::Wildcard).exactMatch(dev))
                {
                    s.replace(QString("$%1").arg(i+1), dev);
                }
                else
                    perr("environment tango device \"%s\", from QTANGO_DEVICES, is not correct", qstoc(dev));
                i++;
            }

        }
        // 	if(i == 0)
        // 		s.replace(QRegExp("\\$\\d+"), "");
    }
}

void TUtil::replaceWildcards(const QObjectList &objects, QStringList devices)
{
    if(warnOldReplaceWildcards)
    {
        pwarn("---");
        pwarn("the replaceWildcards with the object list and the devices list as ");
        pwarn("arguments is deprecated - when used globally on a list of objects -");
        pwarn("and unefficient.");
        pwarn("If you are using the QTangoCommunicationProxies (Reader and Writer)");
        pwarn("they automatically and suitably replace wildcards with application");
        pwarn("command line arguments.");
        pwarn("This method is left for backwards compatibility, but it is recommended");
        pwarn("to delete any call to replaceWildcards(const QObjectList , QStringList )");
        pwarn("when using the proxies");
        pwarn("--");
        warnOldReplaceWildcards = false;
    }
    devices.removeAll(QApplication::instance()->arguments()[0]);
    QList<unsigned int> daTogliere;
    for (int i = 0; i < devices.size(); i++)
    {
        if (!QRegExp("*/*/*", Qt::CaseSensitive, QRegExp::Wildcard).exactMatch(devices[i]))
            daTogliere << i;
    }
    foreach (unsigned int i, daTogliere)
        devices.removeAt(i);

    foreach(QObject *o, objects)
    {
        // // 		qDebug() << QThread::currentThread() << ":" << o;
        QString s;
        QVariant v = o->property("source");
        if (v.isValid())
        {
            s = v.toString();
            if (!s.isEmpty() && (s.contains("$")))
            {
                for (int i = devices.size() - 1; i >= 0; i--)
                {
                    // // 					qDebug() << QThread::currentThread() << ":" << devices[i];
                    s.replace(QString("$%1").arg(i+1), devices[i]);
                }
                o->setProperty("source", QVariant(s));
            }
        }

        v = o->property("targets");
        if (v.isValid())
        {
            s = v.toString();

            if (!s.isEmpty() && (s.contains("$")))
                for (int i = 0; i < devices.size(); i++)
                {
                    s.replace(QString("$%1").arg(i+1), devices[i]);
                }
            o->setProperty("targets", QVariant(s));
        }
        TUtil::replaceWildcards(o->children(), devices);
    }
}

bool TUtil::event(QEvent *e)
{
    qDebug() << QThread::currentThread() << ":" << "TUtil::event <<";
    if (e->type() == QMESSAGEBOXEVENT)
    {
        qDebug() << QThread::currentThread() << ":" << "TUtil::event - QMESSAGEBOXEVENT";
        QString s = dynamic_cast<QMessageBoxEvent *>(e)->message();
        QMessageBox::critical(0, "Error", s);
        return true;
    }
    else if (e->type() == QENABLEEVENT)
    {
        qDebug() << QThread::currentThread() << ":" << "TUtil::event - QENABLEEVENT";
        dynamic_cast<QEnableEvent *>(e)->widget->setEnabled(dynamic_cast<QEnableEvent *>(e)->enable);
        return true;
    }
    else if (e->type() == QSHOWLOGSEVENT)
    {
        qDebug() << QThread::currentThread() << ":" << "TUtil::event - QSHOWLOGSEVENT";
        if (logDialog == 0)
            logDialog = new TLogDialog();
        logDialog->showLogs(dynamic_cast<QShowLogsEvent *>(e)->showAll());
        return true;
    }
    qDebug() << QThread::currentThread() << ":" << "TUtil::event >> (uscita)";
    return QObject::event(e);
}

QStringList TUtil::getArgs(QStringList &rawArgs)
{
    bool res;
    QObject *obj = NULL;
    QStringList	vals;
    QString temp, val, errorMsg;
    foreach (QString s, rawArgs)
    {
        if (s.contains("&"))
        {
            temp = s.remove("&");
            pinfo("looking for an object with the name \"%s\"", qstoc(s));
            obj = findObject(s);
            if(!obj)
            {
                errorMsg = QString("cannot find any object with the name \"%1\".").arg(s);
                errorMsg += "The object you use to obtain the data must have a name and that name ";
                errorMsg += "must be exactly the same as the name provided in the setTarget() method ";
                errorMsg += "of the widget that writes the command or the attribute.\n";
                errorMsg += "Read carefully the documentation of one of the objects TNumeric, TLineEdit or ";
                errorMsg += "TSpinBox...\nThis will generate a Tango exception. Correct the code, thank you";
            }
            else
            {
                res = QMetaObject::invokeMethod(obj, "getData", Qt::DirectConnection, Q_RETURN_ARG(QString, val));
                if (!res)
                {
                    errorMsg = QString("is the object with the name \"%1\" a \e[4mSimpleDataProxy\e[0m?\n").arg(s);
                    errorMsg += "If not, I cannot invoke the getData() method on it!\n";
                    errorMsg += "Read carefully the documentation of one of the objects TNumeric, TLineEdit or";
                    errorMsg += "TSpinBox...\nThis will generate a Tango exception. Correct the code, thank you";
                }
                else
                {
                    vals << val;
                }
            }
        }
        else
            vals << s;
    }
    if(errorMsg != QString())
    {
        QMessageBoxEvent *mbe = new QMessageBoxEvent(errorMsg);
        qApp->postEvent(this, mbe);
        perr("%s", qstoc(errorMsg));
    }
    return vals;
}

DeviceData TUtil::convertToDeviceData(QStringList &args, CommandInfo& info)
{
    DeviceData dd;
    if (args.size() > 0)
    {
        QStringList	vals = getArgs(args);
        if(vals.size())
        {
            switch (info.in_type)
            {
            case DEV_BOOLEAN:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - DEV_BOOLEAN";
                dd << (vals[0].toLower() == "true");
                break;
            }
            case DEV_SHORT:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - DEV_SHORT";
                dd << vals[0].toShort();
                break;
            }
            case DEV_USHORT:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - DEV_USHORT";
                dd << vals[0].toUShort();
                break;
            }
            case DEV_LONG:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - DEV_LONG";
                dd << vals[0].toLong();
                break;
            }
            case DEV_ULONG:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - DEV_ULONG";
                dd << vals[0].toULong();
                break;
            }
            case DEV_FLOAT:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - DEV_FLOAT";
                dd << vals[0].toFloat();
                break;
            }
            case DEV_DOUBLE:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - DEV_DOUBLE";
                dd << vals[0].toDouble();
                break;
            }
            case DEV_STRING:
            case CONST_DEV_STRING:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - DEV_STRING";
                string temp = vals[0].toStdString();
                dd << temp;
                break;
            }
            case DEVVAR_BOOLEANARRAY:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - vector DEVVAR_BOOLEANARRAY unimplemented";
                pwarn("convertToDeviceData - vector DEVVAR_BOOLEANARRAY unimplemented");
                break;
            }
            case DEVVAR_SHORTARRAY:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - vector DEVVAR_SHORTARRAY";
                vector<short> temp;
                foreach (QString v, vals)
                    temp.push_back(v.toShort());
                dd << temp;
                break;
            }
            case DEVVAR_USHORTARRAY:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - vector DEVVAR_USHORTARRAY";
                vector<unsigned short> temp;
                foreach (QString v, vals)
                    temp.push_back(v.toUShort());
                dd << temp;
                break;
            }
            case DEVVAR_LONGARRAY:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - vector DEVVAR_LONGARRAY";
                vector<DevLong> temp;
                foreach (QString v, vals)
                    temp.push_back(v.toLong());
                dd << temp;
                break;
            }
            case DEVVAR_ULONGARRAY:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - vector DEVVAR_ULONGARRAY";
                vector<DevULong> temp;
                foreach (QString v, vals)
                    temp.push_back(v.toULong());
                dd << temp;
                break;
            }
            case DEVVAR_FLOATARRAY:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - vector DEVVAR_FLOATARRAY";
                vector<float> temp;
                foreach (QString v, vals)
                    temp.push_back(v.toFloat());
                dd << temp;
                break;
            }
            case DEVVAR_DOUBLEARRAY:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - vector DEVVAR_DOUBLEARRAY";
                vector<double> temp;
                foreach (QString v, vals)
                    temp.push_back(v.toDouble());
                dd << temp;
                break;
            }
            case DEVVAR_STRINGARRAY:
            {
                qDebug() << QThread::currentThread() << ":" << "TUtil::convertToDeviceData - vector DEVVAR_STRINGARRAY";
                vector<string> temp;
                foreach (QString v, vals)
                    temp.push_back(v.toStdString().c_str());
                dd << temp;
                break;
            }
            default:
                break;
            } /* switch */
        } /* if(vals.size()) */
    } /* if */
    return dd;
}

DeviceAttribute	TUtil::convertToDeviceAttribute(QStringList &args, AttributeInfoEx& info)
{
    if (args.size() > 0)
    {
        QStringList	vals = getArgs(args);
        if(vals.size())
        {
            switch (info.data_format)
            {
            case SCALAR:
                switch (info.data_type)
                {
                case DEV_BOOLEAN:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_BOOLEAN";
                    bool temp = (vals[0].toLower() == "true");
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_SHORT:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_SHORT";
                    short temp = vals[0].toShort();
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_LONG:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_LONG";
                    DevLong temp = vals[0].toLong();
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_FLOAT:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_FLOAT";
                    float temp = vals[0].toFloat();
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_DOUBLE:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_DOUBLE";
                    double temp = vals[0].toDouble();
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_UCHAR:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_UCHAR";
                    unsigned char temp = vals[0].at(0).toLatin1();
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_USHORT:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_USHORT";
                    unsigned short temp = vals[0].toUShort();
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_STRING:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_STRING";
                    string temp = vals[0].toStdString();
                    return DeviceAttribute(info.name, temp);
                }
                default:
                    break;
                }
                break;
            case SPECTRUM:
                switch (info.data_type)
                {
                case DEV_BOOLEAN:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_BOOLEAN";
                    vector<bool> temp;
                    foreach (QString v, vals)
                        foreach (QString s, v.split(","))
                            temp.push_back(s.toLower() == "true");
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_SHORT:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_SHORT";
                    vector<short> temp;
                    foreach (QString v, vals)
                        foreach (QString s, v.split(","))
                            temp.push_back(s.toShort());
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_LONG:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_LONG";
                    vector<DevLong> temp;
                    foreach (QString v, vals)
                        foreach (QString s, v.split(","))
                            temp.push_back(s.toLong());
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_FLOAT:
                {
                    printf("spectrum dev float\n");
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_FLOAT";
                    vector<float> temp;
                    foreach (QString v, vals)
                        foreach (QString s, v.split(","))
                            temp.push_back(s.toFloat());
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_DOUBLE:
                {
                    printf("spectrum dev double\n");
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_DOUBLE";
                    vector<double> temp;
                    foreach (QString v, vals)
                        foreach (QString s, v.split(","))
                            temp.push_back(s.toDouble());
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_UCHAR:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_UCHAR";
                    vector<unsigned char> temp;
                    foreach (QString v, vals)
                        foreach (QString s, v.split(","))
                            temp.push_back(s.at(0).toLatin1()); /* controllare */
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_USHORT:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_USHORT";
                    vector<unsigned short> temp;
                    foreach (QString v, vals)
                        foreach (QString s, v.split(","))
                            temp.push_back(s.toUShort());
                    return DeviceAttribute(info.name, temp);
                }
                case DEV_STRING:
                {
                    qDebug() << QThread::currentThread() << ":" << "convertToDeviceAttribute() - DEV_STRING";
                    vector<string> temp;
                    foreach (QString v, vals)
                        foreach (QString s, v.split(","))
                            temp.push_back(s.toStdString());
                    return DeviceAttribute(info.name, temp);
                }
                default:
                    break;
                }
                break;
            default:
                break;
            }
        } /* if(vals.size()) */
    }
    return DeviceAttribute();
}

TVariant TUtil::oneShot(QString s)
{
    if (s.contains("$"))
        return TVariant();

    bool db;
    QString devname, ac;
    QStringList input_args;
    qDebug() << "oneShot()";
    DataType type = parseName(s, devname, ac, input_args, db);
    string attr_command = ac.toStdString();
    try
    {
        DeviceProxy	*device	= DeviceFactory::instance()->getDeviceProxy(devname);
        if(device == NULL)
            device = DeviceFactory::instance()->create(devname);
        if (type == ATTRIBUTE)
        {
            AttributeInfoEx info	= device->get_attribute_config(attr_command);
            DeviceAttribute devattr	= device->read_attribute(attr_command);

            return TVariant(devattr, info, s);
        }
        else if (type == COMMAND)
        {
            CommandInfo	info 	= device->command_query(attr_command);
            DeviceData	argin	= convertToDeviceData(input_args, info);
            DeviceData	ddout	= device->command_inout(attr_command, argin);
            return TVariant(ddout, info, s);
        }
    }
    catch (DevFailed &e)
    {
        TLog log(e);
        TUtil::instance()->addLog(log.toTimeErrlistPair(), log.formatMessage());
        perr("TUtil::oneShot() failed to connect to the device proxy named  \"%s\" through the DeviceFactory", qstoc(devname));
        Tango::Except::re_throw_exception(e, "failed to execute TUtil::oneShot \"" + devname.toStdString() +
                                          "\", attribute/command: \"" +  attr_command + "\".", "Failed to execute `TUtil::oneShot()'",
                                          "TUtil::oneShot()", Tango::ERR);
    }
    return TVariant();
}

void TUtil::waitRefresh(QMutex *mutex, unsigned long time)
{
    qDebug() << QThread::currentThread() << ":" << "TUtil::waitRefresh <<";
    mutex->lock();
    refreshCondition.wait(mutex, time);
    mutex->unlock();
    qDebug() << QThread::currentThread() << ":" << "TUtil::waitRefresh >>";
}

void TUtil::triggerRefresh()
{
    QString time = QTime::currentTime().toString("hh:mm:ss:zzz");
    qDebug() << "TUTIL triggerRefresh() " << this << QThread::currentThread();
    ADD_READ_STAT_MSG("TUtil", "refresh signal", STATS_OBJINFO(this));
    emit refreshSignal();
    refreshCondition.wakeAll();
}

void TUtil::setGlobalRefreshDisabled(bool d)
{
    qDebug() << "void TUtil::setGlobalRefreshDisabled(bool d), d = " << d << d_externalTimer->running();
    d_globalRefreshDisabled = d;
    if(d)
        d_externalTimer->stopRefreshing();
    else
        d_externalTimer->setInterval(d_externalTimer->interval());
}

int TUtil::globalRefreshInterval()
{
    return d_externalTimer->interval();
}

void TUtil::setGlobalRefreshInterval(int millis)
{
    pinfo("changing global refresh timer from %d ms to %d ms.", d_externalTimer->interval(),
          millis);
    d_externalTimer->setInterval(millis);
}

void TUtil::adjustGlobalRefreshIntervalIfNeeded(int millis)
{
    if(!d_globalRefreshDisabled)
    {
        int neededRefresh = millis * GLOBAL_REFRESH_FACTOR_ADJUST;
        int currentGlobalRefreshInterval = globalRefreshInterval();
        /* change the global refresh interval only if a faster refresh is requested. */
        if(neededRefresh < currentGlobalRefreshInterval)
            setGlobalRefreshInterval(neededRefresh);
    }
}

QString TUtil::modeRepresentation(RefreshMode m, bool noescape)
{
    QString ret = noescape ? "" : "\e[0;33;4m";

    switch(m)
    {
    case AUTO_REFRESH:
        ret += noescape ? "auto" : "\e[0;32;4mauto";
        break;
    case USER_EVENT_REFRESH:
        ret += "user event";
        break;
    case CHANGE_EVENT_REFRESH:
        ret += noescape ? "change event" : "\e[1;32;4mchange event";
        break;
    case PERIODIC_EVENT_REFRESH:
        ret += noescape ? "periodic event" : "\e[0;36;4mperiodic event";
        break;
    case ARCHIVE_EVENT_REFRESH:
        ret += noescape ? "archive event" : "\e[0;34;4marchive event";
        break;
    case POLLED_REFRESH:
        ret += noescape ? "polled" : "\e[1;36;4mpolled";
        break;
    case MANUAL_REFRESH:
        ret += "manual";
        break;
    case TARGET:
        ret += "target";
        break;
    }

    ret = ret + (noescape ? "" : "\e[0m");
    return ret;
}

QString TUtil::typeRepresentation(CmdArgType t) const
{
    const char *argTypes [] =
    {
        "DEV_VOID",
                "DEV_BOOLEAN",
                "DEV_SHORT",
                "DEV_LONG",
                "DEV_FLOAT",
                "DEV_DOUBLE",
                "DEV_USHORT",
                "DEV_ULONG",
                "DEV_STRING",
                "DEVVAR_CHARARRAY",
                "DEVVAR_SHORTARRAY",
                "DEVVAR_LONGARRAY",
                "DEVVAR_FLOATARRAY",
                "DEVVAR_DOUBLEARRAY",
                "DEVVAR_USHORTARRAY",
                "DEVVAR_ULONGARRAY",
                "DEVVAR_STRINGARRAY",
                "DEVVAR_LONGSTRINGARRAY",
                "DEVVAR_DOUBLESTRINGARRAY",
                "DEV_STATE",
                "CONST_DEV_STRING",
                "DEVVAR_BOOLEANARRAY",
                "DEV_UCHAR",
                "DEV_LONG64",
                "DEV_ULONG64",
                "DEVVAR_LONG64ARRAY",
                "DEVVAR_ULONG64ARRAY",
                "DEV_INT",
                "DEV_ENCODED",
                "DEV_ENUM",
                "DEV_PIPE_BLOB",
                "DEVVAR_STATEARRAY",
                "DATA_TYPE_UNKNOWN"

    };
    if(t < DEVVAR_ULONG64ARRAY + 6)
        return QString(argTypes[t]);

    return QString("Unknown type for %1. Check tango sources at lib/cpp/server/tango_const.h").arg(t);
}

QString TUtil::iconPath()
{
    QSettings s(QSettings::NativeFormat, QSettings::UserScope, "Elettra");
    return s.value("PIXMAP_PATH", "/runtime/share/icons").toString();
}

void TUtil::setIconPath(const QString& path)
{
    QSettings s(QSettings::NativeFormat, QSettings::UserScope, "Elettra");
    s.setValue("PIXMAP_PATH", path);
}

