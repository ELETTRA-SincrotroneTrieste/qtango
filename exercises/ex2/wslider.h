#ifndef WSLIDER_H
#define WSLIDER_H

#include <QSlider>
#include <com_proxy_writer.h>

class WSliderPrivate;

class WSlider : public QSlider, public QTangoComProxyWriter
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

    void configure(const TangoConfigurationParameters *cp);

    void sliderChanged(int);

    void write();

private:
    WSliderPrivate *d_ptr;

    int mToSliderValue(double v);
    double mFromSliderValue(int v);
};

#endif // WSLIDER_H
