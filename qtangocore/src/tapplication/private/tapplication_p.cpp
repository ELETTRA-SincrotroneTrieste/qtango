
#include "dbus/TApplicationAdaptor.h"
#include "dbus/TApplicationInterface.h"
#include "dbus/tapplicationspoolproxy.h"
#include "dbus/secondinstancehandler.h"
#include "tapplication_p.h"
#include "tapplication.h"
#include <macros.h>
#include <errno.h>
#include <QUrl>
#include <QStringList>
#include <QWidget>
#include <QMetaMethod>
#include <QMetaProperty>
#include <unistd.h>

/* freeBSD */
#ifdef __FreeBSD__
#define HOST_NAME_MAX _SC_HOST_NAME_MAX
#endif

/* Inherits QObject in order to be accessible from TApplication by means of findChild().
 * This is not strictly necessary and should be removed in the next major release.
 */
TApplicationPrivate::TApplicationPrivate(TApplication *tApp) : QObject(tApp), q_ptr(tApp)
{
    char hostname[HOST_NAME_MAX];
    char *display;
    QString qsdisplay;
    m_parameters = QCoreApplication::arguments();
    m_parameters.removeFirst();
    m_exename = QCoreApplication::arguments().at(0);
    if(m_exename.contains("/"))
        m_exename = m_exename.split("/", QString::SkipEmptyParts).last();
    qDebug() << "exename" << m_exename << "args " << m_parameters;
    d_secondInstanceHandler = NULL;

    if(gethostname(hostname, HOST_NAME_MAX))
    {
        perr("TApplicationPrivate: cannot gethostname(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    display = getenv("DISPLAY");
    if(!display)
    {
        printf("TApplicationPrivate: \e[1;33;4mWARNING\e[0m: \"DISPLAY\" environment variable not set:\n");
        printf("                      the DISPLAY information will not be available in the dbus service name.\n");
    }
    else
    {
        qsdisplay = QString(display);
        qsdisplay.remove(QRegExp("\\.\\d*")).remove(":");
    }
    m_dbusServiceName = "it.trieste.elettra." + QString(hostname) + "_" + qsdisplay  +
             ".TApplications." + composeDbusObjectName() + ".pid_" + QString::number(getpid());
    // printf("dbusServicename \"%s\" len %d\n", qstoc(m_dbusServiceName), m_dbusServiceName.size());
    m_dbusPath = QString("/TApplication");
}

TApplicationPrivate::~TApplicationPrivate()
{
    finiDbus();
}

void TApplicationPrivate::finiDbus()
{
    Q_Q(TApplication);
    QString objName = composeDbusObjectName();
    QDBusConnection connection = QDBusConnection::sessionBus();
    QDBusReply<bool> regReply = connection.interface()->isServiceRegistered(m_dbusServiceName);
    if(regReply.isValid() && regReply.value())
    {
        if(!connection.unregisterService(m_dbusServiceName))
        {
            perr("TApplication: failed to unregister service \"%s\":", qstoc(m_dbusServiceName));
            perr("TApplication: %s: %s", qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));
        }
        else
            pok("TApplication: successfully unregistered service \"%s\" from DBus", qstoc(m_dbusServiceName));
    }
}

void TApplicationPrivate::initDbus()
{
    Q_Q(TApplication);
    TApplicationAdaptor *tappAdaptor = new TApplicationAdaptor(q);
    Q_UNUSED(tappAdaptor);
    QDBusConnection connection = QDBusConnection::sessionBus();

    if(!connection.registerService(m_dbusServiceName))
    {
        perr("TApplication: failed to register service \"%s\":", qstoc(m_dbusServiceName));
        perr("TApplication: %s: %s", qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));
    }
    else
        pok("connected to DBUS with service \"%s\"", qstoc(m_dbusServiceName));

    if(!connection.registerObject(m_dbusPath, q))
    {
        perr("TApplication: failed to register object with path \"%s\":", qstoc(m_dbusPath));
        perr("TApplication: %s: %s", qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));
    }
    else
        pok("connected to DBUS with path \"%s\"", qstoc(m_dbusPath));

}

QString TApplicationPrivate::composeDbusObjectName()
{
    return m_exename;
}

QStringList TApplicationPrivate::widgetList()
{
    Q_Q(TApplication);
    QStringList ret;
    QHash<QString, QString> objHash;
    QHash <QString, int> duplicates;
    foreach(QWidget *o, q->allWidgets())
    {
        if(!o->objectName().isEmpty())
        {
            if(!objHash.contains(o->objectName()))
                objHash.insert(o->objectName(), QString(o->metaObject()->className()));
            else /* insert into duplicates set for later removal from the hash */
            {
                int dupNo; /* the first already present in objHash, that will be removed later */
                if(duplicates.contains(o->objectName()))
                    dupNo = duplicates.value(o->objectName()) + 1;
                else
                    dupNo = 2;
                duplicates.insert(o->objectName(), dupNo);
            }
        }
    }
    /* now we have to remove the duplicates from the hash */
    foreach(QString dup, duplicates.keys())
        objHash.remove(dup);
    /* populate the return string list with the couples object name - class name */
    QHashIterator <QString, QString> i(objHash);
    while(i.hasNext())
    {
        i.next();
        ret << i.key() << i.value();
    }
    if(duplicates.size() > 0)
    {
        pwarn("application \"%s\" has %d objects with duplicate names", qstoc(q->applicationName()), duplicates.size());
        foreach(QString dup, duplicates.keys())
            pstep("%s compares  %d times", qstoc(dup), duplicates.value(dup));
        pwarn("widgets with duplicate names are removed from the list.");
    }
    return ret;
}

QStringList TApplicationPrivate::properties(const QString& widgetName)
{
    Q_Q(TApplication);
    QStringList properties;
    foreach(QWidget *w, q->allWidgets())
    {
        if(w->objectName() == widgetName)
        {
            for(int i = 0; i < w->metaObject()->propertyCount(); i++)
            {
                QMetaProperty mp = w->metaObject()->property(i);
                properties << QString("%1 [%2]").arg(mp.name()).arg(mp.typeName());
            }
            qSort(properties.begin(), properties.end());
        }
    }
    return properties;
}

QStringList TApplicationPrivate::methods(const QString& widgetName)
{
    Q_Q(TApplication);
    QStringList methods;
    foreach(QWidget *w, q->allWidgets())
    {
        if(w->objectName() == widgetName)
        {
            for(int i = 0; i < w->metaObject()->methodCount(); i++)
            {
                QMetaMethod mm = w->metaObject()->method(i);
                if(mm.methodType() == QMetaMethod::Slot)
#if QT_VERSION < 0x050000
                    methods << mm.signature();
#else
                    methods << mm.methodSignature();
#endif
            }
            qSort(methods.begin(), methods.end());
        }
    }
    return methods;
}

QStringList TApplicationPrivate::parameters() const
{
    qDebug() << m_parameters;
    return m_parameters;
}

TApplicationInterface *TApplicationPrivate::getRunningApplicationInterface(const QString& exename, const QStringList &args)
{
    Q_Q(TApplication);
    Q_UNUSED(exename);
    Q_UNUSED(args);
    TApplicationInterface *tAppIface = NULL;
    TApplicationsPoolProxy* appPoolProxy = new TApplicationsPoolProxy(0);
    TAppInfo appInfo = appPoolProxy->findApplicationByNameArgs(m_exename, m_parameters);
   // printf("\e[1;35mappInfo.pid %d getpid() %d\e[0m -- appIndo.isEmpty() %d\n", appInfo.pid, getpid(), appInfo.isEmpty());
    if(!appInfo.isEmpty() && appInfo.pid != getpid())
        tAppIface = new TApplicationInterface(appInfo.dbusServiceName,
                                              appInfo.dbusServicePath, QDBusConnection::sessionBus(), q);
    delete appPoolProxy;
    return tAppIface;
}

/** \brief invokes method on the object widgetName.
  *
  * @param the objectName() of the widget on which you want to invoke the method memberSignature
  * @param memberSignature the name of the method to invoke, full signature, e.g. void setText(const QString&).
  * @param val0, ... val4 the input parameters to pass to memberSignature method.
  *
  * memberSignature is normalized before being used using QMetaObject::normalizedSignature.
  * The full signature is needed to determine whether the method has or not a return type.
  */
QVariant TApplicationPrivate::invokeMethod( const QString&  widgetName,  const QString& memberSignature,
                                            const QVariant& val0,
                                            const QVariant& val1, const QVariant& val2,
                                            const QVariant& val3, const QVariant& val4)
{
    Q_Q(TApplication);
    QVariant vRet;
    foreach(QWidget *w, q->allWidgets())
    {
        if(w->objectName() == widgetName)
        {
            bool success;
            bool hasReturn = true;
            QVariant returnVal;
            /* normalize signature */
            QString member = QString(QMetaObject::normalizedSignature(qstoc(memberSignature)));
           // printf("\e[1;32m invoking \"%s\" on obj \"%s\" ...\e[0m\n", qstoc(member), qstoc(widgetName));
            QMetaMethod method;
            int indexOfProperty;
            int indexOfMethod = w->metaObject()->indexOfMethod(qstoc(member));
            if(indexOfMethod < 0) /* try with property */
            {
                member.remove(QRegExp("\\[.*\\]"));
                indexOfProperty = w->metaObject()->indexOfProperty(qstoc(member));
                if(indexOfProperty > -1)
                {
                    QMetaProperty metaProperty = w->metaObject()->property(indexOfProperty);
                    if(val0.isValid())
                        metaProperty.write(w, val0);
                    else
                        returnVal = metaProperty.read(w);
                }
            }
            if(indexOfMethod > -1)
            {
                /* do not want parameters inside signature any more */
                member.remove(QRegExp("\\(.*\\)"));
                method = w->metaObject()->method(indexOfMethod);
                if(!strlen(method.typeName()))
                    hasReturn = false;

                if(val0.isValid() && val1.isValid() && val2.isValid() && val3.isValid() && val4.isValid())
                {
                    if(hasReturn)
                        success = QMetaObject::invokeMethod(w, qstoc(member), Qt::DirectConnection, Q_RETURN_ARG(QVariant, returnVal),
                                                            QGenericArgument(val0.typeName(), val0.data()),
                                                            QGenericArgument(val1.typeName(), val1.data()),
                                                            QGenericArgument(val2.typeName(), val2.data()),
                                                            QGenericArgument(val3.typeName(), val3.data()),
                                                            QGenericArgument(val4.typeName(), val4.data()));
                    else
                        success = QMetaObject::invokeMethod(w, qstoc(member), Qt::DirectConnection,
                                                            QGenericArgument(val0.typeName(), val0.data()),
                                                            QGenericArgument(val1.typeName(), val1.data()),
                                                            QGenericArgument(val2.typeName(), val2.data()),
                                                            QGenericArgument(val3.typeName(), val3.data()),
                                                            QGenericArgument(val4.typeName(), val4.data()));

                }
                else if(val0.isValid() && val1.isValid() && val2.isValid() && val3.isValid())
                {
                    if(hasReturn)
                        success = QMetaObject::invokeMethod(w, qstoc(member),  Qt::DirectConnection, Q_RETURN_ARG(QVariant, returnVal),
                                                            QGenericArgument(val0.typeName(), val0.data()),
                                                            QGenericArgument(val1.typeName(), val1.data()),
                                                            QGenericArgument(val2.typeName(), val2.data()),
                                                            QGenericArgument(val3.typeName(), val3.data()));
                    else
                        success = QMetaObject::invokeMethod(w, qstoc(member),  Qt::DirectConnection,
                                                            QGenericArgument(val0.typeName(), val0.data()),
                                                            QGenericArgument(val1.typeName(), val1.data()),
                                                            QGenericArgument(val2.typeName(), val2.data()),
                                                            QGenericArgument(val3.typeName(), val3.data()));
                }
                else if(val0.isValid() && val1.isValid() && val2.isValid())
                {
                    if(hasReturn)
                        success = QMetaObject::invokeMethod(w, qstoc(member),  Qt::DirectConnection, Q_RETURN_ARG(QVariant, returnVal),
                                                            QGenericArgument(val0.typeName(), val0.data()),
                                                            QGenericArgument(val1.typeName(), val1.data()),
                                                            QGenericArgument(val2.typeName(), val2.data()));
                    else
                        success = QMetaObject::invokeMethod(w, qstoc(member),  Qt::DirectConnection,
                                                            QGenericArgument(val0.typeName(), val0.data()),
                                                            QGenericArgument(val1.typeName(), val1.data()),
                                                            QGenericArgument(val2.typeName(), val2.data()));
                }
                else if(val0.isValid() && val1.isValid())
                {
                    if(hasReturn)
                        success = QMetaObject::invokeMethod(w, qstoc(member),  Qt::DirectConnection, Q_RETURN_ARG(QVariant, returnVal),
                                                            QGenericArgument(val0.typeName(), val0.data()),
                                                            QGenericArgument(val1.typeName(), val1.data()));
                    else
                        success = QMetaObject::invokeMethod(w, qstoc(member),  Qt::DirectConnection,
                                                            QGenericArgument(val0.typeName(), val0.data()),
                                                            QGenericArgument(val1.typeName(), val1.data()));
                }
                else if(val0.isValid())
                {
                    if(hasReturn)
                        success = QMetaObject::invokeMethod(w, qstoc(member),  Q_RETURN_ARG(QVariant, returnVal),
                                                            QGenericArgument(val0.typeName(), val0.data()));
                    else
                        success = QMetaObject::invokeMethod(w, qstoc(member), QGenericArgument(val0.typeName(), val0.data()));
                }
                else
                {
                    if(hasReturn)
                        success = QMetaObject::invokeMethod(w, qstoc(member),  Q_RETURN_ARG(QVariant, returnVal));
                    else
                        success = QMetaObject::invokeMethod(w, qstoc(member));
                }
                if(success)
                {
                    if(returnVal.isNull())
                        vRet = QString("Void");
                    else
                        vRet = returnVal;
                }
                else
                    perr("error invoking slot \"%s\" on %s\n", qstoc(member), qstoc(w->objectName()));
            }
            if(indexOfMethod < 0 && indexOfProperty < 0)
                vRet = QString("MethodNotFound");
        }
    }
    return vRet;
}

QVariant TApplicationPrivate::readProperty(const QString& widgetName, const QString& propertyName)
{
    Q_Q(TApplication);
    QVariant returnVal = QString("ObjectNotFound");
    foreach(QWidget *w, q->allWidgets())
    {
        if(w->objectName() == widgetName)
        {
            /* normalize signature */
          //  printf("\e[1;32mTApplicationPrivate reading prop \"%s\" on obj \"%s\" ...\e[0m\n", qstoc(propertyName), qstoc(widgetName));
            int indexOfProperty = w->metaObject()->indexOfProperty(qstoc(propertyName));
            if(indexOfProperty > -1)
            {
                QMetaProperty metaProperty = w->metaObject()->property(indexOfProperty);
                returnVal = metaProperty.read(w);
            }
            else
                returnVal = QString("PropertyNotFound");
        }
    }
    return returnVal;
}

void  TApplicationPrivate::writeProperty(const QString& widgetName, const QString& propertyName, const QVariant& val)
{
    Q_Q(TApplication);
    foreach(QWidget *w, q->allWidgets())
    {
        if(w->objectName() == widgetName)
        {
            /* normalize signature */
          //  printf("\e[1;32mTApplicationPrivate writing prop \"%s\" type \"%s\" on obj \"%s\" ...\e[0m\n", qstoc(propertyName), val.typeName(), qstoc(widgetName));
            qDebug() << "Property val " << val;
            int indexOfProperty = w->metaObject()->indexOfProperty(qstoc(propertyName));
            if(indexOfProperty > -1)
            {
                QMetaProperty metaProperty = w->metaObject()->property(indexOfProperty);
                if(val.isValid())
                    metaProperty.write(w, val);
            }
        }
    }
}

void TApplicationPrivate::removeSecondInstanceHandler()
{
    delete d_secondInstanceHandler;
    d_secondInstanceHandler = NULL;
}

void TApplicationPrivate::onSecondInstanceDetected()
{
    if(d_secondInstanceHandler)
        d_secondInstanceHandler->filter();
}
