#ifndef APPINFO_H
#define APPINFO_H
#include <QStringList>

class AppInfo
{
public:
    AppInfo(const QString& appName, const QStringList& args, const pid_t pid);
    AppInfo() {};
    QString appName;
    QStringList args;
    pid_t pid;
    QString dbusServiceName;
    QString dbusServicePath;

    bool isEmpty() { return appName.isEmpty(); }
};

#endif // APPINFO_H
