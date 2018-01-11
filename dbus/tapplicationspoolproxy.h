#ifndef TAPPLICATIONSPOOLPROXY_H
#define TAPPLICATIONSPOOLPROXY_H

#include <QObject>
#include "tappinfolist.h"

class TApplicationsPoolProxyPrivate;
class TAppInfo;

/** \mainpage QTangoDBus - QTango & DBus session bus
 *
 * QTangoDBus is a QTango module introduced with the version 4.2 of the library.
 * It provides an interface to the DBus message bus that can be used by the QTango applications
 * that want to interoperate with each other.
 * It is possible to be notified when a new application registers to the message bus, to connect
 * to a running application and even impart commands to it by means of the TApplicationInterface.
 * An example of the usage of the framework can be found under the dbus/utils/QTangoDBusIntrospector/
 * application.
 *
 * @author Giacomo Strangolino - October 2008
 * @version 4.2.x
 *
 * @see <a href="http://doc.trolltech.com/4.5/">Qt documentation</a>.
 * @see <a href="../../../qtcontrols/doc/html/index.html">qtcontrols</a> documentation.
 * @see <a href="../../../qtangocore/doc/html/index.html">QTangoCore</a> widgets documentation.
 * @see <a href="../../../qtango/doc/html/index.html">QTango</a> widgets documentation.
 * @see <a href="../../../mango/doc/index.html" alt="Mango online development tool">Mango online development tool</a> documentation.
 *
 * <p>
 * The TApplicationsPoolProxy class represents a Proxy to the pool of TApplications registered with
 * the DBus session bus. </p>
 * <p>
 * The TApplicationInterface is an interface to connect to TApplications exported to DBus, i.e. TApplications
 * build with the release 4.2.x of QTango. </p>
 * <p>
 *
 * </p>

 */

/** \brief This class represents a Proxy to the pool of TApplications registered with
  *        the DBus session bus.
  *
  * This class can be used to retrieve information about QTango applications registered with
  * the DBus session bus. Through this object you can be notified when a new TApplication is
  * launched or closed, via the serviceRegistered and serviceUnregistered signals.
  * The object involved in this notification and representing the type of information exchanged
  * with this class is TAppInfo. TApplicationsPoolProxy contains a list of TAppInfo, each one of
  * them storing information about the application name (executable name, without path), the
  * application pid, the application arguments and the service and paths registered with the
  * session bus.
  * <br/>
  * Once obtained the desired TAppInfo, you can interact with the running TApplication by means
  * of the TApplicationInterface class.
  *
  * @see TApplicationInterface
  * @see TAppInfo
  * @see TAppInfoList
  *
  * \since QTango 4.2.0
  * \author Giacomo Strangolino
  *
  * \par Example.
  * Suppose we want to obtain the list of QTango applications currently connected to the DBus message
  * bus, to be notified when a new applications registers with DBus and when an application unregisters.
  * The list of TApplications may be organized in a tree view, while the TApplicationsPoolProxy can be
  * used to store data in the model/view architecture.
  *
  * \code
  * #include <TApplicationsPoolProxy>
  *
  * MainWindow::MainWindow(QWidget *parent) :
  *      QMainWindow(parent),
  *       ui(new Ui::MainWindow)
  * {
  *  ui->setupUi(this);
  *  TApplicationsPoolProxy *appsPoolProxy = new TApplicationsPoolProxy(this);
  *  appsPoolProxy->setObjectName("TAppsPoolProxy");
  *
  *  connect(appsPoolProxy, SIGNAL(serviceRegistered(const TAppInfo&)), this, SLOT(addApplication(const TAppInfo&)));
  *  connect(appsPoolProxy, SIGNAL(serviceUnregistered(const TAppInfo&)), this, SLOT(removeApplication(const TAppInfo&)));
  *  connect(appsPoolProxy, SIGNAL(error(const QString&)), this, SLOT(showError(const QString&)));
  * }
  * \endcode
  * In the addApplication slot you can extract data from the TAppInfo passed as argument, such as application name,
  * pid and arguments. Then create an item in the view and add it to the view.
  * In the removeApplication slot, you will look for items with the given application name and pid, for instance,
  * and delete them from the view.
  *
  * \endpar
  *
  * In the tdbus directory of the qtangocore distribution, you will find an example of using TApplicationsPoolProxy.
  *
  * @see TApplicationInterface
  */
class TApplicationsPoolProxy : public QObject
{
    Q_OBJECT
public:
    /** \brief The constructor of the pool proxy.
      *
      * On construction, a read of the registered services is performed on the DBus session bus,
      * and their list is available, for instance through applicationsList().
      *
      */
    explicit TApplicationsPoolProxy(QObject *parent = 0);

    ~TApplicationsPoolProxy();

    QString errorMessage();

    TAppInfoList applicationsList();

    /** \brief returns the TAppInfo object associated to the service name provided
      *
      * @param serviceName the DBus service name
      * @return the TAppInfo associated to serviceName or an empty TAppInfo if none found
      *
      * You can test the result with TAppInfo::isEmpty() before using the returned object.
      */
    TAppInfo findApplicationByService(const QString& serviceName);

    /** \brief finds the application with appName name and pid
      *
      * This is a shortcut for TAppInfoList::find()
      */
    TAppInfo findApplication(const QString& appName, int pid);

    /** \brief finds the applications with the name provided
      *
      * @return the list of applications registered with dbus having the name provided.
      *
      * This is a shortcut for TAppInfoList::findWithName()
      *
      * <strong>Note</strong>: the AppInfo with pid equal to getpid() is not
      * included in the return list.
      *
      */
    QList<TAppInfo> findApplicationsWithName(const QString& name);

    /** \brief Looks for TApplications with the name and arguments provided.
      *
      * @param name the application name to look for.
      * @param args the QStringList with the command line arguments to look for.
      *
      * Returns an empty TAppInfo if no result was found.
      * Shortcut for TAppInfoList::findByNameArgs()
      *
      * <strong>Note</strong>: the AppInfo with pid equal to getpid() is not
      * considered by TAppInfoList::findByNameArgs(). This means that the object
      * referring to the running application is not returned by this method.
      * @see TAppInfoList::findByNameArgs()
      *
      */
    TAppInfo findApplicationByNameArgs(const QString& name, const QStringList& args);

    /** \brief Returns the TAppInfo running with the pid provided, or an empty TAppInfo if none was found.
      *
      * This is a shortcut for TAppInfoList::findByPid()
      *
      * <strong>Note</strong>: the AppInfo with pid equal to getpid() is not
      * included in the return list.
      *
      * @see TAppInfoList::findByNameArgs
      */
    TAppInfo findApplicationByPid(pid_t pid);

    void watchForOwnerChanged(const QString& dbus_service_name);

    void stopWatchingForOwnerChanged(const QString& dbus_service_name);

signals:

    /** \brief This signal is emitted when a new TApplication is launched.
      *
      * @param appInfo the data structure representing the remote TApplication which is going to
      *        be registered to the DBus session bus.
      *
      */
    void serviceRegistered(const TAppInfo& appInfo);

    /** \brief This signal is emitted when a TApplication is closed and thus unregistering from DBus.
      *
      * @param appInfo the data structure representing the remote TApplication which is going to
      *        unregister from the DBus session bus.
      */
    void serviceUnregistered(const TAppInfo& appInfo);

    /** \brief This signal is emitted when a DBus error happens.
      *
      * @param s the error string.
      */
    void error(const QString& s);

public slots:
    void refreshAppList();

    void dbusServiceOwnerChanged(const QString&, const QString&, const QString& );

protected:

private:
    Q_DECLARE_PRIVATE(TApplicationsPoolProxy);
    TApplicationsPoolProxyPrivate * d_ptr;

};

#endif // TAPPLICATIONSPOOLPROXY_H
