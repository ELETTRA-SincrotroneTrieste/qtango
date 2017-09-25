#include "tapplicationspoolproxy.h"
#include "private/tapplicationspoolproxy_p.h"
#include "tappinfo.h"

#include <QMetaType>
#include <QtDebug>

TApplicationsPoolProxy::TApplicationsPoolProxy(QObject *parent) :
        QObject(parent)
{
    d_ptr = new  TApplicationsPoolProxyPrivate(this);
    /* register meta type */
    qRegisterMetaType<TAppInfo>("TAppInfo");
    refreshAppList(); /* perform first read */
}

TAppInfo TApplicationsPoolProxy::findApplicationByService(const QString& serviceName)
{
    Q_D(TApplicationsPoolProxy);
    return d->getRegisteredApp(serviceName);
}

TAppInfo TApplicationsPoolProxy::findApplication(const QString& appName, int pid)
{
    return applicationsList().find(appName, pid);
}

QList<TAppInfo> TApplicationsPoolProxy::findApplicationsWithName(const QString& name)
{
    return applicationsList().findWithName(name);
}

TAppInfo TApplicationsPoolProxy::findApplicationByNameArgs(const QString& name, const QStringList& args)
{
    return applicationsList().findByNameArgs(name, args);
}

TAppInfo TApplicationsPoolProxy::findApplicationByPid(pid_t pid)
{
    return applicationsList().findByPid(pid);
}

QString TApplicationsPoolProxy::errorMessage()
{
    Q_D(TApplicationsPoolProxy);
    return d->errorMessage();
}

void TApplicationsPoolProxy::dbusServiceOwnerChanged(const QString& name, const QString& oldOwner, const QString& newOwner)
{
    Q_D(TApplicationsPoolProxy);
//    qprintf("\e[0;33mTApplicationsPoolProxy::dbusServiceOwnerChanged(): \e[1;33m \"%s\" oldO: \"%s\" newO \"%s\"\e[0m\n", qstoc(name), qstoc(oldOwner),qstoc
//           (newOwner));
    if(name.contains("TApplications") && newOwner.isEmpty() && !oldOwner.isEmpty())
    {
        TAppInfo ai = d->getRegisteredApp(name);
        if(!ai.isEmpty())
        {
            emit serviceUnregistered(ai); /* do not query dbus */
            d->removeByServiceName(name); /* remove from the internal list */
        }
        else
            emit error(d->errorMessage());
    }
    else if(oldOwner.isEmpty() && name.contains("TApplications") && !newOwner.isEmpty())
    {
        TAppInfo ai = d->getInfo(name);
        if(!ai.isEmpty())
        {
            d->add(ai);
            emit serviceRegistered(ai);
        }
        else
            emit error(d->errorMessage());
    }
}

void TApplicationsPoolProxy::refreshAppList()
{
    Q_D(TApplicationsPoolProxy);
    if(!d->updateTApplicationList())
    {
        printf("\e[1;31m*\e[0m DBus error in TApplicationsPoolProxy::refreshAppList(): %s\e[0\n", d->errorMessage().toStdString().c_str());
        emit error(d->errorMessage());
    }
}

TAppInfoList TApplicationsPoolProxy::applicationsList()
{
    Q_D(TApplicationsPoolProxy);
    return d->appInfoList();
}


