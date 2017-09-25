#include "ex1b.h"
#include "ui_ex1b.h"

Ex1::Ex1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ex1)
{
    ui->setupUi(this);

    connect(ui->pbConnect,    SIGNAL(clicked()), this, SLOT(connectSource()));
    connect(ui->pbDisconnect, SIGNAL(clicked()), this, SLOT(disconnectSource()));
    connect(ui->qlReader, SIGNAL(message(const QString&)), ui->pteMessage, SLOT(setPlainText(const QString&)));
    connect(ui->qlReader, SIGNAL(readOk(bool)), ui->qlLed, SLOT(setOk(bool)));
    ui->lineEdit->setText("test/device/1/double_scalar"); /* example */
}

Ex1::~Ex1()
{
    delete ui;
}

void Ex1::connectSource()
{
    disconnectSource();
    QString src = ui->lineEdit->text();
    ui->qlReader->setSource(src);
}

void Ex1::disconnectSource()
{
    ui->qlReader->unsetSource();
    ui->qlLed->setOk(false);
    ui->qlReader->setText("####");
    ui->pteMessage->setPlainText("Click on connect");
}
