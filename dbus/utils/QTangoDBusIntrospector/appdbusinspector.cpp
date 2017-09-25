#include "appdbusinspector.h"
#include <QTimer>
#include <QtDebug>
#include <QDBusConnectionInterface>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QMetaProperty>
#include <QStringList>
#include <TApplicationInterface.h>

AppDbusInspector::AppDbusInspector(QObject *parent) :
        QObject(parent)
{

}

TApplicationInterface *AppDbusInspector::m_getDBusInterface(const QString& serviceName)
{
    QDBusConnection dbusConn = QDBusConnection::sessionBus();
    QDBusReply<bool>  srv = dbusConn.interface()->isServiceRegistered(serviceName);
    if(srv.isValid())
    {
        bool isRegistered = srv.value();
        if(isRegistered)
        {
            /* get pid */
            QDBusReply<uint> pidReply = dbusConn.interface()->servicePid(serviceName);
            if(pidReply.isValid())
            {
                QString path = "/TApplication";
                TApplicationInterface *dbusIf = new TApplicationInterface(serviceName, path, dbusConn, this);
                return dbusIf;
            }
            else
                m_lastError = pidReply.error().message();
        }
        else
            m_lastError = "Service \"" + serviceName + "\" is not registered in DBUS";
    }
    else
        m_lastError = srv.error().message();
    return NULL;
}

/** if returned widget list is empty, then check lastError()
  */
QStringList AppDbusInspector::getWidgetList(const QString& serviceName)
{
    TApplicationInterface *dbusIf = m_getDBusInterface(serviceName);
    if(dbusIf)
    {
        QDBusReply <QStringList> widListMsg = dbusIf->widgetList();
        if(widListMsg.isValid())
        {
            return widListMsg.value();
        }
        else
        {
            m_lastError = widListMsg.error().message();
        }
    }
    return QStringList();
}

/** if returned widget list is empty, then check lastError()
  */
QStringList AppDbusInspector::getMethodSignatureList(const QString& serviceName, const QString& objectName)
{
    TApplicationInterface *dbusIf = m_getDBusInterface(serviceName);
    if(dbusIf)
    {
        QDBusReply <QStringList> methodListMsg = dbusIf->methods(objectName);
        if(methodListMsg.isValid())
            return methodListMsg.value();
        else
            m_lastError = methodListMsg.error().message();
    }
    return QStringList();
}

QStringList AppDbusInspector::getPropertyList(const QString& serviceName, const QString& objectName)
{
    TApplicationInterface *dbusIf = m_getDBusInterface(serviceName);
    if(dbusIf)
    {
        QDBusReply <QStringList> propertyListMsg = dbusIf->properties(objectName);
        if(propertyListMsg.isValid())
            return propertyListMsg.value();
        else
            m_lastError = propertyListMsg.error().message();
    }
    return QStringList();
}

bool AppDbusInspector::execute(const QString& serviceName,
                               const QString& widgetName, const QString &slot,
                               QVariant& returnValue, const QVariant & arg1,
                               const QVariant & arg2, const QVariant & arg3,
                               const QVariant & arg4, const QVariant & arg5)
{
    QDBusError err;
    bool voidMessage = false;
    QDBusReply<QDBusVariant> reply;
    QDBusMessage voidMsg;
    TApplicationInterface *dbusIf = m_getDBusInterface(serviceName);
    if(dbusIf)
    {
        QVariantList params;
        QString method = slot;
        if(slot.contains(QRegExp("\\[.*\\]")))
            method.remove(QRegExp("\\s+\\[.*\\]"));

        params << qVariantFromValue(widgetName);
        params << qVariantFromValue(method);

        if(arg1.isValid())
        {
            QDBusVariant tmp(arg1);
            params << qVariantFromValue(tmp);
        }
        if(arg2.isValid())
        {
            QDBusVariant tmp(arg2);
            params << qVariantFromValue(tmp);
        }
        if(arg3.isValid())
        {
            QDBusVariant tmp(arg3);
            params << qVariantFromValue(tmp);
        }
        if(arg4.isValid())
        {
            QDBusVariant tmp(arg4);
            params << qVariantFromValue(tmp);
        }       
        if(arg5.isValid())
        {
            QDBusVariant tmp(arg5);
            params << qVariantFromValue(tmp);
        }
        // execMsg = dbusIf->callWithArgumentList(QDBus::Block, QLatin1String("invokeMethod"), params);
        //        execMsg = dbusIf->call("invokeMethod", QVariant("label"), QVariant("update()"));

        m_lastError = QString();
        if(!method.contains(QRegExp("\\(.*\\)"))) /* property */
        {
            if(params.size() == 2) /* read */
                reply  = dbusIf->call(QDBus::Block, "readProperty", widgetName, method);
            else if(params.size() == 3)
            {
              printf("call \e[1;32mwriteProperty\e[0m\n");
              qDebug() << "params di writeProperty " << params;
              voidMessage = true;
              voidMsg = dbusIf->callWithArgumentList(QDBus::AutoDetect, "writeProperty", params);
              printf("ha funzionato? %d\n", voidMsg.type() != QDBusMessage::ErrorMessage);
             }
        }
        else /* slot */
        {
            qDebug() << "callWithArgumentList" << "invokeMethod" << params;
            reply  = dbusIf->callWithArgumentList(QDBus::Block, QString("invokeMethod%1").arg(params.size() - 2), params);
        }
        if(!voidMessage && reply.isValid())
        {
            returnValue = reply.value().variant();
        }
        else if(!voidMessage && !reply.isValid())
            m_lastError = reply.error().message();
        else if(voidMessage && voidMsg.type() == QDBusMessage::ErrorMessage)
        {
            printf("voidMsgType is Error message\n\n");
            m_lastError = voidMsg.errorMessage();
        }

    }
    return m_lastError.isEmpty();
}
