#ifndef RTPLOT_H
#define RTPLOT_H

#include <QWidget>

namespace Ui {
class Widget;
}

class RTPlot : public QWidget
{
    Q_OBJECT

public:
    explicit RTPlot(QWidget *parent = 0);
    ~RTPlot();

private:
    Ui::Widget *ui;
};

#endif // RTPLOT_H
