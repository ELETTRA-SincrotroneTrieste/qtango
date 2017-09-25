#include "plotsaver.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <macros.h>
#include <math.h>

/* dialog widgets */
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QDateTime>
#include <QTextEdit>
#include <QFile>


OptionsDialog::OptionsDialog(QWidget *parent, double xSam, double ySam) : QDialog(parent)
{
    xSample = xSam;
    ySample = ySam;
    mDateTimeFormat = false;
    QGridLayout *lo = new QGridLayout(this);
    QLabel *lXFormat = new QLabel(this);
    QLabel *lXFormatExample = new QLabel(this);
    QLabel *lExample = new QLabel(this);
    lExample->setText("Examples  below:");
    lExample->setAlignment(Qt::AlignHCenter);
    lXFormatExample->setObjectName("xFormatLabel");
    lXFormat->setText("x data format:");
    QLineEdit *leXFormat = new QLineEdit(this);
    leXFormat->setObjectName("LineEditXFormat");

    QCheckBox *cbXDateTime = new QCheckBox(this);
    cbXDateTime->setObjectName("cbXDateTime");
    cbXDateTime->setText("Format x data as date/time");
    leXFormat->setToolTip("For example: %f, %g, %e, %.2f, %.2g...\n"
                          "Change the format and see a sample representation\n"
                          "on the right, or check the \"Format data as date/time\"\n"
                          "above to format the x data as date-time.\n"
                          "A help text will be displayed if that option is chosen.");
    /* y */
    QLabel *lYFormat = new QLabel(this);
    QLabel *lYFormatExample = new QLabel(this);
    lYFormatExample->setObjectName("yFormatLabel");
    lYFormat->setText("y data format:");
    QLineEdit *leYFormat = new QLineEdit(this);
    leYFormat->setObjectName("LineEditYFormat");
    leYFormat->setToolTip("For example: %f, %g, %e, %.2f, %.2g...\n"
                          "Change the format and see a sample representation\n"
                          "on the right.");


    /* layout */
    lo->addWidget(cbXDateTime, 0, 2, 1, 1);
    lo->addWidget(lXFormat, 1, 0, 1, 1);
    lo->addWidget(leXFormat, 1, 1, 1, 2);
    lo->addWidget(lXFormatExample, 1, 3, 1, 1);
    lo->addWidget(lExample, 0, 3, 1, 1);

    /* 2nd row: y */
    lo->addWidget(lYFormat, 2, 0, 1, 1);
    lo->addWidget(leYFormat, 2, 1, 1,2);
    lo->addWidget(lYFormatExample, 2, 3, 1, 1);


    QString filename = QString(SHAREDIR) + QString("/qtcontrols/data/saveDataFormatHelp.html");
    QFile dataFormatHelpHtml(filename);
    if(dataFormatHelpHtml.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextEdit *textEdit = new QTextEdit(this);
        textEdit->setHtml(QString(dataFormatHelpHtml.readAll()));
        textEdit->setVisible(false);
        textEdit->setMinimumHeight(400);
        lo->addWidget(textEdit, 3, 0, 3, 4);
        connect(cbXDateTime, SIGNAL(toggled(bool)), textEdit, SLOT(setVisible(bool)));
        dataFormatHelpHtml.close();
    }
    else
        perr("Error opening file \"%s\" for reading.", qstoc(filename));


    /* ok and cancel */
    QPushButton *ok = new QPushButton(this);
    ok->setText("Ok");
    ok->setToolTip("Apply custom formats to the saved data");
    QPushButton *cancel = new QPushButton(this);
    cancel->setText("Save with default formats");
    cancel->setToolTip("Close this dialog applying the default formats for x and y values");
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

    int lastRow = lo->rowCount();
    lo->addWidget(ok, lastRow, 2, 1, 1);
    lo->addWidget(cancel, lastRow, 3, 1, 1);

    connect(cbXDateTime, SIGNAL(toggled(bool)), this, SLOT(setDateTimeFormat(bool)));
    connect(leXFormat, SIGNAL(textChanged(QString)), this, SLOT(updateXExample(QString)));
    connect(leYFormat, SIGNAL(textChanged(QString)), this, SLOT(updateYExample(QString)));


    leXFormat->setText("%f");
    leYFormat->setText("%g");
    this->setMinimumWidth(500);

    setWindowTitle(windowTitle() + "Change the format of saved data");
}

void OptionsDialog::updateXExample(const QString& format)
{
    if(mDateTimeFormat)
        findChild<QLabel *>("xFormatLabel")->setText(timestampToDateTimeString(xSample, format));
    else
        findChild<QLabel *>("xFormatLabel")->setText(QString().sprintf(qstoc(format), xSample));
}

void OptionsDialog::updateYExample(const QString& format)
{
    findChild<QLabel *>("yFormatLabel")->setText(QString().sprintf(qstoc(format), ySample));
}

void OptionsDialog::setDateTimeFormatEnabled(bool en)
{
    findChild<QCheckBox *>("cbXDateTime")->setChecked(en);
}

bool OptionsDialog::dateTimeFormat() const
{
    return findChild<QCheckBox *>("cbXDateTime")->isChecked();
}

QString OptionsDialog::xFormat() const
{
    return findChild<QLineEdit *>("LineEditXFormat")->text();
}

QString OptionsDialog::yFormat() const
{
    return findChild<QLineEdit *>("LineEditYFormat")->text();
}

void OptionsDialog::setDateTimeFormat(bool dateTimeFmt)
{
    bool prevDateTimeFormat = mDateTimeFormat;
    mDateTimeFormat = dateTimeFmt; /* before subsequent setText */
    /* disabled date time  format: restore a default %f */
    if(prevDateTimeFormat && !dateTimeFmt)
        findChild<QLineEdit *>("LineEditXFormat")->setText("%f");
    if(!prevDateTimeFormat && dateTimeFmt)
        findChild<QLineEdit *>("LineEditXFormat")->setText("yyyy-MM-dd hh:mm:ss.zzz");
}

QString OptionsDialog::timestampToDateTimeString(double x, const QString& format)
{
    QDateTime dt;
    /* tango timestamp has microseconds */
    double usecs = (x - floor(x)) * 1e6;
    int msecs = qRound(usecs/1000.0);
    dt.setTime_t(floor(x));
    dt = dt.addMSecs(msecs);
    return dt.toString(qstoc(format));
}

PlotSaver::PlotSaver()
{

}

bool PlotSaver::save(const QList<EPlotCurve *>curves, bool timeScale)
{
    if(!curves.size())
    {
        d_errorMessage = "No curves";
        return false;
    }
    else
    {
        EPlotCurve *firstCurve = curves.first();
        int dataSize = firstCurve->dataSize();
        /* choose a random value inside the first curve to provide a sample value for the options dialog */
        int index = floor(qrand() / (float) RAND_MAX * dataSize);
        QString xFormat = "%f", yFormat = "%g";
        bool dateTimeFormat = false; /* the default, as ever in qtango */
        QString header, line;
        double x = firstCurve->data()->sample(index).x();
        double y = firstCurve->data()->sample(index).y();
        OptionsDialog optionsDialog(0, x, y);
        if(optionsDialog.exec() == QDialog::Accepted)
        {
            xFormat = optionsDialog.xFormat();
            yFormat = optionsDialog.yFormat();
            dateTimeFormat = optionsDialog.dateTimeFormat();
        }
        d_fileName = QFileDialog::getSaveFileName(0, "Save on file" , QDir::homePath(), "Comma separated values (*.csv)");
        if(!d_fileName.isEmpty())
        {
            QFile file(d_fileName);
            if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                d_errorMessage = file.errorString();
                return false;
            }
            else
            {
                QList<int> sizes;
                QTextStream out(&file);
                foreach(EPlotCurve *c, curves)
                {
                    /* write the header and gather the sizes of all the curves */
                    timeScale ? header += "timestamp" : header += "x";
                    header += ",\"" + c->title().text() + "\",";
                    sizes << c->dataSize();
                }
                if(!header.isEmpty()) /* remove last `,' */
                    header.remove(header.length() - 1, 1);
                out << header + "\n";
                if(sizes.size() > 0)
                {
                    /* we have to find the curve with the greatest number of data inside: not all curves
           * might have the same data size and we do not want to miss any (x,y) data.
           */
                    qSort(sizes.begin(), sizes.end());
                    size_t maxSize = sizes.last(); /* here we have the maximum data size among all curves */
                    for(size_t i = 0; i < maxSize; i++)
                    {
                        line = QString();
                        for(int j = 0; j < curves.size(); j++)
                        {
                            EPlotCurve *jthCurve = curves.at(j);
                            if(jthCurve->dataSize() > i)
                            {
                                QString xVal, yVal;
                                double x = jthCurve->data()->sample(i).x();
                                if(dateTimeFormat)
                                    xVal = optionsDialog.timestampToDateTimeString(x, xFormat);
                                else
                                    xVal.sprintf(qstoc(xFormat), x);

                                yVal.sprintf(qstoc(yFormat), jthCurve->data()->sample(i).y());
                                line += QString("%1,%2,").arg(xVal).arg(yVal);
                            }
                            else
                                pinfo("PlotSaver::save: curve \"%s\" has data size %ld < %ld (max %ld)", qstoc(jthCurve->title().text()),
                                      jthCurve->dataSize(), i, maxSize);
                        }
                        if(line.endsWith(","))
                            line.remove(line.length() - 1, 1); /* remove `,' */
                        out << line + "\n";
                    }
                }
                file.close();
            }
        }
        return true;
    }
}



