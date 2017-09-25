#ifndef APPSLISTTREEHELPER_H
#define APPSLISTTREEHELPER_H

class QTreeWidget;
class TAppInfo;
class QTreeWidgetItem;
class QDomElement;

#include <QStringList>

class AppsListTreeHelper
{
public:
    AppsListTreeHelper();
    void addApplication(QTreeWidget *tree, const TAppInfo& ai);

    QString errorMessage() const { return m_error; }

    bool error() const { return !m_error.isEmpty(); }

private:
    QString m_error;

    QStringList m_getNodes(const QString& xml);

    void m_createSubtree(QTreeWidgetItem *parent, const QDomElement& child);
};

#endif // APPSLISTTREEHELPER_H
