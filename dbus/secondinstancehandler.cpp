#include "secondinstancehandler.h"
#include <QObject>


SecondInstanceHandler::SecondInstanceHandler(QObject *obj)
{
    d_object = obj;
}

SecondInstanceHandler::~SecondInstanceHandler()
{

}

void SecondInstanceHandler::filter()
{

}
