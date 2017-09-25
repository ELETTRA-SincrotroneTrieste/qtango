#include "targetexecutionevent.h"

TargetExecutionEvent::TargetExecutionEvent(const QString& targe,
                                           const time_t timestam,
                                           const bool succes,
                                           const QString& exceptio,
                                           const QString& dat)
        : QEvent((QEvent::Type) TargetExecution)
{
    target = targe;
    timestamp = timestam;
    success = succes;
    exception = exceptio;
    data = dat;
}


