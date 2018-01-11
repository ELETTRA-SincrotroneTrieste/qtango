#ifndef TAPPLICATIONSPOOLPROXY_PH_
#define TAPPLICATIONSPOOLPROXY_PH_

class TApplicationsPoolProxy;
class QDBusConnectionInterface;
class QString;

#include <QHash>
#include "../tappinfo.h"
#include "../tappinfolist.h"

class TApplicationsPoolProxyPrivate
{
public:
    TApplicationsPoolProxyPrivate(TApplicationsPoolProxy *parent);

    bool isTApplication(const QString& serviceName);

    TAppInfo getInfo(const QString& serviceName);

    TAppInfo getRegisteredApp(const QString& serviceName);

    void add(const TAppInfo& ai) { m_appInfoList.add(ai); }

    void removeByServiceName(const QString& serviceName);

    QString errorMessage() const { return m_errorMessage; }

    TAppInfoList appInfoList() const { return m_appInfoList; }

    TAppInfoList &appInfoList() { return m_appInfoList; }

    /* asks the dbus session bus for all the registered applications.
     */
    bool updateTApplicationList();

private:
    TApplicationsPoolProxy * const q_ptr;

    Q_DECLARE_PUBLIC(TApplicationsPoolProxy);

    QString m_errorMessage;

    TAppInfoList m_appInfoList;

    int m_pidFromService(const QString& service);

    QString m_executableFromService(const QString& serviceName);

    QDBusConnectionInterface *m_dbusConnectionInterface;

    QDBusConnectionInterface *m_getDbusConnectionInterface();
};

#endif // TAPPLICATIONSPOOLPROXY_PH_
