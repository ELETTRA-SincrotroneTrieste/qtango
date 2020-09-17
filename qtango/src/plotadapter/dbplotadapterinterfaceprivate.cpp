#include "dbplotadapterinterfaceprivate.h"
#include "dbplotadapterinterface.h"
#include "dbplotbackground.h"
#include "dbcurveinterpolator.h"
#include <qwt_plot_item.h>
#include <QtDebug>
#include <EPlotLight>
#include <TPlotLightMarker>
#include <eplotcurve.h>
#include <QDateTime>
#include <QVariant>

DbPlotAdapterInterfacePrivate::DbPlotAdapterInterfacePrivate(DbPlotAdapterInterface* dbPlotAdapterIf,
                                                             EPlotLight *parent) : q_ptr(dbPlotAdapterIf)
{
    m_plot = parent;
    /* The hdb plot can represent uncorrelated data, with Y scales very different
     * from each other. So if the plot is a TPlotLightMarker, we must disable the
     * bounds from auto configuration property.
     */
    TPlotLightMarker *plm = qobject_cast<TPlotLightMarker *>(m_plot);
    if(plm)
        plm->setBoundsFromAutoConfiguration(false);
    m_plot->setXAxisAutoscaleEnabled(true);
 //   m_plot->setYAxisAutoscaleEnabled(true);
    m_plot->setDateOnTimeScaleDrawEnabled(true);
    m_plot->setProperty("DbPlotAdapterEnabled", true);
    m_background = NULL;
}

void DbPlotAdapterInterfacePrivate::setData(const QString& source,
                                            const QVector<double >&constHistoricalXData,
                                            const QVector<double> &constHistoricalYData)
{
    QTime t;
    t.start();
    QVector<double > xData, yData;
    qDebug() << "const historical data " << constHistoricalXData<< constHistoricalYData;
    bool qtangoCurvePresent = true;
    if(constHistoricalXData.size() == 0 || (constHistoricalXData.size() != constHistoricalYData.size()))
        return;

    /* get a curve with the given source */
    EPlotCurve *curve = q_ptr->plot()->curve(source);
    if(!curve)
    {
        qtangoCurvePresent = false;
        curve = new EPlotCurve(plot(), source);
        plot()->addCurve(source, curve);
    }

    /* getCurve() creates or finds the curve associated to the source.
     * Depending on the type of DbPlotAdapterInterface (hdb or tdb), the
     * curve name will contain [HDB] or [TDB] appended to the name.
     * Moreover, the curve will have the same style of an already present
     * curve with name 'source'.
     */
    QString curveName = curve->title().text();

    /* toggle curve set point visibility */
    if(curveName.contains("[set]"))
        curve->setVisible(m_plot->property("m_setPointEnabled").isValid() &&
                          m_plot->property("m_setPointEnabled").toBool());

    int dataSize = curve->dataSize();

    /*
     * if no data has been read live by QTango, then we have to fill at least
     * two points in order to draw a line
     */
    if(dataSize == 0)
    {
        xData = constHistoricalXData;
        yData = constHistoricalYData;
    }
    else /* mix historical data with live data */
    {
        size_t i = 0;
        int j = 0;
        while(i < dataSize && j < constHistoricalXData.size())
        {
            /* mix data
             * Historical data is in seconds, data from QTango in milliseconds!
            */
            const double& x_ts = curve->data()->sample(i).x() / 1000.0;
            if(x_ts < constHistoricalXData[j])
            {
                xData << x_ts;
                yData << curve->data()->sample(i).y();
                i++;
            }
            else
            {
                xData << constHistoricalXData[j];
                yData << constHistoricalYData[j];
                j++;
            }
        }

        /* finish up */
        while(i < dataSize )
        {
            xData << curve->data()->sample(i).x() / 1000.0; // from QTango: milliseconds
            yData << curve->data()->sample(i).y();
            i++;
        }
        while(j < constHistoricalXData.size())
        {
            xData << constHistoricalXData[j];
            yData << constHistoricalYData[j];
            j++;
        }
    }
    curve->clearData();
    curve->appendData(xData.data(), yData.data(), xData.size());
    curve->updateRawData();

    if(interpolatorDataHash.contains(source))
    {
        DbCurveInterpolator interpolator(curve, interpolatorDataHash.value(source));
        interpolator.interpolate();
    }
    //curve->setVectorUpdateMode(true);
    m_plot->refresh();

    qDebug() << __FUNCTION__ << "time spent " << t.elapsed();
}

void DbPlotAdapterInterfacePrivate::setSetPointData(const QString& source,
                                                    const QVector<double >&xData, const QVector<double> &yData)
{
    /* create a curve name compliant with the name given by TPlotLightMarker */
    QString curveSetPointName = source + "[set]";
    setData(curveSetPointName, xData, yData);
}

void DbPlotAdapterInterfacePrivate::setBackgroundEnabled(bool en)
{
    if(!en && m_background)
    {
        delete m_background;
        m_background = NULL;
    }
    else if(en && !m_background)
    {
        m_background = new DbPlotBackground();
        m_background->attach(m_plot);
    }
}

void DbPlotAdapterInterfacePrivate::clearHistoricalData()
{
    foreach(EPlotCurve *crv, q_ptr->historicalCurvesList())
        crv->clearData();
    m_plot->refresh();
}

void DbPlotAdapterInterfacePrivate::deleteHistoricalCurves()
{
    printf("\e[1;31m deleteHistoricalCurves() size of list %d\e[0m\n", q_ptr->historicalCurvesList().size());

    foreach(EPlotCurve *crv, q_ptr->historicalCurvesList())
    {
        printf("\e[1;31m deleting historical curve \"%s\"\e[0m\n", qstoc(crv->title().text()));
        m_plot->removeCurve(crv->title().text());
    }
    m_plot->refresh();
}

void DbPlotAdapterInterfacePrivate::deleteHistoricalCurve(const QString& source)
{
    foreach(EPlotCurve *crv, q_ptr->historicalCurvesList())
    {
        if(crv->title().text() == q_ptr->curveName(source) ||
                crv->title().text() == q_ptr->curveName(source + "[set]"))
            m_plot->removeCurve(crv->title().text());
    }
    m_plot->refresh();
}

void DbPlotAdapterInterfacePrivate::clearCurveHistory(const QString& source)
{
    foreach(EPlotCurve *crv, q_ptr->historicalCurvesList())
    {
        if(crv->title().text() == q_ptr->curveName(source) ||
                crv->title().text() == q_ptr->curveName(source + "[set]"))
            crv->clearData();
    }
    m_plot->refresh();
}

void DbPlotAdapterInterfacePrivate::addBackground(double start, double stop)
{
    if(m_background)
        m_background->addInterval(start, stop);
}

