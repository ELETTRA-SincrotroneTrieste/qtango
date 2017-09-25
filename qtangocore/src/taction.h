/***************************************************************************
*   Copyright (C) 2008 by  Vincenzo Forchi`, Giacomo Strangolino	  *
*   giacomo.strangolino@elettra.trieste.it   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef _TACTION_H
#define _TACTION_H

#include <qtangocore_common.h>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QEvent>
#include <QStringList>
#include <QVariant>
#include <QQueue>
#include <tvariant.h>
#include <point_events.h>
#include <auto_configuration.h>

class QTangoCommunicationHandle;
class QTangoComProxyReader;
class ActionTimer;
class TActionPrivate;
class TActionListPrivate;

/* The communicator with the tango layer. For library private use.
 *
 * TAction is the reader of the qtangocore elements. TActions are created and destroyed
 * only through the ActionFactory, which has exclusive access on the constructor and the 
 * destructor.
 * Note: it is a good practise to check if a TAction is still alive 
 * calling the ActionFactory::actionAlive() methods of the ActionFactory.
 * Actually, the ActionFactory is the only set where actions are really alive and registered.
 * Different TActions living in a QTangoCore applications might pertain to different threads.
 *
 * Since QTangoCore release 4.x, attribute configuration obtained via get_attribute_config() is
 * performed inside TAction and consequently in DeviceThread.
 * Before, the communication handle used to manage attributes auto configuration, in the main
 * application thread and by means of the singleton class AttrConfManager, defined in 
 * attributes_configuration_manager.h. Such singleton class has hence been removed from the
 * qtango core library since 4.x.
 * To cleanly achieve this simplification, TAction configuration is accomplished in two stages,
 * both for commands and attributes:
 * 
 * a. startConfiguration()
 * b. completeConfiguration()
 * 
 * Auto configuration is performed by the helper class AutoConfiguration, taken from the previous
 * implementation (previous to 4.x) but heavily simplified.
 * AutoConfiguration is instantiated in startConfiguration() method, invoked by the TAction class
 * constructor. In the same method, the AutoConfiguration::get() method is invoked. This mainly 
 * calls the get_attribute_config() of the tango DeviceProxy class and gathers some attribute properties.
 * When get() ends successfully, TAction performs the completeConfiguration() stage, which means,
 * for commands:
 *
 * - start the poller for reading argouts from commands, if the mode is different from `TARGET'
 *
 * and, for attributes:
 *
 * - try to subscribe to the event system or fallback to polling mode, if event system fails;
 *
 * The first value read is obtained by QTangoAutoConfiguration and saved in d_data.
 *
 * The high level interface of the SIGNAL/SLOT connections from QTangoCommunicationHandle and 
 * readers/writers has not changed.
 * Simply, TAction propagates a notifyConfigurationChangedEvent() SIGNAL, which is used by the
 * communication handle.
 *
 */
class TAction : public QObject, public CallBack
{
	friend class ActionFactory;
    friend class DeviceThread;
Q_OBJECT
public:
	
    bool triggeredRefresh() { return d_triggeredRefresh; }
	bool eventDriven() { return d_eventDriven; }
	bool isConfigured()  { return d_configured; }
	
	bool waitRefresh() { return d_waitRefresh; }
	void setWaitRefresh(bool wait);
	
	/** @return the source name, made up of a tango device and an attribute (or command) name */
    const QString		source() const { return d_source; }
	
    DataType		type() { return d_type; }
	
	AttributeInfoEx getAttributeInfo();
	
	CommandInfo getCommandInfo() { return d_cmdInfo; }
	
    const QString		point() const { return d_point; }
    const QStringList	args()  const { return d_args; }

        QString databaseHost() const;

        int databasePort() const;
	
	/** The refresh mode, defined in qtangocore_common.h.
	 * It can have one of the values: 
	* AUTO_REFRESH, USER_EVENT_REFRESH, CHANGE_EVENT_REFRESH, PERIODIC_EVENT_REFRESH,
	* ARCHIVE_EVENT_REFRESH, POLLED_REFRESH, MANUAL_REFRESH, TARGET,
	* the last being specific to the targets.
	* If possible, the mode the action is configured for is equal to the desiredMode, if this 
	* is not possible (i.e. CHANGE_EVENT_REFRESH is asked but no events are available at 
	* subscription time), then the mode falls back to another possible modality (i.e. POLLED_REFRESH
	* in the example above.
	*/
    RefreshMode mode() const { return d_mode; }
	
	/** The desired refresh mode: the mode effectively adopted is not always the desired one:
	 * for instance, one could desire CHANGE_EVENT mode, but, if  events aren't available, then 
	 * the mode switches to POLLED_REFRESH.
	 * Sometimes it is useful to remember what was the desired mode. This feature is used by
	 * the SubscriberProxy to discern the TAction to propagate among the TActions it receives
	 * from the DeviceThread.
	*/
	RefreshMode 		desiredRefreshMode() { return d_desiredMode; }

    bool isZmq() const;
	
    int			period() const { return d_period; }
    const QString		deviceName()  const { return d_devName; }
	
	/** \brief returns current data. 
	 *
	 * The implementation is thread safe since it acquires a read lock on data.
	 */
	TVariant data();
	
	bool operator==(const TAction &other) const;
	
	QString configurationError() { return d_configurationError; }
	
	void disconnectTango();
	
	QTangoAutoConfiguration *autoConfiguration() { return d_autoConfiguration; }

        bool targetExecutionLogEnabled() const { return m_targetExecutionLogEnabled; }

// 	TVariant& execute(QVariant v);
	
signals:
	void configured(bool = true);
	void startedCall();
	void finishedCall();
	void newData(const TVariant&);
	void connectionStatusChanged(const QString&);
	void notifyConfigurationChangedEvent(const TangoConfigurationParameters *);

    void stopTimer();
	
public slots:
	/** \brief action execution method: read/write_attribute or command_inout
	 *
	 * This method, depending on the type of source (target), executes a tango
	 * read_attribute(), write_attribute() or command_inout
	 *
	 * @param v a QVariant containint the argin for the commands or the value to write to the attribute
	 *          converted to QVariant. The default value is an empty QVariant, used for instance for a 
	 *          read_attribute or for a command_inout without input arguments.
	 * @param push tells execute to emit the newData() signal without enqueueing the data to the queue
	 *             `dataQueue', used for optimization.
	 */
	TVariant& execute(QVariant v = QVariant(), bool push = false);
	void executeAsynch(QVariant v = QVariant());
    void setTriggeredRefresh(bool b) { d_triggeredRefresh = b; }

	/** \brief adds a subscriber, for reference count
	 *
	 * used by DeviceThread.
	*/
	void addSubscriber();
	
	/** \brief decrements subscriber reference count
	 *
	 * Used by ActionFactory
	 */
	void removeSubscriber();
	
    int  numSubscribers() { return d_numSubscribers; }
	
	/** returns true if the ActionTimer is active, false otherwise. Used by QTangoCommunicationHandle
	 * to restart the poller if this method returns false.
	 */
	bool timerActive();
	
	/** \brief takes the last datum inserted into the queue and emits newDataAvailable() signal.
	 *
	 * To avoid locks on this queue, do not call this method from another thread.
	 * To dequeue thread-safely, please use lastValueRead() instead.
	 * @see safeDequeue
	 */
	void dequeue();
	
	/** \brief gets the last datum read by TActioin. Thread safe.
	 *
	 * Call this method if you want to get last data available from a thread different from the
	 * DeviceThread in which the TAction lives. This method does not emit newDataAvailable() signal
	 * but you obtain the data as return value. This is used in QTangoCommunicationHandle::configureHandle()
	 * to get the first data read.
	 * This locks the execution, and vice versa the execute() method locks the data.
	 */
	TVariant lastValueRead();
	
	/** \brief returns the number of push_event() invocations, i.e. how many events were received.
	 */
	int refreshCount();

private:
	
	void startPoller();
	
	bool event(QEvent *);
	
	bool		d_configured, d_eventDriven, d_waitRefresh;
	
	DataType 	d_type;
	QString		d_source;
	QString 	d_devName;
	QString		d_point;
	string		d_stdPoint;
	QStringList 	d_args;
	bool		d_dataBase;

    QStringList d_desiredAttributeProperties;
	DeviceProxy	*d_device;
	DeviceData	d_argin;

	CommandInfo	d_cmdInfo;
	
	/* the main thread might read d_data, through data() method, while the DeviceThread is
	 * refreshing. This might happen at startup: being different threads, a polling period
	 * might elapse just when the communication handle first reads data().
	 */
	TVariant	d_data;
	
	void	push_event(EventData*);
	int		d_eventId;
	
	ActionTimer		*d_timer;
	RefreshMode	d_mode, d_desiredMode;
	int		d_period;

	unsigned int d_refreshCount;
	
	QReadWriteLock	d_dataRWLock;
	QMutex 	d_executionMutex;
	QMutex	d_queueMutex;

	
	QQueue<TVariant> dataQueue;
	  
	void startConfiguration();
	
	void completeConfiguration();
		
	QString d_configurationError;
	int		d_numSubscribers;
    bool	d_triggeredRefresh;

    int mDbTimeoutMillis;
		
	/** A TAction is univokely determined by three parameters:
	* @param source: the name of the tango point we are interested in: test/device/1/double_scalar;
	* @param RefreshMode: the mode we want to read the entity, or the TARGET mode for write operations;
	* @param period: the interval at which we perform subsequent reads.
	* The effective RefreshMode might not be the desired one we specify when using this constructor: for instance
	* a desired CHANGE_EVENT_REFRESH might fall back to a POLLED_REFRESH if events are not available at the 
	* tango subscription time. For this reason, to later identify the action constructed here, we must consider the 
	* desiredRefreshMode() value, that memorizes our RefreshMode wishes when creating a new TAction.
	*/
// 	TAction(QString, RefreshMode = AUTO_REFRESH, int = 1000);
    TAction(const QString& srcName, DeviceProxy *, DataType,
      const QString& point, const QStringList &args,
      const QStringList& attributeProperties = QStringList(),
      int attrPropDbTimeoutMillis = -1,
      RefreshMode = AUTO_REFRESH, int = 1000);
	  
	~TAction();
	
	/* manages auto configuration */
	QTangoAutoConfiguration *d_autoConfiguration;


    bool m_targetExecutionLogEnabled;

    TActionPrivate *d_ptr;
};

class TActionList : public QObject
{
Q_OBJECT
public:
	TActionList(const QStringList &, 
		const QStringList& attributeProperties = QStringList(),
		RefreshMode = AUTO_REFRESH, 
		const QString& assocObjectName = QString(), 
		int = 1000, QObject * = NULL);

	~TActionList();
	
	QStringList	sources(){ return d_sources; };
	QList<TAction*>	actions(){ return d_actions; };
	void setAssociatedObjectName(const QString name) { d_assocObjName = name; }
	QString associatedObjectName() { return d_assocObjName; }
		
signals:
	void newData(const TVariant&);
	
	void notifyLastAttributeConfigurationChangedEvent(const TangoConfigurationParameters *);
	
	void started();
	void finished(int);
	void finishedAll();
	/* after setSources() tells if tango configuration ( connection ) was successful */
	void connectionOk(bool ok);
	void connectionFailed();
	void connectionError(const QString &msg);
	void connectionStatusChanged(const QString&);
	/* this signal is emitted when my_sources.size() equals d_sources.size(), i.e. when all 
     * the desired sources have been correctly configured. Emitted in configure() method.
	 */
	void actionListConfigured(bool);

public slots:
	void configure(TAction*);
	QList<TVariant> execute(QVariant v = QVariant(), bool dontStopOnErrors = false);
	void executeAsynch(QVariant v = QVariant());
	
	void newDataForAction(const TVariant& t) { emit newData(t); }
	
  private slots:
	void subscriberDestroyed(QObject *);
	
private:
	QStringList		d_sources, my_sources;
	QList<TAction*>	d_actions;
	QString d_assocObjName;
	QMap<QString, SubscriberProxy *> d_subscriberProxiesMap;

    TActionListPrivate *d_ptr;
	
};

#endif
