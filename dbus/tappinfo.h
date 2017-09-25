#ifndef TAPPINFO_H
#define TAPPINFO_H
#include <QStringList>
#include <QMetaType>

class TAppInfo
{
public:
    TAppInfo(const QString& appName, const QStringList& args, const pid_t pid);
    TAppInfo() {};
    QString appName;
    QStringList args;
    pid_t pid;
    QString dbusServiceName;
    QString dbusServicePath;

    bool isEmpty() { return appName.isEmpty(); }

    bool operator==(const TAppInfo &other) const;
};


Q_DECLARE_METATYPE(TAppInfo);

#endif // APPINFO_H
