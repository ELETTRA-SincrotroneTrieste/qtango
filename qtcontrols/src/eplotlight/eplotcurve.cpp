#include "eplotcurve.h"
#include <QtDebug>
#include <macros.h>
#include <QVariant>
#include <qwt_plot.h>
#include <qwt_scale_div.h>
#include <qwt_painter.h>
#include <QPainter>
#include <elettracolors.h>
#include <qwt_plot_canvas.h>
#include <QPolygon>

#define MAX_STATE_SYMBOLS 6
#define STATE_SYMBOL_SIZE 14
#define YLABEL_OFFSET 3

EPlotCurve::EPlotCurve(QObject *parent) : QObject(parent), QwtPlotCurve()
{
    init();
}

EPlotCurve:: EPlotCurve(QObject *parent, const QwtText &title) : QObject(parent), QwtPlotCurve(title)
{
    init();
}

EPlotCurve:: EPlotCurve(QObject *parent, const QString &title) : QObject(parent), QwtPlotCurve(title)
{
    init();
}

void EPlotCurve::init()
{
    d_data = NULL;
    d_data = new CurveData();
    d_bufSize = -1;
    d_vectorUpdateMode = false;
    m_state = Normal;
    if(parent()->metaObject()->indexOfProperty("showYValuesEnabled") > -1)
        m_showYValuesEnabled = parent()->property("showYValuesEnabled").toBool();
    else
        m_showYValuesEnabled = false;
    /* setting this property to false will cause the curve not to be considered
   * by EPlotLightMarker when seeking the curve closest to the point where the
   * user clicked. See EPlotLightMarker::showLabel and EPlotLightMarker::updateLabel()
   */
    setProperty("sensibleToClicks", QVariant(true));
}

EPlotCurve::~EPlotCurve()
{
    if(d_data)
        delete d_data;
}

void EPlotCurve::setData(const QVector< double > &xData, const QVector< double > &yData)
{
    d_vectorUpdateMode = true;
    d_data->set(xData, yData);
    QwtPlotCurve::setSamples(xData, yData);
}

void EPlotCurve::appendData(double *x, double *y, int count)
{
    /* add count elements */
    d_data->append(x, y, count);
    if(count == 1)
        d_vectorUpdateMode = false;
    else
        d_vectorUpdateMode = true;
    //   else
    //     printf("resize not needed: data size %d bufSize %d\n", d_data->size(), d_bufSize);
}

void EPlotCurve::popFront()
{
    if(d_data->count() > 0)
	d_data->removeFirstElements(1);
}

void EPlotCurve::setTimeInterval(int)
{

}

int EPlotCurve::timeInterval() const
{
    return 0;
}

void EPlotCurve::updateRawData()
{
    setRawSamples(d_data->x(), d_data->y(), d_data->count());
}

void EPlotCurve::clearData()
{
    d_data->removeFirstElements(d_data->count());
    QwtPlotCurve::setSamples(QVector<double>(), QVector<double>());
}

/** \brief one of the drawing methods.
  *
  * This method is invoked to draw the curve from the point from to the point to.
  * When data is appended, the method is normally called with from and to reflecting the distance
  * between the point added and the previous point.
  * When from == 0 and to == -1, then a full redraw is requested.
  * When the state of the curve is Invalid, a certain number of markers, according to the value
  * of the defined MAX_STATE_SYMBOLS, is drawn. In the case of the invalid state, all the symbols
  * are redrawn on the full curve, whether a full draw is requested or not.
  * On the other side, when m_showYValuesEnabled is true, only the requested labels are drawn,
  * according to the value of from and to, eventually all the labels in case of from == 0 and to == -1.
  */
void EPlotCurve::drawCurve(QPainter *p, int style,
                           const QwtScaleMap &xMap,
                           const QwtScaleMap &yMap,
                           const QRectF &canvasRect,
                           int from, int to) const
{
    double y = 0.0, x = 0.0;
    if(m_state == Invalid)
    {
        /* always (re)draw all the markers, because the number of them is limited to
         * MAX_STATE_SYMBOLS.
         */
        int step;
        QColor errColor(KDARKRED);
        errColor.setAlpha(160);
        QPen errPen(errColor);
        QFont f = p->font();
        f.setPixelSize(0.8  * STATE_SYMBOL_SIZE);
        p->save();
        p->setFont(f);

        /* draw an error message */
        QString errMsg = "Invalid data or read error on one (or more) curves.";
        QPointF bottomRight = canvasRect.bottomRight();
        QFontMetrics fm(f);
        QRect errMsgRect(QPoint(bottomRight.x() - fm.width(errMsg),
                                bottomRight.y() - 2 * fm.height()), QSize(fm.width(errMsg), fm.height()));
        p->setPen(errPen);
        QwtPainter::fillRect(p, errMsgRect, Qt::white);
        QwtPainter::drawText(p, errMsgRect, Qt::AlignHCenter|Qt::AlignCenter, errMsg);
        QString errMsg2 = "Right click and select \"Info...\" for details.";
        errMsgRect.moveTop(errMsgRect.top() + fm.height());
        errMsgRect.setWidth(fm.width(errMsg2));
        QwtPainter::fillRect(p, errMsgRect, Qt::white);
        QwtPainter::drawText(p, errMsgRect, Qt::AlignHCenter|Qt::AlignCenter, errMsg2);

        if(count() / MAX_STATE_SYMBOLS > 0)
            step = count() / MAX_STATE_SYMBOLS;
        else
            step = 1;
        int i = 1;
        while(i * step < count())
        {
            x = data()->sample(i * step).x();
            y = data()->sample(i * step).y();
            QRect rect(xMap.transform(x), yMap.transform(y) - STATE_SYMBOL_SIZE / 2,
                       STATE_SYMBOL_SIZE, STATE_SYMBOL_SIZE);
            QPolygon poly(QVector<QPoint>() << rect.bottomLeft() << QPoint(rect.left() + rect.width()/2, rect.top())
                            << rect.bottomRight());
            p->setPen(Qt::black);
            QwtPainter::drawText(p, rect, Qt::AlignHCenter|Qt::AlignCenter, "!");
            errPen.setColor(KRED);
            p->setPen(errPen);
            QwtPainter::drawPolygon(p, poly);
            i++;
        }
        p->restore();
    }
    else if(m_showYValuesEnabled)
    {
        int currentViewDataSize;
        p->save();
        QFont f = p->font();
        f.setPointSizeF(f.pointSizeF() - 2.5);
        p->setFont(f);
        QFontMetrics fm(f);
        /* calculate if all the labels fit the available space, depending on the number of points
         * of the curve. Keep in mind that the painter will be rotated of 90 degrees, so the horizontal
         * space required for each label is equal to the label height (fm.height())
         *
         */
        currentViewDataSize = pointsCountInInterval(plot()->axisScaleDiv(QwtPlot::xBottom).lowerBound(),
             plot()->axisScaleDiv(QwtPlot::xBottom).upperBound());

        if(currentViewDataSize > 0 && fm.height() * currentViewDataSize > canvasRect.width() && from == 0 && to == -1)
        {
            /* draw an error message */
            QString errMsg1 = "Too many points in curve: impossible to draw the curves' y values.";
            QString errMsg2 = "Disable the \"Show Y Values on curves\" on the right click menu.";
            QPointF bottomRight = canvasRect.bottomRight();
            QFontMetrics fm(f);
            /* will draw text on 2 lines */
            QRect errMsgRect(QPoint(bottomRight.x() - fm.width(errMsg1),
                 bottomRight.y() - 2 * fm.height()), QSize(fm.width(errMsg1),
                 fm.height()));
            QPen errPen(KRED);
            p->setPen(errPen);
            /* 1st line */
            QwtPainter::fillRect(p, errMsgRect, Qt::white);
            QwtPainter::drawText(p, errMsgRect, Qt::AlignCenter, errMsg1);
            /* 2nd line */
            errMsgRect.moveTop(errMsgRect.top() + fm.height());
            errMsgRect.setWidth(fm.width(errMsg2));
            QwtPainter::fillRect(p, errMsgRect, Qt::white);
            QwtPainter::drawText(p, errMsgRect, Qt::AlignCenter, errMsg2);
        }
        else if(currentViewDataSize > 0 && fm.height() * currentViewDataSize <= canvasRect.width())
        {
            p->setPen(this->pen());
            size_t start, stop;
            if(from == 0 && to == -1)
            {
                start = 0;
                stop = dataSize();
            }
            else
            {
                start = to;
                stop = from;
            }
            for(size_t i = start; dataSize() > i && i <= stop; i++)
            {
                x = data()->sample(i).x();
                y = data()->sample(i).y();
         //       if((i == 0) || (i > 0 && y(i) != y (i - 1))) // before qwt 6
                if((i == 0) || (i > 0 && y != data()->sample(i - 1).y()))
                {
                    p->save();
                    int h = yMap.transform(y);
                    int l = xMap.transform(x);
                    QString txt = QString("%1").arg(y, 0, 'g', 4);
                    if(style != QwtPlotCurve::Sticks)
                        h > canvasRect.height() / 2 ? h -= (fm.width(txt) + YLABEL_OFFSET) : h += YLABEL_OFFSET;
                    else
                    {
                        y > baseline() ?  h -= fm.width(txt) + YLABEL_OFFSET : h += YLABEL_OFFSET;
                    }
                    p->translate(l, h);
                    p->rotate(90.0);
                    QwtPainter::drawText(p, 0, 0, txt);
                    p->restore();
                }
            }
        }
        p->restore();
    }
//   printf("\e[1;33mNOTE\e[0m: calling drawCurve %s from %d to %d!\n", qstoc(title().text()), from, to);
    QwtPlotCurve::drawCurve(p, style, xMap, yMap, canvasRect, from, to);
}

int EPlotCurve::pointsCountInInterval(double start, double end) const
{
    int count = 1;
    double x;
    const QwtSeriesData <QPointF> *d = data();
    for(size_t i = 0; i < d->size(); i++)
    {
        if(d_vectorUpdateMode)
            x = d->sample(i).x();
        else
            x = d_data->xData().at(i);
        if(x >= start && x <= end)
            count++;
        if(x > end)
            break;
    }
    return count;
}

double EPlotCurve::x(int index) const
{
    return this->data()->sample(index).x();
}

double EPlotCurve::y(int index) const
{
    return this->data()->sample(index).y();
}

size_t EPlotCurve::size() const
{
    return this->data()->size();
}


