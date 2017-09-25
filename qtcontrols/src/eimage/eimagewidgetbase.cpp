#include "eimagewidgetbase.h"
#include <QPaintEvent>
#include <QGLWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QtDebug>
#include <macros.h>
#include <elettracolors.h>
#include "eimagemouseeventinterface.h"
#include <math.h>
#include <QSettings>
#include "confdialog.h"
#include "colortablemap.h"

EImageWidgetBase::EImageWidgetBase(QWidget *widget, bool isOpenGL)
{
    d = new EImageWidgetBasePrivate();
    d->widget = widget;
    d->isOpenGL = isOpenGL;
    d->error = false;
    d->mouseEventIf = NULL;
    d->zoom = 100;
    d->zoomEnabled = true;
    d->image = QImage();
    d->colorTable = ColorTableMap()["c"];
    d->errorImage.load(":/artwork/okguy.png");
    d->noise.load(":/artwork/bwnoise.png");
}

EImageWidgetBase::~EImageWidgetBase()
{
    delete d;
}

QImage &EImageWidgetBase::image() const
{
    return d->image;
}

void EImageWidgetBase::setImage(const QImage& image)
{
    /* zoom taked place in paint() */
    d->image = image;
}

void EImageWidgetBase::setColorTable(const  QVector<QRgb> &rgb)
{
    d->colorTable = rgb;
    d->image.setColorTable(d->colorTable);
}

void EImageWidgetBase::setZoomEnabled(bool en)
{
    d->zoomEnabled = en;
}

bool EImageWidgetBase::zoomEnabled() const
{
    return d->zoomEnabled;
}

float EImageWidgetBase::zoom() const
{
    return d->zoom;
}

void EImageWidgetBase::setErrorImage(const QImage &i)
{
    d->errorImage = i;
}

QImage EImageWidgetBase::errorImage() const
{
    return d->errorImage;
}

QVector<QRgb> &EImageWidgetBase::colorTable() const
{
    return d->colorTable;
}

void EImageWidgetBase::mouseMove(QMouseEvent *ev)
{
    if(!d->mP1.isNull())
        d->mP2= ev->pos();
    if(d->mouseEventIf)
        d->mouseEventIf->imageMouseMoveEvent(ev->pos(), ev->button());
}

void EImageWidgetBase::mousePress(QMouseEvent *ev)
{
    d->mP1 = ev->pos();
    d->leftButton = (ev->button() == Qt::LeftButton);
    if(d->mouseEventIf)
        d->mouseEventIf->imageMousePressEvent(ev->pos(), ev->button());
}

void EImageWidgetBase::mouseRelease(QMouseEvent *ev)
{
    /* call the release method only if the button was pressed inside the label
     */
    QRect r(QPoint(0,0), d->widget->geometry().size());
    if(d->mouseEventIf && !d->mP1.isNull() && r.contains(ev->pos()))
        d->mouseEventIf->imageMouseReleaseEvent(ev->pos(), ev->button());
    else
        printf("\e[1;31m outside boundaries!@!\e[0m\n");

    d->mP1 = QPoint();
    d->mP2 = QPoint();
}

void EImageWidgetBase::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();
    if(delta > 0)
        d->zoom = d->zoom * delta/100.0;
    else
        d->zoom = d->zoom / (-delta/100.0);
}

void EImageWidgetBase::setImageMouseEventInterface(ImageMouseEventInterface* ifa)
{
    d->mouseEventIf = ifa;
}

void EImageWidgetBase::paint(QPaintEvent *e, QWidget *paintDevice)
{
    if(d->error)
        d->image = d->errorImage;
    else if(d->image.isNull())
        d->image = d->noise;

    QPainter p(paintDevice);
    QRect imgRect = d->image.rect();
    imgRect.setSize(d->image.size() * (d->zoom/100.0));
    qDebug() << __FUNCTION__ << "zoom: " << d->zoom << "imgRect" << imgRect << " geom " << paintDevice->geometry() <<
                " paint rect " << e->rect();
    p.drawImage(imgRect, d->image, d->image.rect());


    //
   // p.drawImage(0, 0, d->image);
    if(!d->mP1.isNull() && !d->mP2.isNull())
    {
        QPen pen = p.pen();
        pen.setWidthF(2.0);
        QRect r(QPoint(0,0), d->widget->geometry().size());
        if(!r.contains(d->mP2))
            pen.setColor(Qt::red);
        else
            pen.setColor(Qt::green);

        p.setPen(pen);

        int p1x = d->mP1.x() * 100 / d->zoom;
        int p1y = d->mP1.y() * 100 / d->zoom;
        int p2x = d->mP2.x() * 100 / d->zoom;
        int p2y = d->mP2.y() * 100 / d->zoom;

        QFont f = p.font();
        f.setBold(true);
        p.setFont(f);
        QFontMetrics fm(p.font());

        QColor bg = Qt::white;
        bg.setAlpha(184);

        if(!d->leftButton)
        {
            float fl = sqrt(pow(p2x - p1x, 2) + pow(p2y - p1y, 2));
            QString val = QString("%1").arg(fl, 0, 'f', 2);
            p.fillRect(QRectF(d->mP1.x(), d->mP1.y(), fm.width(val), fm.height()), bg);
            /* first draw line */
            p.drawLine(d->mP1, d->mP2);
            /* then text */
            pen.setColor(Qt::black);
            p.setPen(pen);
            p.drawText(QPointF(d->mP1.x(), d->mP1.y() + fm.height()), val);
        }
        else
        {
            p.drawRect(QRect(d->mP1, d->mP2));
            p.setPen(bg);
            p.fillRect(QRect(d->mP1, d->mP2), bg);
            /* then text */
            QString val = QString("%1,%2 %3x%4").arg(p1x).arg(p1y).arg(fabs(p2x - p1x)).
                    arg(fabs(p2y - p1y));
            p.fillRect(QRectF(d->mP1.x(), d->mP1.y(), fm.width(val), fm.height()), bg);
            pen.setColor(Qt::black);
            p.setPen(pen);
            p.drawText(QPointF(d->mP1.x(), d->mP1.y() + fm.height()), val);
        }
    }
    if(d->error)
    {
        QPen errPen(KDARKRED);
        errPen.setWidth(5);
        p.setPen(errPen);
        /* draw a cross on the figure */
        if(d->errorImage.isNull())
        {
            p.drawLine(e->rect().topLeft(), e->rect().bottomRight());
            p.drawLine(e->rect().bottomLeft(), e->rect().topRight());
        }
        else /* yeah! draw ok guy! */
        {
            if(d->errorImage.size() != e->rect().size())
                d->errorImage = d->errorImage.scaled(e->rect().size(), Qt::KeepAspectRatio);
            p.drawImage(e->rect(), d->errorImage);
        }

        QRect txtRect = e->rect();
        QColor bgColor = Qt::white;
        bgColor.setAlpha(180);
        p.fillRect(txtRect, bgColor);
        /* draw the error message */
        QFont f = p.font();
        f.setPointSize(10);
        f.setBold(true);
        p.setFont(f);
        qDebug() << __FUNCTION__ << "drawing error message";
        p.drawText(txtRect, d->errorMessage);
    }
}

void EImageWidgetBase::execConfigDialog()
{
    ConfDialog co(NULL);
    ColorTableMap colorMap;
    int result = co.exec();
    if(result == QDialog::Accepted)
    {
        d->image.setColorTable(colorMap[co.getColorTableName()]);
    }
}

