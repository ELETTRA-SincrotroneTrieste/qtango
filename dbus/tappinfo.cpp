#include "tappinfo.h"

TAppInfo::TAppInfo(const QString& appN, const QStringList& appArgs, const pid_t appPid)
{
       appName = appN;
       args = appArgs;
       pid = appPid;
}

bool TAppInfo::operator==(const TAppInfo &other) const
{
    return this->appName == other.appName && this->pid == other.pid && this->dbusServiceName == other.dbusServiceName;
}

