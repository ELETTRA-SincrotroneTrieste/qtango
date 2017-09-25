#include "rtplot.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RTPlot w;
    w.show();

    return a.exec();
}
