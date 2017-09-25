#include "eplotlight_marker.h"
#include "ezoomer.h"
#include "eplotcurve.h"
#include <colors.h>
#include <macros.h>
#include <QtDebug>
#include <QMouseEvent>
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>

#define ALPHAVAL 220

void Arrow::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &) const
{
    //	qDebug() << begin << end;
    double x1, x2, y1, y2;
    QColor penColor(KLIGHTGRAY);
    x1 = xMap.transform(begin.x());
    x2 = xMap.transform(end.x());
    y1 = yMap.transform(begin.y());
    y2 = yMap.transform(end.y());
    penColor.setAlpha(ALPHAVAL - 100);
    painter->setPen(KDARKGRAY);
    painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
}

EPlotLightMarker::EPlotLightMarker(QWidget *parent) : EPlotLight(parent)
{
    init();
}

EPlotLightMarker::EPlotLightMarker(const QwtText &title, QWidget *parent) : EPlotLight(title, parent)
{
    init();
}

void EPlotLightMarker::init()
{
    QColor bgColor(QColor(245,245,245));
    QColor txtColor(Qt::black);
    QColor bgPen(KDARKGRAY);
    _marker = new QwtPlotMarker();
    _picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOff, canvas());
    _picker->setStateMachine(new QwtPickerClickPointMachine());
    connect(_picker, SIGNAL(selected(const QPolygon &)), this, SLOT(showLabel(const QPolygon &)));
    QwtText text("", QwtText::PlainText);
    text.setRenderFlags(Qt::AlignCenter | Qt::TextIncludeTrailingSpaces);
    text.setFont(QFont(fontInfo().family(), 10, QFont::Bold));
    bgColor.setAlpha(ALPHAVAL);
    txtColor.setAlpha(ALPHAVAL);
    bgPen.setAlpha(ALPHAVAL);
    text.setColor(txtColor);
    text.setBackgroundBrush(QBrush(bgColor));
    //   text.setBackgroundPen(QPen(bgPen, 1));
    _marker->setLabel(text);
    _marker->setLabelAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _marker->setLineStyle(QwtPlotMarker::NoLine);
    //_marker->setLineStyle(QwtPlotMarker::Cross);
    _marker->setValue(0.0, 0.0);
    _marker->attach(this);
    _marker->hide();
    _arrow = new Arrow();
    _arrow->m_currentClosestCurve = NULL;
    _arrow->m_currentClosestPoint = -1;
    _arrow->attach(this);
    _arrow->hide();
    connect(zoomer, SIGNAL(zoomed(const QRectF&) ), this, SLOT(plotZoomed(const QRectF&) ) );
}

QSet<QwtPlotCurve *> EPlotLightMarker::intersectingCurves(const double x, const double y,
                                                          const QwtPlotCurve* curve)
{
    QSet<QwtPlotCurve *> overlappingCurvesSet;
    double vx, vy;
    foreach(QwtPlotCurve *c, curves())
    {
        /* curves not visible are not added to the overlapping set */
        if(c != curve && c->isVisible())
        {
            for(size_t i = 0; i < c->dataSize(); i++)
            {
                vx = c->data()->sample(i).x();
                vy = c->data()->sample(i).y();
                if(x == vx && y == vy)
                {
                    overlappingCurvesSet.insert(c);
                    break;
                }
            }
        }
    }
    return overlappingCurvesSet;
}


QString EPlotLightMarker::markerText(const QwtPlotCurve *relatedCurve, const int index)
{
    QString s;
    double y, x, y1;
    if(relatedCurve != NULL)
    {
        x = relatedCurve->data()->sample(index).x();
        y = relatedCurve->data()->sample(index).y();
        /* place the x coordinate label, taken from the x axis scale */
        QwtScaleDraw *scaleDraw = axisScaleDraw(relatedCurve->xAxis());
        s += QString("x: %1\n").arg(scaleDraw->label(x).text());
        /* get overlapping curves in point x,y and add to the marker the y values */
        QSet<QwtPlotCurve *> overlappingCurves = intersectingCurves(x, y, relatedCurve);
        /* add the current curve to the overlapping set */
        overlappingCurves.insert((QwtPlotCurve *)(relatedCurve)); /* remove constness */
        foreach(QwtPlotCurve *pc, overlappingCurves.values())
        {
            y1 = pc->data()->sample(index).y();
            /* place the curve title on the marker text first */
            if(pc->title() != QwtText(QString()))
                s += QString("%1: ").arg(pc->title().text());
            s += QString("%1").arg(y1, 0, 'g', 5) + "\n";
        }
    }
    return s;
}

void EPlotLightMarker::showLabel(const QPolygon &p)
{
    int closestPoint;
    QwtPlotCurve *closestCurve;
    double x, y;
    closestPoint = findClosestPoint(p.point(0), &closestCurve);

    if (closestPoint != -1)
    {
        _marker->show();
        _arrow->show();
        updateLabel(closestCurve, closestPoint);
        x = closestCurve->data()->sample(closestPoint).x();
        y = closestCurve->data()->sample(closestPoint).y();
        emit plotClicked(QCursor::pos(), closestCurve, x, y, QPoint(transform(closestCurve->xAxis(), x),
                                                                    transform(closestCurve->yAxis(), y)));
        qDebug() << mapFromGlobal(QCursor::pos()) << "title: " << closestCurve->title().text() << "x " << x <<
                    y << "point on plot " <<
                    QPoint(transform(closestCurve->xAxis(), x), transform(closestCurve->yAxis(), y));
    }
    else
    {
        _marker->hide();
        _arrow->hide();
    }
    replot();
}

void EPlotLightMarker::updateLabel(QwtPlotCurve *closestCurve, int closestPoint)
{
    printf("\e[1;32m updateLabel() closestPoint %d\e[0m\n", closestPoint);
    double xLowerBound, xUpperBound, yLowerBound, yUpperBound;
    /* save current closest point on _arrow */
    _arrow->m_currentClosestPoint = closestPoint;
    _arrow->m_currentClosestCurve = closestCurve; /* and closest curve */

    if(closestCurve == NULL)
    {
        pwarn("EPlotLightMarker::updateLabel: closestCurve is null");
    }
    else if((closestPoint != -1) && _marker->isVisible())
    {
        double hoff, range;
        double y = closestCurve->data()->sample(closestPoint).y();
        double x = closestCurve->data()->sample(closestPoint).x();
        xLowerBound = axisScaleDiv(closestCurve->xAxis()).lowerBound();
        xUpperBound = axisScaleDiv(closestCurve->xAxis()).upperBound();
        yLowerBound = axisScaleDiv(closestCurve->yAxis()).lowerBound();
        yUpperBound = axisScaleDiv(closestCurve->yAxis()).upperBound();

        /* axisScaleDiv(axisId)->lBound(), axisScaleDiv(axisId)->hBound() are the current limits of the axis scale */
        range = xUpperBound - xLowerBound;
        QwtText l = _marker->label();
        l.setText(markerText(closestCurve, closestPoint));
        _marker->setLabel(l);

        double markerWidth = l.textSize(l.font()).width();
        double markerHeight = l.textSize(l.font()).height();

        /* align label and set different x offsets depending on the area of the
       * canvas the click takes place in.
       */
        if(x <= xLowerBound + range/3)
        {
            hoff = 0.1*range; // + .2*x;
            _marker->setLabelAlignment(Qt::AlignRight|Qt::AlignBottom);
        }
        else if(x >= xLowerBound + 2 * range/3)
        {
            hoff = -0.1*range;
            _marker->setLabelAlignment(Qt::AlignLeft|Qt::AlignBottom);
        }
        else
        {
            hoff = -0.1*range;
            _marker->setLabelAlignment(Qt::AlignHCenter|Qt::AlignBottom);
        }

        QPointF begin, end, arrowEnd;
        double up = yLowerBound + 0.93 * (yUpperBound - yLowerBound);
        begin = QPointF(x, y);
        end = QPointF(x + hoff, up);

        _marker->setYAxis(closestCurve->yAxis());
        _marker->setValue(end);

        qDebug() << "marker bounding rect: " << _marker->boundingRect() <<
                    _marker->yAxis() << " y" << _marker->yValue() << " value " << _marker->value();

        /* 1. to obtain the position of the end point of the arrow, transform first the end point into
       * pixel coordinates.
       */
        int arrowXEndPix = transform(closestCurve->xAxis(), x + hoff);
        int arrowYEndPix = transform(closestCurve->yAxis(), up);

        /* 2. position end point for the arrow, depending on the area the user clicks.
       * No need to change arrowXEndPix for central area clicks. We are in pixel coordinates
       * and markerWidth and markerHeight are in pixel coordinates: we can sum each other
       */
        if(x <= xLowerBound + range/3)
            arrowXEndPix += markerWidth/2;
        else if(x >= xLowerBound + 2 * range/3)
            arrowXEndPix -= markerWidth/2; /* last portion */

        arrowYEndPix += markerHeight; /* we are summing pixel coords */

        /* 3. Finally set arrow end point, transforming back into plot coordinates */
        arrowEnd = QPointF(invTransform(closestCurve->xAxis(), arrowXEndPix), invTransform(closestCurve->yAxis(), arrowYEndPix));
        _arrow->setYAxis(closestCurve->yAxis());
        _arrow->begin = begin;
        _arrow->end = arrowEnd;
    }
    replot();
}

int EPlotLightMarker::findClosestPoint(QPoint p, QwtPlotCurve **closestCrv)
{
    QList<double> distances;
    QMap<QwtPlotCurve*, double> curveDistancesMap;
    QMap<QwtPlotCurve*, int> curveClosestPointMap;
    int closestPoint = -1;
    double dist = -1, minDist = -1;
    *closestCrv = NULL;
    
    foreach(QwtPlotItem* i, itemList())
    {
        if(dynamic_cast<QwtPlotCurve* >(i))
        {
            QwtPlotCurve *c = dynamic_cast<QwtPlotCurve* >(i);
            if(c->isVisible())
            {
                EPlotCurve *epc = dynamic_cast<EPlotCurve *>(c);
                if( ! (epc && !epc->property("sensibleToClicks").isNull() && !epc->property("sensibleToClicks").toBool()))
                {
                    closestPoint = c->closestPoint(p, &dist);
                    if(closestPoint > -1)
                    {
                        curveDistancesMap.insert(c, dist);
                        curveClosestPointMap.insert(c, closestPoint);
                    }
                    printf("curve %s, dist %.2f closestPoint %d\n", qstoc(c->title().text()), dist, closestPoint);
                }
            }
        }
    }
    
    
    distances = curveDistancesMap.values();
    if(distances.size() > 0)
    {
        qSort(distances.begin(), distances.end());
        qDebug() << "distances sorted: " << distances;
        minDist = distances.first();
        closestPoint = curveClosestPointMap.value(curveDistancesMap.key(minDist));
        qDebug() << "valori di curveClosestPointMap: " << curveClosestPointMap.values() << "closestPoint " << closestPoint;
        if(curveClosestPointMap.values().contains(closestPoint))
        {
            *closestCrv = curveDistancesMap.key(minDist);
            qDebug() << "closestCurve: " << *closestCrv << (*closestCrv)->title().text();
        }
        else
            qDebug() << "dont contains curve";
    }
    return closestPoint;
}

void EPlotLightMarker::setMarkerLabel(const QwtText &text)
{
    _marker->setLabel(text);
}

void EPlotLightMarker::hideMarker()
{
    _marker->hide();
    _arrow->hide();
    replot();
}

void EPlotLightMarker::mouseReleaseEvent(QMouseEvent *ev)
{
    printf("\e[1;32mMouseRELEASE\e[0m\n");

    if(ev->button() == Qt::MidButton && _marker->isVisible())
        hideMarker();
    QWidget::mouseReleaseEvent(ev);
}

void EPlotLightMarker::keyPressEvent(QKeyEvent *ke)
{
    if(_marker->isVisible())
    {
       printf("key press move marker\n");
       ke->ignore();
       if(ke->key() == Qt::Key_Left)
            moveCurveToYRight(_arrow->m_currentClosestCurve, false);
       else if(ke->key() == Qt::Key_Right)
            moveCurveToYRight(_arrow->m_currentClosestCurve, true);
    }
    else
        EPlotLight::keyPressEvent(ke);
}

void EPlotLightMarker::plotZoomed(const QRectF&)
{
    if(_marker->isVisible())
        hideMarker();
    //   refresh();
}

void EPlotLightMarker::refresh()
{
    /* update arrow if the arrow is visible and if the curve has not been removed */
    if(_arrow->isVisible() && curves().contains(_arrow->m_currentClosestCurve) &&
            _arrow->m_currentClosestCurve->isVisible() &&
            _arrow->m_currentClosestCurve && _arrow->m_currentClosestPoint != -1)
    {
        double x = _arrow->m_currentClosestCurve->data()->sample(_arrow->m_currentClosestPoint).x();
        double y = _arrow->m_currentClosestCurve->data()->sample(_arrow->m_currentClosestPoint).y();
        _arrow->begin = QPointF(x, y);
        //  _arrow->end = QPointF(m_currentClosestCurve->x(closestPoint)+hoff, up);
        QwtText l = _marker->label();
        l.setText(markerText(_arrow->m_currentClosestCurve, _arrow->m_currentClosestPoint));
        _marker->setLabel(l);
    }
    EPlotLight::refresh();
}

void EPlotLightMarker::moveCurveToYRight(QwtPlotCurve *c, bool yr)
{
    int yAxis = c->yAxis();
    if(yAxis == QwtPlot::yLeft && yr)
    {
        printf("moving to --->\n");
        c->setAxes(c->xAxis(), QwtPlot::yRight);
    }
    else if(yAxis == QwtPlot::yRight && !yr)
    {
        printf("<---- moving to\n");
        c->setAxes(c->xAxis(), QwtPlot::yLeft);
    }
}

