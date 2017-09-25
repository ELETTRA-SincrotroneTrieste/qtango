#include "euniqueapplication.h"
#include "euniqueapplication_adapter.h"
#include <QtDebug>
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <signal.h>
#include <unistd.h>
#include "macros.h"

EUniqueApplication::EUniqueApplication(int &argc, char** argv, char* service_nam)
	 :  QApplication(argc, argv)
{
	registration_succeeded = false;
	memset(service_name, 0, SERVICENAMELEN);
	strncpy(service_name, service_nam, SERVICENAMELEN-1);
	// qDebug() << "Setting up DBUS communication...";

	can_start = -1; /* undefined */
	if (!QDBusConnection::sessionBus().isConnected()) 
	{
        	qWarning("Cannot connect to the D-BUS session bus.\n"
                 	"Please check your system settings and try again.\n");
        	can_start = -1;
		QMessageBox::critical(0, "Browser: error connecting with DBUS", 
			"Cannot connect to the D-BUS session bus.\n"
			"Please check your system settings and try again.\n"
			"Try executing \"eval `dbus-launch --auto-syntax`\".");
		_exit(EXIT_FAILURE);
	}

	/* Connect to the session bus */
	new EUniqueApplicationAdapter(this);
	QDBusConnection::sessionBus().registerObject("/", this);
	
	it::trieste::elettra::xxx_interface *interface;
	
	interface = new /*it::trieste::elettra::xxx_interface */
			ItTriesteElettraXXXInterface(QString(), QString(), QDBusConnection::sessionBus(), this);

	connect(interface, SIGNAL(rise_signal(QString)), this, SLOT(riseActionSlot(QString)));

	QString service = QString(INTERFACE_NAME + QString(".")  + QString(service_name) );
	if(! QDBusConnection::sessionBus().registerService(service ) )
	{
		qDebug() << QString("Browser: registration failed for service \"%1\" (application already running?)").arg(service);
		can_start = 0;
		registration_succeeded = false;
		emit rise_signal(QString("rise"));
		/* QApplication::exit() is not like the stdlib _exit(): it does not return to the caller:
 		 * it is the event processing that stops. So I call the _exit() of the stdlib */
		qDebug() << "\e[1;35mExiting\e[0m.";
		_exit(EXIT_SUCCESS);
	}
	else
	{
		// qDebug() << QString("Registration succeded for service \"%1\"").arg(service);
		can_start = 1;
		registration_succeeded = true;
	}
}

void EUniqueApplication::riseActionSlot(const QString& message)
{
	if(message == "rise")
	{
		// qDebug() << "Telling the application to rise!";
		emit Rise();
	}
}

EUniqueApplication::~EUniqueApplication()
{
	QString service = QString(INTERFACE_NAME + QString(".")  + QString(service_name) );
	if(registration_succeeded)
	{
	   pok("euniqueapplication: unregistering \"%s\" from session bus", qstoc(service));
		if(QDBusConnection::sessionBus().unregisterService(service ) )
			; // qDebug() << QString("Service \"%1\" unregistered.").arg(service);
		else
		{	
			QString s;
			qDebug() << QString("Failed to unregister service \"%s\"!").arg(service);
			s = QString("Name: %1 Message %2").arg(QDBusConnection::sessionBus().lastError().name()).
				arg(QDBusConnection::sessionBus().lastError().message());
			perr("euniqueapplication: failed to unsubscribe service \"%s\" from the session bus", qstoc(service));
			perr("euniqueapplication: the last error message was: \"%s\"", qstoc(s));
		}
	}
}

int EUniqueApplication::UniqueExec()
{
	if(can_start > 0 )
	{
		return QApplication::exec();
	}
	else
	{	
		return can_start;
	}
}


