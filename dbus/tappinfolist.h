#ifndef TAppInfoLIST_H
#define TAppInfoLIST_H

#include <QList>
#include "tappinfo.h"

class TAppInfoList
{
public:
    TAppInfoList();

    void add(const TAppInfo& ai) { m_tAppInfoList << ai; }

    TAppInfo at(const int index) const { return m_tAppInfoList.at(index); }

    TAppInfo find(const QString& appName, const pid_t pid) const;

    void remove(const QString& appName, const pid_t pid);

    void remove(const TAppInfo &ai);

    void clear() { m_tAppInfoList.clear(); }

    bool contains(const QString& appName, const pid_t pid) const;

    int size() const { return m_tAppInfoList.size(); }

    /** \brief Returns the list of AppInfo with the name provided.
      *
      * <strong>Note</strong>: the AppInfo with pid equal to getpid() is not
      * included in the return list.
      *
      * @see  TApplicationsPoolProxy::findApplicationsWithName
      */
    QList<TAppInfo> findWithName(const QString& name) const;

    /** \brief Returns the TAppInfo matching name and args.
      *
      * <strong>Note</strong>: does not return the TAppInfo with pid equal to getpid()
      *
      *
      * @see  TApplicationsPoolProxy::findApplicationByNameArgs
      */
    TAppInfo findByNameArgs(const QString& name, const QStringList& args) const;

    QList<TAppInfo> findByNameArgsMulti(const QString& name, const QStringList& args) const;

    /** \brief Returns the TAppInfo with process id equal to pid
      *
      */
    TAppInfo findByPid(pid_t pid) const;

    /** \brief returns TAppInfo associated to service serviceName
      *
      * Each TApplication is associated to an unique unambiguous service name,
      * because it is made up of the application pid and the application name.
      * So it is safe to search by service name.
      */
    TAppInfo findByService(const QString& serviceName) const;

    TAppInfoList &operator = (const TAppInfoList &ail);

private:
    QList<TAppInfo> m_tAppInfoList;
};

#endif // TAppInfoLIST_H
