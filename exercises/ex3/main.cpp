#include "ex3.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ex3 w;
    w.show();

    return a.exec();
}
