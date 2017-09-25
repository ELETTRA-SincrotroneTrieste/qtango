#include "action_timer.h"

#include <macros.h>

ActionTimer::ActionTimer(QObject *parent) : QTimer(parent)
{
  setInterval(1000);
  setSingleShot(false);
}

void ActionTimer::start(int msec)
{
  int interval = msec;
  if(msec <= 0)
  {
	perr("action timer: cannot set an interval less than or equal to zero [%dms]", msec);
	perr("setting 1000ms (one second) instead");
	interval = 1000;
  }
  QTimer::start(interval);
}

void ActionTimer::setInterval(int msec)
{
  int interval = msec;
  if(msec <= 0)
  {
	perr("action timer: cannot set an interval less than or equal to zero [%dms]", msec);
	perr("setting 1000ms (one second) instead");
	interval = 1000;
  }
  QTimer::setInterval(interval);
}

