#ifndef IMAGEMOUSEEVENTINTERFACE_H
#define IMAGEMOUSEEVENTINTERFACE_H

class QPoint;
#include <QMouseEvent>

class ImageMouseEventInterface
{
public:
    ImageMouseEventInterface();

    virtual void imageMousePressEvent(const QPoint& pos, int button) = 0;
    virtual void imageMouseReleaseEvent(const QPoint& pos, int button) = 0;
    virtual void imageMouseMoveEvent(const QPoint& pos, int button) = 0;
};

#endif // IMAGEMOUSEEVENTINTERFACE_H
