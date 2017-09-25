#include "$HFILE$"
#include "ui_$HFILE$"

$MAINCLASS$::$MAINCLASS$(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::$MAINCLASS$)
{
    ui->setupUi(this);
}

$MAINCLASS$::~$MAINCLASS$()
{
    delete ui;
}
