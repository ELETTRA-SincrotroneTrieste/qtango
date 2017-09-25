#ifndef POLLING_PERIOD_CHANGE_EVENT
#define POLLING_PERIOD_CHANGE_EVENT

#include <qtangocore_common.h>
#include <QEvent>

class PollingPeriodChangeEvent : public QEvent
{
  public:
    PollingPeriodChangeEvent(int period) : QEvent(ACTION_POLLING_PERIOD_CHANGE_EVENT) { d_period = period; }
     
    int newPeriod() { return d_period; }
    
    
  private:
    int d_period;
   
};


#endif
