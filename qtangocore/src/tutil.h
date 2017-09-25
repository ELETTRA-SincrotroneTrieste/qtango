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

#ifndef _TUTIL_H
#define _TUTIL_H

#include <qtangocore_macros.h>
#include <qtangocore_common.h>
#include <tvariant.h>
#include <tlog.h>
#include <QEvent>
#include <QObject>
#include <QCache>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>
#include <QPair>

#define DEPRECATED(func) func __attribute__ ((deprecated))

class TLogDialog;
class DeviceThread;
class TAction;
class GlobalRefreshExternalTimer;

class QMessageBoxEvent : public QEvent
{
public:
        QMessageBoxEvent(QString s) : QEvent(QMESSAGEBOXEVENT), mess(s) {};
        QString message(){ return mess; };

private:
        QString mess;
};

class QEnableEvent : public QEvent
{
public:
        QEnableEvent(QWidget *w, bool ena) : QEvent(QENABLEEVENT), widget(w), enable(ena) {};

	QWidget *widget;
	bool enable;
};

class QShowLogsEvent : public QEvent
{
public:
        QShowLogsEvent(bool b) : QEvent(QSHOWLOGSEVENT), all(b) {};
        bool showAll(){ return all; };

private:
        bool all;
};

#define GLOBAL_REFRESH_FACTOR_ADJUST 		0.90
#define SET_GLOBAL_REFRESH_INTERVAL(value) 	( TUtil::instance()->setGlobalRefreshInterval(value) )
#define GLOBAL_REFRESH_INTERVAL 			( TUtil::instance()->globalRefreshInterval(value) )

/** \brief QTangoCore utility class
 *
 * This class provides a set of utility methods to setup QTangoCore layer and to ease some operations.
 * Have a look at the single methods for more details.
 * 
 * <p>
 * Pay special attention  to the optimization methods related to the <strong>global refresh</strong> timer,
 * which can greatly affect performance in case of slow devices or applications with many readers active.
 *
 * @see setGlobalRefreshDisabled
 * @see setGlobalRefreshInterval
 *
 * The related settings can be changed via the environment variables 
 * <ul>
 * <li><strong>QTANGO_GLOBAL_REFRESH_DISABLED</strong> and</li>
 * <li><strong>QTANGO_GLOBAL_REFRESH_SUGGESTED_INTERVAL</strong>.</li>
 * </ul>
 * </p>
 */
class TUtil : public QObject
{
Q_OBJECT
public:

	/**
	 * returns the singleton instance 
	 */
	static TUtil *instance();
	
	/**
	 * sets logging target.
	 * by default programs log to stdout, if you provide a filename 
	 * and TTK_LOG environment variable is set they will log to 
	 * $TTK_LOG/filename
	 */
	void setLoggingTarget(QString);

	/**
	 * returns log list 
	 * @return a list of pairs containing timestamps and DevErrorList
	 * taken from a DevFailed instance
	 * 
	 * <br/><em>Note</em>: This method is deprecated. Please use TLog class in conjunction with
	 * TUtil::addLog().
	 * @see TLog
	 * @see addLog
	 */
    const QList<QPair<time_t, DevErrorList> >& getLogs(){ return logs; }

	void clearLogs();
	
	/** \brief adds a log information entry to the list of logs.
	 *
	 * This method, introduced in version <em>4.x</em>, acquires the lock
	 * on the list of logs, adds and entry and releases the lock.
	 * If the size of the list reaches LOG_MAX entries, the first element is
	 * removed.
	 * This is thread safe. It must be used in conjunction with TLog class methods
	 * for creating the logInfo and the formattedMessage parameters.
	 * @see TLog
	 * 
	 * @param logInfo a couple timestamp/DevErrorList created with TLog::toTimeErrlistPair().
	 * @param formattedMessage a QString formatted with TLog::formatMessage().
	 *
	 * <br/><strong>Note</strong>: do not use TUtil::formatMessage methods since they are deprecated 
	 * and not thread safe.
	 */
	void addLog(const QPair<time_t, DevErrorList> &logInfo, const QString& formattedMessage, bool noConsole = false);
	
	/** \brief adds a log information entry to the list of logs.
	 *
	 * This method is provided for convenience and simplifies the API to add a log information entry to the
	 * list of logs belonging to the application and maintained in TUtil.
	 * This is thread safe and has to be used in conjunction with TLog class methods to create the 
	 * log object passed as parameter.
	 *
	 * @param a TLog object.
	 *
	 * \par Example
	 * \code 
	 * // ...
	 * catch(Tango::DevFailed &e)
	 * {
	 *   TLog logInfo(e); 
	 *   TUtil::addLog(logInfo);
	 * }
	 * \endcode
	 */
	void addLog(TLog& log);

	static DataType parseName(const QString &fullName, QString &devName, QString &attrCommand, QStringList &params, bool &);
	
	DeviceData		convertToDeviceData(QStringList&, CommandInfo&);
	DeviceAttribute	convertToDeviceAttribute(QStringList&, AttributeInfoEx&);
	
	QStringList 	getArgs(QStringList &);

	/**
	 * search tango db for an application associated to a given device server
	 * @param device	the name of the device
	 * @return		associated application, if not found the QString is null
	 */
        QString findHelperApplication(QString device, const QString& database, const int port = -1);

        QString findHelperApplication(QString device);

	QObject* findObject(const QString&, QObject* = NULL);
	
    void setAncestor(QObject *a) { d_ancestor = a; }

	static void replaceWildcards(const QObjectList &, QStringList);
	
	static void replaceWildcards(QString &source, QStringList);

	TVariant oneShot(QString);
	
	void waitRefresh(QMutex*, unsigned long = 1000);

	/** returns a string representing the mode. If noescape is true, then 
	 * escape characters for console colors aren't printed.
	 */
	QString modeRepresentation(RefreshMode m, bool noescape = false);

    QString typeRepresentation(CmdArgType t) const;
	
	/** @return the global refresh interval that triggers the global 
	 * refresh signal.
	 * This is done for optimization.
	 */
	int globalRefreshInterval();
	
	/** \brief state of the global refresh (enabled/disabled)
	 *
	 * @return true global refresh enabled, false: global refresh disabled
	 */
	bool globalRefreshDisabled() { return d_globalRefreshDisabled; }
	
	/** \brief indicates that the TUtil has entered the CORBA shut down procedures.
	 * 
	 * @return true if the TUtil has entered the CORBA shut down procedures.
	 */
	bool shuttingDown() { return d_shuttingDown; }
	
	/** \brief the system wide directory where icons are installed. 
	 *
	 * The default directory is /runtime/share/icons.
	 * The location can be changed with TUtil::setIconPath and the new value is 
	 * saved into a user wide  QSettings object.
	 * @see setIconPath
	 */
	QString iconPath();
	
	/** \brief change the default location of the user-wide directory where icons are looked for.
	 *
	 * The default path is /runtime/share/icons
	 * QTango widgets use iconPath method to retrieve the location for their icons and pixmap.
	 * An example of QTango widget that uses iconPath() is qtango TLabel.
	 */
	void setIconPath(const QString& path);
	
signals:
	void refreshSignal();

public slots:
	
	/** \brief changes the refresh interval of the global refresh timer.
	 *
	 * Set the global refresh timer. It should be as faster as or faster than the 
 	 * minimum refresh period among all the readers.
	 * Readers with a period faster than the one set by this method might lower this 
	 * value by themselves. To see if this happens in your application, export the
	 * <strong>QTANGO_PRINT</strong> environment variable before starting the application, 
	 * and look for a <em>pinfo</em> string in the console output.
	 *
	 * The global refresh interval can be suggested at startup by means of the environment 
	 * variable <strong>QTANGO_GLOBAL_REFRESH_SUGGESTED_INTERVAL</strong>.
	 * For instance
	 * <ul><li>export <strong>QTANGO_GLOBAL_REFRESH_SUGGESTED_INTERVAL=800</strong></li></ul>
	 * will suggest to QTangoCore a refresh interval of <em>800</em> milliseconds.
	 * If no such environment variable is exported, then a default interval is chosen and set
	 * to <em>1000</em> milliseconds.
	 */
	void setGlobalRefreshInterval(int millis);
	 
	/** \brief given the new refresh interval, this method adjust the global refresh if necessary
	 *
	 * @param millis the source refresh value that needs to be checked to adjust the global
	 *               refresh interval.
	 *
	 * If the global refresh interval is too slow with respect to the value passed as argument,
	 * it gets adjusted to support it according to the GLOBAL_REFRESH_FACTOR_ADJUST constant.<br/>
	 * <strong>Note</strong>: since qtangocore version 4.x.0, the communication handle automatically
	 * calls this method at every setSource() or setPeriod(), to guarantee that the refresh interval
	 * is updated if needed. So you should not need to call this method by yourself.
	 */
	void adjustGlobalRefreshIntervalIfNeeded(int millis);
	
	/** \brief disables global refresh trigger for all QTangoCore widgets
	 *
	 * @param d if true disables the global refresh trigger for all QTangoCore widgets and they 
	 * are refreshed as soon as new data is available.
	 * if false, enables the global refresh trigger for all QTangoCore widgets.
	 * Specific QTangoCore objects may still refresh asynchronously if configured
	 * with QTangoComProxyReader::setWaitRefresh().
	 *
	 * <h3>Note</h3><p>Disabling global refresh interval disables it <strong>globally</strong>, also for
	 * the objects configured with QTangoComProxyReader::setWaitRefresh() to true.
	 * <br/>Enabling global refresh leaves the single reader to decide to refresh synchronously or asynchronously,
	 * depending on QTangoComProxyReader::waitRefresh().</p>
	 *
	 * <h3>Environment variable to disable global refresh</h3>
	 * <p>
	 * It is possible to start a QTango application with the global refresh timer disabled. This can 
	 * require major CPU load if the application contains lots of readers, but may improve GUI efficiency
	 * when some Device servers respond slowly. This is dued to the lock acquisition to protect data 
	 * both by the execute() method and the dequeue() method.
	 * The two environment variables
	 * <ul>
	 * <li><strong>QTANGO_GLOBAL_REFRESH_DISABLED</strong> and</li>
	 * <li><strong>QTANGO_GLOBAL_REFRESH_SUGGESTED_INTERVAL</strong>.</li>
	 * </ul>
	 *
	 * can be defined to disable the global refresh timer (it is enough to export any
	 * value for the variable <strong>QTANGO_GLOBAL_REFRESH_DISABLED</strong>, for instance
	 * <em>export <strong>QTANGO_GLOBAL_REFRESH_DISABLED=1</strong> is ok.
	 *
	 * @see setGlobalRefreshInterval
	 *
	 * </p>
	 */
	void setGlobalRefreshDisabled(bool d);
	
	void triggerRefresh();
		
	void cleanup();

protected:
	TUtil();
	~TUtil();

	bool		event(QEvent *);
	TLogDialog	*logDialog;

	private slots:
	
private:
  
    void d_log(QPair<time_t, DevErrorList>, bool noConsole = false);
	
	QList<QPair<time_t, DevErrorList> >	logs;
	static TUtil			*_instance;
	QString					loggingTarget;

        /* unused! */
        Database				*db;
	QVector<pair<QString, QString> >	patterns_applications;

	QMutex					deviceMutex;
	
	/* protect the logs QList from concurrent threads trying to add and remove elements */
	QMutex                                  d_logMutex;
	
	QMap<QString, DeviceProxy*>		d_proxies;
	
	QMap<QString, QObject*>			d_objects;
	QObject			*d_ancestor;
	
// 	QTimer			*d_refreshTimer;
	GlobalRefreshExternalTimer *d_externalTimer;
	QWaitCondition	refreshCondition;
	bool d_globalRefreshDisabled;
	bool d_shuttingDown;
};

#endif
