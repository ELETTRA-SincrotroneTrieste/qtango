/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	   *
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

/* $Id: communication_handle.h,v 1.42 2013-07-03 13:54:43 giacomo Exp $ */


#ifndef  QTANGO_COMMUNICATION_HANDLE_H
  #define QTANGO_COMMUNICATION_HANDLE_H
  
#include <QObject>
#include <QStringList>
#include <QVariant>
#include <taction.h>
#include <tutil.h>
#include <subscriber_proxy.h>
#include <unsubscriber_proxy.h>
#include <auto_configuration.h>

class QTangoCommunicationHandlePrivate;

class QTangoCommunicationHandle : public QObject
  {
    Q_OBJECT
	Q_ENUMS(ExecutionMode)
    
    friend class QTangoComProxyReader;
    friend class QTangoComProxyWriter;
	friend class QTangoHelperApplication; /* access to d_action */
	
	friend class ProxyInterface; /* provides a representation of the reader/writer */
    
    public:
     
      /* see QTangoComProxyWriter::executionMode */
	  enum ExecutionMode { Synchronous, Asynchronous, DontStopOnErrors };
	  
      bool configured() { return d_handleConfigured; }
	  QString connectionErrorMessage() { return d_connectionErrorMessage; }
	  QString connectionStatus() { return d_connectionStatus; }
	  
	  const TangoConfigurationParameters *tangoConfigurationParameters() const;
      
    public slots:

      /** \brief notifies connectionOk signal
       */
      void connectionOkNotification(bool);     

     /** \brief notifies connectionFailed signal
       */
      void connectionFailedNotification();      
      
      /** \brief notifies connectionErrorMessage signal 
       */
      void connectionErrorMessageNotification(const QString& msg);
	  
	  /* used by TActionList */
	  void actionListConfigured(bool c);
	  
	  /** \brief triggers an execution of the associated action.
	   *
	   * This method can be used to force an executeAsynch() on the action associated to 
	   * the handle. For instance, it can be used to read an attribute on demand.
	   */
          void executeAsynch(QVariant v = QVariant());

    signals:
      /** \brief notifies a reader that new data is available through a TVariant object
       *
       * This signal notifies a reader that new data is available through the <strong>Tango</strong> layer.
       * Each reader that inherits from QTangoComProxyReader must connect QTangoCommunicationHandle's newData() signal
       * to its <strong>refresh()</strong> slot.
       * \par Example reader header file :
       * \code
       * class SimpleReader : public QObject, public QTangoComProxyReader
          {
          Q_OBJECT
          public:
	   SimpleReader(QObject *parent);
	  
          protected slots:
	   void refresh(const TVariant &v);
        }
       * \endcode
       * \par Example reader cpp file:
       * \code
       SimpleReader::SimpleReader(QObject *parent) : QObject(parent),
          QTangoComProxyReader(parent)
        {
           connect(qtangoComHandle(), SIGNAL(newData(const TVariant &)), this,
	      SLOT(refresh(const TVariant&)));
        }
       * \endcode
	   *
	   * @see newData(const QString&, const TVariant&);
       */
      void newData(const TVariant&);
	  
	  /** \brief this signal is currently unused.
	   *
	   * Use this signal if you are interested also in the name of the source that executed the action
	   * which made available the new data contained into the value parameter.
	   * 
	   * @param tangoPoint the name of the tango point to which the handle is connected (source or target)
	   * @param value the return value of the execution, the same as the one returned by newData(const TVariant&)
	   *
	   * @see newData(const TVariant&);
	   * 
	   * <em>Note</em>: introduced in QTangoCore version 4.x but <em>never emitted</em>: it is not used yet.
	   */
	  void newData(const QString& tangoPoint, const TVariant& value);
      
      void allActionsExecuted();
      
      void progress(int step, int total);
      
      /** \brief signal emitted when the auto configuration is possible and the Tango configuration parameters are available
       *
       * This signal is emitted when the source is able to configure itself with the Tango attribute parameters.
       * Connect this signal to your slot aimed at configuring your object with tango attribute minimum/maximum values,
       * alarm and/or warning thresholds, measurement units, data format and so on.
       * \par Example
       * \code
        SimpleReader::SimpleReader(QObject *parent) : QObject(parent),
        QTangoComProxyReader(parent)
        {
          setAutoConfiguration(true); // necessary step to enable attribute auto configuration
          // the following is used to refresh data. @see newData() signal
          connect(qtangoComHandle(), SIGNAL(newData(const TVariant &)), this,
	      SLOT(refresh(const TVariant&)));
          // here it is. When the attribute configuration is available, the slot init() is invoked
          // If tango events are available, the signal is emitted on every attribute configuration change event
          connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), 
	     this, SLOT(init(const TangoConfigurationParameters *)));   
          // ...
        }
       * \endcode
       * <strong>Notes</strong>
       * <ul><li>the attribute must be configured into the tango database with at least a <strong>minimum and maximum</strong> value.</li>
       * <li>warning and alarm thresholds are also available through TangoConfigurationParameters if configured into the database</li>
       * <li>there is no need to test the argument against a NULL value: the signal is emitted only if the configuration parameters are
             not null.</li>
       * <li>If tango events are available, the signal is emitted on every attribute configuration change event</li></ul>
       */
      void attributeAutoConfigured(const TangoConfigurationParameters *);
      
      /** \brief this signal is emitted after the source or target configuration and tells if it succeeded or not
       * 
       * After the handle configures via setSource(), emits a signal that contains a parameter revealing the 
       * outcome of the operation.
       * @param succeeded if true, setSource() ended successfully.
       *                                      if false, setSource() ended with an error
       *
       * <strong>Note</strong> the signal reveals only if the tango connection to the device was successful. It does not 
       * have to do with the <em>auto configuration</strong> of the source.</p>
       */
      void connectionOk(bool succeeded);
      
      /** \brief this signal is emitted after the source or target configuration and reveals a connection failure
       *
       * After the handle configures via setSource(), emits a signal to reveal a connection failure
       */
      void connectionFailed();
      
      /** \brief a signal that contains the error message related to setSource - or setTargets - failures.
       *
       * Connect this signal to any slot accepting a QString as input parameter if you want to know the error message
       * related to the setSource() or setTargets() failure.
       * @param msg the error message
       */
      void connectionErrorMessage(const QString& msg);
	  
	 
     
    protected slots:
	  
      void configureHandle(TAction* action);
	  
	  /** \brief This slot emits newData() signal after saving the value received as parameter.
	   *
	   * Saves the value <em>v</em> received for later retrieval through the lastValueRead() method.
	   * Then emits a signal to notify the new data arrival, that can be safely connected via Qt::DirectConnection
	   * to the (widgets) readers refresh slots.
	   */
      void newDataAvailable(const TVariant& v);
      void actionListFinished() {emit allActionsExecuted(); }
      void stepAccomplished(int s);
      
      QList<TVariant> executeAll(QVariant v);
      TVariant execute();
	  
	  /* saves configuration parameters internally and emits the attributeAutoConfigured signal */
	  void slotAttributeAutoConfigured(const TangoConfigurationParameters *cp);
      
    protected:
      
      /** \brief the communication handle constructor, only for protected usage and friends.
       *
       * The QTangoCommunicationHandle implements the tango core communication. It is not advised to use
       * the handle directly. One should always consider inheriting from QTangoComProxyReader or QTangoComProxyWriter
       * and use the interface provided by them.
       * QTangoComProxyReader and QTangoComProxyWriter are real proxies that translate all the necessary calls to use
       * the handle.
       */
      QTangoCommunicationHandle(QObject *parent, RefreshMode mode = AUTO_REFRESH, int period = 1000);
      
      
      ~QTangoCommunicationHandle();
      
      void setSource(QString);
      
      /** \brief unsets the tango source for the readers
       *
       * remember to check that the action pointer, d_action, represents a valid action alive in the ActionFactory
       * before using it.
       */
      void unsetSource(bool by_show_event = false);
      QString source() { return d_source; } 
      
      void setSources(QStringList sources);
      void unsetSources();
      QStringList sources() { return d_sources; }
      QString sourcesAsSingleString();
      
      RefreshMode refreshMode();
      bool isEventZmq() const;
      void setRefreshMode(RefreshMode mode);
      int period();
      void setPeriod(int p);
	  
	  void setDesiredAttributePropertyList(const QStringList &attProps) { d_desiredAttributeProperties = attProps; }

	  QStringList desiredAttributePropertyList() { return d_desiredAttributeProperties; }

      void setAttributePropertyDbTimeoutMillis(int dbTimeoutMillis) { mAttrPropDbTimeoutMillis = dbTimeoutMillis; }

      int attributePropertyDbTimeoutMillis() const { return mAttrPropDbTimeoutMillis; }
      
      /** Set auto configuration for the attribute */
      void setAutoConfiguration(bool en, bool confChangeEventEnabled = false);
	  
	  bool autoConfiguration() const { return d_autoconfEnabled; }
       
      /** 
       * @return auto configuration: true: enabled, false: disabled
	*/
       bool getAutoConfiguration() { return d_autoconfEnabled; }
       bool getAttributeConfigurationChangeEventEnabled() { return d_confChangeEventEnabled; }
      
       void setActionWaitRefresh(bool enable);
       bool actionWaitRefresh();
       
       void setToolTipsEnabled(bool enable) { d_toolTipsEnabled = enable; }
       bool toolTipsEnabled() { return d_toolTipsEnabled; }
       
       void setHideEventEnabled(bool e);
       bool hideEventEnabled() { return d_hideEventEnabled; }
       
       /* for readers: it enables designer mode. Designer mode represents the
        * condition in which the handle is used inside the designer. This context
        * might require special care or options (for instance, must disable hide
        * and show events
       */
       void setDesignerMode(bool dm) { d_designerMode = dm; }
       bool designerMode() { return d_designerMode; }
       
      int actionsCount();
      
	  /** \brief returns the last value read by the associated action and locally saved.
	   *
	   * Each time a newDataAvailable slot is invoked, it saves the last value passed as TVariant.
	   * This method returns such value.
	   */
      TVariant lastValueRead() const { return d_lastValueRead; }
		  
      /** \brief called when a hide event is received, it stops the polling of an attribute.
       *
       * If hide events are enabled and the source is in POLLED_MODE, then this method 
       * retrieves the TAction associated to the handle and sends it an event to stop
       * polling on that attribute.
       */
      void stopPoller();	  
      
      /** \brief called when a show event is received, it restarts the polling of an attribute.
       *
       * If hide events are enabled and the source is in POLLED_MODE, then this method 
       * retrieves the TAction associated to the handle and sends it an event to start
       * the poller on that attribute.
       */
      void startPoller();
	  
	   /** \brief returns the execution mode
	   *
	   * see QTangoComProxyWriter::executionMode()
	   */
	  ExecutionMode executionMode() { return d_executionMode; }
		
	  /** \brief sets the execution mode
	   *
	   * see QTangoComProxyWriter::setExecutionMode()
	   */
	  void setExecutionMode(ExecutionMode mode) { d_executionMode = mode; }
	  
	  /** \brief returns the action refresh count in the device thread.
	   *
	   * If the handle is a reader and the action is alive (configured), then this method
	   * returns the number of times the device thread has performed the execute() method.
	   *
	   * @see QTangoComProxyReader::actionRefreshCount
	   */
	  int readerActionRefreshCount() const;
      
       private slots:
	    void subscriberDestroyed(QObject *obj);
	    void d_stopPollingIfWidgetHidden();
		void updateConnectionStatus(const QString &);
		/* connects the source checked and configured with setSource to tango */
		void connectSource();
	    
    private:
      
      bool    d_handleConfigured;
      bool    d_actionWaitRefresh;
      bool    d_autoconfEnabled, d_confChangeEventEnabled;
      bool d_toolTipsEnabled;
      bool	d_hideEventEnabled;
      bool 	d_designerMode;
	  ExecutionMode d_executionMode;
	  
      QString 	d_source;
      QStringList 	d_sources/*, d_oldSources*/;
      TAction *	d_action;	
      TActionList*  d_actionList;
	  QStringList d_desiredAttributeProperties;
	  TVariant d_lastValueRead;
	  
	  /* maintain a reference to a SubscriberProxy. Since each time a handle connects a source it creates also
	   * a subscriber proxy, and the subscriber proxy stays alive until the source is configured, starting an
	   * infinite retry cycle when configuration fails, it is good to delete the subscriber proxy before creating
	   * a new one. This deletion stops the retry cycle for a previous source which failed the configuration.
	   */
	  SubscriberProxy *d_subscriberProxy;
      
      RefreshMode	d_refreshMode;
      int		d_period;

      int mAttrPropDbTimeoutMillis;
      
      void autoconf(QString );
	  
	  /* the following two use QObject MetaProperties to enable or disable widgets */
      bool log(Tango::DevFailed &e);
	  void enableIfQTangoWidget(bool en);
      
      QMutex		refreshMutex;
      QString d_connectionErrorMessage;
	  QString d_connectionStatus;

      QTangoCommunicationHandlePrivate *d_ptr;
  };

#endif
