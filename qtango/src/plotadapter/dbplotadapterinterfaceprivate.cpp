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

    qDebug() << __FUNCTION__ << "source " << source << "curve " << curve;

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
//        printf("\e[1;33mcaso dataSize == 0: no data read from tango yet\e[0m\n");
        xData = constHistoricalXData;
        yData = constHistoricalYData;
  //      if(qtangoCurvePresent)
  //      {
  //          xData << QDateTime::currentDateTime().toTime_t();
  //          yData << yData.last();
  //      }
    }
    else /* mix historical data with live data */
    {
        size_t i = 0;
        int j = 0;
        while(i < dataSize && j < constHistoricalXData.size())
        {
            /* mix data */
            if(curve->data()->sample(i).x() < constHistoricalXData[j])
            {
                xData << curve->data()->sample(i).x();
//                printf("\e[0;35mOLD CURVE: adding %s\e[0m, ", qstoc(QDateTime::fromTime_t(xData.last()).toString()));
                yData << curve->data()->sample(i).y();
                i++;
            }
            else
            {
                xData << constHistoricalXData[j];
//                printf("\e[1;32mNEW: adding %s\e[0m, ", qstoc(QDateTime::fromTime_t(constHistoricalXData[j]).toString()));
                yData << constHistoricalYData[j];
                j++;
            }
        }

//        printf("\e[1;31mfinish up: i %d dataSIze %d j %d histo data size %d\e[0m\n",
//               i, dataSize, j, constHistoricalXData.size());

        /* finish up */
        while(i < dataSize )
        {
            xData << curve->data()->sample(i).x();
            yData << curve->data()->sample(i).y();
            i++;
        }
        while(j < constHistoricalXData.size())
        {
            xData << constHistoricalXData[j];
            yData << constHistoricalYData[j];
            j++;
        }

//        double x1 = curve->data()->sample(0).x();
//        double lastHistoricalX = constHistoricalXData.last();
//        if(lastHistoricalX <= x1)
//        {
//            printf("\e[1;33mcaso last x (%f) < x1 (%f)\e[0m\n", lastHistoricalX, x1);
//            /* first historical data */
//            xData = constHistoricalXData;
//            yData = constHistoricalYData;
//            /* then already acquired live data */

//            /// aggiungere un punto poco prima??? per far vedere che costante
//            /// fino al dato live???
//            for(int i = 0; i < dataSize; i++)
//            {
//                xData << curve->data()->sample(i).x();
//                yData << curve->data()->sample(i).y();
//            }
//        }
//        else if(lastHistoricalX > x1)
//        {
//            printf("\e[1;33mcaso last x (%f) (%s) > x1 (%f) (%s) data siz %d\e[0m\n", lastHistoricalX, x1,
//                   qstoc(QDateTime::fromTime_t(lastHistoricalX).toString()),
//                   qstoc(QDateTime::fromTime_t(x1).toString()), curve->dataSize());
//            qDebug() << "lastHistoricalX" << QDateTime::fromTime_t(lastHistoricalX)
//                        << QDateTime::fromTime_t(x1);
//            xData = constHistoricalXData;
//            yData = constHistoricalYData;
//            /* maybe setSource was called before the hdb call and the hdb stop date
//             * is in the future with respect to the first tango read(s).
//             * Remove all the historical data preceding the first read.
//             */
//            double x = lastHistoricalX;
//            while(x >= x1)
//            {
//                /* remove last */
//                xData.remove(xData.size() - 1, 1);
//                yData.remove(yData.size() - 1, 1);
//                if(xData.size())
//                    x = xData.last();
//                else
//                    break;
//            }
//            for(int i = 0; i < dataSize; i++)
//            {
//                xData << curve->data()->sample(i).x();
//                yData << curve->data()->sample(i).y();
//            }
//        }

    }
//    printf("curve->setData in DbPlotAdapterInterfacePrivate::SetData\e[1;35m: %s\e[0m\n", qstoc(source));
//    for(int i = 0; i  < xData.size(); i++)
//        printf("\e[0;36m%s\e[0m, ", qstoc(QDateTime::fromTime_t(xData.at(i)).toString()));
//    printf("\n==================\e[1;33m==============\e[0m=====================\n");
//    qDebug() << yData;
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

