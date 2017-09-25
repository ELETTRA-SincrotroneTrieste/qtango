#include "ex2.h"
#include "ui_ex2.h"

#include <QMessageBox>

Ex2::Ex2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ex2)
{
    ui->setupUi(this);
    if(qApp->arguments().count() < 2)
    {
        QMessageBox::information(this,
                                 "Usage",
                                 QString("%1 target [e.g. %1 test/device/1/double_scalar]"
                                         "\nTarget must be a double scalar attribute").
                                 arg(qApp->arguments().first()));
    }
    else
    {
        ui->qlRead->setSource(qApp->arguments().at(1));
        ui->hslWriter->setTargets(qApp->arguments().at(1));
        ui->lAttName->setText(qApp->arguments().at(1));
        connect(ui->hslWriter, SIGNAL(configured(double,double)),
                this, SLOT(init(double,double)));
    }
}

Ex2::~Ex2()
{
    delete ui;
}

void Ex2::init(double min, double max)
{
    ui->sbValue->setRange(min, max);
    /* connections involving the slider must be set up here, not
     * before! (See comments to void WSlider::configure in wslider.cpp)
     */
    connect(ui->hslWriter, SIGNAL(valueChanged(double)),
            ui->sbValue, SLOT(setValue(double)));
    connect(ui->sbValue, SIGNAL(valueChanged(double)),
            ui->hslWriter, SLOT(setValue(double)));
}
