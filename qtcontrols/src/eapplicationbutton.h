#ifndef _PROCESS_BUTTON_H
#define _PROCESS_BUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class EApplicationLauncher;
class EApplicationButtonPrivate;

class EApplicationButton : public QPushButton
{
	Q_OBJECT
	Q_PROPERTY(QString cmdLine READ cmdLine WRITE setCmdLine)
			
	public:
		/** The base constructor */
		EApplicationButton(QWidget*);
		/** The constructor: it wants the parent, a text and
		 * a string containing 
		 * the command line. 
		 */
		EApplicationButton(QWidget*, QString, QString);
		/** The constructor: it wants the parent, a text and
		 * a string containing 
		 * the command line. This one accepts an icon specified by iconpath. 
		 */
		EApplicationButton(QWidget*, QString button_name, QString command, QString iconpath);
		
		/** The destructor. Don't care. */
		~EApplicationButton();
		
		/** Returns the command line that initialized the 
		 * class.
		 */
		QString Command() { return command; }
		
		/** A pointer to the EApplicationLauncher. */
		EApplicationLauncher* Appli() { return app; }
		
		void mouseMoveEvent(QMouseEvent* e);
		
		/** The following two are for internal use only. */
		QString cmdLine() { return command; }
		void setCmdLine(QString cmd);

	public slots:
		void RiseApplication();
		
	private:
		void SetupButton();
		QString command;
		/* EApplicationLauncher associated. */
		EApplicationLauncher *app;

        EApplicationButtonPrivate *d_ptr;
};

#endif
