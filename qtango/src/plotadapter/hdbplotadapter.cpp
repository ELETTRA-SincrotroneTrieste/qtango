#include "hdbplotadapter.h"
#include <eplotcurve.h>
#include <EPlotLight>
#include <macros.h>
#include <QVariant>

/*! \brief The constructor
  *
  * @param parent an instance of EPlotLight
  */
HdbPlotAdapter::HdbPlotAdapter(EPlotLight *parent) : DbPlotAdapterInterface(parent)
{

}

EPlotCurve *HdbPlotAdapter::getCurve(const QString& source)
{
    EPlotCurve *curve = plot()->curve(source);
    if(!curve) /* must create a curve */
    {
        printf("\e[1;32m+ \e[0m creating new curve for %s [HDB]\n", qstoc(source));
        /* create a curve just in the same way EPlotLight does */
        curve = new EPlotCurve(plot(), source);
        plot()->addCurve(source, curve);
    }
    else
        printf("\e[1;35mHdbPlotAdapter::getCurve(): curve already in plot for %s\e[0m\n", qstoc(source));
    return curve;
}

/** \brief Given a tango source in the form a/b/c/AttributeName or a/b/c->CommandName,
  *        this method returns the curve name, which is exactly the same source name.
  *        (Since 4.3.4)
  *
  * \note In order to distinguish between historical and temporary data, test the
  *       isTemporary Qt property on the curve. For example:
  * \code
  * EPlotCurve *curve = plot->curve("a/b/c/d");
  * if(curve)
  * {
  *     bool isTemporary = curve->property("isTemporary").toBool();
  *     bool isHdb = !curve->property("isTemporary").toBool();
  * }
  * \endcode
  *
  * @param source the tango point associated to the historical curve.
  *
  * @return the historical curve name, which is the source name itself.
  *
  *
  */
QString HdbPlotAdapter::curveName(const QString& source)
{
    return source;
}

/** \brief returns the list of all the historical curves present in the plot.
  *
  * @return a list of EPlotCurve containing all the historical curves.
  *
  * The search criterium to individuate the historical curves is to test whether the
  * curve has the isTemporary Qt property set to false
  *
  */
QList<EPlotCurve *> HdbPlotAdapter::historicalCurvesList()
{
    QList<EPlotCurve *> ret;
    QList<QwtPlotCurve *> crvList = plot()->curves();
    foreach(QwtPlotCurve *c, crvList)
    {
        if(c->rtti() == QwtPlotCurve::Rtti_PlotUserItem + RTTI_CURVE_OFFSET)
        {
            EPlotCurve *curve = static_cast<EPlotCurve *>(c);
            if(!curve->property("isTemporary").toBool())
                ret << curve;
        }
    }
    return ret;
}

/*! \brief this method does nothing since 4.3.4, because the curve contains both data from
 *         historical database and live data from QTango.
  *
  */
 void HdbPlotAdapter::uniformizeCurveStyles()
 {
     printf("HdbPlotAdapter::uniformizeCurveStyles(): dummy method since QTango 4.3.4");
 }
