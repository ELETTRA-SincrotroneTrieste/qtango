#include <com_proxy_reader.h>
#include "show_event.h"
#include "qtango_wcommon.h"


ShowEvent::ShowEvent(QObject *parent) : QObject(parent)
{
  
}

/* always return false not to filter out show events and propagate them */
bool ShowEvent::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
  if(event->type() == QEvent::Show)
  {
	QTangoComProxyReader *reader = dynamic_cast<QTangoComProxyReader *>(parent());
	if(reader)
	  reader->showEvent();
  }
  return false; /* let the event be handled further */
}








