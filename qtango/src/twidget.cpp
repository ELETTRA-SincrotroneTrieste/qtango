#include "twidget.h"
#include <QEvent>
#include <QDropEvent>
#include <QtDebug>
#include <macros.h>
#include <QMimeData>

class TWidgetPrivate
{
public:
    bool designMode;
};

TWDropEventFilter::TWDropEventFilter(QObject *parent) : QObject(parent)
{
    printf("\e[1;32mINSTANITATI DROPEVENTIFLER\e[0n\n");
}

bool TWDropEventFilter::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::Drop)
    {
        qDebug() << __FUNCTION__ << "eventFilter IS DROOOOP";
        QDropEvent *de = static_cast<QDropEvent *>(e);
        if(de->mimeData()->hasText())
        {
          QString source = de->mimeData()->text();
          qDebug() << __FUNCTION__ << source;
          de->accept();;
        }

        return true;
    }
    return QObject::eventFilter(o, e);
}

TWidget::TWidget(QWidget *parent) : QWidget(parent)
{
    d = new TWidgetPrivate();
    d->designMode = false;
}

TWidget::~TWidget()
{
    delete d;
}

void TWidget::setDesignerMode(bool dm)
{
    d->designMode = dm;
    printf("\e[1;32msetDesigner mode ! \e[0m\n");
    TWDropEventFilter *def = findChild<TWDropEventFilter *>();
    if(dm && !def)
    {
        qDebug() << __FUNCTION__ << "Install event filter (drop)";
        setAcceptDrops(true);
        printf("\e[1;34mcreating new DropEventFilter....\e[0m\n");
        def = new TWDropEventFilter(this);
        qDebug() << __FUNCTION__ << def;
        installEventFilter(def);
    }
    else if(!dm && def)
    {
        removeEventFilter(def);
        delete def;
    }
}
