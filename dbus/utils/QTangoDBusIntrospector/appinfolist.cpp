#include "appinfolist.h"

AppInfoList::AppInfoList()
{

}

bool AppInfoList::contains(const QString& appName, const pid_t pid) const
{
    foreach(AppInfo ai, m_appInfoList)
        if(ai.appName == appName && ai.pid == pid)
            return true;
    return false;
}

AppInfo AppInfoList::find(const QString& appName, const pid_t pid) const
{
    foreach(AppInfo ai, m_appInfoList)
        if(ai.appName == appName && ai.pid == pid)
            return ai;
    return AppInfo();
}
