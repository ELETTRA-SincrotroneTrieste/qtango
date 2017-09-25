#include "left_click_event.h"

#include <QMouseEvent>
#include <com_proxy_reader.h>
#include "source_metadata.h"
#include "qtango_wcommon.h"
#include "left_click_event.h"
#include "tinfo_textbrowser.h"
#include "proxy_interface.h"

LeftClickEvent::LeftClickEvent(QObject *parent) : QObject(parent)
{
  
}
	
/* the core processing of the right click: take each component, get its configuration and 
 * create actions. Return true to eat the event when it is left mouse click + alt key.
 */
bool LeftClickEvent::eventFilter(QObject *obj, QEvent *event)
{
  QMouseEvent *mouseEvent;
  
  if(event->type() == QEvent::MouseButtonPress && ((mouseEvent = static_cast<QMouseEvent *>(event)) != NULL)
	&& mouseEvent->button() == Qt::LeftButton && mouseEvent->modifiers() == Qt::ControlModifier)
  {
	InfoWidget *iw = dynamic_cast<InfoWidget *>(obj);
	if(iw)
	  iw->showInfoBrowser();
	  
	
	return true; /* prevent the event be handled further */
  }
  return false;
}







