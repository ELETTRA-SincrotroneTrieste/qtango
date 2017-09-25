#include <com_proxy_reader.h>
#include "hide_event.h"
#include "qtango_wcommon.h"

HideEvent::HideEvent(QObject *parent) : QObject(parent)
{
  Q_ASSERT(parent != NULL);
 
}

/* always return false not to filter out show events and propagate them */
bool HideEvent::eventFilter(QObject *obj, QEvent *event)
{
  if(event->type() == QEvent::Hide)
  {
	QTangoComProxyReader *reader = dynamic_cast<QTangoComProxyReader *>(parent());
	if(reader)
	  reader->hideEvent();
	else
	  pinfo("HideEvent::cannot cast \"%s\" to reader", qstoc(obj->objectName()));
  }
  return false; /* let the event be handled further */
}








