#ifndef _QTANGOCORE_COMMON_H
#define _QTANGOCORE_COMMON_H

#include <tango.h>

using namespace Tango;

#define QMESSAGEBOXEVENT        (QEvent::Type) 2000
#define QENABLEEVENT					(QEvent::Type) 2001
#define QSHOWLOGSEVENT					(QEvent::Type) 2002

#define ADDSOURCEEVENT        				(QEvent::Type) 3000
#define REMOVESOURCEEVENT     				(QEvent::Type) 3001
#define EXITLOOPEVENT		  			(QEvent::Type) 3010

#define ACTION_EVENT					(QEvent::Type) 4000
#define ACTION_POLLING_PERIOD_CHANGE_EVENT 		(QEvent::Type) 4001
#define ACTION_EXECUTION_EVENT				(QEvent::Type) 4002
#define ACTION_CONFIGURATION_FAILED_EVENT	(QEvent::Type) 4003
#define ACTION_CONFIGURATION_CHANGED	(QEvent::Type) 4005

/* the following two are used to stop and restart polling of an action in POLLED_MODE
 * They are used between the communication handle (show/hide events) and the handle's
 * TAction. See action_polling_events.h
 */
#define ACTION_RESTART_POLLING_EVENT			(QEvent::Type) 4103
#define ACTION_STOP_POLLING_EVENT			(QEvent::Type) 4104

#define ACTION_NEWDATA_EVENT				(QEvent::Type) 4105

#define ATTRIBUTE_CONFIGURATION_QEVENT			(QEvent::Type) 4110

#define GLOBAL_REFRESH_TIMER_EVENT			(QEvent::Type) 4120
#define GLOBAL_REFRESH_TIMER_CHANGE_PERIOD_EVENT	(QEvent::Type) 4121

#define MAX_LOGS 100

#define QTANGOCORE_VERSION_3	1


/**
 * \brief List of possibilities to refresh mode.
 *
 * Enum used to indicate refresh mode for a TDataViewer object,
 * AUTO_REFRESH means that that the event mode is tried first, if
 * something fails the system falls back to polled mode
 */
enum RefreshMode
{
	AUTO_REFRESH= 0x0000,
	USER_EVENT_REFRESH = 0x0001,
	CHANGE_EVENT_REFRESH = 0x0002,
	PERIODIC_EVENT_REFRESH = 0x0004,
	ARCHIVE_EVENT_REFRESH = 0x0008,
	POLLED_REFRESH = 0x0010,
	MANUAL_REFRESH = 0x0020,
 	TARGET = 0x0040
};
		
/**
 * Enum indicating the type of data to be shown
 */
enum DataType 
{ 
	INVALID=0,
	ATTRIBUTE,
	COMMAND,
	DUPLICATE
};

#endif
