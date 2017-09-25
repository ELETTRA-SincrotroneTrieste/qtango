#include "html_notification_center.h"

ProxyNotificationEvent::ProxyNotificationEvent(events t, QObject *o) : QEvent( (QEvent::Type) t)
{
  d_object = o;
}

HtmlNotificationCenter::HtmlNotificationCenter(QObject *parent) : QObject(parent)
{
  
}

