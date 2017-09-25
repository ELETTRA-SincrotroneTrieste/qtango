#include "actionfactoryobjectfilter.h"
#include "targetexecutionevent.h"
#include <macros.h>

ActionFactoryObjectFilter::ActionFactoryObjectFilter(QObject *parent) :
    QObject(parent)
{

}

bool ActionFactoryObjectFilter::eventFilter(QObject *obj, QEvent* event)
{
    if(event->type() == (int) TargetExecutionEvent::TargetExecution)
    {
        TargetExecutionEvent *tev = static_cast<TargetExecutionEvent *>(event);
//        printf("ActionFactoryObjectFilter: executed target %s %s %d %s\n",
//               qstoc(tev->target), ctime(&tev->timestamp), tev->success, qstoc(tev->exception));
        emit targetExecuted(tev->target, tev->timestamp, tev->success, tev->exception, tev->data);
        return true;
    }
    return QObject::eventFilter(obj, event);
}

