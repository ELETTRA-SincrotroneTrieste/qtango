#include "ex3.h"
#include "ui_ex3.h"
#include <QTWatcher>
#include <QTWriter>
#include <scenecurve.h>

#include <QtDebug>

Ex3::Ex3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ex3)
{
    ui->setupUi(this);

    QTWatcher *lcdWatcher = new QTWatcher(this);
    lcdWatcher->attach(ui->lcdNumber, SLOT(display(double)));
    lcdWatcher->setAutoDestroy(true);
    lcdWatcher->setSource("$1/double_scalar");

    QTWatcher *lineEditWatcher = new QTWatcher(this);
    lineEditWatcher->attach(ui->lineEdit,   SLOT(setText(const QString&)));
    lineEditWatcher->setSource("$1/double_scalar");

    QTWatcher *pBarWatcher = new QTWatcher(this);
    pBarWatcher->attach(ui->progressBar, SLOT(setValue(int)));
    pBarWatcher->setAutoConfSlot(QTWatcher::Min, SLOT(setMinimum(int)));
    pBarWatcher->setAutoConfSlot(QTWatcher::Max, SLOT(setMaximum(int)));
    pBarWatcher->setSource("$1/double_scalar");

    QTWriter *sliderWriter = new QTWriter(this);
    /* setMinimum, setMaximum are not slots!
     * Must configure manually
     */
    connect(sliderWriter, SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters*)),
            this, SLOT(configureSlider(const TangoConfigurationParameters *)));
    /* please note: we will always write int */
    sliderWriter->attach(ui->horizontalSlider, SIGNAL(sliderMoved(int)));
    sliderWriter->setTargets("$1/double_scalar");

    QTWatcher *boolWatcher = new QTWatcher(this);
    boolWatcher->attach(ui->checkBox, SLOT(setChecked(bool)));
    boolWatcher->setSource("$1/boolean_scalar");
    QTWriter *boolWriter = new QTWriter(this);
    boolWriter->attach(ui->checkBox, SIGNAL(clicked(bool)), SLOT(setChecked(bool)));
    boolWriter->setTargets(boolWatcher->source());

    QTWatcher *shortWatcher = new QTWatcher(this);
    shortWatcher->attach(&mShort);
    shortWatcher->setSource("$1/short_scalar_ro");

    QTWatcher *spectrumWatcher = new QTWatcher(this);
    spectrumWatcher->attach(this, SLOT(spectrumDataReady(const QVector<double> &)));
    spectrumWatcher->setSource("$1/double_spectrum_ro");
    ui->plot->addLineCurve("double_spectrum_ro");
}

Ex3::~Ex3()
{
    delete ui;
}

void Ex3::configureSlider(const TangoConfigurationParameters *cp)
{
    ui->horizontalSlider->setMinimum(cp->minValue());
    ui->horizontalSlider->setMaximum(cp->maxValue());
    ui->horizontalSlider->setValue(cp->currentValue().toInt(false));
}

void Ex3::spectrumDataReady(const QVector<double> &v)
{
    Data *data = ui->plot->findCurve("double_spectrum_ro")->data();
   // if(data->xData.size() != v.size())
    {
        QVector<double> x;
        for(int i = 0; i < v.size(); i++)
            x << i;
        ui->plot->setData("double_spectrum_ro", x, v);
    }
  //  else
  //      ui->plot->setData("double_spectrum_ro", v);

 //   ui->plot->scene()->update();
}
