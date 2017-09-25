#ifndef EX1_H
#define EX1_H

#include <QWidget>

namespace Ui {
class Ex1;
}

class Ex1 : public QWidget
{
    Q_OBJECT

public:
    explicit Ex1(QWidget *parent = 0);
    ~Ex1();

private slots:
    void connectSource();
    void disconnectSource();

private:
    Ui::Ex1 *ui;
};

#endif // EX1_H
