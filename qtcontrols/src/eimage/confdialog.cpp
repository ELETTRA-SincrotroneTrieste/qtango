#include "confdialog.h"
#include "ui_confdialog.h"
#include <math.h>

ConfDialog::ConfDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfDialog)
{
    ui->setupUi(this);
}

ConfDialog::~ConfDialog()
{
    delete ui;
}

QString ConfDialog::getColorTableName() const
{
    foreach(QRadioButton *rb, ui->palette_groupBox->findChildren<QRadioButton *>())
        if(rb->isChecked())
            return rb->property("paletteName").toString();
    return "c";
}
