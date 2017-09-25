#include "tdbplotadapter.h"
#include <eplotcurve.h>
#include <EPlotLight>
#include <macros.h>

TdbPlotAdapter::TdbPlotAdapter(EPlotLight *parent) : DbPlotAdapterInterface(parent)
{

}

EPlotCurve *TdbPlotAdapter::getCurve(const QString& source)
{
    QString tdbSource = this->curveName(source);
    EPlotCurve *curveForSource = plot()->curve(source);

    EPlotCurve *curve = plot()->curve(tdbSource);
    if(!curve) /* must create a curve */
    {
        printf("\e[1;32m+ \e[0m creating new curve for %s (TDB)\n", qstoc(source));
        /* create a curve just in the same way EPlotLight does */
        curve = new EPlotCurve(plot(), tdbSource);
        plot()->addCurve(tdbSource, curve);
        if(curveForSource) /* found a curve for the source */
        {
            curve->setStyle(curveForSource->style());
            curve->setPen(curveForSource->pen());
            curve->setBrush(curveForSource->brush());
        }
    }
    else
        printf("\e[0;35mTdbPlotAdapter::getCurve(): curve already in plot for %s\e[0m\n", qstoc(tdbSource));

    return curve;
}

QString TdbPlotAdapter::curveName(const QString& source)
{
    return source + "[TDB]";
}

QList<EPlotCurve *> TdbPlotAdapter::historicalCurvesList()
{
    QList<EPlotCurve *> ret;
    QList<QwtPlotCurve *> crvList = plot()->curves();
    foreach(QwtPlotCurve *c, crvList)
    {
        if(c->rtti() == QwtPlotCurve::Rtti_PlotUserItem + RTTI_CURVE_OFFSET)
        {
            EPlotCurve *curve = static_cast<EPlotCurve *>(c);
            if(curve->title().text().endsWith("[TDB]"))
                ret << curve;
        }
    }
    return ret;
}

 void TdbPlotAdapter::uniformizeCurveStyles()
 {
    QList<EPlotCurve *> hCurves = historicalCurvesList();
    foreach(EPlotCurve *c, hCurves)
    {
        QString name = c->title().text();
        QString srcName = name.remove("[TDB]");
        QList<QwtPlotCurve *> crvs = plot()->curves();
        foreach(QwtPlotCurve *pc, crvs)
        {
            if(srcName.compare(pc->title().text(), Qt::CaseInsensitive) == 0)
            {
                printf("\e[1;32mYES match (TDB) %s %s\e[0m\n", qstoc(srcName), qstoc(pc->title().text()));
                pc->setStyle(c->style());
                pc->setBrush(c->brush());
                pc->setPen(c->pen());
            }
            else
                printf("\e[1;31mno match %s %s\e[0m\n", qstoc(srcName), qstoc(pc->title().text()));
        }
    }
 }
