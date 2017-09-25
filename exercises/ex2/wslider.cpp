#include "wslider.h"
#include <QTimer>


class WSliderPrivate
{
public:
    WSliderPrivate();

    double min, max;
    QTimer *execTimer;
};

WSliderPrivate::WSliderPrivate()
{
    execTimer = NULL;
    min = max = 0.0;
}

WSlider::WSlider(QWidget *parent)
    : QSlider(parent),
      QTangoComProxyWriter(this)
{
    d_ptr = new WSliderPrivate();

    connect(qtangoComHandle(),
            SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)),
            this,
            SLOT(configure(const TangoConfigurationParameters *)));

    /* NOTE: connect(this, SIGNAL(valueChanged(int)), this, ... must be made
     * after slider configuration, which is asynchronous
     */
    setDisabled(true);
    setMinimum(0);
    setMaximum(1000);
    d_ptr->execTimer = new QTimer(this);
    d_ptr->execTimer->setSingleShot(true);
    d_ptr->execTimer->setInterval(1000);
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

        connect(d_ptr->execTimer, SIGNAL(timeout()), this, SLOT(write()));
        /* 1. define range */
        emit configured(d_ptr->min, d_ptr->max);
        /* 2. define value */
        emit valueChanged(value);

        /* this connection must be made here to avoid unwanted writing
         * caused by the setValue. This call must be made also after the
         * emit valueChanged above, to avoid spinbox update and again slider
         * value change.
         */
        connect(this, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));

    }
    else /* minimum and maximum values in tango db are compulsory */
    {
        setDisabled(true);
        setToolTip("Minimum and maximum values are not set in the Tango database");
    }
}

void WSlider::sliderChanged(int val)
{
    d_ptr->execTimer->start();
    emit valueChanged(mFromSliderValue(val));
}

void WSlider::write()
{
    printf("writing %f\n", mFromSliderValue(this->value()));
    /* optional */ setExecutionMode(QTangoCommunicationHandle::Asynchronous);
    QList<TVariant > results = execute(mFromSliderValue(this->value()));
    /* results are empty if execution is asynchronous */
    if(results.size() == 1 && results.first().executionFailed()) /* perr(): macros.h */
        perr("should place the slider and the spinbox back to their previous values\n");
}

int WSlider::mToSliderValue(double v)
{
    return qRound((maximum() - minimum()) * (v - d_ptr->min) / (d_ptr->max - d_ptr->min));
}

double WSlider::mFromSliderValue(int v)
{
    return v * (d_ptr->max - d_ptr->min) / (maximum() - minimum()) + d_ptr->min;
}
