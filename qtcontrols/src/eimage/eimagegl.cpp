#include "eimagegl.h"
#include <QtDebug>

ImageGL::ImageGL(QWidget *parent) :
#if QT_VERSION >= 0x050000
    QOpenGLWidget(parent),
#else
    QGLWidget(parent),
#endif
    EImageWidgetBase(this, true)
{

}

void ImageGL::setImage(const QImage& img)
{
    EImageWidgetBase::setImage(img);
    /* get a reference to the image */
  //  QImage& imgRef = image();
  //  setGeometry(x(), y(), imgRef.width(), imgRef.height());
    update();
}

void ImageGL::mousePressEvent(QMouseEvent *ev)
{
    mousePress(ev);
}

void ImageGL::mouseMoveEvent(QMouseEvent *ev)
{
    mouseMove(ev);
}

void ImageGL::mouseReleaseEvent(QMouseEvent *ev)
{
    mouseRelease(ev);
}


void ImageGL::paintEvent(QPaintEvent *e)
{
    paint(e, this);
}
