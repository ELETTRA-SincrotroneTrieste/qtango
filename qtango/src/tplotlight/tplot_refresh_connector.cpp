#include "tplot_refresh_connector.h"
#include "tplotlight_marker.h"
#include <eplotlight.h>
#include <eplotcurve.h>
#include <colors.h>
#include <QTimer>
#include <QMenu>
#include <QCoreApplication>
#include <QtDebug>

#define PLOT_REFRESH_INTERVAL (d_period * 0.9)

TPlotRefreshConnector::TPlotRefreshConnector(QWidget *parent) :
        QTangoWidgetCommon(parent),
        ProxyInterface()
{
    d_parentWidget = parent;
    d_period = 1000;
    d_refreshMode = AUTO_REFRESH;
    d_autoConfiguration = true;
    d_hideEventEnabled = false;
    d_fillFromHistoryEnabled = false;
    d_plotRefreshTimer = new QTimer(parent);
    d_plotRefreshTimer->setSingleShot(false);
    d_plotRefreshTimer->setInterval(PLOT_REFRESH_INTERVAL);
    QObject::connect(d_plotRefreshTimer, SIGNAL(timeout()), parent, SLOT(refresh()));
}

PlotComProxyReader *TPlotRefreshConnector::reader(const QString &sourceName)
{
    if(m_readersMap.values().contains(sourceName))
	return m_readersMap.key(sourceName);
    return NULL;
}

TPlotRefreshConnector::~TPlotRefreshConnector()
{
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(PlotComProxyReader *reader, readers)
    {
        delete reader;
    }
    m_readersMap.clear();
}

void TPlotRefreshConnector::restartAcquisition()
{
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(PlotComProxyReader *reader, readers)
    {
	reader->connectNewDataSignal();
	reader->setPeriod(d_period);
	d_plotRefreshTimer->setInterval(PLOT_REFRESH_INTERVAL);
    }
}

void TPlotRefreshConnector::pauseAcquisition()
{
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(PlotComProxyReader *reader, readers)
    {
	reader->disconnectNewDataSignal();
	reader->setPeriod(3600 * 10 * 1000);
	d_plotRefreshTimer->setInterval(3600 * 10 * 1000);
    }
}

void TPlotRefreshConnector::unsetSources()
{
    EPlotLight *plot;
    plot = qobject_cast<EPlotLight *>(d_parentWidget);
    if(plot)
    {
        d_plotRefreshTimer->setInterval(3600 * 10 * 1000);
        QList<PlotComProxyReader *> readers = m_readersMap.keys();
        foreach(PlotComProxyReader *reader, readers)
        {
            qprintf("\e[1;31munsetting source \"%s\" on plot \"%s\"\e[0m\n", qstoc(reader->realSource()), qstoc(plot->objectName()));
            reader->disconnect();
            reader->unsetSource();
        }
    }
}

void TPlotRefreshConnector::configureAndConnectReader(PlotComProxyReader *reader)
{
    EPlotLight *plot;
    plot = qobject_cast<EPlotLight *>(d_parentWidget);
    if(plot)
    {
	reader->setPeriod(d_period);
	reader->setRefreshMode(d_refreshMode);
	reader->setAutoConfiguration(d_autoConfiguration);
	reader->enableHideEvents(d_hideEventEnabled);
	QObject::connect(reader, SIGNAL(newData(const TVariant&)),
                         plot, SLOT(updateCurve(const TVariant&)));

	/* configuration opportunities */
	QObject::connect(reader, SIGNAL(sourceUpperBound(const double)), plot, SLOT(extendYUpperBoundFromAutoConf(const double)));
	QObject::connect(reader, SIGNAL(sourceLowerBound(const double)), plot, SLOT(extendYLowerBoundFromAutoConf(const double)));
	QObject::connect(reader, SIGNAL(sourceConfigurationAvailable(const  TangoConfigurationParameters *)), plot, SLOT(autoConfiguration(const TangoConfigurationParameters *)));
    }
}

void TPlotRefreshConnector::m_insertReader(PlotComProxyReader *r, const QString& source)
{
    m_readersMap.insert(r, m_makeCurveTitleFromSource(source));
}

PlotComProxyReader *TPlotRefreshConnector::m_readerFromSource(const QString& source)
{
    QString cleanSource = m_makeCurveTitleFromSource(source);
    if(m_readersMap.values().contains(cleanSource))
        return m_readersMap.key(cleanSource);
    return NULL;
}

QString TPlotRefreshConnector::m_makeCurveTitleFromSource(const QString& source)
{
    QString title = source;
    title.remove(QRegExp("\\(.*\\)"));
    return title;
}

void TPlotRefreshConnector::setSourcesList(QStringList &sl)
{
    if(sl.size())
	setSources(sl.join(";"));
}

QStringList TPlotRefreshConnector::sourcesList()
{
    QStringList srcs;
    if(d_sources.contains(";"))
	srcs = d_sources.split(";", QString::SkipEmptyParts);
    else if(!d_sources.isEmpty())
	srcs << d_sources;
    return srcs;
}

void TPlotRefreshConnector::_addSource(const QString& s)
{
    QString realSource(s);
    TUtil::replaceWildcards(realSource, qApp->arguments());
    PlotComProxyReader *reader = m_readerFromSource(realSource);
///    printf("\e[1;33m_addSource(): reader found for \"%s\" : %p\e[0m\n", qstoc(realSource), reader);
    if(!reader) /* must create a new reader because there is no reader associated yet to source s */
    {
        reader = new PlotComProxyReader(d_parentWidget, realSource);
        QObject::connect(reader->qtangoComHandle(), SIGNAL(connectionOk(bool)), d_parentWidget, SIGNAL(connectionOk(bool)));
        QObject::connect(reader->qtangoComHandle(), SIGNAL(connectionErrorMessage(const QString&)), d_parentWidget, SIGNAL(connectionErrorMessage(const QString&)));
        /* add the new reader to the sources map */
        m_insertReader(reader, realSource);
    }
    else
        reader->disconnect();
    configureAndConnectReader(reader);
    /* create EPlotCurve. updateSource() would add a curve on the fly if needed, but the user might be
     * interested in retrieving the available curves just right after calling setSource()
     */
    QString curveTitle = m_makeCurveTitleFromSource(realSource);
    EPlotCurve *newCurve = new EPlotCurve(d_parentWidget, curveTitle);
    static_cast<EPlotLight *>(d_parentWidget)->addCurve(curveTitle, newCurve);
    reader->setSource(realSource);
    d_sources += ";" + s; /* s, not realSource! */
    d_plotRefreshTimer->setInterval(PLOT_REFRESH_INTERVAL);
}

void TPlotRefreshConnector::setSources(const QString& sources)
{
    QStringList srcs, wildcardReplacedSrcs, sourcesWithoutArgs;
    QStringList readerNames;
    EPlotLight *plot = qobject_cast<EPlotLight *>(d_parentWidget);
    if(!d_sources.isEmpty())
        unsetSources();

    if(!sources.isEmpty())
    {
        if(sources.contains(";"))
            srcs = sources.split(";", QString::SkipEmptyParts);
        else
            srcs << sources;

        foreach(QString src, srcs)
        {
            TUtil::replaceWildcards(src, qApp->arguments());
            wildcardReplacedSrcs << src;
            sourcesWithoutArgs << m_makeCurveTitleFromSource(src);
        }
        /* get the title from the readers' source */
        foreach(QString source, m_readersMap.values())
        {
            /* the readers map values contains sources with wildcards. Substitute them
             * to reconstruct the curve titles associated with the values.
             */
            TUtil::replaceWildcards(source, qApp->arguments());
            readerNames << m_makeCurveTitleFromSource(source);
        }
        foreach(QString readerName, readerNames)
        {
            if(!sourcesWithoutArgs.contains(readerName))
            {
                pinfo("removing \e[0;35mleftover curve \"%s\"\e[0m", qstoc(readerName));
                plot->removeCurve(readerName);
                if(plot->curve(readerName + "[set]"))
                    plot->removeCurve(readerName + "[set]");
                m_readersMap.remove(m_readersMap.key(readerName));
            }
        }
        foreach(QString s, srcs)
        {
      //      printf("set sources: \e[1;34msource \"%s\"\e[0m READERS IN MAP %d\n", qstoc(s), m_readersMap.size());
            _addSource(s);
        }
        d_plotRefreshTimer->setInterval(PLOT_REFRESH_INTERVAL);
    }
    d_sources = sources;
}
/* test/device/1/double_scalar $1/double_scalar test/device/1/long_scalar test/device/2/long_scalar 
    test/device/1/double_spectrum   test/device/2/double_spectrum test/device/2/double_spectrum_ro
*/
void TPlotRefreshConnector::setPeriod(int period)
{
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(PlotComProxyReader * r, readers)
        r->setPeriod(period);
    d_period = period;
    d_plotRefreshTimer->setInterval(PLOT_REFRESH_INTERVAL);
}

void TPlotRefreshConnector::setRefreshMode(RefreshMode rm)
{
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(PlotComProxyReader * r, readers)
        r->setRefreshMode(rm);
    d_refreshMode = rm;
}

void TPlotRefreshConnector::setDesignerMode(bool s)
{
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(PlotComProxyReader * r, readers)
        r->setDesignerMode(s);
}

bool TPlotRefreshConnector::designerMode()
{
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(PlotComProxyReader * r, readers)
        return r->designerMode();
    return false;
}

QTangoCommunicationHandle* TPlotRefreshConnector::qtangoComHandle() const
{
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    if(readers.size())
	return readers.first()->qtangoComHandle();
    return NULL;
}

QList<QTangoCommunicationHandle*> TPlotRefreshConnector::handleList() const
{
    QList<QTangoCommunicationHandle*> list;
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(PlotComProxyReader * r, readers)
        list << r->qtangoComHandle();
    return list;
}

QString TPlotRefreshConnector::tangoPoint() const
{
    QStringList realSources;
    QList<PlotComProxyReader *> readers = m_readersMap.keys();
    foreach(QTangoComProxyReader *reader, readers)
        realSources << reader->realSource();
    return realSources.join(";");
}

/* reimplements QTangoWidgetCommon's */
void TPlotRefreshConnector::changeTangoPoint(const QString &source)
{
    d_pastedSource = source;
    if(sources().isEmpty())
	setSources(pastedSource());
    else
    {
	QMenu *menu = new QMenu("Actions", widget());
	menu->setPalette(QPalette(Qt::white));
	menu->addAction("Add source", dynamic_cast<TPlotLightMarker *>(this), SLOT(addPastedSource()));
	menu->addAction("Substitute source", dynamic_cast<TPlotLightMarker *>(this), SLOT(substitutePastedSource()));
	menu->addSeparator();
	menu->addAction("Cancel");
	menu->exec(QCursor::pos());
    }
}

bool TPlotRefreshConnector::hideEventEnabled()
{
    return d_hideEventEnabled;
}

void TPlotRefreshConnector::setHideEventEnabled(bool en)
{
    foreach(PlotComProxyReader *r, m_readersMap.keys())
	r->enableHideEvents(en);
    d_hideEventEnabled = en;
}

void TPlotRefreshConnector::plotHideEvent()
{
    foreach(PlotComProxyReader *r, m_readersMap.keys())
    {
	r->hideEvent();
    }
}

void TPlotRefreshConnector:: plotShowEvent()
{
    foreach(PlotComProxyReader *r, m_readersMap.keys())
	r->showEvent();
}

