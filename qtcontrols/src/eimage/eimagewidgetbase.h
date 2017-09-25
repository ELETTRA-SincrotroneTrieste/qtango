#ifndef IMAGEWIDGETBASE_H
#define IMAGEWIDGETBASE_H

#include <QColor>
#include <QPoint>
#include <QImage>

class ImageMouseEventInterface;
class ExternalScaleWidget;
class QPaintEvent;
class QPaintDevice;
class QWidget;
class QMouseEvent;
class QWheelEvent;

class EImageWidgetBasePrivate
{
    public:
        QImage image, errorImage, noise;

        int isOpenGL;

        float zoom;

        bool error;

        QString errorMessage;

        ImageMouseEventInterface *mouseEventIf;

        bool leftButton, zoomEnabled;

        QPoint mP1, mP2;

        QWidget *widget;

        QVector<QRgb> colorTable;
};

class EImageWidgetBase
{
public:
    EImageWidgetBase(QWidget *widget, bool isOpenGL);

    virtual ~EImageWidgetBase();

    QImage& image() const;

    void setImageMouseEventInterface(ImageMouseEventInterface* ifa);

    void setError(bool error) { d->error = error ; }

    void setColorTable(const QVector<QRgb> &rgb);

    void setErrorMessage(const QString& msg) { d->errorMessage = msg; }

    void setZoom(int n) { d->zoom = n; }

    void setZoomEnabled(bool en);

    bool isOpenGL() const { return d->isOpenGL; }

    void setErrorImage(const QImage& i);

    QVector<QRgb>& colorTable() const;

    bool zoomEnabled() const;

    float zoom() const;

    void setImage(const QImage &image);

    QImage errorImage() const;

protected:

    void mouseMove(QMouseEvent *e);

    void mousePress(QMouseEvent *e);

    void mouseRelease(QMouseEvent *e);

    void wheelEvent(QWheelEvent * event);

    void paint(QPaintEvent *e, QWidget *paintDevice);

    void execConfigDialog();

private:

    EImageWidgetBasePrivate *d;
};

#endif // IMAGEWIDGETBASE_H
