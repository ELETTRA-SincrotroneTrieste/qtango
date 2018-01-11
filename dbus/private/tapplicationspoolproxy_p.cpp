#include "tapplicationspoolproxy_p.h"

#include <QDBusConnectionInterface>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QtDebug>
#include <macros.h>
#include "../tapplicationspoolproxy.h"

#define SERVICE_NAME "it.trieste.elettra.tapplicationspoolproxy"

TApplicationsPoolProxyPrivate::TApplicationsPoolProxyPrivate(TApplicationsPoolProxy *parent) : q_ptr(parent)
{
    m_dbusConnectionInterface = NULL;
    m_dbusConnectionInterface = m_getDbusConnectionInterface();
    QObject::connect(m_dbusConnectionInterface, SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString& )), parent,
                     SLOT(dbusServiceOwnerChanged(const QString&, const QString&, const QString& )));
}

bool TApplicationsPoolProxyPrivate::isTApplication(const QString& serviceName)
{
    return serviceName.contains("TApplications");
}

void TApplicationsPoolProxyPrivate::removeByServiceName(const QString& serviceName)
{
    TAppInfo appInfo = getRegisteredApp(serviceName);
    if(!appInfo.isEmpty())
        m_appInfoList.remove(appInfo.appName, appInfo.pid);
}

TAppInfo TApplicationsPoolProxyPrivate::getRegisteredApp(const QString& serviceName)
{
    int pid = -1;
    QString appName;
    pid = m_pidFromService(serviceName);
    appName = m_executableFromService(serviceName);
    if(pid > 0 && !appName.isEmpty())
        return m_appInfoList.find(appName, pid);
    m_errorMessage = QString("%1: No application associated to \"%2\" was found.").arg(__FUNCTION__).arg(serviceName);
    return TAppInfo();
}

QString TApplicationsPoolProxyPrivate::m_executableFromService(const QString& serviceName)
{
    int pos;
    QStringList sl;
    QRegExp rx("TApplications\\.(.*)\\..*");
    QString appName;
    pos = rx.indexIn(serviceName);
    if(pos > -1)
    {
        sl = rx.capturedTexts();
        if(sl.size() > 1)
            appName = sl.at(1); /* capture 0 contains full match! */
    }
    return appName;
}

int TApplicationsPoolProxyPrivate::m_pidFromService(const QString& serviceName)
{
    QStringList sl;
    QRegExp rx(".*\\.pid_(\\d+)");
    int pid = -1;
    int pos = rx.indexIn(serviceName);
    if(pos > -1)
    {
        sl = rx.capturedTexts();
        if(sl.size() > 1) /* capture  (0) is full match */
            pid = sl.at(1).toInt();
    }
    return pid;
}

TAppInfo TApplicationsPoolProxyPrivate::getInfo(const QString& serviceName)
{
    m_errorMessage.clear();
    if(serviceName.contains("TApplications"))
    {
        /* retrieve pid from service name, avoid doing an extra call to dbus */
        //        printf("TApplicationsPoolProxyPrivate: obtaining pid from service \"%s\"", serviceName.toStdString().c_str());
        pid_t appPid = (pid_t) m_pidFromService(serviceName);
        QString path = QString("/TApplication");
        QDBusInterface dbusIf(serviceName, path);
        if(dbusIf.isValid())
        {
            /* retrieve pid from service */
            QString executable = m_executableFromService(serviceName);
            QDBusReply<QStringList> argsReply = dbusIf.call("parameters");
            if(!argsReply.isValid())
                m_errorMessage = argsReply.error().message();
            else
            {
                TAppInfo ai(executable, argsReply.value(), appPid);
                ai.dbusServiceName = serviceName;
                ai.dbusServicePath = path;
                return ai;
            }
        }
    }
    /* no error message if service name does not contain TApplications */
    return TAppInfo();
}

bool TApplicationsPoolProxyPrivate::updateTApplicationList()
{
    /* save current list */
    TAppInfoList newList;
    m_errorMessage.clear();
    QDBusReply<QStringList> services = m_dbusConnectionInterface->registeredServiceNames();
    if(services.isValid())
    {
        /* populate the refreshed information list */
        foreach(QString service, services.value())
        {
            if(service.contains("TApplications"))
            {
                /* test whether we already have it memorized in the current list */
                TAppInfo ai = m_appInfoList.findByService(service);
                if(ai.isEmpty())
                {
                    ai = getInfo(service);
                }
                if(!ai.isEmpty())
                    newList.add(ai);
            }
        }
    }

    /* update internal list */
    m_appInfoList = newList;
    /* success if no error message */
    return m_errorMessage.isEmpty();
}

QDBusConnectionInterface *TApplicationsPoolProxyPrivate::m_getDbusConnectionInterface()
{
    if(m_dbusConnectionInterface)
        return m_dbusConnectionInterface;

    QDBusConnection connection = QDBusConnection::sessionBus();

    if(!connection.registerService(SERVICE_NAME))
        perr("TApplicationsPoolProxyPrivate: failed to register service \"%s\": %s: %s", SERVICE_NAME, qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));

    else if(!connection.registerObject("/TApplicationsPoolProxyPrivate", q_ptr))
        perr("TApplicationsPoolProxyPrivate: failed to register object with path \"%s\" %s: %s:", SERVICE_NAME, qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));

    if(connection.isConnected())
        m_dbusConnectionInterface = connection.interface();

    return m_dbusConnectionInterface;
}
