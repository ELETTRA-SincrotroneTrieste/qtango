#include "timagewidget.h"
#include "timagedata_impl.h"
#include "eimagewidget.h"
#include <colortablemap.h>
#include <QImage>
#include <QDebug>

TImageWidget::TImageWidget(QWidget *parent) : EImageWidget(parent),
    QTangoComProxyReader(this),
    QTangoWidgetCommon(this)
{
    connect(this->qtangoComHandle(), SIGNAL(newData(TVariant)), this, SLOT(refresh(TVariant)));
    connect(this->qtangoComHandle(), SIGNAL(connectionErrorMessage(QString)), this, SLOT(setErrorMessage(QString)));
    connect(this->qtangoComHandle(), SIGNAL(connectionOk(bool)), this, SLOT(setOk(bool)));
}

void TImageWidget::refresh(const TVariant &v)
{
    bool read = true;
    setDisabled(v.quality() == ATTR_INVALID);
    setToolTip(v.message());

    setToolTip(v.message());
    setError(v.quality() == ATTR_INVALID);

    if(v.quality() == ATTR_INVALID)
    {
        setErrorMessage(v.message());
    }
    else if(v.canConvertToUCharImage())
    {
        TImageData<unsigned char> imgData = v.toUCharImage(read);
        QImage& img = image();
        if(img.size() != QSize(imgData.dimX(), imgData.dimY()))
        {
            QVector<QRgb> &colorT = colorTable();
            QImage newImage = QImage(imgData.dimX(), imgData.dimY(), QImage::Format_Indexed8);
            newImage.setColorTable(colorT);
            for(int i = 0; i < imgData.dimX(); i++)
            {
                for(int j = 0; j < imgData.dimY(); j++)
                    newImage.setPixel(i, j, imgData[i][j]);
            }
            /* newImage without colorTable. setImage will save current color table from current image
             * and then restore it
             */
            setImage(newImage);
        }
        else /* no resize needed */
        {
            for(int i = 0; i < imgData.dimX(); i++)
            {
                for(int j = 0; j < imgData.dimY(); j++)
                    img.setPixel(i, j, imgData[i][j]);
            }
            /* img is a reference to the current image. No resize needed, so image has been reused.
             * setImage will find a non empty color table and won't save/restore it
             */
            setImage(img);
        }
//        printf("TImageWidget::refresh dim x %d dim y %d this w %d this h %d\n", image().width(), image().height(),
//               width(), height());
    }
}
