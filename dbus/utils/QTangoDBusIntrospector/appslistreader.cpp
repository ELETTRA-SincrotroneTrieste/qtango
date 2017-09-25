#include "appslistreader.h"
#include "appinfo.h"
#include <QTimer>
#include <QtDebug>
#include <QDBusInterface>
#include <QMetaProperty>
#include <QStringList>

AppsListReader::AppsListReader(QObject *parent) : QObject(parent)
{
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
    timer->start();
}

void AppsListReader::refresh()
{
    QStringList services;
    QDBusConnection dbusConn = QDBusConnection::sessionBus();
    d_dBusInterface = dbusConn.interface();
    QDBusReply<QStringList> servicesReply  = d_dBusInterface->registeredServiceNames();
    d_appInfoList.clear();
    if(servicesReply.isValid())
    {
        foreach(QString service, servicesReply.value())
        {
            if(service.contains("TApplications"))
            {
                pid_t appPid = (pid_t)d_dBusInterface->servicePid(service).value();
                QString path = QString("/TApplication/%1").arg(appPid);
                QDBusInterface dbusIf(service, path);
                if(!dbusIf.isValid())
                {
                    emit applicationListAvailable(false, dbusIf.lastError().message());
                    qDebug() << "error dbusIf" << dbusIf.lastError().message();
                    return;
                }
                else
                {
                    QDBusMessage exeMsg = dbusIf.call("executable");
                    QDBusMessage argsMsg = dbusIf.call("parameters");
                    if(exeMsg.arguments().size() == 1 && argsMsg.arguments().size() == 1)
                    {
                        services << service;
                        AppInfo ai(exeMsg.arguments().at(0).toString(),
                                   argsMsg.arguments().at(0).toStringList(),
                                   (pid_t)d_dBusInterface->servicePid(service).value());
                        ai.dbusServiceName = service;
                        ai.dbusServicePath = path;
                        d_appInfoList.add(ai);
                    }
                    else
                        emit applicationListAvailable(false, exeMsg.errorMessage());
                 }
            }
        }

        emit applicationListAvailable(true, QString());
    }
    else
        emit applicationListAvailable(false, dbusConn.lastError().message());
}

