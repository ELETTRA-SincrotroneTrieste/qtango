#ifndef EX2_H
#define EX2_H

#include <QWidget>

namespace Ui {
class Ex2;
}

class Ex2 : public QWidget
{
    Q_OBJECT

public:
    explicit Ex2(QWidget *parent = 0);
    ~Ex2();

private slots:
    /* set up min max for spinbox */
    void init(double min, double max);

private:
    Ui::Ex2 *ui;
};

#endif // EX2_H
