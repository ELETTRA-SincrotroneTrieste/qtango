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

    for(size_t i = 0; i < d_ptr->curve->dataSize() - 1; i++)
    {
     //   printf("cycle for i=%d\n", i);
        x = newxdata.last();
        /* fill with points with the same y between x(i) and x(i+1)
         * every period milliseconds until we reach next real point
         * on c
         */
     //   while(x < c->x(i + 1))
        while(x < c->data()->sample(i + 1).x())
        {
            newxdata << x;
        //    newydata << c->y(i);
            newydata << c->data()->sample(i).y();
            x = x + d_ptr->period * 0.001;
          //  printf("\e[0;36m(%s,%.1f)\e[0m, ", qstoc(QDateTime::fromTime_t(newxdata.last()).toString()),
           //        newydata.last());
        }
        /* place the real x,y data */
        newxdata << c->data()->sample(i + 1).x();
        newydata << c->data()->sample(i + 1).y();
        printf("\e[0;32m(%s,%.1f)\e[0m, ", qstoc(QDateTime::fromTime_t(newxdata.last()).toString()),
               newydata.last());
    }
    printf("new xdata size %d ydata size %d\n\n", newxdata.size(), newydata.size());

    c->setData(newxdata, newydata);
    c->updateRawData();

    printf("interpolate() took %d milliseconds\n", t.elapsed());
}
