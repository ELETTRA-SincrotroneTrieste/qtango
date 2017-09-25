#include "dbplotbackground.h"
#include <qwt_plot_scaleitem.h>
#include <qwt_scale_map.h>
#include <qwt_painter.h>
#include <QPainter>
#include <QDateTime>
#include <elettracolors.h>
#include <macros.h>

bool DateTimeInterval::canMerge(const DateTimeInterval &other) const
{
    if((this->stop < other.start) || (this->start > other.stop))
        return false;
    return true;
}

DateTimeInterval& DateTimeInterval::merge(const DateTimeInterval &other)
{
    this->start = qMin(this->start, other.start);
    this->stop = qMax(this->stop, other.stop);
    return *this;
}

bool  DateTimeInterval::operator==(const DateTimeInterval &other) const
{
    return this->start == other.start && this->stop == other.stop;
}

DbPlotBackground::DbPlotBackground() : QwtPlotScaleItem()
{
    m_colors << KGREEN;
}

void DbPlotBackground::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &rect) const
{
    double pStart, pEnd;
    QList<DateTimeInterval> neededIntervals = m_intervals;

    /* check if some of the intervals in m_intervals falls outside the current scale.
     * They might have been added before and now they are not visible and so can be
     * removed from the list to free space.
     */
    int i = 0;
    printf("\e[1;32mintervals:\e[0m\n");
    while(i < neededIntervals.size())
    {
        if(neededIntervals.at(i).stop < xMap.s1() || neededIntervals.at(i).start > xMap.s2() )
        {
            printf("\e[1;31m - %s->%s\e[0m\n",
                 qstoc(QDateTime::fromTime_t(m_intervals.at(i).start).toString("yyyy-MM-dd hh:mm:ss")),
                qstoc(QDateTime::fromTime_t(m_intervals.at(i).stop).toString("yyyy-MM-dd hh:mm:ss")));

            neededIntervals.removeAt(i);
        }
        else
        {
            printf("\e[1;32m + %s->%s\e[0m\n",
                   qstoc(QDateTime::fromTime_t(m_intervals.at(i).start).toString("yyyy-MM-dd hh:mm:ss")),
                   qstoc(QDateTime::fromTime_t(m_intervals.at(i).stop).toString("yyyy-MM-dd hh:mm:ss")));

            i++;
        }
    }

    for(int i = 0; i < neededIntervals.size(); i++)
    {
        DateTimeInterval dti = neededIntervals.at(i);
        pStart = xMap.transform(dti.start);
        pEnd   = xMap.transform(dti.stop);
        /* Operator % avoids going out of colours boundary, beginning again from the first available colour
         * when i reaches the size of the colour vector.
         */
        QColor c = m_colors.at(i % m_colors.size());
        c.setAlpha(80);
        QBrush brush(c);
        QRect r = QRect(pStart, yMap.p2(), pEnd - pStart, -yMap.p2() + yMap.p1() );
     //   qDebug() << "Filling rect " << r;
        QwtPainter::fillRect(p, r, brush);
//        c = c.darker();
//        p->save();
//        p->setPen(QPen(c));
//        QFont f = p->font();
//        f.setPointSize(12);
//        f.setBold(true);
//        p->setFont(f);
   //     QwtPainter::drawText(p, r, Qt::AlignLeft, m_labels.at(i).toUpper());
//        p->restore();
    }
    QwtPlotScaleItem::draw(p, xMap, yMap, rect);
}

void DbPlotBackground::addInterval(double start, double stop)
{
    DateTimeInterval dti(start, stop);
    if(!m_intervals.size())
        m_intervals << dti;
    else
    {
        for(int i = 0; i < m_intervals.size(); i++)
        {
            DateTimeInterval itv = m_intervals[i];
            if(!(itv == dti))
            {
                if(itv.canMerge(dti))
                {
                    m_intervals[i].merge(dti);
                    printf("\e[1;33m merged %s-%s,  %s-%s into \e[0;33m%s-%s\e[0m\n",
                           qstoc(QDateTime::fromTime_t(itv.start).toString("yyyy-MM-dd hh:mm:ss")),
                           qstoc(QDateTime::fromTime_t(itv.stop).toString("yyyy-MM-dd hh:mm:ss")),
                           qstoc(QDateTime::fromTime_t(dti.start).toString("yyyy-MM-dd hh:mm:ss")),
                           qstoc(QDateTime::fromTime_t(dti.stop).toString("yyyy-MM-dd hh:mm:ss")),
                           qstoc(QDateTime::fromTime_t(m_intervals[i].start).toString("yyyy-MM-dd hh:mm:ss")),
                           qstoc(QDateTime::fromTime_t(m_intervals[i].stop).toString("yyyy-MM-dd hh:mm:ss")));
                }
                else
                    m_intervals << dti;
            }
        }
    }
}


