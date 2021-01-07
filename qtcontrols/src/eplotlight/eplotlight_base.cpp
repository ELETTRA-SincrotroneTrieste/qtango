#include "eplotlight_base.h"
#include "ezoomer.h"
#include "eplotcurve.h"

#include <macros.h>
#include <colors.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <sys/time.h>
#include <QDateTime>
#include <QPainter>
#include <QPaintEvent>
#include <QtDebug>
#include <qwt_plot_marker.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>

#ifndef __FreeBSD__
    #include <values.h>
#endif

/* in time mode, the x axis scale is enlarged at most by this value.
 * Every TIME_SCALE_LOOKAHEAD seconds the time scale is enlarged of
 * TIME_SCALE_LOOKAHEAD seconds. See adjustXScale().
 */
#define TIME_SCALE_LOOKAHEAD 10

using namespace std; /* std::isnan issue across different compilers/compiling standards */

EScaleEngine::EScaleEngine() : QwtLinearScaleEngine()
{

}

void EScaleEngine::autoScale (int maxNumSteps, double &x1, double &x2, double &stepSize) const
{
//    printf(" \e[0;33mEScaleEngine::autoScale: maxNumSteps %d x1 %f x2 %f stepSize %f[0m\n",
//           maxNumSteps, x1, x2, stepSize);
    QwtLinearScaleEngine::autoScale(maxNumSteps, x1, x2, stepSize);
}

void EScaleEngine::divideInterval(double interval, int numSteps) const
{
//    printf(" \e[1;33mEScaleEngine::divideInterval: inteval %f steps %d\e[0m\n", interval, numSteps);
    QwtLinearScaleEngine::divideInterval(interval, numSteps);
}

QwtScaleDiv EScaleEngine::divideScale (double x1, double x2, int maxMajSteps, int maxMinSteps, double stepSize)
{
//    printf(" \e[2;33mEScaleEngine::divideScale\e[0m: x1 %f x2 %f\n", x1, x2);
    return QwtLinearScaleEngine::divideScale(x1, x2, maxMajSteps, maxMinSteps, stepSize);
}

void EScaleWidget::scaleChange()
{
//    printf("\e[1;36mEScaleWidget::scaleChanged()\e[0m\n");
    QwtScaleWidget::scaleChange();
}

QwtText ETimeScaleDraw::label(double v) const
{
    return QwtDateScaleDraw::label(v);
    QDateTime d;
    d.setTime_t((int) v );

    //   deltaT = tN - t0;
    //   if(deltaT <= 10)
    // 	return d.toString("hh:mm:ss");
    //   else if(deltaT <= 20 && d.time().second() % 2 == 0)
    // 	return d.toString("hh:mm:ss");
    //   if(d.time().second() == 0)
    if(!m_dateEnabled)
        return d.toString("hh:mm.ss");
    else
        return d.toString("dd/MM/yy\nhh:mm:ss");
    //   else
    // 	return QString();
}

bool ShiftClickEater::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if(me->modifiers() == Qt::ShiftModifier)
            return true;
    }
    return QObject::eventFilter(obj, event);
}


EPlotLightBase::EPlotLightBase(QWidget *parent) : QwtPlot(parent)
{
    init();
}

EPlotLightBase::EPlotLightBase(const QwtText &title, QWidget *parent) : QwtPlot(title, parent)
{
    init();
}

/* QwtPlot xAxis autoscale is disabled and axis autoscale is managed internally 
 * through the refresh() method.
 */
void EPlotLightBase::init()
{
    d_timeScaleDraw = NULL;
    d_titleOnCanvasEnabled = false;
    d_displayZoomHint = false;
    d_curvesStyle = Lines;
    setFrameStyle(QFrame::NoFrame);
    plotLayout()->setAlignCanvasToScales(true);
    plotLayout()->setCanvasMargin(0, QwtPlot::yLeft);
    plotLayout()->setCanvasMargin(0, QwtPlot::yRight);
    /* white background */
    setCanvasBackground(Qt::white);
    /* grid */
    QwtPlotGrid* plotgrid = new QwtPlotGrid;
    plotgrid->setPen(QPen(Qt::DotLine));
    plotgrid->attach(this);
    plotgrid->enableX(true);
    plotgrid->enableY(true);
    


    setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

    zoomer = new Zoomer(this->canvas());
    zoomer->setRubberBandPen(QPen(KDARKGRAY, 1, Qt::DotLine));
    zoomer->setTrackerPen(QPen(KGRAY));
    d_scheduleAdjustScales = false;
    /* after creating the zoomer */
    setTimeScaleDrawEnabled(true);

    EScaleEngine *eScaleEngineX = new EScaleEngine();
   eScaleEngineX->setAttribute(QwtScaleEngine::Floating, false);
    EScaleEngine *eScaleEngineY = new EScaleEngine();
 //   setAxisScaleEngine(QwtPlot::xBottom, eScaleEngineX);
    setAxisScaleEngine(QwtPlot::yLeft, eScaleEngineY);
    connect(zoomer, SIGNAL(zoomHint()), this, SLOT(displayZoomHint()));
    connect(zoomer, SIGNAL(removeZoomHint()), this, SLOT(eraseZoomHint()));

    /* axis autoscaling */
    setAxisAutoScale(QwtPlot::yLeft);
    /* NOTE: disable QwtPlot x axis autoscale */
    d_xAutoscale = true; /* enabled by default */
    d_yAutoscale = true;
    setAxisScale(QwtPlot::xBottom, 0, 1000);

    d_xAutoscaleAdjustment = d_yAutoscaleAdjustment = 2.0;
    d_xAutoscaleAdjustEnabled = d_yAutoscaleAdjustEnabled = true;
    setProperty("numericScale", true);

    QwtPlot::replot(); /* do not need EPlotLightBase::replot() here */

    //   connect(zoomer, SIGNAL(zoomed(const QwtDoubleRect &)), this, SLOT(plotZoomed(const QwtDoubleRect &)));
}

bool EPlotLightBase::timeScaleDrawEnabled()
{
    if(zoomer)
        return zoomer->xAsDate();
    return false;
}

void EPlotLightBase::setTimeScaleDrawEnabled(bool enable)
{
    /* if xAutoscale is enabled, scales are automatically adjusted at first refresh */
    if(!enable)
    {

  	setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw());
    setAxisScaleEngine(QwtPlot::xBottom, new EScaleEngine());
	/* next time replot() is invoked, adjust scales */
        // 	d_scheduleAdjustScales = true;
    }
    else
    {
        d_timeScaleDraw = new ETimeScaleDraw();
        setAxisScaleDraw(QwtPlot::xBottom, d_timeScaleDraw);
        setAxisLabelRotation(QwtPlot::xBottom, -50.0);
        setAxisScaleEngine(QwtPlot::xBottom, new QwtDateScaleEngine(Qt::LocalTime));
        setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

    }
    /* deleting d_timeScaleDraw produces a crash */
    replot();
    zoomer->setXAsDate(enable);
}

void EPlotLightBase::setDateOnTimeScaleDrawEnabled(bool en)
{
    Q_UNUSED(en);
   if(d_timeScaleDraw)
   {
   //    if(en)
    //       d_timeScaleDraw->setDateFormat(QwtDate::Day, "yyyy-MM-dd hh:mm:ss");
   }

       if(d_timeScaleDraw)
           d_timeScaleDraw->setDateEnabled(en);
}

bool EPlotLightBase::dateOnTimeScaleDrawEnabled() const
{
 //   if(d_timeScaleDraw)
 //       return d_timeScaleDraw->dateEnabled();
    return false;
}

/* This is the refresh method.

  */
void EPlotLightBase::refresh()
{
    int size, offset, bufsiz = -1;
    double x1 = 0.0, x2 = 0.0, xstart = 0.0, y1 = 0.0, y2 = 0.0;
    bool scaleChanged = false;
    bool forceReplot = false;
    bool vectorUpdateMode = false;
    bool inZoom = zoomer->zoomRectIndex() != 0;
    QPointF point;

    /* test whether curves are in vector update mode and if the state of one (or more) of
     * them is not EPlotCurve::Normal. In that case, data might not be updated on the curve
     * and the replot must be forced in all circumstances. For instance when TPlotLightMarker
     * detects invalid data, sets the state of the corresponding curve to Invalid but data
     * stored into the curve does not change. In this case, this method would not replot().
     *
     */
    QList<QwtPlotCurve *> curvesList = curves();
    foreach(QwtPlotCurve *c, curvesList)
    {
        if(c->rtti() == EPlotCurve::Rtti_PlotUserItem + RTTI_CURVE_OFFSET)
        {
            vectorUpdateMode = static_cast<EPlotCurve *>(c)->vectorUpdateMode();
            forceReplot |= static_cast<EPlotCurve *>(c)->state() != EPlotCurve::Normal;
            if(forceReplot) /* no need to go further: mixed update modes are unsupported */
                break;
        }
    }

    /* user might call replot after adding curves but before setting data on them:
   * it is not possible  to adjust scales if the curves haven't data yet.
   */
    if(d_scheduleAdjustScales && adjustScales())
        d_scheduleAdjustScales = false;

    if(!inZoom)
    {
        /* if autoscale adjust is required and x autoscale is required too, then call
         * adjustXScale here. The method adjustXScale calculates min and max values of
         * each curve and, if the current lower bound and upper bound need to be extended,
         * it extends them according to the minimum and maximum values of each curve and
         * to the specified x autoscale adjustment .
         */
        if(d_xAutoscaleAdjustEnabled && d_xAutoscale)
            scaleChanged |=  adjustXScale(d_xAutoscaleAdjustment, false); /* false: do not update() */
    }
    if((inZoom && !vectorUpdateMode) || (!inZoom && d_xAutoscale && !d_xAutoscaleAdjustEnabled)) /* not else if */
    {
        /* we need to calculate xstart and x1 in two cases:
         * 1. in zoom mode when _not_ in vectorUpdateMode, to set the zoom base;
         * 2. not in zoom mode when we have to autoscale the x axis without the x axis autoscale
         *    adjust enabled.
         */
        foreach(QwtPlotCurve* c, curvesList)
        {
            /* need to calculate x start and x1 when not in vectorUpdateMode (to scroll the zoom)
           * or when in optimizeXAutoscale mode (to rescale the plot)
           */
            if(/*vectorUpdateMode || */d_xAutoscale)
            {
                size = c->dataSize();
                if(size > 1)
                {
                    x1 = c->data()->sample(size - 1).x();
                    x2 = c->data()->sample(size - 2).x();
                    y1 = c->data()->sample(size - 1).y();
                    y2 = c->data()->sample(size - 2).y();

                    if(x2 == x1 && y1 == y2)
                    {
                        /* there is no need to update the plot if the same data has been inserted two times! */
//                        printf("\e[1;31m x1 e x2 uguali (%f)\e[0m\n", x2);
                        return;
                    }
                    xstart = c->data()->sample(0).x();
                }
                else if(size > 0)
                {
                    x1 = x2 = xstart = c->data()->sample(0).x();
                }
            }
            break;
        }
    }

    qprintf("EPlotLightBase::refresh() \"%s\": d_xAutoscale %d | d_xAutoscaleAdjustEnabled %d | d_xAutoscaleAdjustment %.2f\n",
           qstoc(objectName()), d_xAutoscale, d_xAutoscaleAdjustEnabled, d_xAutoscaleAdjustment);
    qprintf("EPlotLightBase::refresh() \"%s\": d_yAutoscale %d | d_yAutoscaleAdjustEnabled %d | d_yAutoscaleAdjustment %.2f\n",
           qstoc(objectName()), d_yAutoscale, d_yAutoscaleAdjustEnabled, d_yAutoscaleAdjustment);
    qprintf("EPlotLightBase::refresh() \"%s\" (Qwt) X axis autoscale %d, Y axis autoscale: %d\n", qstoc(objectName()),
           axisAutoScale(QwtPlot::xBottom), axisAutoScale(QwtPlot::yLeft));

    if(!inZoom)
    {

        if(d_xAutoscale && !d_xAutoscaleAdjustEnabled) /* put the scales manually */
        {
            /* change the scales only if needed */
//            printf("calling setAxisScaleIfNeeded with %f - %f\n", xstart, x1);
            scaleChanged |= setAxisScaleIfNeeded(QwtPlot::xBottom, xstart, x1);
        }
        if(d_yAutoscale && d_yAutoscaleAdjustEnabled)
        {
            /* this effectively adjusts scales only when necessary */
            scaleChanged |= adjustYScale(d_yAutoscaleAdjustment, false);
        }
        /* buggy zoomer? Sometimes scroll bars appear when unneeded. Switch to AlwaysOff when not zooming */
        if(zoomer->hScrollBarMode() != Qt::ScrollBarAlwaysOff)
            zoomer->setHScrollBarMode(Qt::ScrollBarAlwaysOff);
        if(zoomer->vScrollBarMode() != Qt::ScrollBarAlwaysOff)
            zoomer->setVScrollBarMode(Qt::ScrollBarAlwaysOff);
    }
    else /* inside zoom: the needed xstart and x1 have been calculated above */
    {
        /* Restore scroll bar mode policy to `As needed' */
        if(zoomer->hScrollBarMode() != Qt::ScrollBarAsNeeded)
            zoomer->setHScrollBarMode(Qt::ScrollBarAsNeeded);
        if(zoomer->vScrollBarMode() != Qt::ScrollBarAsNeeded)
            zoomer->setVScrollBarMode(Qt::ScrollBarAsNeeded);

        QRectF r = zoomer->zoomBase();
        /* obtain x data vector to establish an offset */
        /* look for a curve, if present */
        if(!vectorUpdateMode && d_xAutoscale)
        {
            offset = x1 - x2;
            qDebug() << "offset " << offset << "r.right()" << r.right()
                    << "new right" << r.right() + offset << "x1" << x1 << "x2" << x2;

            /* FIXME: bufsiz is locally defined here and initialized to -1: first if branch will
             * never be honoured.
             */
            if ((bufsiz != -1) || (x1 - x2) < bufsiz) /// never getting inside here
                r.setRight(r.right()+offset); /* make it bigger */
            else
                r.moveRight(r.right()+offset); /* just move */
            qDebug() << "r.left()" << r.left() << "new left" << r.left() - offset;
            r.setLeft(r.left() - offset);
            // 	zoomer->moveBy(offset, 0);
            zoomer->setZoomBase(r);
            forceReplot = true;
        }
        else
            zoomer->setZoomBase(r);
    }
    /* QwtPlot's replot() essentially calls updateAxes() and canvas->replot().
     * Both are needed, so directly call QwtPlot::replot().
     */
    if(scaleChanged || vectorUpdateMode || forceReplot)
     replot();

    /* set zoom base only if a value of the scale is really changed */
    if(!zoomer->zoomRectIndex() && scaleChanged)
    {
        zoomer->setZoomBase(false); /* false: do not trigger a replot() */
    }
}

void EPlotLightBase::drawCanvas(QPainter *p)
{
    QwtPlot::drawCanvas(p);
    QPen pen;
    QRectF myRect;
    QFont f = font();
    f.setPointSize(11.0);
    p->setFont(f);
    QFontMetrics fm(f);
    if(d_titleOnCanvasEnabled)
    {
	int i = 0;
	foreach(QwtPlotCurve * c, curves())
	{
            if(c->isVisible())
            {
		myRect.setWidth(width());
		myRect.setHeight(fm.height());
		myRect.setX(10);
		myRect.moveTop(20 + i * fm.height());
                // 	  printf("draw text x %f y %f w %f h %f\n", myRect.x(), myRect.y(), myRect.width(), myRect.height());
		p->setPen(c->pen());
		p->drawText(myRect, Qt::AlignLeft,  c->title().text());
		i++;
            }
	}
    }
    if(d_displayZoomHint && !zoomDisabled())
    {
	myRect.setSize(geometry().size());
	myRect.setX(10);
	myRect.setY(geometry().height() - axisWidget(QwtPlot::xBottom)->height() - 40);
	QColor txtColor = KDARKGREEN;
	txtColor.setAlpha(160);
	pen.setColor(txtColor);
	p->setPen(pen);
        // 	printf("Drawing control to zoom\n");
	p->drawText(myRect, Qt::AlignLeft,  "Press Shift Key to zoom");
    }
    /* no curves or curves without data? Inform the user */
    foreach(QwtPlotCurve * c, curves())
	if(c->dataSize() > 1)
            return;
    QString txt;
    curves().size() ? txt = "Waiting for data..." : txt = "No data";
    QFontMetrics fm2(f);
    p->setFont(f);
    myRect.setWidth(width());
    myRect.setHeight(fm2.height());
    myRect.setX(10);
    myRect.moveTop(canvas()->rect().height() -10 - fm2.height());
    QColor txtColor = KDARKGRAY;
    txtColor.setAlpha(160);
    pen.setColor(txtColor);
    p->setPen(pen);
    p->drawText(myRect, Qt::AlignLeft,  txt);
}

void EPlotLightBase::setTitleOnCanvasEnabled(bool en)
{
    d_titleOnCanvasEnabled = en;
    replot();
}

void EPlotLightBase::enableTrackerText(bool en)
{
    zoomer->enableTrackerText(en);
    if(en)
        zoomer->setTrackerMode(QwtPicker::AlwaysOn);
    else
        zoomer->setTrackerMode(QwtPicker::AlwaysOff);
}

bool EPlotLightBase::trackerTextEnabled()
{
    return zoomer->trackerTextEnabled();
}

QList<QwtPlotCurve *> EPlotLightBase::curves()
{
    QList<QwtPlotCurve *> curves;
    foreach(QwtPlotItem* i, itemList())
    {
        if(dynamic_cast<QwtPlotCurve* >(i))
        {
            QwtPlotCurve * c = dynamic_cast<QwtPlotCurve* >(i);
            curves.push_back(c);
        }
    }
    return curves;
}

void EPlotLightBase::setCurvesStyle(CurveStyle style)
{
    QList<QwtPlotCurve *> crvs = curves();
    int cs = style;
    foreach(QwtPlotCurve *c, crvs)
        c->setStyle((QwtPlotCurve::CurveStyle) cs);
    d_curvesStyle = style;
}


bool EPlotLightBase::setAxisScaleIfNeeded(int axisId, double min, double max, double step)
{
    if(axisId == QwtPlot::xBottom)
    {
	double xlb = xLowerBound();
	double xub = xUpperBound();
	if((min != xlb || max != xub) && (fabs(min - xlb) > 0.0001 * xlb ||  fabs(max - xub) > 0.0001 * xub))
	{
            QwtPlot::setAxisScale(axisId, min, max, step);
            return true;
	}
    }
    if(axisId == QwtPlot::yLeft)
    {
	double ylb = yLowerBound();
	double yub = yUpperBound();
	if((min != ylb || max != yub) && (fabs(min - ylb) > 0.0001 * ylb ||  fabs(max - yub) > 0.0001 * yub))
	{
            QwtPlot::setAxisScale(axisId, min, max, step);
            return true;
	}
    }
    return false;
}

bool EPlotLightBase::adjustXScale(double percent, bool do_replot)
{
    int appendMode = -1;
    double xMin = 0, xMax = 0, extend = 0;
    QList<QwtPlotCurve *> crvs = curves();
    foreach(QwtPlotCurve *c, crvs)
    {
        if(c->dataSize() < 1 || !c->isVisible()) /* it is not possible  to adjust scales if the curves haven't enough data yet. */
            continue;
        /* if curve is EPlotCurve, then test whether she is updated via appendData or full
         * setData.
         */
        if(appendMode == -1 && c->rtti() == EPlotCurve::Rtti_PlotUserItem + RTTI_CURVE_OFFSET)
            appendMode = !static_cast<EPlotCurve *>(c)->vectorUpdateMode();

        if(c->minXValue() < xMin || xMin == xMax)
            xMin = c->minXValue();
        if(c->maxXValue() > xMax || xMin == xMax)
            xMax = c->maxXValue();

    }
    if(xMin < xMax) /* values must be well ordered */
    {
        double xlb = xLowerBound();
        double xub = xUpperBound();
        double newXub, newXlb;
        /* in time mode, we try to minimize replots enlarging the x scale of a bunch of seconds
         * ahead.
         * Every TIME_SCALE_LOOKAHEAD seconds the time scale is enlarged of
         * TIME_SCALE_LOOKAHEAD seconds.
         */
        if(timeScaleDrawEnabled() && appendMode)
        {
            if(xub <= xMax)
            {
                /* need more space becaus current upper bound is less than or equal to
                 * current xMax. Extend xMax to ask more space for the x axis.
                 */
                xMax += TIME_SCALE_LOOKAHEAD; /* add some lookahead */
                extend = ((xMax - xMin) * percent/100.0);
                newXub = xMax + extend/2;
            }
            else
            {
                newXub = xub; /* no need to extend upper bound */
            }
        }
        else
        {
            extend = ((xMax - xMin) * percent/100.0);
            newXub = xMax + extend/2;
        }

        newXlb = xMin - extend/2;

        if((newXlb != xlb || newXub != xub)/* && (fabs(newXlb - xlb) > DBL_MIN ||  fabs(newXub - xub) > DBL_MIN)*/)
        {
            qprintf("adjusting x scale from %f to %f because it was %f/%f \n", newXlb,
                    newXub, xlb, xub);
            setAxisScale(QwtPlot::xBottom, newXlb, newXub);
            if(do_replot)
            {
                zoomer->setZoomBase(do_replot);
            }
            return true;
        }
    }
    return false;
}

void EPlotLightBase::getXBoundsFromCurves(double *xMin, double *xMax)
{
    *xMin = 0, *xMax = 0;
    QList<QwtPlotCurve *> crvs = curves();
    foreach(QwtPlotCurve *c, crvs)
    {
        if(c->dataSize() < 1 || !c->isVisible()) /* it is not possible  to adjust scales if the curves haven't enough data yet. */
            continue;
        if(c->minXValue() < *xMin || *xMin == *xMax)
            *xMin = c->minXValue();
        if(c->maxXValue() > *xMax || *xMin == *xMax)
            *xMax = c->maxXValue();
    }
}


double EPlotLightBase::getXScaleInterval(double xMin, double xMax)
{
    double percent = 0.0, extend;
    if(d_xAutoscaleAdjustEnabled)
        percent = d_yAutoscaleAdjustment;
    if(xMin < xMax) /* values must be well ordered */
    {
        extend = (xMax - xMin) * percent/100.0;
        xMin = xMin - extend/2;
        xMax = xMax + extend/2;
        return xMax - xMin;
    }
    return -1;
}

bool EPlotLightBase::adjustYScale(double percent, bool do_replot)
{
    double yMin = 0, yMax = 0, tmp = 0, extend = 0;
    QList<QwtPlotCurve *> crvs = curves();
    foreach(QwtPlotCurve *c, crvs)
    {
        if(c->dataSize() < 1 || !c->isVisible()) /* it is not possible  to adjust scales if the curves haven't enough data yet. */
            continue;
        /* y */
//        printf("\e[1;31m curve %s min val max val %f %f\e[0m\n", qstoc(c->title().text()),
//               c->minYValue(), c->maxYValue() );
        if(!std::isnan(c->minYValue()) && (c->minYValue() < yMin || yMin == yMax))
            tmp = c->minYValue(); /* to be able to make a valid test yMin == yMax below we need tmp */
        if(!std::isnan(c->maxYValue()) && (c->maxYValue() > yMax || yMin == yMax))
            yMax = c->maxYValue();
        yMin = tmp; /* finally update yMin */
    }

    if(yMin < yMax) /* values must be well ordered */
    {
        extend = (yMax - yMin) * percent/100.0;
        double ylb = yLowerBound();
        double yub = yUpperBound();
        double newYlb = yMin - extend/2;
        double newYub = yMax + extend/2;

	if((newYlb != ylb || newYub != yub)/* && (fabs(newYlb - ylb) > DBL_MIN ||  fabs(newYub - yub) > DBL_MIN)*/)
        {
            qprintf("adjusting y scale from %f to %f because it was %f/%f \n", yMin - extend/2,
                    yMax + extend/2, yLowerBound(), yUpperBound());
            setAxisScale(QwtPlot::yLeft, newYlb, newYub);
            if(do_replot)
            {
                zoomer->setZoomBase(do_replot);
            }
            // 	   qprintf("adjusted Y scale from %f to %f\n", yMin,yMax);
            return true;
        }
    }
    return false;
}

bool EPlotLightBase::adjustScales(double percent, bool do_replot)
{
    return adjustXScale(percent) && adjustYScale(percent, do_replot);
}

void EPlotLightBase::setAlignCanvasToScalesEnabled(bool en)
{
    if(plotLayout())
        plotLayout()->setAlignCanvasToScales(en);
}

bool EPlotLightBase::alignCanvasToScalesEnabled()
{
    if(plotLayout())
        return plotLayout()->alignCanvasToScale(QwtPlot::xBottom);
    return false;
}

void EPlotLightBase::setXAxisAutoscaleEnabled(bool autoscale)
{
    d_xAutoscale = autoscale;
    //    if(autoscale)
    // 	 d_xAutoscaleAdjustEnabled = false; /* disable autoscale adjust */
    /* if !d_autoscale make clear to QwtPlot that we do not want any x axis autoscale, although if we use
    * EPlotLightBase api, x axis autoscale shouldn't be enabled.
    */
    if(!d_xAutoscale)
    {
        setAxisScale(QwtPlot::xBottom, axisScaleDiv(QwtPlot::xBottom).lowerBound(), axisScaleDiv(QwtPlot::xBottom).upperBound());
    }
    /* else if autoscale is enabled the x scale will be adjusted on the next refresh() method */
    QwtPlot::replot();
    zoomer->setZoomBase();
}


void EPlotLightBase::setYAxisAutoscaleEnabled(bool autoscale)
{
    d_yAutoscale = autoscale;

    if(autoscale)
        setAxisAutoScale(QwtPlot::yLeft); /* enable auto scale */
    else /* disable autoscale by setting axis scale */
        setAxisScale(QwtPlot::yLeft, axisScaleDiv(QwtPlot::yLeft).lowerBound(), axisScaleDiv(QwtPlot::yLeft).upperBound());
    QwtPlot::replot();
    zoomer->setZoomBase();
}

double EPlotLightBase::yUpperBound()
{
   return axisScaleDiv(QwtPlot::yLeft).upperBound();
}

double EPlotLightBase::yLowerBound()
{
    return axisScaleDiv(QwtPlot::yLeft).lowerBound();
}

/* implicitly disables y axis autoscale (via the QwtPlot::setAxisScale() ) */
void EPlotLightBase::setYLowerBound(double l)
{
    d_yAutoscale = false;
    setAxisScale(QwtPlot::yLeft, l, axisScaleDiv(QwtPlot::yLeft).upperBound());
    QwtPlot::replot();
    zoomer->setZoomBase();
}

/* implicitly disables y axis autoscale (via the QwtPlot::setAxisScale() ) */
void EPlotLightBase::setYUpperBound(double u)
{
    d_yAutoscale = false;
    setAxisScale(QwtPlot::yLeft, axisScaleDiv(QwtPlot::yLeft).lowerBound(), u);
    QwtPlot::replot();
    zoomer->setZoomBase();
}

double EPlotLightBase::xUpperBound()
{
    return axisScaleDiv(QwtPlot::xBottom).upperBound();
}

double EPlotLightBase::xLowerBound()
{
    return axisScaleDiv(QwtPlot::xBottom).lowerBound();
}

/* implicitly disables x autoscale in QwtPlot via setAxisScale() and explicitly disables it
 * here in EPlotLightBase by d_xAutoscale set to false.
 * d_xAutoscale set to false also prevents the refresh() method from scrolling.
 */
void EPlotLightBase::setXLowerBound(double l)
{
    d_xAutoscale = false;  /* disables autoscale */
    setAxisScale(QwtPlot::xBottom, l, axisScaleDiv(QwtPlot::xBottom).upperBound());
    QwtPlot::replot();
    zoomer->setZoomBase();
}

/* implicitly disables x autoscale in QwtPlot via setAxisScale() and explicitly disables it
 * here in EPlotLightBase by d_xAutoscale set to false.
 * d_xAutoscale set to false also prevents the refresh() method from scrolling.
 */
void EPlotLightBase::setXUpperBound(double u)
{
    d_xAutoscale = false;  /* disables autoscale */
    setAxisScale(QwtPlot::xBottom, axisScaleDiv(QwtPlot::xBottom).lowerBound(), u);
    QwtPlot::replot();
    zoomer->setZoomBase();
}

/* implicitly disables y axis autoscale (via the QwtPlot::setAxisScale() ) */
void  EPlotLightBase::extendYLowerBound(double l)
{
    d_yAutoscale = false;
    double lBound, uBound;
    lBound = axisScaleDiv(QwtPlot::yLeft).lowerBound();
    uBound = axisScaleDiv(QwtPlot::yLeft).upperBound();
    lBound = qMin(lBound, l);
    setAxisScale(QwtPlot::yLeft, lBound, uBound);
    QwtPlot::replot();
    zoomer->setZoomBase();
}

/* implicitly disables y axis autoscale (via the QwtPlot::setAxisScale() ) */
void  EPlotLightBase::extendYUpperBound(double u)
{
    d_yAutoscale = false;
    double lBound, uBound;
    lBound = axisScaleDiv(QwtPlot::yLeft).lowerBound();
    uBound = axisScaleDiv(QwtPlot::yLeft).upperBound();
    uBound = qMax(uBound, u);
    setAxisScale(QwtPlot::yLeft, lBound, uBound); /* disables autoscale */
    QwtPlot::replot();
    zoomer->setZoomBase();
}

void EPlotLightBase::setXAutoscaleAdjustEnabled(bool en)
{ 
//    if(en)
//	setXAxisAutoscaleEnabled(false);
    /* otherwise I can disable this property without affecting xAxisAutoscaleEnabled property */
    d_xAutoscaleAdjustEnabled = en;
}

void EPlotLightBase::setYAutoscaleAdjustEnabled(bool en)
{ 
    /* otherwise I can disable this property without affecting yAxisAutoscaleEnabled property */
    d_yAutoscaleAdjustEnabled = en;
}

void EPlotLightBase::setXAutoscaleAdjustment(double d)
{
    if(d >= 0)
        d_xAutoscaleAdjustment = d;
}

void EPlotLightBase::setYAutoscaleAdjustment(double d)
{
    if(d >= 0)
        d_yAutoscaleAdjustment = d;
}

bool EPlotLightBase::xAxisLogScale()
{
    if(dynamic_cast<QwtLogScaleEngine *>(axisScaleEngine(QwtPlot::xBottom)))
        return true;
    return false;
}

bool EPlotLightBase::yAxisLogScale()
{
    if(dynamic_cast<QwtLogScaleEngine *>(axisScaleEngine(QwtPlot::yLeft)))
        return true;
    return false;
}

void EPlotLightBase::setXAxisLogScale(bool l)
{
    if(l)
        setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine());
    else
        setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine());
}

void EPlotLightBase::setYAxisLogScale(bool l)
{
    if(l)
    setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine());
    else
	setAxisScaleEngine(QwtPlot::yLeft, new EScaleEngine());
}

void EPlotLightBase::displayZoomHint()
{
    d_displayZoomHint = true;
    this->replot();
}

void EPlotLightBase::eraseZoomHint()
{
    d_displayZoomHint = false;
    this->replot();
}

bool EPlotLightBase::zoomDisabled() const
{
    return findChild<ShiftClickEater *>("shiftClickEater") != NULL;
}

void EPlotLightBase::setZoomDisabled(bool disable)
{
    ShiftClickEater *shiftClickEater = findChild<ShiftClickEater *>("shiftClickEater");
    if(disable && !shiftClickEater)
    {
        pinfo("creating shift + click eater to eat zoom events");
        shiftClickEater = new ShiftClickEater(this);
        shiftClickEater->setObjectName("shiftClickEater");
        canvas()->installEventFilter(shiftClickEater);
    }
    if(!disable && shiftClickEater)
    {
        canvas()->removeEventFilter(shiftClickEater);
        delete shiftClickEater;
    }
}

