#ifndef ACTION_CONFIGURATION_FAILED_EVENT_H
#define ACTION_CONFIGURATION_FAILED_EVENT_H

#include <QString>
#include <QEvent>
#include <qtangocore_common.h> /* for ACTION_EVENT */

class ActionConfigurationFailedEvent : public QEvent
{
	public:
		ActionConfigurationFailedEvent(QString source) : 
			QEvent(ACTION_CONFIGURATION_FAILED_EVENT), d_source(source) {};

		QString sourceName() { return d_source; }
		
	private:
		QString d_source;
};


#endif
