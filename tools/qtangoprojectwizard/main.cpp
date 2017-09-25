#include "qtangoprojectwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTangoProjectWizard w;
    w.show();

    return a.exec();
}
