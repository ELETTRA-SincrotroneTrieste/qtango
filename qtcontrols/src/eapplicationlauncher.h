/* This file contains the classes related to the
 * 	
 * information about the application launched 
 * by the button and the x11 related information
 */
#ifndef X11APPLI_H
#define X11APPLI_H 

class EApplicationLauncher;
class AppliUtils;


#include <QStringList>
#include <QThread>
#include <QProcess>
#include <QPixmap>
#include <QApplication>
#include <TAppInfo> /* dbus/ */
#include <QX11Info>


/* Window manager properties */
#define NOT_EXISTING 	0
#define EXISTING	1
#define REGISTERING	2
#define RAISING		3
#define UNREGISTERED 	(-1)

/* Application state */
#define NOT_RUNNING 	0
#define RUNNING 	1
#define STARTING	2
#define BRUTALLY_EXITED 3

/* To avoid including here Xmu.h */
typedef unsigned long Window;

/** X11 related information.
 * The creation of this object is handled by the class
 * EApplicationLauncher.
 * The programmer can ask for the status of an application
 * by invoking X11Status() on this class.
 */
class X11Utils
{
	public:
		X11Utils(EApplicationLauncher* application);
		~X11Utils();
		/** The function returns the window status:
		 * NOT_EXISTING, EXISTING, REGISTERING, RAISING,
		 * UNREGISTERED.
		 */
		int X11Status() { return stato; }
		void SetX11Status(int st) { stato = st; }
		void SetWindowId(int id) { winid = id; }
		int RaiseAppWindow();
		int IconifyAppWindow();
		void SetDisplay(Display *d) { disp = d; }
		Window WindowID() { return winid; }

		/** The method returns the Window ID of the application
		 * which was launched with the specified command_args.
		 * command_args must be the exact execution command.
		 * Let this method be used internally by EApplicationLauncher
		 */
		Window QueryX11Status(QString command_args);

                /** \brief Accesses DBus session bus to test whether the application is exported.
                  *
                  * \since 4.2.0
                  *
                  * @return TAppInfo associated to command.
                  */
                TAppInfo getDBusAppInfo(const QString& command) const;

		bool RegisterApplication();
		bool UnregisterApplication();
		
	private:
		int stato; /* NOT_EXISTING or EXISTING */
		EApplicationLauncher* x11_myapplication;
		/* The X Window id */
		Window winid;
		Display* disp;
};

/** Application related utilities.
 * This class is specialized on the application
 * properties, not strictly related to the X11
 * side, but just to the application side.
 * For instance, it starts the process associated
 * to the EApplicationLauncher and stops it.
 * It is also responsible for emitting signals when 
 * an application changes its state, i.e. it gets 
 * started or exits.
 * Let this class be managed by EApplicationLauncher,
 * querying at most its status or starting/ stopping 
 * the associated process.
*/
class AppliUtils : public QObject
{
	Q_OBJECT
	public:
		/* See note on X11Utils constructor */
		AppliUtils(EApplicationLauncher* application);
		~AppliUtils();
		/** Returns the application status */
		int AppliStatus() { return stato; }
		void SetAppliStatus(int st) { stato = st; }
		/** Starts the application with the command specified. */
		bool StartProcess(QString command);
				/** Returns the exit code of the application.
		 * Querying this value makes sense only when an
		 * application has been stopped, that is when
		 * you receive the signal dedicated to such event.
		 * See the signals associated with EApplicationLauncher
		 * to learn about them and to know when to query your
		 * EApplicationLauncher to obtain the correct exit code.
		 */
		int ExitCode() { return exit_code; }
		/** A Q-specific exit status. See the ExitCode() method for
		 * querying details. 
		*/
		QProcess::ExitStatus ExitStatus() { return exit_status; }
		
	signals:
		void ApplicationFinished();
		void ApplicationBrutallyFinished();
		void ApplicationRaised(bool);
			
	public slots:
		/** Stops the application */
		bool StopProcess();
		/** kills with INT signal (not KILL ;) */
		void KillINT();
		void Kill9();

		void ProcessFinished( int exitCode, 
				QProcess::ExitStatus exitStatus);
		//void ProcessStarted(bool correctly);
	private:
		/* QProcess */
		QProcess* process;
		int stato;
		EApplicationLauncher* app_myapplication;
		int exit_code;
		QProcess::ExitStatus exit_status;
		
};

/** 
 * \brief An interface to manage the startup and the raising 
 * of an application. The user does not need to know anything
 * about the X window system infrastructure to use this interface, 
 * nor if the application is already running or not.
 * When using this class, just call 'Rise()' to see the 
 * application window appear on the screen.
 *
 * This is the interface by means of which the programmer
 * mainly interacts with his application.
 * One constructs a new object passing the command line 
 * argument of the application he wants to start.
 * Then the user is able to know when a process has been started
 * or raised or stopped (see signals below).
 *
 * If you keep alive each EApplicationLauncher object in your 
 * program, you will easily be able to keep track of 
 * the application status in every moment, knowing if it
 * is running, not running, stopped, or if it has failed to
 * start or stop.
 *
 * The behaviour of the Application launcher is configurable by the 
 * constructor.
 * There are some options which can be specified: they are all true by default.
 * If popup_on_unclean_exit a popup notifying the unclean exit of the program 
 * will not be shown. The caller will anyway receive a signal and so handle the
 * case by itself.
 * The same goes for popup_on_brutally_killed and popup_on_start_failed.
 * Have a look at the  ApplicationFinished(), ApplicationBrutallyFinished()
 * and ApplicationRaised() signals.
 *
 *
 * Since <strong>QTango version 4.2</strong>, this class begins interacting with
 * DBus.
 */
class EApplicationLauncher : public QObject
{
	Q_OBJECT
	public:
		/** The constructor will take the global path and
		 * split it into the path and the executable name.
		 * Then it will initialize the status and the X11Utils.
		 */
		EApplicationLauncher(QString path, bool popup_on_unclean_exit = true,
			bool popup_on_start_failed = true, bool popup_on_brutally_killed = true );
		
		virtual ~EApplicationLauncher();
		
		/** Sets the complete path */
		void SetCompletePath(QString completepath) { completecmd = completepath; }
                /** Returns the path name, without the executable */
		QString PathName() { return pathname; }
		/** Returns just the executable name */
		QString ExeName() { return exename; }
		/** Returns the complete command line */
		QString CompleteCommand() { return completecmd; }
		/** Returns the arguments passed to the application */
                QString Args() { return args; }
                /** Returns the full path of the executable, for instance /usr/local/bin/apanel */
                QString fullExePath();
                /** Returns the arguments passed to the application in a string list */
                QStringList arguments();
		/** Returns a pointer to the application utility class.
		 * By means of this, you can ask for the application status
		 * using the method AppliStatus() above.
		*/
		AppliUtils* AppUtils() { return apputils; }
		/** Returns a pointer to the application utility class X11-specialized. */
		X11Utils* XUtils() { return x11utils; }

		/** Starts or raises the application, if already running */
		void Rise();
			
		bool PopupUncleanExit() { return popup_unclean_exit; }
		bool PopupStartFailed() { return popup_start_failed; }
		bool PopupBrutallyKilled() { return popup_brutally_killed; }
		
		bool checkForUnique() { return unique_check_enabled; }
		void enableCheckForUnique(bool check) { unique_check_enabled = check; }
		
		void addEnvironmentVariables(const QStringList& envlist) { d_additionalEnvVars  = envlist; }
		QStringList additionalEnvironmentVariables() const { return d_additionalEnvVars; }

		
	signals:
		/** This signal is emitted when the application represented
		 * by this class has exited.
		 * A pointer to ourselves is passed so that the programmer 
	         * can obtain all the information stored in the 
		 * EApplicationLauncher object, such as the application 
		 * status (exit code for instance).
		 * Connect this signal to your own personal slot to do 
		 * everything you want when the application exits.
		 */
		void ApplicationFinished(EApplicationLauncher*);
		/** This second signal is provided to improve the object oriented
		 *  development. You should keep a reference to the 
		 *  EApplicationLauncher in your class to access the information
		 *  related with the status of the application./
		 */
		void ApplicationFinished();

		/** This signal is emitted in two cases: a new executable is 
		 * launched or an already running application is raised by
		 * the window manager.
		 * The signal does not allow to know which of the two cases
		 * happened, but gives a boolean indicator for the success
		 * of the operation.
		 * Connect this signal to your own personal slot to do 
		 * everything you want when the application starts / gets raised.
		 */
		void ApplicationRaised(EApplicationLauncher*, bool);

		/** This second signal provides only the success of the operation.
		 * Keep a handle to the EApplicationLauncher in your class to 
		 * access its information on signal reception.
		 */
		void ApplicationRaised(bool);

		/** When the user decides to kill the application with the signal 9
		 * (SIGKILL), the application emits this signal, to inform that
		 * a SIGKILL was sent to the application.
		 * Since SIGKILL cannot be ignored by a process, it is supposed that
		 * it always performs a brutal shutdown of the process itself.
		 */
		void ApplicationBrutallyFinished(EApplicationLauncher*);

		/** The version without the pointer to myself (see above) 
		 */
		void ApplicationBrutallyFinished();

	protected slots:
		void EApplicationFinished();
		void EApplicationBrutallyFinished();
		void EApplicationRaised(bool correctly);
		
	private:
		bool popup_unclean_exit;
        	bool popup_start_failed;
		bool popup_brutally_killed;
		bool unique_check_enabled;
		QString pathname;
		QString exename;
		QString args;
		QString completecmd;
		AppliUtils *apputils;
		X11Utils *x11utils;
		
		QStringList d_additionalEnvVars;
};

#endif

