#include "fontscaling_eventfilter.h"
#include "simplefontscalingwidget.h"

#include <QResizeEvent>
#include <QtDebug>
#include <macros.h>

FontScalingEventFilter::FontScalingEventFilter(SimpleFontScalingWidget *scalingWidget)  : QObject()
{
  d_scalingWidget = scalingWidget;
//   d_previousFontAdjustment = false;
}

bool FontScalingEventFilter::eventFilter(QObject *obj, QEvent *event)
{
//   if(event->type() == QEvent::FontChange)
//   {
// 	printf("\e[1;31m scheduling gnam gnam eating FontChange event for \"%s\"....\e[0m\n\n", qstoc(obj->objectName()));
// 	d_previousFontAdjustment = true;
//   }
//   if(event->type() == QEvent::Resize && d_previousFontAdjustment)
//   {
// 	printf("\e[0;31m gnam gnam eating Resize after font changte event for \"%s\"....\e[0m\n\n", qstoc(obj->objectName()));
// 	d_previousFontAdjustment = false;
// 	return true;
//   }
  if(event->type() == QEvent::Resize)
  {
	QResizeEvent *re = static_cast<QResizeEvent *>(event);
	qDebug() << "resize event for " << obj << "old size " << re->oldSize() << "new size " << re->size();
	qDebug() << "spontaneous " << event->spontaneous();
	d_scalingWidget->rescaleFont(re->size());
  }
  else if(event->type() == QEvent::Show)
  {
	d_scalingWidget->rescaleFont();
  }
  return false;
}

