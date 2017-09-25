#include "$HFILE$"
#include "ui_$HFILE$"

$MAINCLASS$::$MAINCLASS$(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::$MAINCLASS$)
{
    ui->setupUi(this);
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
