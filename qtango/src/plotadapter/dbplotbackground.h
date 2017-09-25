#ifndef DBPLOTBACKGROUND_H
#define DBPLOTBACKGROUND_H

class QStringList;
class QPainter;
class QwtScaleMap;
class QRect;
class QDateTime;

#include <QList>
#include <QColor>
#include <qwt_plot_scaleitem.h>


class DateTimeInterval
{
public:
    DateTimeInterval(double sta, double sto) { start = sta; stop = sto; }

    DateTimeInterval() { start = stop = 0.0; }

    double start;

    double stop;

    bool operator ==(const DateTimeInterval &other) const;

    bool canMerge(const DateTimeInterval &other) const;

    DateTimeInterval& merge(const DateTimeInterval &other);
};

class DbPlotBackground : public QwtPlotScaleItem
{
public:
    DbPlotBackground();

    void draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &rect) const;

    void setLabels(const QStringList &labels) { m_labels = labels; }

    void setColors(const QList<QColor>& colors) { m_colors = colors; }

    int rtti() const { return Rtti_PlotUserItem + 1; }

    void addInterval(double start, double stop);

    void clearIntervals() { m_intervals.clear(); }

private:

    QStringList m_labels;
    QList<QColor> m_colors;
    QList<DateTimeInterval> m_intervals;
};

#endif // DBPLOTBACKGROUND_H
