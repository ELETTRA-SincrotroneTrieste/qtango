#include "ledb.h"

Led::Led(QWidget *parent) :
    QLabel(parent)
{
    setOk(false);
    setMinimumWidth(32);
    setAutoFillBackground(true);
}

void Led::setOk(bool ok)
{
    setText("  "); /* can be done from the designer */
    QPalette p = palette();
    if(ok)
        p.setColor(backgroundRole(), QColor(Qt::green));
    else
        p.setColor(backgroundRole(), QColor(Qt::red));
    setPalette(p);
}
