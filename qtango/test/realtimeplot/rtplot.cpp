#include "rtplot.h"
#include "ui_rtplot.h"

RTPlot::RTPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

RTPlot::~RTPlot()
{
    delete ui;
}
