#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "appinfolist.h"
#include <TApplicationsPoolProxy>
#include <macros.h>
#include "appdbusinspector.h"
#include "appslisttreehelper.h"
#include "tactionlisthelper.h"
#include <QtDebug>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QMap>
#include <ActionFactoryInterface.h>
#include <DeviceThreadFactoryInterface.h>
#include <TAppInfo>
#include <QTimer>
#include <tutil.h>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TApplicationsPoolProxy *appsPoolProxy = new TApplicationsPoolProxy(this);
    appsPoolProxy->setObjectName("TAppsPoolProxy");

    for(int i = 0; i < appsPoolProxy->applicationsList().size(); i++)
        addApplication(appsPoolProxy->applicationsList().at(i));

    connect(appsPoolProxy, SIGNAL(serviceRegistered(const TAppInfo&)), this, SLOT(addApplication(const TAppInfo&)));
    connect(appsPoolProxy, SIGNAL(serviceUnregistered(const TAppInfo&)), this, SLOT(removeApplication(const TAppInfo&)));
    connect(appsPoolProxy, SIGNAL(error(const QString&)), this, SLOT(showError(const QString&)));


    /* connections between tree widgets */
 //   connect(ui->appsListW, SIGNAL(itemSelectionChanged()), this, SLOT(appListItemSelectionChanged()));
    connect(ui->appsListW, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(appListItemSelectionChanged()));

    connect(ui->widgetListW, SIGNAL(itemSelectionChanged()), this, SLOT(widgetListItemSelectionChanged()));
    connect(ui->slotsListW, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slotItemClicked()));
//    connect(ui->twActions, SIGNAL(itemSelectionChanged()), this, SLOT(actionSelectionChanged()));
    connect(ui->twActions, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(actionSelectionChanged()));
    connect(ui->propertiesListW, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(propertiesItemClicked()));
    connect(ui->pbExecute, SIGNAL(clicked()), this, SLOT(execute()));
    connect(ui->pbRead, SIGNAL(clicked()),  this, SLOT(read()));

    connect(ui->leArg1, SIGNAL(textChanged(QString)), this, SLOT(le1TextChanged(QString)));
    ui->pbExecute->setDisabled(true); /* is enabled when there's text on leArg1 */

    ui->appsListW->setToolTip("List of \"TApplications\"");
    ui->widgetListW->setToolTip("List of widgets of the selected application\n(Note: widgets must have an object name"
                                "\nin order to be in the list)");

    m_supportedTypes << "int" << "short" << "unsigned int" << "unsigned short" << "bool" << "long" << "unsigned long"
            << "double" << "float" << "QString" << "void" << "QStringList";

    QString tooltip = "slots with compatible arguments:\n";
    foreach(QString s, m_supportedTypes)
        tooltip += s + "\n";
    ui->slotsListW->header()->setToolTip(tooltip);
    tooltip.replace("slots", "Qt properties");
    ui->propertiesListW->header()->setToolTip(tooltip);

    /* sorting enabled */
    foreach(QTreeWidget* w, findChildren<QTreeWidget*>())
    {
        w->setSortingEnabled(true);
	#if QT_VERSION < 0x050000
        w->header()->setResizeMode(QHeaderView::ResizeToContents);
	#else
        w->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	#endif
    }

    ui->stw2->setHidden(true);
    ui->stw3->setHidden(true);

    /* action auto refresh */
    m_actionRefreshTimer = new QTimer(this);
    m_actionRefreshTimer->setInterval(1000);
    m_actionRefreshTimer->setSingleShot(false);
    connect(m_actionRefreshTimer, SIGNAL(timeout()), this, SLOT(actionSelectionChanged()));
    connect(ui->cbActionAutoRefresh, SIGNAL(toggled(bool)), this, SLOT(setSelectedActionAutoRefreshEnabled(bool)));
    connect(ui->sbActRefreshPeriod, SIGNAL(valueChanged(int)), this, SLOT(setSelectedActionRefreshInterval(int)));

    /* Target execution events section */
    connect(ui->pbClearEvents, SIGNAL(clicked()), ui->twTargetExecutionEvents, SLOT(clear()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setSelectedActionAutoRefreshEnabled(bool e)
{
    e ? m_actionRefreshTimer->start(ui->sbActRefreshPeriod->value()) : m_actionRefreshTimer->stop();
}

void MainWindow::setSelectedActionRefreshInterval(int period)
{
    m_actionRefreshTimer->setInterval(period);
}

void MainWindow::le1TextChanged(QString s)
{
    ui->pbExecute->setEnabled(!s.isEmpty());
}

void MainWindow::addApplication(const TAppInfo& ai)
{
    AppsListTreeHelper helper;
    helper.addApplication(ui->appsListW, ai);
}

TAppInfo MainWindow::selectedItemAppInfo()
{
    QList<QTreeWidgetItem *> itemList = ui->appsListW->selectedItems();
    if(!itemList.size())
        return TAppInfo();
    QTreeWidgetItem *parentItem = itemList.first()->parent();
    QString appName = parentItem->text(0);
    pid_t pid = parentItem->text(2).toUInt();
    TApplicationsPoolProxy *tAppPoolProxy = findChild<TApplicationsPoolProxy *>("TAppsPoolProxy");

    TAppInfo ai = tAppPoolProxy->findApplication(appName, pid);
    return ai;
}

void MainWindow::setupTAppIf(const QTreeWidgetItem *item)
{
    TAppInfo ai = selectedItemAppInfo();
    if(!ai.isEmpty())
    {
        ui->widgetListW->clear();
        AppDbusInspector dbusInspector(this);
        qDebug () << "Getting service name list" << ai.dbusServiceName;
        /* widget list contains couples className/objectName */
        QStringList widgetList = dbusInspector.getWidgetList(ai.dbusServiceName);
        qDebug() << "got map, size " << widgetList.size();
        for(int i = 0; i < widgetList.size() - 1; i = i+2)
        {
            QTreeWidgetItem *it = new QTreeWidgetItem(ui->widgetListW, QStringList()<< widgetList.at(i) << widgetList.at(i+1));
        }
        ui->widgetListW->setToolTip(QString("Widget list of the selected application.\nNote: widgets must have an object name\n"
                                            "to be in the list.\n%1 widgets currently visualized").arg(widgetList.size()));
        if(!widgetList.size())
            QMessageBox::critical(this, "Error obtaining widget list", QString("Error: %1").arg(dbusInspector.lastError()));
    }
}

void MainWindow::setupDevThreadFactoryIf(const QTreeWidgetItem *it)
{
    TAppInfo ai = selectedItemAppInfo();
    if(!ai.isEmpty())
    {

        DeviceThreadFactoryInterface *dthreadif = findChild<DeviceThreadFactoryInterface *>();
        if(dthreadif)
            delete dthreadif;
        dthreadif = new DeviceThreadFactoryInterface(ai.dbusServiceName,
             "/DeviceThreadFactory", QDBusConnection::sessionBus(), this);
        qDebug() <<dthreadif->path() << dthreadif->service();
        if(dthreadif->isValid())
        {
            QStringList devThreads = dthreadif->threads();
            ui->twThreads->clear();
            foreach(QString t, devThreads)
                new QTreeWidgetItem(ui->twThreads, QStringList() << t);
        }
        else
            QMessageBox::critical(this, "Error obtaining DeviceThreadFactoryInterface", dthreadif->lastError().message());
    }
    else
        perr("setupDevThreadFactoryIf: selected app info null!");
}

void MainWindow::slotTargetExecuted(const QString &point, int timestamp, bool success, const QString &exception, const QString &data)
{
    QString succ, exc;
    success ? succ = "true" : succ = "false";
    success ? exc = "-" : exc = exception;
    QTreeWidgetItem *it = new QTreeWidgetItem(ui->twTargetExecutionEvents, QStringList() <<
                   point << QDateTime::fromTime_t(timestamp).toString() << succ << exc );
    if(!success)
        it->setToolTip(5, exc);
}

void MainWindow::connectActionFactorySignals(ActionFactoryInterface* currentIface)
{
    if(connect(currentIface, SIGNAL(targetExecuted(const QString &, int , bool , const QString &, const QString &)),
               this, SLOT(slotTargetExecuted(const QString &, int , bool , const QString &, const QString &))) )
        pinfo("Correctly connected targetExecuted signals coming from interface \"%s\"", qstoc(currentIface->service()));
    else
        perr("Failed connecting targetExecuted signals coming from current DBus interface \"%s\"", qstoc(currentIface->service()));

}

void  MainWindow::actionSelectionChanged()
{
    QList<QTreeWidgetItem *> itemList = ui->twActions->selectedItems();
    if(!itemList.size())
        return;

    QTreeWidgetItem *it = itemList.first();
    pstep("refreshing action %s %s %s", qstoc(it->text(0)), qstoc(it->text(1)), qstoc(it->text(2)));
    QWidget * w = ui->scrollArea->takeWidget();
    if(w)
        delete w;
    TAppInfo ai = selectedItemAppInfo();
    if(!ai.isEmpty())
    {

        ActionFactoryInterface *afif = this->findChild<ActionFactoryInterface *>();
        if(afif && afif->isValid())
        {
            QString point = it->text(0);
            int refMode = it->data(1, Qt::UserRole).toInt();
            QString argsAsSingleString = it->data(0, Qt::UserRole).toString();
            QStringList args;
            if(!argsAsSingleString.isEmpty())
                args = argsAsSingleString.split(",", QString::SkipEmptyParts);
            QString actionAsXml = afif->actionAsXml(point, args, refMode);
            printf("looking for action %s %d\n", qstoc(point), refMode);
            qslisttoc(args);
            printf("\e[0;35mgot\n\"%s\"\e[0m\n", qstoc(actionAsXml));
            TActionListHelper alhelper;
            w = alhelper.createWidget(actionAsXml);
            ui->scrollArea->setWidget(w);
            connect(w->findChild<QPushButton *>("ApplyButton"), SIGNAL(clicked()), this, SLOT(actionSelectionChanged()));
        }
        else if(afif)
            QMessageBox::critical(this, "Error obtaining DeviceThreadFactoryInterface", afif->lastError().message());
    }
    else
        perr("setupDevThreadFactoryIf: selected app info null!");
}


void MainWindow::setupActionFactoryIf(const QTreeWidgetItem *it)
{
    TAppInfo ai = selectedItemAppInfo();
    ui->twActions->clear();
    if(!ai.isEmpty())
    {
        ActionFactoryInterface *afif = this->findChild<ActionFactoryInterface *>();
        if(afif)
            delete afif;
        afif = new ActionFactoryInterface(ai.dbusServiceName,
             "/ActionFactory", QDBusConnection::sessionBus(), this);
        qDebug() <<afif->path() << afif->service();
        if(afif->isValid())
        {
            /* connect target executed signals */
            connectActionFactorySignals(afif);

            QStringList actionListAsXml = afif->actionsListAsXml();
            TActionListHelper alhelper;
            foreach(QString xml, actionListAsXml)
            {
                QStringList actionRepresentation = alhelper.getItemStringList(xml);
                QString args = actionRepresentation.takeLast();
                QString mode = TUtil::instance()->modeRepresentation((RefreshMode)actionRepresentation.at(1).toInt(), true);
                QTreeWidgetItem *it = new QTreeWidgetItem(ui->twActions, QStringList() << actionRepresentation.first() << mode);
                it->setData(0, Qt::UserRole, args);
                it->setData(1, Qt::UserRole, actionRepresentation.at(1).toInt());
            }

            qDebug() << "actionslist\n" << actionListAsXml;
        }
        else
            QMessageBox::critical(this, "Error obtaining ActionFactoryInterface", afif->lastError().message());
    }
    else
        perr("setupActionFactoryIf: selected app info null!");
}

void MainWindow::switchInterface(const QString& path)
{
    QList<QTreeWidgetItem *> itemList = ui->appsListW->selectedItems();
    if(!itemList.size())
        return;
    QTreeWidgetItem *item = itemList.first();

    /* stop the TAction refresh timer when selection changes in application list */
    ui->cbActionAutoRefresh->setChecked(false);

    ui->stw3->setVisible(!path.contains("DeviceThreadFactory"));
    ui->stw2->setHidden(false);

    if(path.contains("ActionFactory"))
    {
        ui->stw2->setCurrentIndex(2);
        ui->stw3->setCurrentIndex(2);
        setupActionFactoryIf(item);
    }
    else if(path.contains("DeviceThreadFactory"))
    {
        ui->stw2->setCurrentIndex(1);
        ui->stw3->setCurrentIndex(1);
        setupDevThreadFactoryIf(item);
    }
    else if(path.contains("TApplication"))
    {
        ui->stw2->setCurrentIndex(0);
        ui->stw3->setCurrentIndex(0);
        setupTAppIf(item);
    }
}

void MainWindow::removeApplication(const TAppInfo& app)
{
    /* suppose that it is enough to identify the application by its name and pid */
    foreach(QTreeWidgetItem *it, ui->appsListW->findItems("*", Qt::MatchWildcard))
        if(it->text(0) == app.appName &&  it->text(2).toInt() == app.pid)
            delete it;
}

void  MainWindow::appListItemSelectionChanged()
{
    QList<QTreeWidgetItem *> itemList = ui->appsListW->selectedItems();
    if(!itemList.size())
        return;
    switchInterface(itemList.first()->text(0));

}

void MainWindow::widgetListItemSelectionChanged()
{
    QList<QTreeWidgetItem *> widgetItemList = ui->widgetListW->selectedItems();
    if(!widgetItemList.size())
        return;
    QTreeWidgetItem *widgetIt = widgetItemList.first();
    /* get service */
    QList<QTreeWidgetItem *> appsItemList = ui->appsListW->selectedItems();
    if(!appsItemList.size())
        return;
    /* the application item is top level and always has children */
    QTreeWidgetItem *appIt = appsItemList.first()->parent();

    TApplicationsPoolProxy *tAppPoolProxy = findChild<TApplicationsPoolProxy *>("TAppsPoolProxy");
    TAppInfo ai = tAppPoolProxy->findApplication(appIt->text(0), appIt->text(2).toInt());

    if(!ai.isEmpty())
    {
        ui->slotsListW->clear();
        AppDbusInspector dbusInspector(this);
        QStringList slotList = dbusInspector.getMethodSignatureList(ai.dbusServiceName, widgetIt->text(0));
        for(int i = 0; i < slotList.size(); i++)
        {
            QString type;
            QRegExp rx(".*(\\(.*\\))");
            int pos = rx.indexIn(slotList.at(i));
            if(pos > -1)
            {
                QString arg;
                QStringList parts = rx.capturedTexts();
                if(parts.size() > 1)
                    arg = parts.at(1);
                /* remove parenthesis, and * (pointer type ) before checking supported  types */
                arg.remove("(").remove(")").remove("*");
                if(arg.isEmpty() || m_supportedTypes.contains(arg))
                    new QTreeWidgetItem(ui->slotsListW, QStringList()<< slotList.at(i));
                else
                    printf("slot %s not supported\n", qstoc(slotList.at(i)));
            }
            else
                printf("\e[1;31m regexp WIDGET_MAX_ROWSd in slot %s\e[0m\n", qstoc(slotList.at(i)));
        }
        ui->propertiesListW->clear();
        QStringList propertyList = dbusInspector.getPropertyList(ai.dbusServiceName, widgetIt->text(0));
        for(int i = 0; i < propertyList.size(); i++)
        {
            QString type;
            QRegExp rx(".*(\\[.*\\])");
            int pos = rx.indexIn(propertyList.at(i));
            if(pos > -1)
            {
                QString arg;
                QStringList parts = rx.capturedTexts();
                if(parts.size() > 1)
                    arg = parts.at(1);
                arg.remove("[").remove("]").remove("*");
                if(arg.isEmpty() || m_supportedTypes.contains(arg))
                    new QTreeWidgetItem(ui->propertiesListW, QStringList()<< propertyList.at(i));
                else
                    printf("property %s not supported\n", qstoc(propertyList.at(i)));
            }
            else
                printf("\e[1;31m regexp WIDGET_MAX_ROWSd in property %s\e[0m\n", qstoc(propertyList.at(i)));
        }
    }
}

void MainWindow::read()
{
    QList<QTreeWidgetItem *> widgetItemList = ui->widgetListW->selectedItems();
    if(!widgetItemList.size())
        return;
    QTreeWidgetItem *widgetIt = widgetItemList.first();
    /* get service */
    QList<QTreeWidgetItem *> appsItemList = ui->appsListW->selectedItems();
    if(!appsItemList.size())
        return;
    QTreeWidgetItem *appIt = appsItemList.first();
    if(!appsItemList.size())
        return;
    appIt = appIt->parent();
    /* get method */
    QList<QTreeWidgetItem *> methodItemList = ui->slotsListW->selectedItems();
    if(!methodItemList.size()) /* test whether a property is selected */
        methodItemList = ui->propertiesListW->selectedItems();
    if(!methodItemList.size())
        return;
    QTreeWidgetItem *slotItem = methodItemList.first();
    QString method = slotItem->text(0);
    qDebug() << "executing method" << method;

    TApplicationsPoolProxy *tAppPoolProxy = findChild<TApplicationsPoolProxy *>("TAppsPoolProxy");
    TAppInfo ai = tAppPoolProxy->findApplication(appIt->text(0), appIt->text(2).toInt());

    if(!ai.isEmpty())
    {
        AppDbusInspector dbusInspector(this);

        QVariant returnValue, arg1, arg2, arg3, arg4;
        qDebug() << "reading " << method << " on " <<  widgetIt->text(0) << "params" << arg1 << arg2 << arg3 << arg4;
        bool ret = dbusInspector.execute(ai.dbusServiceName, widgetIt->text(0),
                                         method,
                                         returnValue,
                                         arg1, arg2, arg3, arg4);
        if(!ret)
            QMessageBox::critical(this, "Error invoking remote method",
                                  QString("Error invoking \"%1\" on application \"%2\"\nService name: \"%3\"\n%4").arg(method)
                                  .arg(ai.appName).arg(ai.dbusServiceName).arg(dbusInspector.lastError()));
        qDebug() << "execute(): returned " << returnValue;
        ui->leReturnValue->setText(returnValue.toString());
    }
    else
        qDebug() << "application " << appIt->text(0) << "pid " << appIt->text(2) << "not found";
}

void MainWindow::execute()
{
    QList<QTreeWidgetItem *> widgetItemList = ui->widgetListW->selectedItems();
    if(!widgetItemList.size())
        return;
    QTreeWidgetItem *widgetIt = widgetItemList.first();
    /* get service */
    QList<QTreeWidgetItem *> appsItemList = ui->appsListW->selectedItems();
    if(!appsItemList.size())
        return;
    QTreeWidgetItem *appIt = appsItemList.first();
    if(!appsItemList.size())
        return;
    appIt = appIt->parent();
    /* get method */
    QList<QTreeWidgetItem *> methodItemList = ui->slotsListW->selectedItems();
    if(!methodItemList.size()) /* test whether a property is selected */
        methodItemList = ui->propertiesListW->selectedItems();
    if(!methodItemList.size())
        return;
    QTreeWidgetItem *slotItem = methodItemList.first();
    QString method = slotItem->text(0);
    qDebug() << "executing method" << method;

    TApplicationsPoolProxy *tAppPoolProxy = findChild<TApplicationsPoolProxy *>("TAppsPoolProxy");
    TAppInfo ai = tAppPoolProxy->findApplication(appIt->text(0), appIt->text(2).toInt());

    if(!ai.isEmpty())
    {
        AppDbusInspector dbusInspector(this);
        QVariant returnValue, arg1, arg2, arg3, arg4;

        arg1 = m_getArg(1);
        arg2 = m_getArg(2);
        arg3 = m_getArg(3);
        arg4 = m_getArg(4);

        qDebug() << "executing " << method << " on " <<  widgetIt->text(0) << "params" << arg1 << arg2 << arg3 << arg4;
        bool ret = dbusInspector.execute(ai.dbusServiceName, widgetIt->text(0),
                                         method,
                                         returnValue,
                                         arg1, arg2, arg3, arg4);
        if(!ret)
            QMessageBox::critical(this, "Error invoking remote method",
                                  QString("Error invoking \"%1\" on application \"%2\"\nService name: \"%3\"\n%4").arg(method)
                                  .arg(ai.appName).arg(ai.dbusServiceName).arg(dbusInspector.lastError()));
        qDebug() << "execute(): returned " << returnValue;
        ui->leReturnValue->setText(returnValue.toString());
    }
    else
        qDebug() << "application " << appIt->text(0) << "pid " << appIt->text(2) << "not found";
}

void MainWindow::m_getArgTypes()
{
    QTreeWidgetItem *selectedItem = NULL;
    foreach(QTreeWidgetItem *it, ui->slotsListW->selectedItems())
    {
        selectedItem = it;
        break;
    }
    if(!selectedItem) /* look into properties */
    {
        foreach(QTreeWidgetItem *it, ui->propertiesListW->selectedItems())
        {
            selectedItem = it;
            break;
        }
    }
    if(!selectedItem)
        return;
    m_argTypes.clear();
    /* not escaped : .*\(([a-zA-Z\s\*\&:]*)[,]?([a-zA-Z\s\*\&:]*)[,]?([a-zA-Z\s\*&:]*)[,]?([a-zA-Z\s\*&:]*)\) */
    QRegExp rx(".*\\(([a-zA-Z\\s\\*\\&:]*)[,]?([a-zA-Z\\s\\*\\&:]*)[,]?([a-zA-Z\\s\\*&:]*)[,]?([a-zA-Z\\s\\*&:]*)\\)");
    int pos = rx.indexIn(selectedItem->text(0));

    if(pos >= 0)
    {
        m_argTypes = rx.capturedTexts();
    }
    else
    {
        /* .*\[([a-zA-Z_]+)\] */
        rx.setPattern(".*\\[([a-zA-Z_:]+)\\]");
        pos = rx.indexIn(selectedItem->text(0));
        if(pos >= 0)
            m_argTypes = rx.capturedTexts();
    }

    /* m_argTypes contains the full capture in element 0 */
    if(m_argTypes.size())
        m_argTypes.removeFirst();
    m_argTypes.removeAll("");
    QList<QLineEdit *> ledits = findChildren<QLineEdit *>(QRegExp("leArg(\\d)"));
    foreach(QLineEdit *le, ledits)
    {
        rx.setPattern("leArg(\\d)"); /* sure we'll capture */
        rx.indexIn(le->objectName());
        int num = rx.capturedTexts().at(1).toInt();
        le->setEnabled(num <= m_argTypes.size());
        if(m_argTypes.size() >= num)
            le->setToolTip(m_argTypes.at(num - 1));
        else
            le->setToolTip("");
        if(le->toolTip().contains("stringlist", Qt::CaseInsensitive))
            le->setToolTip("StringList: strings separated by \",\"");
        else if(le->toolTip().contains("color", Qt::CaseInsensitive))
            le->setToolTip("QColor: type color as \"245,255,119\"\n"
                           "or \"243,233,245,127\", where last field is alphaChannel");
        else if(le->toolTip().contains("rect", Qt::CaseInsensitive))
            le->setToolTip("QRect: type rect as \"10,10,200,300\"\n"
                           "(x,y,width,height)");
        else if(le->toolTip().contains("size", Qt::CaseInsensitive))
            le->setToolTip("QSize: type size as \"200x300\"\n"
                           "(x,y,width,height)");
    }
}

QVariant MainWindow::m_getArg(int labelPos)
{
    QVariant v;
    bool ok;
    QString oName = QString("leArg%1").arg(labelPos);
    QLineEdit *le = findChild<QLineEdit *>(oName);
    if(!le)
        return v;
    if(!le->isEnabled() || le->text().isEmpty() || labelPos > m_argTypes.size())
        return v;
    QString txt = le->text();
    QString t = m_argTypes.at(labelPos - 1);
    if(t.contains("double", Qt::CaseInsensitive) && txt.toDouble(&ok) && ok)
        v = txt.toDouble();
    else if((t.contains("uint", Qt::CaseInsensitive) || t.contains(QRegExp("unsigned\\s+int"))) && txt.toUInt(&ok) && ok)
        v = txt.toUInt();
    else if((t.contains("ushort", Qt::CaseInsensitive) || t.contains(QRegExp("unsigned\\s+short"))) && txt.toUShort(&ok) && ok)
        v = txt.toUShort();
    else if((t.contains("ulong", Qt::CaseInsensitive) || t.contains(QRegExp("unsigned\\s+long"))) && txt.toULong(&ok) && ok)
        v = txt.toUInt();
    else if(t.contains("bool", Qt::CaseInsensitive))
    {
        if(txt.contains("true", Qt::CaseInsensitive) || (txt.toInt(&ok) != 0 && ok))
            v = true;
        else if(txt.contains("false", Qt::CaseInsensitive) || (txt.toInt(&ok) == 0 && ok))
            v = false;
    }
    else if(t.contains("int", Qt::CaseInsensitive))
    {
        v =  txt.toInt();
    }
    else if(t.contains("short", Qt::CaseInsensitive) && txt.toShort(&ok) && ok)
        v = txt.toShort();
    else if(t.contains("float", Qt::CaseInsensitive) && txt.toFloat(&ok) && ok)
        v = txt.toFloat();
    else if( t.contains("stringlist", Qt::CaseInsensitive))
    {
        QStringList sl = txt.split(",", QString::SkipEmptyParts);
        v = sl;
    }
    else if(t.contains("char") || t.contains("string", Qt::CaseInsensitive))
        v = txt;
    else if(t.contains("color", Qt::CaseInsensitive))
    {
        QStringList rgb = txt.split(",", QString::SkipEmptyParts);
        if(rgb.size() == 3)
            v = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
        else if(rgb.size() == 4)
            v = QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt(), rgb.at(3).toInt());
    }
    else if(t.contains("rect", Qt::CaseInsensitive))
    {
        QStringList rpars = txt.split(",", QString::SkipEmptyParts);
        if(rpars.size() == 4)
            v = QRect(rpars.at(0).toInt(), rpars.at(1).toInt(), rpars.at(2).toInt(), rpars.at(3).toInt());
    }
    else if(t.contains("size", Qt::CaseInsensitive))
    {
        QStringList spars = txt.split("x", QString::SkipEmptyParts);
        if(spars.size() == 2)
            v = QSize(spars.at(0).toInt(), spars.at(1).toInt());
    }
    qDebug() << __FUNCTION__ << "variant type recognized " << v.typeName();

    return v;
}


void MainWindow::slotItemClicked()
{
    foreach(QTreeWidgetItem* it, ui->propertiesListW->findItems("*", Qt::MatchWildcard))
        it->setSelected(false);
    m_getArgTypes();
}

void MainWindow::propertiesItemClicked()
{
    foreach(QTreeWidgetItem* it, ui->slotsListW->findItems("*", Qt::MatchWildcard))
        it->setSelected(false);
    m_getArgTypes();
}

void MainWindow::showError(const QString& message)
{
    QMessageBox::critical(this, "Error", message);
}

