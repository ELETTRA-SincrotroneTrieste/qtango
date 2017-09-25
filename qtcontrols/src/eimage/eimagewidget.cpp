#include "eimagewidget.h"
#include "colortablemap.h"
#include <QtDebug>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>

/*
 * Class with properties specific to EImageWidget, as a QWidget.
 * Other properties are stored by EImageWidgetBasePrivate
 */
class EImageWidgetPrivate
{
public:

    bool scaleWithZoom;
};

EImageWidget::EImageWidget(QWidget *parent) :
    QWidget(parent),
    EImageWidgetBase(this, false)
{
    d = new EImageWidgetPrivate;
    d->scaleWithZoom = true;
}

EImageWidget::~EImageWidget()
{
    delete d;
}

bool EImageWidget::scaleWithZoom() const
{
    return d->scaleWithZoom;
}

QImage EImageWidget::errorImage() const
{
    return EImageWidgetBase::errorImage();
}

void EImageWidget::setErrorMessage(const QString &msg)
{
    EImageWidgetBase::setErrorMessage(msg);
}

void EImageWidget::setError(bool error)
{
    EImageWidgetBase::setError(error);
}

void EImageWidget::setOk(bool ok)
{
    setError(!ok);
}

void EImageWidget::setImage(const QImage& img)
{
    EImageWidgetBase::setImage(img);
    if(d->scaleWithZoom)
    {
        QImage& imgRef = image();
        setGeometry(x(), y(), imgRef.width() * (zoom() / 100.0), imgRef.height() * (zoom() / 100.0));
    }
    update();
}

void EImageWidget::setErrorImage(const QImage &img)
{
    EImageWidgetBase::setErrorImage(img);
}

void EImageWidget::mousePressEvent(QMouseEvent *ev)
{
    mousePress(ev);
}

void EImageWidget::mouseMoveEvent(QMouseEvent *ev)
{
    mouseMove(ev);
}

void EImageWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    mouseRelease(ev);
}

void EImageWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    EImageWidgetBase::paint(e, this);
}

void EImageWidget::wheelEvent(QWheelEvent *we)
{
    EImageWidgetBase::wheelEvent(we);
    we->accept();
    setImage(image());
}

QMenu *EImageWidget::rightClickMenu()
{
    QMenu *menu = new QMenu(this);
    menu->addAction("Change Color Map...", this, SLOT(execConfigDialog()));
    return menu;
}

QSize EImageWidget::minimumSizeHint() const
{
    const int min = 32;
    const QImage& i = image();
    if(!i.isNull() && i.width()/4.0 * i.height()/4.0 > min * min)
        return i.size();
    return QSize(min, min);
}

QSize EImageWidget::sizeHint() const
{
    if(image().isNull())
        return QSize(256, 256);
    return image().size();
}

void EImageWidget::execConfigDialog()
{
    EImageWidgetBase::execConfigDialog();
    update();
}

void EImageWidget::setScaleWithZoom(bool scale)
{
    d->scaleWithZoom = scale;
    update();
}


