#ifndef APPSLISTREADER_H
#define APPSLISTREADER_H

#include <QObject>

#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QList>
#include "appinfolist.h"

class AppsListReader : public QObject
{
    Q_OBJECT

public:
    AppsListReader(QObject *parent);

    AppInfoList appInfoList() const { return d_appInfoList; }

public slots:
    void refresh();

signals:
    void applicationListAvailable(bool success, const QString& error);

private:
    QDBusConnectionInterface *d_dBusInterface;

    AppInfoList d_appInfoList;
};

#endif // APPSLISTREADER_H
