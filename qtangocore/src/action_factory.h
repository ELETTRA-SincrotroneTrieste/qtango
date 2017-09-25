/***************************************************************************
*   Copyright (C) 2008 by  Giacomo Strangolino   			   *
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

#ifndef QTANGO_ACTION_FACTORY_H
#define QTANGO_ACTION_FACTORY_H

#include <stdlib.h>
#include <QObject>
#include <QMultiMap>
#include <QString>
#include <QReadWriteLock>

#include "point_events.h"
#include "taction.h"

#define ACTION_ALIVE(action) 	(ActionFactory::actionFactory()->actionAlive(action))
#define ACTION_DEAD(action) 	(!ActionFactory::actionFactory()->actionAlive(action))

/** \brief The set of actions that live in each QTangoCore application.
 *
 * Create and destroy actions through this factory.
 * This class exposes methods to create an ActionFactory, which is a singleton, to get actions
 * with a certain source name or point event data and to test whether an action is alive or not.
 * Since this class is a singleton, its methods are thought to be <em>thread safe</em>.
 * Nevertheless, this is a class intended to be used privately by the library.
 * It also manages the distribution of configuration change events to all the actions connected
 * to the same source (there might be SOURCE and TARGET actions with the same source name).
 */
class ActionFactory : public QObject
{
    Q_PROPERTY(bool targetExecutionLogEnabled READ targetExecutionLogEnabled)
    Q_PROPERTY(QStringList actionsListAsXml READ actionsListAsXml)
     Q_OBJECT
     public:
	  static ActionFactory *actionFactory();
	  
	  TAction *actionForName(const QString &name);
      TAction* actionConfiguredForPoint(const PointEventData *event);
	  
	  /** create and destroy methods */
	  TAction* create(DeviceProxy*, const PointEventData *ev);
      void destroy(TAction *);
	  void destroy(const PointEventData *ev);
	  
	  /** checks if the action is still in the d_actions map. This means it is alive, since we
	   *   assume that actions in the map are alive actions.
	   */
	  bool actionAlive(TAction *a);
	  
	  /** the same as before, but checks the couple source/TAction 
	   */
	  bool actionAlive(const QString& source, TAction * a);
	  
	  void cleanup();
	  
	  QList<TAction *> actionsForDeviceProxy(Tango::DeviceProxy *);

          bool targetExecutionLogEnabled() const { return m_targetExecutionLogEnabled; }

          /** \brief The default refresh mode to apply to <strong>reader</strong> sources
            *        configured with AUTO_REFRESH refreshMode.
            *
            * This property, which can be modified with setDefaultRefreshMode or by means of
            * the environment variable <strong>DEFAULT_REFRESH_MODE</strong>, determines which
            * is the default RefreshMode to apply to a reader when setSource is invoked with
            * AUTO_REFRESH as RefreshMode parameter (or when setSource is simply called with the
            * tango source name, without specifying the RefreshMode argument).
            * If no DEFAULT_REFRESH_MODE is defined, then AUTO_REFRESH is automatically chosen,
            * which means "try to subscribe for events and fallback to POLLED_REFRESH refresh if they are
            * not available for the source to be read".
            * <p>
            * If <strong>DEFAULT_REFRESH_MODE</strong> is set to <strong>POLLED_REFRESH</strong>, then
            * no event subscription is performed inside setSource.
            * In environments where evenst are not available or undesired, setting the variable to
            * POLLED_REFRESH speeds up connection setup.
            * The values for the DEFAULT_REFRESH_MODE environment variable can be chosen among the
            * names of the enumeration RefreshMode defined in qtangocore_common.h
            * Remember that they concern readers, so the available values restrict to:
            * <ul>
            * <li>"AUTO_REFRESH", the default if no environment value is set;</li>
            * <li>"USER_EVENT_REFRESH";</li>
            * <li>"CHANGE_EVENT_REFRESH";</li>
            * <li>"PERIODIC_EVENT_REFRESH";</li>
            * <li>"ARCHIVE_EVENT_REFRESH";</li>
            * <li>"POLLED_REFRESH";</li>
            * <li>"MANUAL_REFRESH;</li>
            * </ul>
            * Any value other than AUTO_REFRESH and POLLED_REFRESH is discouraged.
            * Remember that this defaultRefreshMode is applied <strong>only to sources
            * configured with AUTO_REFRESH</strong> RefreshMode in the setSource method.
            * </p>
            * \par Example
            * In the bash we export the DEFAULT_REFRESH_MODE environment variable:<br/>
            * <strong>export DEFAULT_REFRESH_MODE=POLLED_REFRESH</strong><br/>
            *
            * \code
            * TLabel *label = new TLabel(this); // create TLabel reader
            * label->setSource("test/device/1/double_scalar);
            * TLabel *label2 = new TLabel(this);
            * label2->setSource("test/device/2/double_scalar", POLLED_REFRESH, 1000);
            * TLabel *label3 = new TLabel(this);
            * label3->setSource("test/device/3/double_scalar", AUTO_REFRESH, 1200);
            * \endcode
            *
            * In the code above:
            * <ul><li>label and label3 will behave at the same way as to RefreshMode: their mode
            * will be POLLED_REFRESH dued to the DEFAULT_REFRESH_MODE=POLLED_REFRESH environmental
            * variable.</li>
            * <li>label2 will be <strong>explicitly polled</strong>, independently of the value of the
            * DEFAULT_REFRESH_MODE environment variable.
            *
            * @see setDefaultRefreshMode
            * @see QTangoCommunicationHandle::setSource
            * @see QTangoComProxyReader::setSource
            *
            * <h3>Important note</h3>
            * <p>
            *  If you want to avoid all subscribe_event from your application by means of the
            *  DEFAULT_REFRESH_MODE environment variable, in case <strong>all your setSource</strong>
            *  have been configured with <strong>AUTO_REFRESH</strong>, remember to export also
            *  QTANGO_ATTR_CONF_CHANGE_DISABLED set to 1, because a polled RefreshMode does not
            *  exclude the event subscription for attribute configuration change.
            *
            * @see QTangoAutoConfiguration::getConfigurationChangeEventEnabled
            *
            * </p>
            */
          RefreshMode defaultRefreshMode() const { return m_defaultRefreshMode; }

          /** \brief Sets the default RefreshMode to be used for the setSource calls that will follow.
            *
            * Remember to call this method before any of the setSource you want to adopt
            * the desired RefreshMode as default for the tango connection.
            *
            * This is an alternative to set the environment variable <strong>DEFAULT_REFRESH_MODE</strong>.
            *
            * @see defaultRefreshMode
            */
          void setDefaultRefreshMode(RefreshMode m) { m_defaultRefreshMode = m; }

          /** \brief returns the list of actions represented in xml.
            *
            * The returned xml string contains the parameters that allow to further
            * retrieve the full TAction description through the action() method below.
            *
            */
          QStringList actionsListAsXml() const;

      public slots:
          /** \brief returns an xml representation of the action individuated by the parameters or
            *        an empty string if the action is not found.
            *
            * @param point the tango source or target point
            * @param args the arguments of the tango point
            * @param desiredRefreshMode the desired refresh mode.
            *
            * @return an xml representation of the TAction.
            *
            * You might want to use actionListAsXml to retrieve the input parameters
            * that have to be passed to this method.
            *
            * @see actionListAsXml
            *
            */
          QString actionAsXml(const QString& point, const QStringList& args, int desiredRefreshMode);

          /** \brief disables Tango exceptions logging via TLog/TUtil::addLog.
            *
            * Sometimes, reimplementing readers or writers you may want to manage errors
            * and exceptions on your own, without making the library log them for you
            * through TLog/TUtil::addLog/TLogDialog.
            * This disables logging both for readers and for writers.
            * QTango logging is enabled by default.
            *
            * @see loggingDisabled
            *
            */
          void setLoggingDisabled(bool disabled) { m_loggingDisabled = disabled; }

          /** \brief returns true if the QTango logging via TLog/TLogDialog is disabled.
            *
            * @see setLoggingDisabled
            *
            */
          bool loggingDisabled() const { return m_loggingDisabled; }

  signals:
	void configurationChangedEvent(const TangoConfigurationParameters *);

        void targetExecuted(const QString&, int, bool, const QString&, const QString&);
	  
  protected:
	/** \brief Manages attribute configuration change events.
	 *
	 * When an ACTION_CONFIGURATION_CHANGED event is received, this method gets the list of actions 
	 * linked to the source stored into the AttributeConfigurationChangedEvent data structure.
	 * For each action with that name, another event of the same type and class is posted to 
	 * each action found with that name.
	 */
	bool event(QEvent *e);
	 
     private:
	  ActionFactory();
	  ~ActionFactory();

          void m_initDBus();
	  
	  static ActionFactory *__actionFactory;
	  
	  QMultiMap<QString, TAction*>	d_actions;
	  QMultiMap<DeviceProxy *, TAction*> d_devActions;
	  
	  QReadWriteLock  d_actionsRWLock;

          bool m_targetExecutionLogEnabled;

          bool m_loggingDisabled;

          RefreshMode m_defaultRefreshMode;
};

#endif
