#include "ex2b.h"
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
        QString target = qApp->arguments().at(1);
        ui->qlRead->setSource(target);
        ui->hslWriter->setTargets(target);
        ui->lAttName->setText(target);
        ui->tpbApply->setTargets(target + "(&hslWriter)");

        connect(ui->hslWriter, SIGNAL(configured(double,double)),
                this, SLOT(init(double,double)));
        /* slider does not write. Can connect immediately */
        connect(ui->hslWriter, SIGNAL(valueChanged(double)),
                ui->sbValue, SLOT(setValue(double)));
        connect(ui->sbValue, SIGNAL(valueChanged(double)),
                ui->hslWriter, SLOT(setValue(double)));
    }
}

Ex2::~Ex2()
{
    delete ui;
}

void Ex2::init(double min, double max)
{
    ui->sbValue->setRange(min, max);
}
