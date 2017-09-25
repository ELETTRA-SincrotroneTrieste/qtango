#include "simplefontscalingwidget.h"
#include "fontscaling_eventfilter.h"

SimpleFontScalingWidget::SimpleFontScalingWidget(QWidget *parent) : FontScalingWidget(parent)
{
  d_eventFilter = NULL;
}

void SimpleFontScalingWidget::setFontScaleEnabled(bool en)
{
  if(en && !d_eventFilter)
  {
	d_eventFilter = new FontScalingEventFilter(this);
	widget()->installEventFilter(d_eventFilter);
  }
  else if(!en && d_eventFilter)
  {
	widget()->removeEventFilter(d_eventFilter);
	delete d_eventFilter;
	d_eventFilter = NULL;
  }
  FontScalingWidget::setFontScaleEnabled(en);
}

void SimpleFontScalingWidget::rescaleFont(const QSize &containerSize)
{
  FontScalingWidget::rescaleFont(currentText(), containerSize);
}

void SimpleFontScalingWidget::rescaleFont()
{
  FontScalingWidget::rescaleFont(currentText(), widget()->size());
}

QSize SimpleFontScalingWidget::calculateTextSpace()
{
  QSize s;
  if(widget())
	s = widget()->size();
  return s;
}

