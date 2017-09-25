#include "wslider.h"

class WSliderPrivate
{
public:
    WSliderPrivate();

    double min, max;
};

WSliderPrivate::WSliderPrivate()
{
    min = max = 0.0;
}

WSlider::WSlider(QWidget *parent)
    : QSlider(parent),
      QTangoComProxyWriter(this),
      SimpleDataProxy()
{
    d_ptr = new WSliderPrivate();

    connect(qtangoComHandle(),
            SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)),
            this,
            SLOT(configure(const TangoConfigurationParameters *)));

    connect(this, SIGNAL(valueChanged(int)),
            this, SLOT(sliderChanged(int)));

    setDisabled(true);
    setMinimum(0);
    setMaximum(1000);
}

QString WSlider::getData()
{
    return QString::number(mFromSliderValue(this->value()));
}

void WSlider::setValue(double v)
{
    if(v != mFromSliderValue(value()))
        QSlider::setValue(mToSliderValue(v)); /* notice QSlider::setValue! */
}

/** The configuration slot
 * Always remember that this method is invoked asynchronously.
 * For example, you must think of the consequences that would be caused
 * by a connect() of the valueChanged signal placed close to the setTargets
 * call.
 * Connections to the valueChanged signal must be performed after
 * configure() ends, for example, inside a slot connected to the
 * configured() signal emitted at the end of this method
 */
void WSlider::configure(const TangoConfigurationParameters *cp)
{
    if(cp->minIsSet() && cp->maxIsSet())
    {
        setEnabled(true);
        d_ptr->min = cp->minValue();
        d_ptr->max = cp->maxValue();
        setToolTip(QString("Adjust the value from %1 to %2").arg(d_ptr->min)
                   .arg(d_ptr->max));
        /* false parameter means get the set point! */
        double value = cp->currentValue().toDouble(false);
        /* be aware that this will cause valueChanged to be emitted */
        setValue(mToSliderValue(value));

        /* 1. define range */
        emit configured(d_ptr->min, d_ptr->max);
        /* 2. define value */
        emit valueChanged(value);
    }
    else /* minimum and maximum values in tango db are compulsory */
    {
        setDisabled(true);
        setToolTip("Minimum and maximum values are not set in the Tango database");
    }
}

void WSlider::sliderChanged(int val)
{
    printf("slider %d mapped to %f\n", val, mFromSliderValue(val));
    emit valueChanged(mFromSliderValue(val));
}

int WSlider::mToSliderValue(double v)
{
    return qRound((maximum() - minimum()) * (v - d_ptr->min) / (d_ptr->max - d_ptr->min));
}

double WSlider::mFromSliderValue(int v)
{
    return v * (d_ptr->max - d_ptr->min) / (maximum() - minimum()) + d_ptr->min;
}
