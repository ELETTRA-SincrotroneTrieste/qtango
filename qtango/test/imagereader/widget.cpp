#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    TImageWidget *imageInScroll = new TImageWidget(this);
    ui->scrollArea->setWidget(imageInScroll);
    ui->cbScaleWithZoom->setChecked(true); /* default for TImageWidget is enabled */
    connect(ui->cbScaleWithZoom, SIGNAL(toggled(bool)), ui->image, SLOT(setScaleWithZoom(bool)));
    connect(ui->cbScaleWithZoom, SIGNAL(toggled(bool)), imageInScroll, SLOT(setScaleWithZoom(bool)));
}

Widget::~Widget()
{
    delete ui;
}
