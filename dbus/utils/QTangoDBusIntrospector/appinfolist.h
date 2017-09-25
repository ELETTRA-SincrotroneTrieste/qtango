#ifndef APPINFOLIST_H
#define APPINFOLIST_H

#include <QList>
#include "appinfo.h"

class AppInfoList
{
public:
    AppInfoList();

    void add(const AppInfo& ai) { m_appInfoList << ai; }

    AppInfo at(const int index) const { return m_appInfoList.at(index); }

    AppInfo find(const QString& appName, const pid_t pid) const;

    void clear() { m_appInfoList.clear(); }

    bool contains(const QString& appName, const pid_t pid) const;

    int size() const { return m_appInfoList.size(); }


private:
    QList<AppInfo> m_appInfoList;
};

#endif // APPINFOLIST_H
