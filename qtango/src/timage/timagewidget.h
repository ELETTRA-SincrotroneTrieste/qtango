#ifndef TIMAGEWIDGET_H
#define TIMAGEWIDGET_H

#include <QWidget>
#include <com_proxy_reader.h>
#include <eimagewidget.h>
#include <qtango_wcommon.h>

class TImageWidget : public EImageWidget, public QTangoComProxyReader, public QTangoWidgetCommon
{

 //   Q_PROPERTY(QString pixmapName READ pixmapName WRITE setPixmapName DESIGNABLE true)
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
    Q_PROPERTY(int period READ period WRITE setPeriod)
    Q_PROPERTY(bool autoConfiguration READ autoConfiguration WRITE setAutoConfiguration)
    Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)
    Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
    Q_PROPERTY(QString helperApplication READ helperApplication WRITE setHelperApplication)
    Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)

    Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
    Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
    Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
    Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
    Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
    Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)

    Q_OBJECT
public:
    TImageWidget(QWidget *parent = NULL);

public slots:
    void refresh(const TVariant &);
};

#endif // TIMAGEWIDGET_H
