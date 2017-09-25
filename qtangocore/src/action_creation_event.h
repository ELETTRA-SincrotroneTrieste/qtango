#ifndef ACTION_CREATION_EVENT
#define ACTION_CREATION_EVENT

#include <QString>
#include <QEvent>
#include <qtangocore_common.h> /* for ACTION_EVENT */

class TAction;

class ActionCreationEvent : public QEvent
{
	public:
		ActionCreationEvent(TAction* new_action) : 
			QEvent(ACTION_EVENT), d_action(new_action) {};
		virtual ~ActionCreationEvent() { }
		TAction *action() { return d_action; }
			
	private:
		TAction* d_action;
};


#endif
