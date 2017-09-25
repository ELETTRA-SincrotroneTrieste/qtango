#include "tappinfolist.h"
#include <QtDebug>
#include <sys/types.h>
#include <unistd.h>

TAppInfoList::TAppInfoList()
{

}

bool TAppInfoList::contains(const QString& appName, const pid_t pid) const
{
    foreach(TAppInfo ai, m_tAppInfoList)
        if(ai.appName == appName && ai.pid == pid)
            return true;
    return false;
}

TAppInfo TAppInfoList::find(const QString& appName, const pid_t pid) const
{
    foreach(TAppInfo ai, m_tAppInfoList)
        if(ai.appName == appName && ai.pid == pid)
            return ai;
    return TAppInfo();
}

void TAppInfoList::remove(const QString& appName, const pid_t pid)
{
    TAppInfo appInfo = find(appName, pid);
    m_tAppInfoList.removeAll(appInfo);
}

void TAppInfoList::remove(const TAppInfo &ai)
{
    m_tAppInfoList.removeAll(ai);
}

/* The AppInfo with pid == getpid() is not included in the list */
QList<TAppInfo> TAppInfoList::findWithName(const QString& appName) const
{
    QList<TAppInfo> aList;
    foreach(TAppInfo ai, m_tAppInfoList)
        if(ai.appName == appName && ai.pid != getpid())
            aList << ai;
    return aList;
}

/* The AppInfo with pid == getpid() is not considered */
TAppInfo TAppInfoList::findByNameArgs(const QString& name, const QStringList& args) const
{
    foreach(TAppInfo ai, m_tAppInfoList)
    {
        /* check for getpid() != ai.pid: the application might be already registered with dbus */
        if(ai.pid != getpid())
        {
            qDebug() << "TAppInfoList::findByNameArgs" << ai.appName << name << ai.args << args;
            if((ai.appName == name &&  ai.args == args)
                || (ai.appName == name && ai.args.size() == 0 && args.size() == 0))
                return ai;
        }
    }
    return TAppInfo();
}

TAppInfo TAppInfoList::findByService(const QString& serviceName) const
{
    foreach(TAppInfo ai, m_tAppInfoList)
    {
        if(ai.dbusServiceName == serviceName)
            return ai;
    }
    return TAppInfo();
}

TAppInfo TAppInfoList::findByPid(pid_t pid) const
{
    foreach(TAppInfo ai, m_tAppInfoList)
    {
        if(ai.pid == pid)
            return ai;
    }
    return TAppInfo();
}

TAppInfoList & TAppInfoList::operator = (const TAppInfoList &ail)
{
    for(int i = 0; i < ail.size(); i++)
        this->add(ail.at(i));
    return *this;
}

