#include "enter_leave_event.h"
#include <QTimer>
#include <QCoreApplication>
#include "info_notification_center.h"
#include <macros.h>

EnterLeaveEvent::EnterLeaveEvent(QObject *parent) : QObject(parent)
{
  d_enterDelayTimer = new QTimer(this);
  d_enterDelayTimer->setSingleShot(true);
  d_enterDelay = 500;
  connect(d_enterDelayTimer, SIGNAL(timeout()), this, SLOT(triggerLinkEvent()));
}

EnterLeaveEvent::~EnterLeaveEvent()
{
  qprintf("\e[1;31m deleted enter leav event for \"%s\" 0x%p\e[0m\n", qstoc(parent()->objectName()), parent());
  /* it is important to stop notifications when the widget is destroyed. Otherwise, it could happen that
   * the info browser still references a destroyed ProxyInterface/QTangoWidgetCommon.
   */
  d_enterDelayTimer->stop(); 
  ProxyNotificationEvent *pne = new ProxyNotificationEvent(ProxyNotificationEvent::ProxyUnlinkEvent, parent());
  QCoreApplication::instance()->postEvent(InfoNotificationCenter::instance(), pne);
}

bool EnterLeaveEvent::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
  if(event->type() == QEvent::Enter)
	d_enterDelayTimer->start(d_enterDelay);
  else if(event->type() == QEvent::Leave)
  {
	/* if we enter and exit before timeout, do not trigger link event */
	d_enterDelayTimer->stop();
  }
  return false; /* let the event be handled further */
}

void EnterLeaveEvent::triggerLinkEvent()
{
  ProxyNotificationEvent *pne = new ProxyNotificationEvent(ProxyNotificationEvent::ProxyLinkEvent, parent());
  d_enterDelayTimer->stop();
  QCoreApplication::instance()->postEvent(InfoNotificationCenter::instance(), pne);
}






