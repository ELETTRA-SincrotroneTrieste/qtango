#include "ezoomer.h"
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot.h>
#include <QDateTime>
#include <QMouseEvent>

Zoomer::Zoomer(QWidget* plot) : ScrollZoomer(plot), d_trackerTextEnabled(true), d_xAsDate(false)
{
    setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ShiftModifier);
}

void Zoomer::widgetMouseMoveEvent(QMouseEvent *e)
{
  if(e->modifiers() != Qt::ShiftModifier && e->buttons() == Qt::LeftButton)
	emit zoomHint();
  ScrollZoomer::widgetMouseMoveEvent(e);
}

void Zoomer::widgetMouseReleaseEvent(QMouseEvent *e)
{
  emit removeZoomHint();
  ScrollZoomer::widgetMouseReleaseEvent(e);
}

void Zoomer::rescale()
{
        QwtScaleWidget *scaleWidget = plot()->axisWidget(yAxis());
        QwtScaleDraw *sd = scaleWidget->scaleDraw();
        int minExtent = 0;
        if ( zoomRectIndex() > 0 )
        {
         /* When scrolling in vertical direction
          * the plot is jumping in horizontal direction
          * because of the different widths of the labels
          * So we better use a fixed extent.
          */
         minExtent = sd->spacing() + sd->maxTickLength() + 1;
         minExtent += sd->labelSize(scaleWidget->font(), 1000).width();
        }
        sd->setMinimumExtent(minExtent);
        ScrollZoomer::rescale();
}

QwtText Zoomer::trackerText(const QPoint &pos) const
{
  QString txt;
  if(d_xAsDate)
  {
    QPointF p = invTransform(pos);
    QDateTime d;
    d.setTime_t((time_t) p.x() );
    txt = d.toString("dd MMM\nhh:mm:ss");
    txt += QString("\n%1").arg(p.y());
    return QwtText(txt);
  }
  return ScrollZoomer::trackerText(pos);
}



