#ifndef IMAGEGL_H
#define IMAGEGL_H

#include "eimagewidgetbase.h"

#if QT_VERSION >= 0x050000
#include <QOpenGLWidget>
class ImageGL : public QOpenGLWidget, public EImageWidgetBase
#else
#include <QGLWidget>
class ImageGL : public QGLWidget, public EImageWidgetBase
#endif
{
    Q_OBJECT

public:
    explicit ImageGL(QWidget *parent = 0);
    void setImage(const QImage& image);

signals:

public slots:

protected:

    void paintEvent(QPaintEvent *);

    void mousePressEvent(QMouseEvent *ev);

    void mouseMoveEvent(QMouseEvent *ev);

    void mouseReleaseEvent(QMouseEvent *ev);

private:
    
};

#endif // IMAGEGL_H
