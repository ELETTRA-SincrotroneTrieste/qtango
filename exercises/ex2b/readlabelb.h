#ifndef READLABEL_H
#define READLABEL_H

#include <QLabel>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>

class TVariant;
class ReadLabelPrivate;
class TangoConfigurationParameters;

class ReadLabel : public QLabel,
        public QTangoComProxyReader,
        public QTangoWidgetCommon
{
    Q_OBJECT
public:
    explicit ReadLabel(QWidget *parent = 0);

signals:
    void readOk(bool);
    void message(const QString&);

protected:

public slots:

protected slots:

    void refresh(const TVariant &v);
    void autoConfigure(const TangoConfigurationParameters* cp);


private:
    ReadLabelPrivate *d_ptr;
};

#endif // READLABEL_H
