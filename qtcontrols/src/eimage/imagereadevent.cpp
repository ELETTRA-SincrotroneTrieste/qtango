#include "imagereadevent.h"

ImageReadEvent::ImageReadEvent() : QEvent(QEvent::Type(QEvent::User + 1))
{
    error = false;
    save = false;
    message = "";
    dimX = dimY = 0;
}

ImageReadEvent::~ImageReadEvent()
{

}
