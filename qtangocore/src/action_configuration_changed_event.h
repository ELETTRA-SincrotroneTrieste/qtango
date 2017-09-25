#ifndef ACTION_CONFIGURATION_CHANGED_EVENT_H
#define ACTION_CONFIGURATION_CHANGED_EVENT_H

#include <QString>
#include <QEvent>
#include <qtangocore_common.h> /* for ACTION_CONFIGURATION_CHANGED_EVENT */

class TAction;
class TangoConfigurationParameters;

class ActionConfigurationChangedEvent : public QEvent
{
	public:
                ActionConfigurationChangedEvent(QString source, const TangoConfigurationParameters* cp) :
                        QEvent(ACTION_CONFIGURATION_CHANGED),  d_source(source), d_confParams(cp) {}
			
                 ActionConfigurationChangedEvent(TAction* act, const TangoConfigurationParameters* cp) :
                        QEvent(ACTION_CONFIGURATION_CHANGED),  d_action(act), d_confParams(cp) {}
			
		const TangoConfigurationParameters *configurationParameters() const { return d_confParams; }
		const QString source() const { return d_source; }
		const TAction *action() const { return d_action; }
		
        private:
                const QString d_source;
                const TAction *d_action;
                const TangoConfigurationParameters* d_confParams;
};


#endif
