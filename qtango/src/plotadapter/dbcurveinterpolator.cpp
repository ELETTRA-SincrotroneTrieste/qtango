#include "dbcurveinterpolator.h"
#include <eplotcurve.h>

#include <QtDebug>
#include <macros.h>
#include <QDateTime>

class DbCurveInterpolatorPrivate
{
public:
    DbCurveInterpolatorPrivate() {}

    EPlotCurve *curve;

    int period;
};

DbCurveInterpolator::DbCurveInterpolator(EPlotCurve *curve, int period) :
    QObject(curve)
{
    d_ptr = new DbCurveInterpolatorPrivate();
    d_ptr->curve = curve;
    d_ptr->period = period;
}

void DbCurveInterpolator::interpolate()
{
    EPlotCurve *c = d_ptr->curve;
    double x;
    QVector<double> newxdata, newydata;

    if(c->dataSize() < 2)
        return;

    qprintf("\e[1;32m + interpolating: \"%s\"\e[0m:\n", qstoc(c->title().text()));
    qprintf("\e[1;32m + data size %lu\n", c->dataSize());

    /* place first (x,y) */
    newxdata << c->data()->sample(0).x();
    newydata << c->data()->sample(0).y();

    QTime t;
    t.start();

    for(size_t i = 0; i < d_ptr->curve->data()->size() - 1; i++)
    {
        x = newxdata.last();
        /* fill with points with the same y between x(i) and x(i+1)
         * every period milliseconds until we reach next real point
         * on c
         */

//        printf("DbCurveInterpolator::interpolate \e[0;32m(%s,%.1f next sample at index %d %s)\e[0m\n", qstoc(QDateTime::fromTime_t(newxdata.last()).toString()),
//               newydata.last(), i+1, qstoc(QDateTime::fromTime_t(c->data()->sample(i + 1).x()).toString()));

        x += d_ptr->period * 0.001;
        while(0.3  < c->data()->sample(i + 1).x() - x)
        {
            newxdata << x;
            newydata << c->data()->sample(i).y();
            printf("(\e[0;36m%s,%.1f - period %d \e[1;31mdelta %f\e[0m [x:%f sample %f])\e[0m, ", qstoc(QDateTime::fromTime_t(newxdata.last()).toString()),
                   newydata.last(), d_ptr->period, c->data()->sample(i + 1).x() - x, x, c->data()->sample(i + 1).x());

            x += d_ptr->period * 0.001;
        }
        /* place the real x,y data */
        newxdata << c->data()->sample(i + 1).x();
        newydata << c->data()->sample(i + 1).y();
    }
//    printf("DbCurveInterpolator::interpolate new xdata size %d ydata size %d\n\n", newxdata.size(), newydata.size());

    c->setData(newxdata, newydata);
    c->updateRawData();

//    printf("DbCurveInterpolator::interpolate interpolate() took %d milliseconds\n", t.elapsed());
}
