/***************************************************************************
*   Copyright (C) 2008 by Vincenzo Forchi`		   *
*      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

/* $Id: tdbbrowser.cpp,v 1.13 2014-08-26 14:55:11 giacomo Exp $ */

#include "tdbbrowser.h"

#include <QHeaderView>
#include <QInputDialog>
//#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QMimeData>

#include "TLabel"
#include "TLineEdit"
#include "TPushButton"

#include <QtDebug>
#include <QClipboard>
#include <macros.h>

TDbBrowser::TDbBrowser(QWidget *parent, QString name, int port, Mode m) : QTreeWidget(parent)
{
    setMinimumSize(100,200);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    //	header()->setToolTip("Double click to change Database");
#if QT_VERSION < 0x050000
    header()->setClickable(true);
#else
    header()->setSectionsClickable(true);
#endif

    d_menu = new QMenu(this);
    d_showHwDevices = d_menu->addAction("Show Hardware Devices", this, SLOT(changeMode()));
    d_showHwDevices->setCheckable(true);
    d_showAdmDevices = d_menu->addAction("Show Admin Devices", this, SLOT(changeMode()));
    d_showAdmDevices->setCheckable(true);

    d_menu2 = new QMenu(this);

    setMode(m);

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(getExpandedDevices(QTreeWidgetItem *)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(launchApplication(QTreeWidgetItem *, int)));

    //connect(header(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(changeTangoHost()));
    setIconSize(QSize(24,24));
    if(name == QString() && port == 0)
        d_dbByEnv = true;
    else
        d_dbByEnv = false;
    setDatabase(name, port);
}

void TDbBrowser::setDatabase(QString name, unsigned int port)
{
    try {
        if(d_dbByEnv)
        {
            db = new Database();
            dbName = getenv("TANGO_HOST");
            dbHost = dbName.split(":")[0];
            dbPort = dbName.split(":")[1].toUInt();
        }
        else
        {
            string n = name.toStdString();
            db = new Database(n, port);
            dbName = name + ":" + QString::number(port);
            dbHost = name;
            dbPort = port;
        }
        string info = db->get_info();
        QString dbDevName = QString::fromStdString(info).split("\n")[0].split(' ')[2];
        string devname = dbName.toStdString() + "/" + dbDevName.toStdString();
        DeviceProxy *dbDevice = new DeviceProxy(devname);
        QStringList labels;
        labels << dbName << "";
        setHeaderLabels(labels);

        string pat("*");
        DeviceData in;
        in << pat;
        DeviceData out = dbDevice->command_inout("DbGetDeviceDomainList", in);
        vector<string> devs;
        out >> devs;
        clear();
        for (size_t i = 0; i < devs.size(); i++)
        {
            QString dev = QString::fromStdString(devs[i]);
            if (!(d_mode & ShowAdmDevices))
            {
                QStringList hidden = QStringList() << "dserver" << "sys" << "tango" << "tmp";
                if (hidden.contains(dev))
                    continue;
            }
            if (!(d_mode & ShowHwDevices))
            {
                if (QRegExp("ec*", Qt::CaseSensitive, QRegExp::Wildcard).exactMatch(dev))
                    continue;
            }
            TDbBrowserItem *item = new TDbBrowserItem(this, QStringList(), dev);
            new QTreeWidgetItem(item, QStringList());
            addTopLevelItem(item);
        }
        resizeColumnToContents(0);
        //hideColumn(1);
    } catch (DevFailed &e) {
        Except::print_exception(e);
    }
}

QString TDbBrowser::database()
{
    return dbName;
}

void TDbBrowser::changeTangoHost()
{
    bool ok;
    QString db = QInputDialog::getText(this, "Change Database", "Input new TANGO_HOST in host:port format", QLineEdit::Normal, QString(), &ok);
    if (ok && !db.isEmpty())
    {
        if (db.count(":") == 1)
        {
            QStringList temp = db.split(":");
            QString host = temp[0];
            unsigned int port = temp[1].toUInt(&ok);
            if (ok)
            {
                d_dbByEnv = false;
                setDatabase(host, port);
            }
        }
        else
            ok = false;
    }
    //aggiungere QMessageBox if (!ok)
}

void TDbBrowser::getExpandedDevices(QTreeWidgetItem *it)
{
    TDbBrowserItem *item = dynamic_cast<TDbBrowserItem*>(it);
    if(!item)
        perr("\"%s\" not item!\n", qstoc(it->text(0)));
    DbDatum datum;
    QStringList data;
    if (item->d_level < 3)
    {
        string pattern = item->d_pattern.toStdString() + "/*";
        if (item->d_level == 1)
            datum = db->get_device_family(pattern);
        else
            datum = db->get_device_member(pattern);
        for (size_t j = 0; j < datum.size(); j++)
        {
            data << QString("/") + QString::fromStdString(datum.value_string[j]);
        }
        item->setFlags(item->flags()/* | ~Qt::ItemIsDragEnabled*/);
    }
    else if (item->d_level == 3)
    {
        try {
            string name = (dbName + "/" + item->d_pattern).toStdString();
            DeviceProxy *dev = new DeviceProxy(name);
            CommandInfoList *commands = dev->command_list_query();
            for (size_t i = 0; i < commands->size(); i++)
                data << QString("->") + QString::fromStdString((*commands)[i].cmd_name);
            vector<string> *attributes = dev->get_attribute_list();
            for (size_t i = 0; i < attributes->size(); i++)
                data << QString("/") + QString::fromStdString((*attributes)[i]);
            delete dev;
        } catch (DevFailed &e) {
            item->setIcon(0, QIcon(":icons/device_off.png"));
            Except::print_exception(e);
        }
    }
    //	else if (item->d_level == 4)
    //		data << "";

    item->takeChildren();
    for (int j = 0; j < data.size(); j++)
    {
        TDbBrowserItem *item2 = new TDbBrowserItem(item, QStringList(), item->d_pattern+data[j]);
        if (item->d_level+1 != 4)
            new QTreeWidgetItem(item2, QStringList());
    }
    resizeColumnToContents(0);
    if(item->d_level == 1)
        emit domainExpanded(item);
    else if(item->d_level == 2)
        emit familyExpanded(item);
    else
        emit memberExpanded(item);
}

QStringList TDbBrowser::selectedAttrCommands()
{
    QStringList temp;
    foreach (QTreeWidgetItem *it, selectedItems())
    {
        TDbBrowserItem *item = dynamic_cast<TDbBrowserItem*>(it);
        if (item->d_level == 4)
            temp << item->d_fullName;
    }
    return temp;
}

QStringList TDbBrowser::selectedDevices()
{
    QStringList temp;
    foreach (QTreeWidgetItem *it, selectedItems())
    {
        TDbBrowserItem *item = dynamic_cast<TDbBrowserItem*>(it);
        temp << item->d_device;
    }
    return temp;
}

QStringList TDbBrowser::selectedMembers()
{
    QStringList temp;
    foreach (QTreeWidgetItem *it, selectedItems())
    {
        TDbBrowserItem *item = dynamic_cast<TDbBrowserItem*>(it);
        temp << item->d_member;
    }
    return temp;
}

QStringList TDbBrowser::selectedFamilies()
{
    QStringList temp;
    foreach (QTreeWidgetItem *it, selectedItems())
    {
        TDbBrowserItem *item = dynamic_cast<TDbBrowserItem*>(it);
        temp << item->d_family;
    }
    return temp;
}

QStringList TDbBrowser::selectedDomains()
{
    QStringList temp;
    foreach (QTreeWidgetItem *it, selectedItems())
    {
        TDbBrowserItem *item = dynamic_cast<TDbBrowserItem*>(it);
        temp << item->d_domain;
    }
    return temp;
}

void TDbBrowser::launchApplication(QTreeWidgetItem *it, int column)
{
    if (column != 0)
        return;

    TDbBrowserItem *item = dynamic_cast<TDbBrowserItem*>(it);

    if (item->d_level == 4)
    {
        QWidget *it = item->widget;
        if (it)
        {
            delete it;
            item->widget = 0;
        }

        /*TLabel *l = qobject_cast<TLabel*>(it);
            if (l)
            {
                if (l->isVisible())
                {
                    l->unsetSource();
                    l->hide();
                }
                else
                {
                    l->setSource(item->d_pattern);
                    l->show();
                }
            }
            else
            {
                TPushButton *b = qobject_cast<TPushButton*>(it);
                if (b)
                    b->setVisible(!b->isVisible());
            }*/
        //delete it;
        //item->setSizeHint(1, item->oldSize);
        else
        {
            //QWidget *w;
            if (item->isCommand)
            {
                QWidget *w = new QWidget(this);
                QHBoxLayout *l = new QHBoxLayout(w);
                l->setMargin(0);
                l->setSpacing(0);
                TLineEdit *le = new TLineEdit(w);
                le->setObjectName(item->d_pattern);
                l->addWidget(le);
                TPushButton *p = new TPushButton(w);
                p->setText(item->d_attr_comm);
                p->setTargets(item->d_pattern + "(&" + item->d_pattern + ")");
                l->addWidget(p);
                setItemWidget(item, 1, w);
                item->widget = w;
            }
            else
            {
                TLabel *l = new TLabel(this);
                l->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
                l->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
                l->setMinimumHeight(0);
                l->setSource(item->d_pattern);
                setItemWidget(item, 1, l);
                item->widget = l;
            }
            //showColumn(1);
        }
    }
    else
        item->launchApplication();
}

void TDbBrowser::contextMenuEvent(QContextMenuEvent *event)
{
    printf("context menu event\n");
    QTreeWidgetItem *item = itemAt(event->pos());
    TDbBrowserItem	*it = dynamic_cast<TDbBrowserItem*>(item);
    if (item == 0)
        d_menu->popup(event->globalPos());
    else
    {
        d_menu2->clear();
        printf("item d_level %d\n", it->d_level);
        if(it->d_level == 4)
            d_menu2->addAction("Copy source", it, SLOT(copyToClipboard()));
        /* helper application */
        it->searchApplication();
        if (it->d_helperApplication != "NULL")
            d_menu2->addAction("Open Control Panel", it, SLOT(launchApplication()));
        if(d_menu2->actions().size())
            d_menu2->popup(event->globalPos());
    }
}

QMimeData * TDbBrowser::mimeData ( const QList<QTreeWidgetItem *> items ) const
{
    if(!items.size())
        return NULL;
    QMimeData *mimeData = new QMimeData();
    TDbBrowserItem *item = dynamic_cast<TDbBrowserItem *>(items.first());
    if(item)
    {
        QString sdata = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                "<!DOCTYPE qtangoCopyPasteXml>\n"
                                "<property name=\"source\" ><string>%1</string></property>").
                arg(item->d_fullName);
        mimeData->setText(sdata);
    }
    return mimeData;
}

void TDbBrowser::setMode(Mode m)
{
    d_mode = m;
    d_showHwDevices->setChecked((d_mode & ShowHwDevices) == ShowHwDevices);
    d_showAdmDevices->setChecked((d_mode & ShowAdmDevices) == ShowAdmDevices);

    setDatabase(dbHost, dbPort);
}

void TDbBrowser::changeMode()
{	
    setMode((Mode)((d_showHwDevices->isChecked() ? ShowHwDevices : 0) | (d_showAdmDevices->isChecked() ? ShowAdmDevices : 0)));
}

void TDbBrowser::setMaxLevel(unsigned int l)
{
    d_maxLevel = l;
}

void TDbBrowser::setReadOnly(bool ro)
{
    d_readOnly = ro;
}

TDbBrowserItem::TDbBrowserItem(TDbBrowser *p, QStringList text, QString pattern) : QObject(0), QTreeWidgetItem(p, text)
{
    init(pattern);
}

TDbBrowserItem::TDbBrowserItem(TDbBrowserItem *p, QStringList text, QString pattern) : QObject(0), QTreeWidgetItem(p, text)
{
    init(pattern);
}

void TDbBrowserItem::init(QString pattern)
{
    widget = 0;
    app = 0;

    d_pattern = pattern;
    d_fullName = pattern;

    QStringList temp;
    if (d_fullName.contains("->"))
    {
        isCommand = true;
        temp = QString(d_fullName).replace("->", "/").split("/");
    }
    else
    {
        isCommand = false;
        temp = d_fullName.split("/");
    }

    setText(0, temp.last());
    d_level = temp.size();

    d_domain 	= temp.value(0);
    d_family 	= temp.value(1);
    d_member 	= temp.value(2);
    d_attr_comm	= temp.value(3);
    if (d_level >= 4)
        temp.pop_back();
    d_device	= temp.join("/");

    if (d_level == 3)
    {
        setIcon(0, QIcon(":icons/device.png"));
    }
    else if (d_level == 4)
    {
        if (d_pattern.contains("->"))
            setIcon(0, QIcon(":icons/command.png"));
        else
            setIcon(0, QIcon(":icons/attribute.png"));
    }
}

void TDbBrowserItem::copyToClipboard()
{
    QString appName = qApp->applicationName();
    if(appName.isEmpty())
    {
        appName = qApp->arguments().at(0);
        appName.remove("./");
    }
    QString cpTxt = appName + ":" + "Tango DB browser" + ":" + d_fullName;
    QApplication::clipboard()->setText(cpTxt);
}

void TDbBrowserItem::searchApplication()
{
    if (d_level != 3)
        d_helperApplication = "NULL";
    else if (d_helperApplication.isEmpty() && qobject_cast<TDbBrowser *>(this->treeWidget()))
    {
        QString host = qobject_cast<TDbBrowser *>(this->treeWidget())->database();
        int port = qobject_cast<TDbBrowser *>(this->treeWidget())->databasePort();
        QString app = TUtil::instance()->findHelperApplication(d_device, host, port);
        if (app != "")
            d_helperApplication = app + " " + d_device;
        else
            d_helperApplication = "NULL";
    }
}

void TDbBrowserItem::launchApplication()
{
    searchApplication();
    if (d_helperApplication != "NULL")
    {
        if (!app)
        {
            app = new EApplicationLauncher(d_helperApplication);
            app->enableCheckForUnique(false);
        }
        app->Rise();
    }
}

