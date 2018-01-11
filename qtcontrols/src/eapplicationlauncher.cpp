#include <QtDebug>
#include <QStringList>
#include <macros.h>
#include <dbus/TApplicationInterface.h>
#include <dbus/tapplicationspoolproxy.h>
#include <QMessageBox>
#include <QApplication>

#ifdef __FreeBSD__
    #include <X11/Xmu/WinUtil.h>
#else
    #include <Xmu/WinUtil.h>
#endif


#include "eapplicationlauncher.h"

//#define X11DEBUG 1

#ifdef X11DEBUG
#include <iostream>
#endif

using namespace std;


EApplicationLauncher::EApplicationLauncher(QString path,
		bool popup_on_unclean_exit,
		bool popup_on_start_failed, 
		bool popup_on_brutally_killed)
{
	QStringList complete_path;
	QString comm;
	QStringList list_cmd;
	unsigned pathlen;
	popup_unclean_exit =  popup_on_unclean_exit;
	popup_start_failed = popup_on_start_failed;
	popup_brutally_killed = popup_on_brutally_killed;
	/* Let's separate command path from arguments */
	completecmd = path;
        list_cmd = path.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if(list_cmd.size())
        {
            comm = list_cmd.first();
            args = path.remove(comm);
            complete_path = comm.split('/');
            pathlen = complete_path.size();
            if(pathlen > 0)
                exename = complete_path.last();
            else
                exename = comm;
            pathname = comm.remove(exename);
        }
        unique_check_enabled = true;
// 	cout << "Application: \e[1;32m" << exename.toStdString() << "\e[0m  [" <<
// 			pathname.toStdString() << "]" << endl;
	/* Associate application and X11 info to the application object */
	apputils = new AppliUtils(this);
	x11utils = new X11Utils(this);
	
	connect(apputils, SIGNAL(ApplicationFinished() ), this, SLOT(EApplicationFinished() ) );
	connect(apputils, SIGNAL(ApplicationBrutallyFinished() ), this, SLOT(EApplicationBrutallyFinished() ) );
	connect(apputils, SIGNAL(ApplicationRaised(bool) ), this, SLOT(EApplicationRaised(bool) ) );
}

EApplicationLauncher::~EApplicationLauncher()
{
	delete apputils;
	delete x11utils;
}

/* This slot emits the ApplicationFinished(EApplicationLauncher* ) signal */
void EApplicationLauncher::EApplicationFinished()
{
	emit ApplicationFinished(this);
	emit ApplicationFinished();
}

/* This slot emits the ApplicationBrutallyFinished(EApplicationLauncher* ) signal */
void EApplicationLauncher::EApplicationBrutallyFinished()
{
	emit ApplicationBrutallyFinished(this);
	emit ApplicationBrutallyFinished();
}


/* This slot emits the ApplicationRaised(EApplicationLauncher*, bool  ) signal */
void EApplicationLauncher::EApplicationRaised(bool correctly)
{
	emit ApplicationRaised(this, correctly);
	emit ApplicationRaised(correctly);
}

QString EApplicationLauncher::fullExePath()
{
    if(completecmd.contains(QRegExp("\\s+")))
        return completecmd.split(QRegExp("\\s+")).first();
    return completecmd;
}

QStringList EApplicationLauncher::arguments()
{
    QStringList args = completecmd.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if(args.size())
        args.removeFirst();
    return args;
}

void EApplicationLauncher::Rise()
{
	Window existing;
	QString command = completecmd;
	if(command == "")
	{
		QMessageBox::information(0, "Error", QString("The command \"%1\" is not a "
			"valid command!\n"
			"You must call setCmdLine() with a valid application name on your launcher!")
			.arg(command) );
		return;
	}
	
	/* See if the application is already alive, started by another
	 * launcher...
	 */
	XUtils()->SetX11Status(REGISTERING);
//        TAppInfo dbusAppInfo = XUtils()->getDBusAppInfo(command);
        /* first check whether the application is registered with DBus (QTango version > 4.2.0 ) */
//        if(!dbusAppInfo.isEmpty())
//        {
//            TApplicationInterface *tai = new TApplicationInterface(dbusAppInfo.dbusServiceName, dbusAppInfo.dbusServicePath,
//                QDBusConnection::sessionBus(), 0);
//            tai->raise();
//            printf("\e[1;34mRAISING WITH DBUS!\e[0m\n");
//                XUtils()->SetWindowId(existing);
//            /* Raise it */
//            XUtils()->SetX11Status(EXISTING);
//            emit ApplicationRaised(this, true);
//            emit ApplicationRaised(true);
//            delete tai;
//        }
//        else

        if(unique_check_enabled && (existing = XUtils()->QueryX11Status(command) ) != 0)
	{
                qprintf("\e[1;32munique_check_enabled && (existing = XUtils()->QueryX11Status(command) ) != 0\e[0m\n");
		XUtils()->SetWindowId(existing);
		/* Raise it */
		XUtils()->SetX11Status(RAISING);
		XUtils()->RaiseAppWindow();
		XUtils()->SetX11Status(EXISTING);
		emit ApplicationRaised(this, true);
		emit ApplicationRaised(true);
		/* done */
	}
	else /* application is not running: start it */
	{
                qprintf("\e[1;32mApplication not running: starting it\e[0m\n");
		XUtils()->SetX11Status(STARTING);
		if(AppUtils()->StartProcess(command) )
		{			
			if(XUtils()->X11Status() != NOT_EXISTING)
				XUtils()->SetX11Status(EXISTING);
			emit ApplicationRaised(this, true);
			emit ApplicationRaised(true);
		}
		else
		{
			if(PopupStartFailed() )
				QMessageBox::critical(0, "Error", 
					QString("The process \"%1\" failed to start!").arg(command) );
			XUtils()->SetX11Status(NOT_EXISTING);
			emit ApplicationRaised(this, false);
			emit ApplicationRaised(false);
		}
        }
	return;
}

/* Starts the application and registers the
 * associated window id.
 */
bool AppliUtils::StartProcess(QString command)
{
	bool started_ok = false;
#ifdef X11DEBUG
	cout << "StartProcess(): starting \"" <<
			command.toStdString() << " ..."; 
#endif
	/* Start the process, pass our button as QObject */
	if(app_myapplication && app_myapplication->additionalEnvironmentVariables().size() > 0)
	{
		QStringList modifiedEnviron;
		QStringList currentEnv = process->systemEnvironment();
		modifiedEnviron = currentEnv + app_myapplication->additionalEnvironmentVariables();
		process->setEnvironment(modifiedEnviron);
#ifdef X11DEBUG
		printf("starting %s con env:\n", command.toStdString().c_str());
		foreach(QString s, modifiedEnviron)
			printf("%s, ", s.toStdString().c_str());
		printf("\n");
#endif
	}

	
	process->start(command);
	stato = STARTING;
	/* This will block until the application is started */
	started_ok = process->waitForStarted(); /* 30 secs timeout */
	if(!started_ok)
	{
		stato = NOT_RUNNING;
#ifdef X11DEBUG
		cout << "[\e[1;31mFAILED\e[0m]" << endl;
#endif
		emit ApplicationRaised(false);
		return false;
	}
	else
	{
#ifdef X11DEBUG
		cout << "[\e[1;32mOK\e[0m]" << endl;
#endif
		stato = RUNNING;
		emit ApplicationRaised(true);
		return true;
	}
	return false;
}

/* Stops the application and unregisters the
 * associated window id.
 */
bool AppliUtils::StopProcess()
{
	int response;
	/* Stop the application with Qt */
#ifdef X11DEBUG
	cout << "Trying to stop the process: " << app_myapplication->ExeName().toStdString() <<
			" (waiting for it to close [max 30secs.])..." << endl;
#endif
	if(process->pid() > 1)
	{

		process->terminate();
		if(process->waitForFinished() )
		{
#ifdef X11DEBUG
			cout << "[\e[1;32mOK\e[0m]" << endl;
#endif
			stato = NOT_RUNNING;
			return true;
		}
		else
		{
#ifdef X11DEBUG
			cout << "\t[\e[1;31mFAILED\e[0m]" << endl;
#endif
			QApplication::restoreOverrideCursor();
			response = QMessageBox::critical(0, "Error", 
					QString("There was an error stopping the process\n%1!\n"
						"Do you want to KILL it?").
					arg(app_myapplication->CompleteCommand() ),
					"Yes, KILL it", "No, I will close it by myself later");
			switch(response)
			{
				case 0:
					KillINT();
					stato = NOT_RUNNING;
					app_myapplication->XUtils()->SetX11Status(NOT_EXISTING);
					return true;
				case 1:
					return false;
			}
			/* stato remains untouched */
		}
	}
	return false;	
}

void AppliUtils::KillINT()
{
	Q_PID pid;
	pid = process->pid();
	if(pid <= 1)
	{
		QMessageBox::information(0, "warning", QString("cannot kill a process with pid %1.\n"
			"This is strange Try to reproduce this situation and contact the author!"));
		return;
	}
#ifdef X11DEBUG
	cout << "\e[1;31mKILLING THE PROCESS " <<
			app_myapplication->CompleteCommand().toStdString() <<
			" [PID: " << pid << "] WITH SIGNAL INTERRUPT (SIGINT)!\e[0m" << endl;
#endif
	QProcess killer;
	killer.start(QString("kill -INT %1").arg(pid) );
#ifdef X11DEBUG
	cout << "\e[0;31mWaiting for the process to be killed... \e[0m";
#endif
	if(killer.waitForFinished() )
#ifdef X11DEBUG
		cout << "[\e[1;32mOK\e[0m]" << endl;
#else
		;
#endif
	else
	{
#ifdef X11DEBUG
		cout << "\t[\e[1;31mFAILED\e[0m]" << endl;
#else
		;	
#endif
	}
	return; 
}

void AppliUtils::Kill9()
{
	Q_PID pid;
	pid = process->pid();

	if(pid <= 1)
	{
		QMessageBox::information(0, "warning", QString("cannot kill a process with pid %1.\n"
			"This is strange Try to reproduce this situation and contact the author!"));
		return;
	}

	int choice = QMessageBox::question(0, "Brutally kill the program?", "This action will force the application to quit.\n"
			"It should not be used unless you are perfectly sure that the application\n"
			"does not quit with the signal INTERRUPT. Would you like to try to interrupt it first?",
			"Yes, brutally kill it",
			"Cancel, don't kill.");
	switch(choice)
	{
		case 0:
			break;
		default:
			QMessageBox::information(0, "Try closing the window.", "Try closing the window and see if the program exits now.");
			return;
	}
#ifdef X11DEBUG
	cout << "\e[1;31mKILLING THE PROCESS " <<
			app_myapplication->CompleteCommand().toStdString() <<
			" [PID: " << pid << "] WITH SIGNAL KILL (SIGKILL)!\e[0m" << endl;
#endif
	QProcess killer;
	killer.start(QString("kill -INT %1").arg(pid) );
#ifdef X11DEBUG
	cout << "\e[0;31mWaiting for the process to be killed... \e[0m";
#endif
	if(killer.waitForFinished() )
	{
#ifdef X11DEBUG
		cout << "[\e[1;32mOK\e[0m]" << endl;
	
#else
		;
#endif
		stato = BRUTALLY_EXITED;
		emit(ApplicationBrutallyFinished() );
	}
	else
	{
#ifdef X11DEBUG
		cout << "\t[\e[1;31mFAILED\e[0m]" << endl;
#else
		;	
#endif
	}
	return; 
}

/* This is called when the process is finished */
void AppliUtils::ProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
#ifdef X11DEBUG
	cout << "\e[1;33mThe process " << app_myapplication->ExeName().toStdString() <<
			" has finished with code " << exitCode << " and status: " << 
			exitStatus << "\e[0m" << endl;
#endif
	if(exitCode != 0 && app_myapplication->PopupUncleanExit() )
		QMessageBox::information(0, "Warning: processed exited abnormally", 
			QString("The application %1 \nhas returned the"
			" code %2 [status %3]: it did not successfully stop!").
					arg(app_myapplication->ExeName() ).
					arg(exitCode).arg(exitStatus) );
	/* Update the values on X11 structure */
	app_myapplication->XUtils()->SetDisplay(NULL);
	app_myapplication->XUtils()->SetWindowId(0);
	app_myapplication->XUtils()->SetX11Status(NOT_EXISTING);
	/* Store the exit code and the exit status of the process */
	exit_code = exitCode;
	exit_status = exitStatus;
	/* if stato is BRUTALLY_EXITED, another signal has been emitted to
	 * indicate the KILL has been sent.
	 */
	if(stato != BRUTALLY_EXITED)
		emit ApplicationFinished();
	/* Now we can restore the status to NOT_RUNNING */
	stato = NOT_RUNNING;
}

/* X11Utils constructor: when the button is created, the application is not
 * running, and so its window not existing.
 */
X11Utils::X11Utils(EApplicationLauncher* app)
{
	stato = NOT_EXISTING;
	x11_myapplication = app;
	winid = 0;
	disp = NULL;
}

X11Utils::~X11Utils()
{
	
}

bool X11Utils::RegisterApplication()
{
	//cout << "Getting \e[1;33mWindow ID\e[0m...";
	winid = QueryX11Status(x11_myapplication->CompleteCommand() );
	//cout << "\e[1;36m" << winid << "\e[0m" << endl;
	if(winid != 0)
		return true;
	else
		return false;
}

bool X11Utils::UnregisterApplication()
{
	SetX11Status(NOT_EXISTING);
	SetWindowId(0);
	return true;
}

TAppInfo X11Utils::getDBusAppInfo(const QString& command) const
{
//    printf("\e[1;35m checking if command %s is exported to dbus\e[0m\n", qstoc(command));
    TApplicationsPoolProxy *tapp = new TApplicationsPoolProxy(0);
    TAppInfo appInfo;
    QStringList args = command.split((QRegExp("\\s+")), QString::SkipEmptyParts);
    if(args.size())
    {
        QString executable = args.at(0).split("/", QString::SkipEmptyParts).last();
        args.removeFirst();
//        printf("executable %s, params\n", qstoc(executable));
        qDebug() << "args "<< args;
        appInfo = tapp->findApplicationByNameArgs(executable, args);
        if(!appInfo.isEmpty())
        {
//            printf("\e[0;32m!! Found registered with dbus app \"%s\" service \"%s\"!\e[0m \n", qstoc(appInfo.appName), qstoc
//                   (appInfo.dbusServiceName));

        }
    }
    delete tapp;
    return appInfo;
}

/* This is a private function: it invokes X11.
 * It looks for an X11 window with a registered WM_COMMAND property.
 * It returns the status of the window: EXISTING or NOT_EXISTING.
 */
Window X11Utils::QueryX11Status(QString command)
{
	int i, k;
	unsigned int j;
	unsigned int num_children;
	Window* children_list;
	Window dummy, client;
	char **argv;
	int argc;
	/* An integer revealing if nonzero the good end of a function */
	Status wstatus, wstatus2;
	/* Obtain the Display */

#if (QT_VERSION < 0x050000)
    disp = ( (QWidget*) QApplication::desktop() )->x11Info().display();
#else
    disp = QX11Info::display();
#endif
	/* Look for the application on all screens */
#ifdef X11DEBUG
	cout << "\n{" << endl << " Number of displays: " << ScreenCount(disp) << endl;
#endif

	for(i = 0; i < ScreenCount(disp); i++)
	{
		/* Get the list of windows */
		wstatus = XQueryTree(disp, RootWindow(disp,i), &dummy, &dummy,
				     &children_list, &num_children);
#ifdef X11DEBUG
		cout << "  Display " << i + 1 << ": num children: " <<  num_children << endl;
#endif
		if(wstatus != 0)
		{
			for(j = 0; j < num_children; j++)
			{
				/* Client is a variable of type 'Window' */
				client = XmuClientWindow(disp, children_list[j]);
				if(client != None)
				{
					/* get XA_WM_COMMAND property. */
					wstatus2 = XGetCommand(disp, client, &argv, &argc);
					if(wstatus2 != 0)
					{
						QString comm("");
						/* There might be a wstatus2 != 0 together with argv NULL
						 * and argc == 0! Take care.
						 */
						if(argv != NULL)
						{
							/* Place argv in QString comm */
							comm = argv[0];
							/* arguments have to be separated by spaces */
							for(k = 1; k < argc; k++)
								comm = comm + " " + argv[k];
						}
					#ifdef X11DEBUG
						cout << "   Comparing \"\e[1;35m" << comm.toStdString() 
								<< "\e[0m\" with \"\e[1;34m" <<
								command.toStdString() << "\e[0m\"" << endl;
					#endif 
						if(QString::compare(comm, command) == 0)
						{
						#ifdef X11DEBUG
							cout << "   Found " << command.toStdString() 
									<< endl;
						#endif
							XFreeStringList(argv);
							XFree((void *)children_list);
							/* return now */
						#ifdef X11DEBUG
							cout << "}" << endl;
						#endif
							return client;
						}
						XFreeStringList(argv);
					}
				}
 
			} /* j: num children */
		}
		XFree( (void *)children_list);
	} /* i: look on all screens */
#ifdef X11DEBUG
	cout << "}" << endl;
#endif
	client = 0;
	return client;
}

int X11Utils::RaiseAppWindow()
{
	if(disp != NULL && winid != 0)
	{
#ifdef X11DEBUG
		cout << "\e[1;32mRaising window with id " << winid << ", named " <<
				x11_myapplication->ExeName().toStdString() << "\e[0m" << endl;
#endif
		XMapWindow(disp, winid); //de-iconify
		XRaiseWindow(disp, winid);//put on top of stack of visisibility
		return 0;
	}
	else
	{
#ifdef X11DEBUG
		cout << "\e[1;31mWindow id is 0!\e[0m" << endl;
#endif
		return -1;
	}
		
}

int X11Utils::IconifyAppWindow()
{
	return 0;
}

/* AppliUtils constructor: when the button is created, the application is not
 * running.
 */
AppliUtils::AppliUtils(EApplicationLauncher* app)
{
	stato = NOT_RUNNING;
	app_myapplication = app;
	/* Allocate the new QProcess */
	process = new QProcess();
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus) ), this, 
		SLOT(ProcessFinished(int, QProcess::ExitStatus) ) );
	
	/* Signal parent that the process is finished.
	 * In this way the parent can signal to the
	 * upper class..
	*/
// 	connect(this, SIGNAL(ApplicationFinished() ), app_myapplication, 
// 		SLOT(ApplicationFinished() ) );
	
	exit_code = 0;
	exit_status = QProcess::NormalExit;
}

AppliUtils::~AppliUtils()
{
    if(process->state() == QProcess::NotRunning)
        delete process;
    else
        perr("AppliUtils.~AppliUtils: cannot destroy a process which is still running [%s] %p",
             qstoc(app_myapplication->CompleteCommand()), process);
}

