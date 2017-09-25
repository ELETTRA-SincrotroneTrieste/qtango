#include "readlabel.h"
#include <tvariant.h>

ReadLabel::ReadLabel(QWidget *parent) :
    QLabel(parent), QTangoComProxyReader(this)
{
    setText("####");
    connect(qtangoComHandle(), SIGNAL(newData(const TVariant &)),
            this, SLOT(refresh(const TVariant& )));

    /* connection setup health specific signals */
    connect(qtangoComHandle(), SIGNAL(connectionOk(bool)),
            this, SIGNAL(readOk(bool)));
    connect(qtangoComHandle(), SIGNAL(connectionErrorMessage(QString)),
            this, SIGNAL(message(QString)));
}

void ReadLabel::refresh(const TVariant &v)
{
    /* test that it works for commands and attributes */
    if(v.quality() == ATTR_INVALID)
        setText("####");
    else if(v.canConvertToString())
        setText(v.toString());

    setToolTip(v.message()); /* good in general */

    /* read health status */
    emit message(v.message());
    emit readOk(v.quality() != ATTR_INVALID);
}
