#include "eapplicationbutton.h"
#include <QMessageBox>
#include <QtDebug>
#include <QPixmap>
#include <QIcon>
#include <iostream>

#ifdef __FreeBSD__
    #include <X11/Xmu/WinUtil.h>
#else
    #include <Xmu/WinUtil.h>
#endif

#include <QX11Info>
#include <QFont>
#include <QToolTip>

#include "eapplicationlauncher.h"

EApplicationButton::EApplicationButton(QWidget* parent):
	        QPushButton(parent)
{
	setText(QString("Launch"));
	command = QString("");
	/* Create an EApplicationLauncher object handler */
	app = new EApplicationLauncher(command);	
	SetupButton();	
}

 /* The constructor: wants the parent, the name and
  * a string list containing all necessary stuff:
  * the argument vector, the x and y positions
  */
EApplicationButton::EApplicationButton(QWidget* parent, QString text, QString arguments) :
	        QPushButton(text, parent)
{
	command = arguments;
	/* Create an EApplicationLauncher object handler */
	app = new EApplicationLauncher(command);	
	SetupButton();	
	 
}

EApplicationButton::EApplicationButton(QWidget* parent, QString text, QString arguments, QString iconpath) : 
		QPushButton(QIcon(iconpath), text, parent)
{
	command = arguments;
	/* Create an EApplicationLauncher object handler */
	app = new EApplicationLauncher(command);	
	SetupButton();
}

EApplicationButton::~EApplicationButton()
{
	if(app)
		delete app;
}

void EApplicationButton::SetupButton()
{
	setMouseTracking(true);
//	setToolTip(QString("This button raises the application\n"
//			"\"%1\".")
//			.arg(app->ExeName() ) );
	connect(this, SIGNAL(clicked() ), this, SLOT(RiseApplication() ) );
}

void EApplicationButton::mouseMoveEvent(QMouseEvent* e)
{
	QString statustip;
	if(e->type() != QEvent::MouseMove)
		return;
	statustip = QString("\"%1\" ").arg(app->ExeName());
	switch(app->XUtils()->X11Status() )
	{
		case NOT_EXISTING:
			statustip += QString(" not running: click to start.");
			update();
			qApp->processEvents();
			break;
		case EXISTING:
			statustip += QString(" running: click to maximize.");
			update();
			qApp->processEvents();
			break;
		case REGISTERING:
			statustip += QString(" registering... please wait...");
			update();
			qApp->processEvents();
			break;
		case RAISING:
			statustip += QString(" raising... please wait...");
			update();
			qApp->processEvents();
			break;
		case UNREGISTERED:
			statustip += QString(" is running \nbut not registered...");
			update();
			qApp->processEvents();
			break;
	}
	setStatusTip(statustip);
}

void EApplicationButton::RiseApplication()
{
	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
	if(app)
		app->Rise();
	else
		qDebug() << "The application is NULL!";
	/* Restore the normal cursor and enable the button again */	
	QApplication::restoreOverrideCursor();		
	return;
}

void EApplicationButton::setCmdLine(QString cmd)
{
	command = cmd;
	if(app)
		delete app;
	app = new EApplicationLauncher(command);
}

