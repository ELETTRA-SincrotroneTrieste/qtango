#include "attributes_configuration_manager.h"
#include <stdlib.h>
#include <QCoreApplication>
#include "qtangocore_macros.h"
#include "communication_handle.h"

#include <QtDebug>

AttrConfManager* AttrConfManager::_instance = NULL;

AttrConfManager* AttrConfManager::instance()
{
	if(_instance == NULL)
	{
		_instance = new AttrConfManager(QCoreApplication::instance());
	}
	return _instance;
}

AttrConfManager::AttrConfManager(QObject *parent) : QObject(parent)
{
	d_minConfigDistributionInterval = 3; /* hope 3 seconds are good enough */
}
		
AttrConfManager::~AttrConfManager()
{
	
}

bool AttrConfManager::registerObject(QString source, QTangoCommunicationHandle *subscriber, bool attr_conf_change_event)
{
	/* clean the source removing its arguments, if present. Auto configuration does not have to know anything about 
	 * the arguments of the attribute, used to write values in the attribute itself.
	 */
	QRegExp argsRe("\\(.*\\)");
	source.remove(argsRe);
	
	if(qobject_cast<QTangoCommunicationHandle *>(subscriber) == NULL)
	{
		perr("AttrConfManager::registerObject: you must register an object of type QTangoCommunicationHandle");
		perr("The object passed is not valid and will not be considered");
		return false;
	}
	qDebug() << "registerObject(): thread: " << QThread::currentThread() << " subscr " << subscriber << 
			"source " << source;
	
	d_attrConfChangeEnabled = attr_conf_change_event;
	if(d_srcHandleMap.contains(source)) /* the source was previously registered */
	{
		/* 1. obtain the set associated to the source */
		QSet<QTangoCommunicationHandle *> set =   d_srcHandleMap[source];
		if(set.isEmpty()) /* there is no Object in the set, i.e. there is no set associated */
		{
			perr("configuration manager: found an empty set for the source \"%s\" which should yet contain one",
			       qstoc(source));
		}
		else /* there is already a set with that source associated: add the object */
		{
			padd("configuration manager: adding %s to a set containing already %d elements",
			       qstoc(source), set.size());
			set.insert(subscriber); /* another subscriber: add it to the set */
			/* explicitly trigger a refresh when a new objects registers to a 
			 * set already created and filled in. In this condition (set created
			 * and with at least one element), we should be able to have a 
			 * QTangoAutoConfiguration object associated - created in the `else'
			 * branch below - and so probably distributeAttributeConfiguration() 
			 * had arrived before the new subscriber was inserted.
			 */
			d_srcHandleMap[source] = set;
			triggerRefresh(source, subscriber);
		}
	}
	else
	{
		/* the source is not in the map => not subscribed for auto configuration */
		/* 1. create the set */
		QSet<QTangoCommunicationHandle *> set;
		set.insert(subscriber);
		/* 2. create the auto configuration for the source */
		/* true is to enable configuration event */
		QTangoAutoConfiguration *autoConf = new QTangoAutoConfiguration(this, d_attrConfChangeEnabled);
		/* 3. insert the new couple source, set into the map. */
		d_srcHandleMap[source] = set;
		/* 4. insert the couple source, QTangoAutoConfiguration into the map */
		d_configuratorForSourceMap[source] = autoConf;
		connect(autoConf, SIGNAL(attributeConfigAvailable(const TangoConfigurationParameters *)),
			this, SLOT(distributeAttributeConfiguration(const TangoConfigurationParameters *)), Qt::QueuedConnection);
		/* 5. init the auto configuration object and, thanks to the connect() below, we are ready 
		 * to receive events.
		 */
		autoConf->init(source);
		
	}
	return true;
}
		
void AttrConfManager::unregisterObject(QString source, QTangoCommunicationHandle *handle)
{
	if(handle)
	{
		/* sources are stored without arguments, so remove them */
		QRegExp argRe("\\(.*\\)");
		source.remove(argRe);
		if(d_srcHandleMap.contains(source))
		{
			QSet<QTangoCommunicationHandle *> set = d_srcHandleMap[source];
			if(!set.isEmpty())
			{
				/* the set contains the handle: remove it since from now on we 
				 * will not notify it any attribute configuration event.
				 */
				if(set.contains(handle))
				{
					set.remove(handle);
					d_srcHandleMap[source] = set;
					if(set.isEmpty()) /* it was the last element  */
					{
						pstep("configuration manager: the set associated to the source \"%s\" is empty: unsubscribing",
								qstoc(source));
						/* get the auto configuration object for the source */
						QTangoAutoConfiguration *autoConf = d_configuratorForSourceMap[source];
						autoConf->unsubscribe();
						/* clean the maps */
						d_configuratorForSourceMap.remove(source);
						d_srcHandleMap.remove(source);
					}
					else
						pstep("configuration manager: removed the handle \"%s\" from the set associated to source \"%s\", yet not empty, "
							"so not unsubscribing.", qstoc(handle->objectName()), qstoc(source));
				}
				else
				{
					perr("configuration manager: the set does not contain  \"%s\" associated to source \"%s\"",
					     qstoc(handle->objectName()), qstoc(source));
				}
					     
			}
		}
		else
			perr("configuration manager: source \"%s\" is not contained in the set", qstoc(source));
	}
}

/* triggerRefresh is invoked with source already clean */
void AttrConfManager::triggerRefresh(QString source, QTangoCommunicationHandle *receiver)
{
	if(!receiver || source == QString())
		perr("AttrConfManager::triggerRefresh: receiver or source NULL: bad (BUG!)");
	else
	{
		QTangoAutoConfiguration *autoconf = d_configuratorForSourceMap.value(source, NULL);
		if(autoconf == NULL)
			perr("configuration manager: triggerRefresh: autoconfiguration is NULL! BUG!");
		else
		{
			TangoConfigurationParameters *cp = autoconf->autoConfigurationData();
			if(cp == NULL)
				pwarn("configuration manager: no attribute configuration data available for source: \"%s\"",
				      qstoc(source));
			else
			{
				AttributeConfigurationEvent *ace = new AttributeConfigurationEvent(cp);
				QCoreApplication::instance()->postEvent(receiver, ace);
			}
		}
	}
}

void AttrConfManager::distributeAttributeConfiguration(const TangoConfigurationParameters *cp)
{
	int n = 0;
	QTangoAutoConfiguration *autoconf = qobject_cast<QTangoAutoConfiguration *>(sender());
	if(!autoconf)
	{
		perr("configuration manager: cannot cast to a QTangoAutoConfiguration inside AttrConfManager::distributeAttributeConfiguration()");
		return;
	}
	QString source = autoconf->source();
	
	if(!cp)
	{
		pwrn("configuration manager: could not setup auto configuration for \"%s\"", qstoc(source));
		return;
	}
	
	QSet<QTangoCommunicationHandle *> objects = d_srcHandleMap[source];
	
	if(objects.isEmpty())
		perr("oconfiguration manager: bjects empty for %s\n", qstoc(source));
	else
		pok("attribute configuration manager: distributing \"%s\" \e[1;4;37mconfiguration\e[0m to %d objects", qstoc(source), objects.size());
	
	QSetIterator<QTangoCommunicationHandle *> i(objects);
	while(i.hasNext())
	{
		
		QTangoCommunicationHandle *handle = qobject_cast<QTangoCommunicationHandle *>(i.next());
		if(handle)
		{
			AttributeConfigurationEvent *ace = new AttributeConfigurationEvent(cp);
			QCoreApplication::instance()->postEvent(handle, ace);
		}
		else
			perr("configuration manager: cannot cast the registered object into a communication handle (source:\"%s\")",
			     qstoc(source));
	}
}


