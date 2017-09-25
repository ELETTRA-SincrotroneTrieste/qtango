#include "widget.h"
#include "ui_widget.h"
#include <stdio.h>
#include "utils/ttextdbproperty.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    if(qApp->arguments().size() < 3)
    {
        printf("\nusage: \"%s device/class_name propertyName [--class]\"\n\n",
               qApp->arguments().first().toStdString().c_str());

        exit(EXIT_FAILURE);
    }
    ui->lPropName->setText(qApp->arguments().at(1));

    new TTextDbProperty(qApp->arguments().at(1),
                        qApp->arguments().at(2),
                        ui->lPropValue,
                        SLOT(setText(const QString&)),
                        !qApp->arguments().contains("--class"));


    /* window title */
    new TTextDbProperty(qApp->arguments().at(1),
                        "windowTitle",
                        this,
                        SLOT(setWindowTitle(QString)));

    new TTextDbProperty(qApp->arguments().at(1),
                        "description",
                        ui->textEdit,
                        SLOT(setHtml(QString)));

}

Widget::~Widget()
{
    delete ui;
}
