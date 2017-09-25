#include "appinfo.h"

AppInfo::AppInfo(const QString& appN, const QStringList& appArgs, const pid_t appPid)
{
       appName = appN;
       args = appArgs;
       pid = appPid;
}
