#include "readlabelb.h"
#include <tvariant.h>
#include <elettracolors.h>
#include <macros.h>

class ReadLabelPrivate
{
public:
    ReadLabelPrivate();

    double mMinAlarm, mMaxAlarm, mMinWarn, mMaxWarn;
    QString unit, format;
};

ReadLabelPrivate::ReadLabelPrivate()
{
    mMinAlarm = mMaxAlarm = mMinWarn = mMaxWarn = 0.0;
}

ReadLabel::ReadLabel(QWidget *parent) :
    QLabel(parent),
    QTangoComProxyReader(this),
    QTangoWidgetCommon(this)
{
    d_ptr = new ReadLabelPrivate();

    connect(qtangoComHandle(), SIGNAL(newData(const TVariant &)),
            this, SLOT(refresh(const TVariant& )));
    /* configuration */
    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters*)),
            this, SLOT(autoConfigure(const TangoConfigurationParameters*)));

    /* connection setup health specific signals */
    connect(qtangoComHandle(), SIGNAL(connectionOk(bool)),
            this, SIGNAL(readOk(bool)));
    connect(qtangoComHandle(), SIGNAL(connectionErrorMessage(QString)),
            this, SIGNAL(message(QString)));
}

void ReadLabel::refresh(const TVariant &v)
{
    QString txt, format;
    double value;
    bool wrongDataType = false;
    QColor textColor(Qt::black);

    !d_ptr->format.isEmpty() ? format = d_ptr->format : format = "%f";

    /* test that it works for commands and attributes */
    if(v.quality() == ATTR_INVALID)
    {
        txt = "####";
        emit message(v.message());
        emit readOk(v.quality() != ATTR_INVALID);
    }
    else if(v.canConvertToDouble())
    {
        value = v.toDouble();
        /* qstoc: qstring to c string, #include <macros.h> */
        txt.sprintf(qstoc(format), value);

        setToolTip(v.message()); /* good in general */
        /* unit */
        if(!d_ptr->unit.isEmpty())
            txt += " [" + d_ptr->unit + "]";

        if(d_ptr->mMinAlarm != d_ptr->mMaxAlarm &&
                (value <= d_ptr->mMinAlarm || value >= d_ptr->mMaxAlarm))
            textColor = KRED;
        else if(d_ptr->mMinWarn != d_ptr->mMaxWarn &&
                (value <= d_ptr->mMinWarn || value >= d_ptr->mMaxWarn))
            textColor = KORANGE;
        emit message(v.message());
        emit readOk(v.quality() != ATTR_INVALID);
    }
    else
        wrongDataType = true;

    setText(txt);
    QPalette p = palette();
    if(p.color(QPalette::WindowText) != textColor)
    {
        p.setColor(QPalette::WindowText, textColor);
        setPalette(p);

    }

    /* read health status */
    emit readOk(v.quality() != ATTR_INVALID && !wrongDataType);

    if(!wrongDataType)
        emit message(v.message());
    else
        emit message("wrong data type: not a number");

}

void ReadLabel::autoConfigure(const TangoConfigurationParameters *cp)
{
    if(cp->mWarnIsSet())
    {
        d_ptr->mMaxWarn = cp->maxWarning();
        d_ptr->mMinWarn = cp->minWarning();
    }
    if(cp->mErrIsSet())
    {
        d_ptr->mMaxAlarm = cp->maxError();
        d_ptr->mMinAlarm = cp->minError();
    }
    d_ptr->unit = cp->displayUnit();
    d_ptr->format = cp->format();
}
