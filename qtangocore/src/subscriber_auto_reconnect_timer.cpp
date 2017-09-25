#include "subscriber_auto_reconnect_timer.h"
#include <macros.h>

SubscriberAutoReconnectTimer::SubscriberAutoReconnectTimer(QObject *parent, const QString &srcName = QString()) :
  QTimer(parent)
{
  d_attemptNo = 0;
  d_sourceName = srcName;
  d_timeos << 5000 << 10000 << 15000 << 20000;
  
  /* do not continuously retry: always wait for the subscriber proxy to explicitly 
   * tell us to retry, in consequence of an ActionConfigurationFailedEvent.
   * If we continuously retried, without waiting for a configuration failed event,
   * we might post 2 requests to the device thread that could, in rare cases, be
   * contemporarily satisfied, and the SubscriberProxy would receive two ActionConfigurationEvents
   * with bad consequences. Of course, it would be unuseful to schedule two retries if 
   * we still do not know whether the first really failed, or maybe just succeeding while a second
   * retry has already been fired (case discussed above).
   */
  setSingleShot(true); 
  connect(this, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

SubscriberAutoReconnectTimer::~SubscriberAutoReconnectTimer()
{
  if(d_attemptNo > 0)
	pdelete("subscriber \e[1;36mauto reconnect timer\e[0m deleted after %d retries", d_attemptNo);
  unscheduleRetry();
}

void SubscriberAutoReconnectTimer::scheduleRetry()
{
  int interval;
  if(!d_timeos.size())
	perr("SubscriberAutoReconnectTimer::startRetryCicle(): timeouts vector empty!");
  else
  {
	interval = d_attemptNo < d_timeos.size() ? d_timeos.at(d_attemptNo) : d_timeos.last();
	pstep("subscriber proxy \e[1;36mreconnect timer\e[0m: scheduling retry in %dms for source \"%s\"...",
		interval, qstoc(d_sourceName));
	start(interval);
  }
}

void SubscriberAutoReconnectTimer::slotTimeout()
{
  d_attemptNo++;
  pstep("subscriber proxy \e[1;36mreconnect timer\e[0m: signaling retry number %d for \"%s\"", d_attemptNo,
		 qstoc(d_sourceName));
  emit retry();
}

void SubscriberAutoReconnectTimer::unscheduleRetry()
{
  stop();
}