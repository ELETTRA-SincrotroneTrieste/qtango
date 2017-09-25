#include "qtangothdb.h"
#include "thdbsingleton.h"
#include <thdbconfigwidget.h>
#include <macros.h>
#include <QAction>
#include <TPlotLightMarker>
#include <thdbplot.h>
#include <plotadapter/dbplotadapterinterface.h>
#include <plotadapter/hdbplotadapter.h>
#include <plotadapter/tdbplotadapter.h>

QTangoTHdb::~QTangoTHdb()
{
    printf("\e[1;31m~QTangoTHdb()\e[0m\n");
    if(m_hdbWidget)
    {
        printf("\e[0;31mdeleting hdbWidget\e[0m\n");
        delete m_hdbWidget;
    }
}

QTangoTHdb::QTangoTHdb(QObject *parent, THdbBase::Type t) :
    QObject(parent)
{
    connect(THdbSingleton::instance(), SIGNAL(isArchived(const QString&,bool)),
            this, SLOT(sourceArchived(const QString&, bool)));
    m_action = NULL;
    m_hdbWidget = NULL;
}

void QTangoTHdb::isSourceArchived(const QString& src)
{
    printf("\e[1;32misSourceArchived \"%s\"\e[0m\n", qstoc(src));
    if(src.contains(';'))
    {
       foreach(QString s, src.split(";", QString::SkipEmptyParts))
           m_sources.insert(s);
    }
    else
        m_sources.insert(src);

    THdbSingleton::instance()->setType(THdbBase::Historical);
    foreach(QString s, m_sources)
    {
        printf("asking if archived source \"%s\"\n", qstoc(s));
            THdbSingleton::instance()->isArchived(s);
    }
    THdbSingleton::instance()->setType(THdbBase::Temporary);
    foreach(QString s, m_sources)
            THdbSingleton::instance()->isArchived(s);
    printf("LEAVING \e[1;33misSourceArchived\e[0m\n");
}

void QTangoTHdb::setupAction()
{
    if(!m_action)
    {
        m_action = new QAction(this);
        connect(m_action, SIGNAL(triggered()), this, SLOT(execute()));
    }
    m_action->setIcon(QIcon(":../icons/view-history.png"));
    m_action->setDisabled(true);
}

void QTangoTHdb::sourceArchived(const QString& src, bool archived)
{
    printf("\e[1;35m sourceArchived(%s, %d)\e[0m\n", qstoc(src), archived);
    if(m_action &&  m_sources.contains(src))
    {
        archived ? m_action->setIcon(QIcon(":../icons/dialog-ok-apply.png")) :
                   m_action->setIcon(QIcon(":../icons/edit-delete.png"));
        m_action->setEnabled(archived || m_action->isEnabled());
    }
}

void QTangoTHdb::confDialogDestroyed()
{
    /* configuration dialog does not exist anymore */
    m_hdbWidget = NULL;
}

void QTangoTHdb::execute()
{
    m_hdbWidget = new HdbWidget(NULL);
    m_hdbWidget->configWidget()->setSources(m_sources.toList());

    /* create plot for scalar numbers.
     * Its parent should be an HistoricalViewWidget.
     */
    printf("creating plot with \e[1;33mparent\e[0m] %s %s\n", (m_hdbWidget->historicalViewWidget()->metaObject()->className()),
           qstoc(m_hdbWidget->historicalViewWidget()->objectName()));
    THdbPlot *thdbPlot = new THdbPlot(m_hdbWidget->historicalViewWidget());
    /* tell thdbPlot which THdb instance to use */
    thdbPlot->setTHdb(THdbSingleton::instance());
    thdbPlot->setType(m_hdbWidget->configWidget()->hdb() ? THdbBase::Historical : THdbBase::Temporary);
    /* auto delete on close */
    m_hdbWidget->setAttribute(Qt::WA_DeleteOnClose, true);

    connect(m_hdbWidget->configWidget(), SIGNAL(viewClicked()), this, SLOT(updateView()));
    connect(m_hdbWidget->configWidget(), SIGNAL(sourceRemoved(const QString&)),
            thdbPlot, SLOT(removeHistoricalCurve(const QString&)));
    connect(m_hdbWidget->configWidget(), SIGNAL(liveEnabled(bool)),
            thdbPlot, SLOT(setLiveModeEnabled(bool)));
    connect(m_hdbWidget->configWidget(), SIGNAL(viewClicked()), m_hdbWidget, SLOT(showData()));

    /* be notified when the dialog is destroyed */
    connect(m_hdbWidget, SIGNAL(destroyed()), this, SLOT(confDialogDestroyed()));
    /* show the hdb widget with the configuration page */
    m_hdbWidget->showConfig();

    /* we do not want the main application to block while the dialog is executed.
     * So we call show() and we use WA_DeleteOnClose attribute on the dialog itself.
     */
    m_hdbWidget->show();
}

 void QTangoTHdb::updateView()
 {
    /* create or retrieve an already created plot adapter interface */
    HdbWidget *hdbWidget = m_hdbWidget;
    THdbPlot *plot =
            static_cast<THdbPlot*>(hdbWidget->historicalViewWidget()->widget(HistoricalViewWidget::NumberScalar));
    if(plot) /* if pai then also dlg (ensured by m_getAdapterIf) */
    {
        THdbBase::Type t;
        hdbWidget->configWidget()->hdb() ? t = THdbBase::Historical : t = THdbBase::Temporary;
        if(t != plot->type())
        {
            plot->setType(t);
            printf("\e[0;36mswitched database type to %d\e[0m\n", t);
        }

        QDateTime startDt = hdbWidget->configWidget()->startDateTime();
        QDateTime stopDt = hdbWidget->configWidget()->stopDateTime();
        QStringList sources = hdbWidget->configWidget()->sources();
        plot->getData(sources, startDt, stopDt);
    }
 }

