#ifndef LED_H
#define LED_H

#include <QLabel>

class Led : public QLabel
{
    Q_OBJECT
public:
    explicit Led(QWidget *parent = 0);

signals:

public slots:
    void setOk(bool ok);

};

#endif // LED_H
