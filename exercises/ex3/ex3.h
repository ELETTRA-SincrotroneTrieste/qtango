#ifndef EX3_H
#define EX3_H

#include <QWidget>

class TangoConfigurationParameters;

namespace Ui {
class Ex3;
}

class Ex3 : public QWidget
{
    Q_OBJECT

public:
    explicit Ex3(QWidget *parent = 0);
    ~Ex3();

private slots:
    void configureSlider(const TangoConfigurationParameters *cp);
    void spectrumDataReady(const QVector<double> &);
private:
    Ui::Ex3 *ui;
    short mShort;
};

#endif // EX3_H
