#ifndef ACTION_EXECUTION_EVENT
#define ACTION_EXECUTION_EVENT

#include <QString>
#include <QEvent>
#include <qtangocore_common.h> /* for ACTION_EXECUTION_EVENT */

class TAction;

class ActionExecutionEvent : public QEvent
{
	public:
		ActionExecutionEvent(QString source, TAction* new_action) : 
			QEvent(ACTION_EVENT), d_action(new_action), d_source(source) {};
		 
		TAction *action() { return d_action; }
		QString source() { return d_source; }
		
	private:
		TAction* d_action;
		QString d_source;
};


#endif