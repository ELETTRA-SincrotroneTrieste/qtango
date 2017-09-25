#ifndef EUNIQUEAPPLICATION_H
#define EUNIQUEAPPLICATION_H

#include <QApplication>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusAbstractInterface>
#include <QtDBus/QDBusAbstractAdaptor>
#include "euniqueapplication_interface.h"

#define SERVICENAMELEN 200

class EUniqueApplicationPrivate;

class EUniqueApplication:  public QApplication
{	
	Q_OBJECT

	public:
		/** This is the constructor. Use it as you would use 
		 *  the QApplication one.
		 *  It builds a QApplication, and in addition it connects
		 *  to the DBUS session bus to register for the service 
		 *  "it.trieste.elettra.browser". If the registration fails,
		 *  the result is stored in an internal variable and the
		 *  execution of the application does not take place.
		 *  In order to benefit from this feature, one has to call
		 *  UniqueExec() instead of the exec().
		 *  Example:
		 *
		 *  -------------------------------------------------------------
		 *  #include "euniqueapplication.h"
		 *  #include <QMainWindow>
		 *  #include <QtDebug>
		 *
		 *  int main(int argc, char** argv)
		 *  {	
		 *      // Create an EUniqueApplication object
		 *      // instead of a traditional QApplication:
        	 *	EUniqueApplication a(argc, argv);
		 *      // Setup the interface: 
        	 *      QMainWindow *w = new QMainWindow(0);
        	 *      w->show();
		 *      // EUniqueApplication emits a signal named Rise() when 
		 *      // it discovers that another instance has been started.
		 *      // If you want to raise the first instance's window when
		 *      // one tries to start another one, just
		 *      // connect the EUniqueApplication's `Rise()' signal to the
		 *      // window's slot `raise()'. 
		 *      QObject::connect(&a, SIGNAL(Rise() ), w, SLOT(raise() ) );
		 *      // Finally call UniqueExec() instead of the exec():
                 *      return a.UniqueExec();
        	 *   }
		 *   ------------------------------------------------------------
		 */
		EUniqueApplication(int &argc, char** argv, char* servicename);
		
		virtual ~EUniqueApplication();

		/** Call this one instead of the exec().
		 * It returns QApplication::exec() if the application
		 * is unique, otherwise 0 if another instance is already
		 * running, -1 if the program failed to connect to the 
		 * session bus.
		 */	
		int UniqueExec();

		/** Returns > 0 if the application is unique and so can be started,
		 *  0 if there is another instance already running, -1 if there is 
		 *  a problem on the D-Bus session connection.
		 */

		int CanStart() { return can_start; }
		/* Returns true if the DBus message bus is connected and if the 
		 * registration for the service represented by the application
		 * succeeded.
		 */
		bool RegistrationSucceeded() { return registration_succeeded; }

		QString ServiceName() { return QString(service_name); }
	private:
		int can_start;
		bool registration_succeeded;
		char service_name[SERVICENAMELEN];

	private slots:
		void riseActionSlot(const QString& message);

	signals:
		/** Connect this signal with any slot you want to be executed when 
		 * the application discovers that another instance of itself has
		 * been launched.
		 */
		void Rise();
		void rise_signal(const QString &rise_mess);

private:
        EUniqueApplicationPrivate *d_ptr;
};

#endif

