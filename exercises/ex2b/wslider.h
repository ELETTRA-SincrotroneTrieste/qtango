#ifndef WSLIDER_H
#define WSLIDER_H

#include <QSlider>
#include <com_proxy_writer.h>
#include <simple_dataproxy.h>

class WSliderPrivate;

class WSlider : public QSlider, public QTangoComProxyWriter,
        public SimpleDataProxy
{
    Q_OBJECT
public:
    WSlider(QWidget *parent = 0);

public slots:
    void setValue(double v);

signals:
    void valueChanged(double);
    void configured(double min, double max);

protected slots:
    /* must be implemented from SimpleDataProxy */
    virtual QString getData();

    void configure(const TangoConfigurationParameters *cp);
    void sliderChanged(int);

private:
    WSliderPrivate *d_ptr;

    int mToSliderValue(double v);
    double mFromSliderValue(int v);
};

#endif // WSLIDER_H
