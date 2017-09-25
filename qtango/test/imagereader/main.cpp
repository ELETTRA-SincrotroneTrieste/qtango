#include "widget.h"
#include <TApplication>
#include <timagewidget.h>
#include <QMessageBox>
#include <QTabWidget>

int main(int argc, char *argv[])
{
    TApplication a(argc, argv);

    Widget w;
    w.show();
    int period = 0;
    if(argc > 2)
        period = atoi(argv[2]);
    if(period == 0)
        period = 1000;
    if(argc > 1)
    {
        // foreach(TImageWidget *i, w.findChildren<TImageWidget *>())
        TImageWidget *i = w.findChild<QTabWidget *>()->widget(0)->findChild<TImageWidget *>();
        {
            i->setSource(argv[1]);
            i->setPeriod(period);
        }
    }
    else
    {
        QMessageBox::information(&w, "Usage", QString("%1 test/device/1/uchar_image_ro [ or other image attributes ]"));
        a.quit();
    }

    return a.exec();
}
