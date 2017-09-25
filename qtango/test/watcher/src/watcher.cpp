/***************************************************************************
 *   Copyright (C) 2007 by Claudio Scafuri, Giacomo Strangolino   *
 *   claudio@hyo   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "watcher.h"
#include "qtwatcher.h"
#include "qtwriter.h"
#include <qtango.h>
#include <QSettings>
#include <QMessageBox>
#include <QtDebug>


Watcher::Watcher(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    QTWatcher *lcdWatcher = new QTWatcher(this);
    lcdWatcher->attach(ui.lcdNumber, SLOT(display(double)));
    lcdWatcher->setAutoDestroy(true);
    lcdWatcher->setSource("$1/double_scalar");

    QTWatcher *lineEditWatcher = new QTWatcher(this);
    lineEditWatcher->attach(ui.le,   SLOT(setText(const QString&)));
    lineEditWatcher->setSource("$1/string_scalar");
    /* write string scalar
     * NOTE: textChanged would perform a write at startup! Use textEdited
     */
    QTWriter *lineEditWriter = new QTWriter(this);
    lineEditWriter->attach(ui.leW, SIGNAL(textEdited(const QString&)), SLOT(setText(const QString&)));
    lineEditWriter->setTargets(lineEditWatcher->source());

    QTWatcher *labelWatcher = new QTWatcher(this);
    labelWatcher->attach(ui.l1,  SLOT(setText(const QString&)));
    labelWatcher->setSource("$1/short_scalar");

    /* use the dial to change  short scalar */
    QTWriter *dualWriter = new QTWriter(this);
    /* adding SLOT(setValue(int)) would not be useful because the QSlider has not
     * got the slots to set minimum and maximum values. So
     */
    dualWriter->attach(ui.writeDial, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));

    /* connect auto configuration signal to dialConfigured slot to set max and min
     * on the dial. Sliders are all connected to the dialConfigured slot because they
     * do not have special slots to set minimum and maximum.
     * NOTE: dialConfigured SLOT sets minimum and maximum of all sliders!
     * Current initialization values are then set by the auto configuration performed by
     * QTWriter by invoking the auto configuration slot provided in the attach() method
     * for the writers.
     */
    connect(dualWriter, SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters*)),
            this, SLOT(dialConfigured(const TangoConfigurationParameters*)));
    dualWriter->setTargets(labelWatcher->source());

    QTWatcher *dialWatcher = new QTWatcher(this);
    dialWatcher->attach(ui.dial, SLOT(setValue(int)));
    dialWatcher->setSource("$1/short_scalar");

    QTWatcher *sliderWatcher = new QTWatcher(this);
    sliderWatcher->attach(ui.horizontalSlider, SLOT(setValue(int)));
    sliderWatcher->setSource("$1/short_scalar");
    QTWriter *sliderWriter = new QTWriter(this);
    /* NOTE: sliderMoved not valueChanged! */
    sliderWriter->attach(ui.writeSlider, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));
    sliderWriter->setTargets(sliderWatcher->source());

    /* combo box. Let's try with it too! */
    QTWatcher *comboWatcher = new QTWatcher(this);
    comboWatcher->attach(ui.comboBox, SLOT(setCurrentIndex(int)));
    comboWatcher->setSource("$1/short_scalar");
    for(int i = 0; i < 512; i++)
        ui.comboBox->insertItem(i, QString("index %1").arg(i));
    QTWriter *comboWriter = new QTWriter(this);
    comboWriter->attach(ui.comboBox, SIGNAL(activated(int)), SLOT(setCurrentIndex(int)));
    comboWriter->setTargets(sliderWatcher->source());


    QTWatcher *progressBarWatcher = new QTWatcher(this);
    progressBarWatcher->attach(ui.progressBar, SLOT(setValue(int)));
    /* maximum and minimum */
    progressBarWatcher->setAutoConfSlot(QTWatcher::Min, SLOT(setMinimum(int)));
    progressBarWatcher->setAutoConfSlot(QTWatcher::Max, SLOT(setMaximum(int)));

    progressBarWatcher->setSource("$1/short_scalar_ro");

    /* use watcher to watch simple variables */
    connect(ui.pbUpdateInternal, SIGNAL(clicked()), this, SLOT(displayInternalVariables()));

    QTWatcher *intWatcher = new QTWatcher(this);
    QTWatcher *doubleWatcher = new QTWatcher(this);
    QTWatcher *shortWatcher = new QTWatcher(this);
    QTWatcher *stringWatcher = new QTWatcher(this);

    intWatcher->attach(&intVar);
    intWatcher->setSource("$1/long_scalar");
    doubleWatcher->attach(&doubleVar);
    doubleWatcher->setSource("$1/double_scalar");
    shortWatcher->attach(&shortVar);
    shortWatcher->setSource("$1/short_scalar_ro");
    stringWatcher->attach(&stringVar);
    stringWatcher->setSource("$1/string_scalar");

    QTWatcher *boolWatcher = new QTWatcher(this);
    boolWatcher->attach(ui.cb, SLOT(setChecked(bool)));
    boolWatcher->setSource("$1/boolean_scalar");
    QTWriter *boolWriter = new QTWriter(this);
    boolWriter->attach(ui.cb, SIGNAL(clicked(bool)), SLOT(setChecked(bool)));
    boolWriter->setTargets(boolWatcher->source());

    /* writer for state */
    QTWriter *stateWriter = new QTWriter(this);
    stateWriter->attach(ui.pushButton,  SIGNAL(clicked()));
    stateWriter->setTargets("$1->SwitchStates");
    /* switch states changes the state of the device. Let's connect the state to
     * the label aimed at indicating it.
     */
    QTWatcher *stateWatcher = new QTWatcher(this);
    stateWatcher->attach(ui.labelState,  SLOT(setText(const QString&)));
    stateWatcher->setSource("$1->State");
}

void Watcher::displayInternalVariables()
{
    QString text;
    text += "<h3>Internal variables values</h3>\n";
    text += "<ul>\n";
    text += QString("<li><strong>intVar</strong>: %1;</li>\n").arg(intVar);
    text += QString("<li><strong>doubleVar</strong>: %1;</li>\n").arg(doubleVar);
    text += QString("<li><strong>shortVar</strong>: %1;</li>\n").arg(shortVar);
    text += QString("<li><strong>stringVar</strong>: %1;</li>\n").arg(stringVar);
    text += "</ul>\n";

    ui.textEdit->setHtml(text);
}

void Watcher::dialConfigured(const TangoConfigurationParameters *cp)
{
    if(cp->minIsSet() && cp->maxIsSet())
    {
        ui.dial->setMinimum(cp->minValue());
        ui.dial->setMaximum(cp->maxValue());
        ui.horizontalSlider->setMinimum(cp->minValue());
        ui.horizontalSlider->setMaximum(cp->maxValue());
        printf("\e[0;33msetting min and max on the writeDial %f %f\e[0m\n",
               cp->minValue(), cp->maxValue());
        ui.writeDial->setMinimum(cp->minValue());
        ui.writeDial->setMaximum(cp->maxValue());
        ui.writeSlider->setMinimum(cp->minValue());
        ui.writeSlider->setMaximum(cp->maxValue());
        /* NOTE: might set cp->currentValue() initialization values here,
         * but if this slot is invoked before automatic configuration of the
         * QTangoizer, QTangoizer itself will initialize the currentValue
         */
    }
}

