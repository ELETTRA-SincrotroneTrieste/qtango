#include "appslisttreehelper.h"

#include <TAppInfo>
#include <QTreeWidget>
#include <QStringList>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDomDocument>
#include <QtDebug>
#include <macros.h>

AppsListTreeHelper::AppsListTreeHelper()
{

}

void AppsListTreeHelper::addApplication(QTreeWidget *tree, const TAppInfo& ai)
{
    QString service = ai.dbusServiceName;
    QDBusInterface dbi(service, "/", "org.freedesktop.DBus.Introspectable", QDBusConnection::sessionBus());
    QDBusReply<QString> reply = dbi.call(QDBus::Block, "Introspect");
    if(reply.isValid())
    {
        qDebug() << "Introspection " << reply.value();
        QDomDocument doc("DBusIntrospection");
        if(doc.setContent(reply.value()))
        {
            QDomElement firstEl = doc.firstChildElement("node");
            if(!firstEl.isNull())
            {
                QStringList l;
                /* build the columns of the tree widget item */
                l <<  ai.appName << ai.args.join(",") << QString::number(ai.pid);
                /* create application item */
                QTreeWidgetItem *it = new QTreeWidgetItem(tree, l);
                it->setToolTip(1, ai.args.join("\n"));
                it->setFlags(it->flags() & ~Qt::ItemIsSelectable);
                m_createSubtree(it, firstEl.firstChildElement("node"));
            }
        }
    }
    else
        m_error = reply.error().message();
}


void AppsListTreeHelper::m_createSubtree(QTreeWidgetItem *parent, const QDomElement& child)
{

    /* add children */
    QTreeWidgetItem *childIt = new QTreeWidgetItem(parent, QStringList() << child.attribute("name"));
    childIt->setFlags(childIt->flags() | Qt::ItemIsSelectable);
    childIt->setData(0, Qt::UserRole, parent->childCount());
    childIt->setToolTip(0, "DBus path");
    /* go on with recursion */
    QDomElement nextSiblingEl = child.nextSiblingElement("node");
    if(!nextSiblingEl.isNull())
        m_createSubtree(parent, nextSiblingEl);
    QDomElement firstChildEl = child.firstChildElement("node");
    if(!firstChildEl.isNull())
        m_createSubtree(childIt, firstChildEl);

}



