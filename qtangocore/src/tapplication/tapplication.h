#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include <QApplication>
#include <QtDebug>
#include <QStringList>
#include <QVariant>

#if VER_MAJ < 7
#define TAPP_D(TApplication)  TApplicationPrivate * d = findChild<TApplicationPrivate *>()
#endif

class TApplicationPrivate;
class SecondInstanceHandler;

/** \brief This class manages QTango applications, registering with the DBus system.
  *
  * <strong>Dynamic properties</strong>
  * <p>unique property, boolean type, can be set to mark this application as unique.
  * </p>
  */
class TApplication : public QApplication
{
  Q_OBJECT
  Q_PROPERTY(QString version READ version)
  
  public:
    TApplication ( int & argc, char ** argv );
    TApplication (bool doNotInit, int & argc, char ** argv );
	
	/** \brief sets the application version in the form of QString
	 *
	 * Unfortunately, the QCoreApplication omonimous method has been introduced in 4.4..
         * Anyway, it removes "$ Name: ($)" strings before setting application version.
	 */
	void setApplicationVersion(const QString &version);
	QString applicationVersion() { return d_applicationVersion; }
	
    pid_t applicationPid();

        int uniqueExec();

        /** \brief If you call uniqueExec() in your application, then when a second instance of the
          *        same application is detected, the SecondInstanceHandler::filter() method is called.
          *
          * If you need to implement custom behaviour when a second instance of the same application
          * has been detected, then reimplement SecondInstanceHandler and its filter() method.
          * Then install it with this method.
          */
        void installSecondInstanceHandler(SecondInstanceHandler *sif);

        SecondInstanceHandler* secondInstanceHandler();
	
  public slots:
	void messageReceived(const QString &);
	QString version() { qDebug() << "version" ; return applicationVersion(); }
        void raise();

        void onSecondInstanceDetected();

        QStringList methods(const QString& widgetName);

        QStringList properties(const QString& widgetName);

        QString executable();

        QStringList arguments();

	QStringList parameters();

        QStringList widgetList();

        QVariant invokeMethod5(const QString& widgetName, const QString& member,
                           const QVariant& val0,
                           const QVariant& val1, const QVariant& val2,
                           const QVariant& val3, const QVariant& val4);

        QVariant invokeMethod4( const QString& widgetName, const QString& member,
                           const QVariant& val0,
                           const QVariant& val1, const QVariant& val2,
                           const QVariant& val3);

        QVariant invokeMethod3( const QString& widgetName, const QString& member,
                           const QVariant& val0,
                           const QVariant& val1, const QVariant& val2);

        QVariant invokeMethod2( const QString& widgetName, const QString& member,
                           const QVariant& val0,
                           const QVariant& val1);

        QVariant invokeMethod1( const QString& widgetName, const QString& member,
                           const QVariant& val0);

        QVariant invokeMethod0( const QString& widgetName, const QString& member);

        QVariant readProperty(const QString& widgetName, const QString& propertyName);

        void writeProperty(const QString& widgetName, const QString& propertyName, const QVariant& val);

        /** \brief Returns the DBus service name under which the TApplication is exported on the session bus
          *
          * @return the DBus service name that represents the application in the session bus.
          *
          * The name contains the necessary information to identify the application in the current
          * DBus session bus, and so in the desktop session.
          * The service name is a dot separated name made up of the following fields:
          *
          * <ul>
          * <li>the <strong>organization domain name</strong>: it.trieste.elettra;</li>
          * <li>the <strong>hostname-display_number</strong> (without screen_number information);</li>
          * <li>the <strong>TApplications</strong> string that identifies all TApplications;</li>
          * <li>the <strong>application name</strong>, i.e. the <strong>executable</strong> file name;</li>
          * <li>the <strong>process id</strong>, preceded by the <strong>pid_</strong> indentifier, since
          *     dbus service names cannot begin with a digit;</li>
          * </ul>
          * <p>
          *     A typical example of a TApplication DBus  service name might be as follows:
          *     <p>
          *         "<em>it.trieste.elettra.woody-0.TApplications.connectionIssues.pid_8859</em>"
          *     </p>
          *     Thus, from the service name, as described above, you can retrieve all information related
          *     to the application: its name, the process id, the host where the application runs and
          *     the <strong>X</strong> display where it is visualized.
          * </p>
          */
        QString dbusServiceName() const;

        /** \brief Returns the path  where the TApplication object is registered.
          *
          * The TApplication path simply stays at the root (<strong>/</strong>) of the path tree
          * of the TApplication service.
          */
        QString dbusPath() const;

    protected:
        void initialize() { init(); }
        void initializeDBus() { initDbus(); }

    protected slots:
        void slotInitializeDBus() { initDbus(); }

  private:
	bool d_aboutToQuit;
	void initDbus();
	void init();
	QString composeDbusObjectName();
	QString d_applicationVersion;
    int m_generateDBusTimeout() const;

    TApplicationPrivate * d_ptr;
	
  private slots:
	void setAboutToQuit();

};

#endif
