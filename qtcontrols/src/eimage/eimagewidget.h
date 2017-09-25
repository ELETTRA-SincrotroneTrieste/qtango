#ifndef IMAGE_H
#define IMAGE_H

#include <QWidget>
#include <QRgb>
#include <QVector>
#include <right_click_menuable.h>
#include <eimagewidgetbase.h>

class QImage;
class ExternalScaleWidget;
class ImageMouseEventInterface;
class QMenu;
class EImageWidgetPrivate;

class EImageWidget : public QWidget , public EImageWidgetBase, public RightClickMenuable
{
    Q_OBJECT

    Q_PROPERTY(bool scaleWithZoom READ scaleWithZoom WRITE setScaleWithZoom DESIGNABLE true)
    Q_PROPERTY(QImage errorImage READ errorImage WRITE setErrorImage  DESIGNABLE true)

public:
    explicit EImageWidget(QWidget *parent = 0);

    virtual ~EImageWidget();

    bool scaleWithZoom() const;

    QImage errorImage() const;

signals:
    
public slots:

    void setImage(const QImage& image);

    void setErrorImage(const QImage& img);

    void setErrorMessage(const QString &msg);

    void setError(bool error);

    void setOk(bool ok);

    void execConfigDialog();

    void setScaleWithZoom(bool scale);

protected:

    QSize minimumSizeHint() const;

    QSize sizeHint() const;

    void paintEvent(QPaintEvent *);

    void mousePressEvent(QMouseEvent *ev);

    void mouseMoveEvent(QMouseEvent *ev);

    void mouseReleaseEvent(QMouseEvent *ev);

    void wheelEvent(QWheelEvent *we);

    virtual QMenu* rightClickMenu();

private:
    EImageWidgetPrivate *d;

};

#endif // IMAGE_H
