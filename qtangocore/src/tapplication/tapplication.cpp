#include "tapplication.h"
#include <macros.h>
#include <QWidget>
#include "private/tapplication_p.h"
#include <dbus/tapplicationspoolproxy.h>
#include <dbus/TApplicationInterface.h>
#include <sys/types.h>
#include <unistd.h>

/*
void signal_handler(int signo)
{
  switch(signo)
  {
	case SIGTERM:
	  printf("\e[1;32m!\e[0m closing application in signal (TERM) handler\e[0m\n", signo);
	  QCoreApplication::instance()->quit();
	  break;
	case SIGINT:
	  printf("\e[1;32m!\e[0m closing application in signal (INT) handler\e[0m\n", signo);
	  QCoreApplication::instance()->quit();
	  break;
  }
}
*/

void TApplication::init()
{
  QObject::setProperty("unique", false);
  TApplicationPrivate* privatePtr = new TApplicationPrivate(this);
  Q_UNUSED(privatePtr);
  d_aboutToQuit = false;
  connect(this, SIGNAL(aboutToQuit()), this, SLOT(setAboutToQuit()));
}

TApplication::TApplication ( int & argc, char ** argv ) : QApplication(argc, argv)
{
  init();
  QTimer::singleShot(m_generateDBusTimeout(), this, SLOT(slotInitializeDBus()));
}

TApplication::TApplication (bool doNotInit, int & argc, char ** argv ) : QApplication(argc, argv)
{
    if(!doNotInit)
    {
        init();
        QTimer::singleShot(m_generateDBusTimeout(), this, SLOT(slotInitializeDBus()));
    }
}

pid_t TApplication::applicationPid()
{
    return getpid();
}

int TApplication::m_generateDBusTimeout() const
{
    int to = rand() / (float) RAND_MAX * 2500;
    to += 1000; /* always wait at least one second */
    return to;
}

void TApplication::setAboutToQuit() 
{ 
  d_aboutToQuit = true; 
}

void TApplication::setApplicationVersion(const QString &version)
{
  d_applicationVersion = version;
  d_applicationVersion.remove("$Name: release_5_4_0 $");
}

void TApplication::initDbus()
{
    TAPP_D(TApplication);
    d->initDbus();
}

QStringList TApplication::properties(const QString& widgetName)
{
    TAPP_D(TApplication);
    return d->properties(widgetName);
}

QStringList TApplication::methods(const QString& widgetName)
{
    TAPP_D(TApplication);
    return d->methods(widgetName);
}

QString TApplication::executable()
{
    TAPP_D(TApplication);
    return d->executable();
}

QStringList TApplication::arguments()
{
    return QCoreApplication::arguments();
}

QStringList TApplication::parameters()
{
   TAPP_D(TApplication);
   return d->parameters();
}

QStringList TApplication::widgetList()
{
    TAPP_D(TApplication);
    return d->widgetList();
}


QVariant TApplication::invokeMethod4( const QString& widgetName, const QString& member,
                   const QVariant& val0,
                   const QVariant& val1, const QVariant& val2,
                   const QVariant& val3)
{
    TAPP_D(TApplication);
    return d->invokeMethod(widgetName, member, val0, val1, val2, val3, QVariant());
}

QVariant TApplication::invokeMethod3( const QString& widgetName, const QString& member,
                   const QVariant& val0,
                   const QVariant& val1, const QVariant& val2)
{
    TAPP_D(TApplication);
    return d->invokeMethod(widgetName, member, val0, val1, val2, QVariant(), QVariant());
}

QVariant TApplication::invokeMethod2( const QString& widgetName, const QString& member,
                   const QVariant& val0,
                   const QVariant& val1)
{
    TAPP_D(TApplication);
    return d->invokeMethod(widgetName, member, val0, val1, QVariant(), QVariant(), QVariant());
}

QVariant TApplication::invokeMethod1( const QString& widgetName, const QString& member,
                   const QVariant& val0)
{
    TAPP_D(TApplication);
    return d->invokeMethod(widgetName, member, val0, QVariant(), QVariant(), QVariant(), QVariant());
}

QVariant TApplication::invokeMethod0( const QString& widgetName, const QString& member)
{
    TAPP_D(TApplication);
    return d->invokeMethod(widgetName, member, QVariant(), QVariant(), QVariant(), QVariant(), QVariant());
}


QVariant TApplication::invokeMethod5( const QString& widgetName, const QString& member,
                   const QVariant &val0,
                   const QVariant& val1, const QVariant& val2,
                   const QVariant& val3, const QVariant& val4)
{
    TAPP_D(TApplication);
    return d->invokeMethod(widgetName, member, val0, val1, val2, val3, val4);
}


QVariant TApplication::readProperty(const QString& widgetName, const QString& propertyName)
{

    TAPP_D(TApplication);
    return d->readProperty(widgetName, propertyName);
}

void TApplication::writeProperty(const QString& widgetName, const QString& propertyName, const QVariant& val)
{
    TAPP_D(TApplication);
    return d->writeProperty(widgetName, propertyName, val);
}

void TApplication::messageReceived(const QString &msg)
{
  printf("\e[1;32m|'.'|\e[0m received message \"%s\"\n", qstoc(msg));
}

QString TApplication::composeDbusObjectName()
{
  QString appName = applicationName();
  if(appName.isEmpty())
	appName = arguments().at(0);
  if(appName.contains("/"))
	appName = appName.split("/").last();
  return appName;
}

void TApplication::raise()
{
  qDebug() << topLevelWidgets();
  foreach(QWidget *w, topLevelWidgets())
  {
//      printf("\"%s\" top level widget \"%s\" class %s\n", qstoc(applicationName()), qstoc(w->objectName()),
//             w->metaObject()->className());
        qDebug() << w->objectName() << w << w->metaObject()->className();
        w->show();
	w->activateWindow();
	w->raise();
  }
}

SecondInstanceHandler* TApplication::secondInstanceHandler()
{
    TAPP_D(TApplication);
    return d->secondInstanceHandler();
}

void TApplication::installSecondInstanceHandler(SecondInstanceHandler *sif)
{
    TAPP_D(TApplication);
    d->installSecondInstanceHandler(sif);
}

void TApplication::onSecondInstanceDetected()
{
    TAPP_D(TApplication);
    d->onSecondInstanceDetected();
}

QString TApplication::dbusServiceName() const
{
    TAPP_D(TApplication);
    return d->dbusServiceName();
}

QString TApplication::dbusPath() const
{

    TAPP_D(TApplication);
    return d->dbusPath();
}



