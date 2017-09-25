#ifndef ATTRIBUTES_CONF_MANAGER_H
#define ATTRIBUTES_CONF_MANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QSet>

#include "auto_configuration.h"
#include "communication_handle.h"

/** \brief The attribute configuration manager, for internal use.
 * 
 * This class is intended to be used internally by QTango.
 */
class AttrConfManager : public QObject
{
	Q_OBJECT
	public:
		static AttrConfManager* instance();
		
		/** \brief given the source and the subscriber handle, it registers the source for receiving auto configuration (events).
		 *
		 * @param source the tango source, in the classical form a/b/c/attribute_name (if followed by arguments inside
		 *               `()', they are ignored and removed from the source stored internally by the manager).
		 *		<strong>Note</strong>: it must be a tango <em>attribute</em>.
		 * @param subscriber the subscriber proxy in charge of registering the tango source point.
		 * @param attr_conf_change_event if true, registers the attribute for the configuration change event - now unused
		 *                               because always set to true.
		 */
		bool registerObject(QString source, QTangoCommunicationHandle *subscriber, bool attr_conf_change_event);
		
		/** \brief remove the source point from the set 
		 */
		void unregisterObject(QString source, QTangoCommunicationHandle *);
		
	protected:
		
	protected slots:
		void distributeAttributeConfiguration(const TangoConfigurationParameters *);
		
	private:
		AttrConfManager(QObject *parent);
		~AttrConfManager();
		static AttrConfManager* _instance;
		
		void triggerRefresh(QString source, QTangoCommunicationHandle *receiver);
		
		/* this map associate a source to a set of objects that 
		 * want to auto configure themselves.
		 * Assume that every source in the map is configured.
		 * When a source is removed from the map, it is first 
		 * deconfigured.
		*/
		QMap<QString, QSet<QTangoCommunicationHandle *>  > d_srcHandleMap;
		QMap<QString , QTangoAutoConfiguration*> d_configuratorForSourceMap;
		
		/* distributeAttributeConfiguration() notifies all QTangoComHandles in the set
		 * of handles associated to a source. Each variation in the set associated to
		 * a source (a handle (i.e. a QTango reader or writer) is added ),
		 * must trigger a refresh to all the elements in the set. This is normally 
		 * pretty good in the case of attribute configuration events, but not in the
		 * initial configuration phase, when each reader/writer wants to register and
		 * receive the configuration. With this interval, we avoid refreshing a reader
		 * or writer if the last refresh event was emitted in a period less than this
		 * minimum.
		 */
		int d_minConfigDistributionInterval;
		
		bool d_attrConfChangeEnabled;
};

#endif


