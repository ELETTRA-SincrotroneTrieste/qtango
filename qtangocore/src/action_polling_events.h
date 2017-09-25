#ifndef ACTION_POLLING_EVENTS
#define ACTION_POLLING_EVENTS

#include <QEvent>
#include <qtangocore_common.h> /* for ACTION_EXECUTION_EVENT */


class ActionStopPollingEvent : public QEvent
{
	public:
		ActionStopPollingEvent() : QEvent(ACTION_STOP_POLLING_EVENT) {};
};


class ActionRestartPollingEvent : public QEvent
{
	public:
		ActionRestartPollingEvent() : QEvent(ACTION_RESTART_POLLING_EVENT) {};
};

#endif