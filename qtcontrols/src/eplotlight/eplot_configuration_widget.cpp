#include "eplot_configuration_widget.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QGridLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QtDebug>
#include <QColorDialog>
#include <QSettings>
#include <macros.h>
#include "eplotcurve.h"

EPlotConfigurationWidget::EPlotConfigurationWidget(QWidget *parent) : QWidget(parent)
{
    d_plot = qobject_cast<EPlotLight *>(parent);
    Q_ASSERT(d_plot != NULL);
    ui.setupUi(this);
    init();
}

void EPlotConfigurationWidget::connectAppearenceWidgets()
{
    connect(ui.cbCurves, SIGNAL(currentIndexChanged(int)), this, SLOT(curveSelectionChanged(int)));
    connect(ui.curveColor, SIGNAL(clicked()), this, SLOT(editCurveColor()));
}

void EPlotConfigurationWidget::init()
{
    ui.rbXAutoConf->setDisabled(true);
    ui.rbXAutoConf->setHidden(true); /* hide it for now: auto configuration applies only to Y axis */
    /* need to initialize to true the following two so that xAxisAutoscale and yAxisAutoscale
   * correctly trigger a signal to disable the spin boxes if auto scales are enabled
   */
    ui.rbXManual->setChecked(true);
    ui.rbYManual->setChecked(true);

    const QMetaObject *mo = d_plot->metaObject();
    /* setup the ui */
    int idx;
    idx = getIndexOfProperty("timeScaleEnabled");
    if(idx >= 0)
	ui.gbX->setVisible(!mo->property(idx).read(d_plot).toBool());

    idx = getIndexOfProperty("DbPlotAdapterEnabled");
    printf("index of prop DbPlotAdapterEnabled %d\n", idx);
    if(idx >= 0)
        ui.gbX->setVisible(mo->property(idx).read(d_plot).toBool());

    idx = getIndexOfProperty("xAxisAutoscale");
    if(idx >= 0)
	ui.rbXAuto->setChecked(mo->property(idx).read(d_plot).toBool());

    idx = getIndexOfProperty("yAxisAutoscale");
    if(idx >= 0)
	ui.rbYAuto->setChecked(mo->property(idx).read(d_plot).toBool());

    idx = getIndexOfProperty("autoConfigured");
    if(idx >= 0 && !ui.rbYAuto->isChecked()) /* if not already auto configured */
    {
        if(idx >= 0)
            ui.rbYAutoConf->setChecked(mo->property(idx).read(d_plot).toBool());
    }
    else if(idx < 0)
        ui.rbYAutoConf->setVisible(false);
    /* initialize spin boxes with their values */
    foreach(QLineEdit *le, findChildren<QLineEdit *>(QRegExp("\\b(x|y).*")))
    {
	le->setText(QString("%1").arg(getPropertyAsDouble(le->objectName())));
    }
    QString autoscaleToolTip =
            "If this is enabled, set the percentage value on the spin box on the right.\n"
            "This option enlarges the scale of the suggested percentage, optimizing the\n"
            "scale for a better readability.";
    ui.xAutoscaleAdjustEnabled->setToolTip(autoscaleToolTip);
    ui.yAutoscaleAdjustEnabled->setToolTip(autoscaleToolTip);

    /* x autoscale adjustment */
    idx = getIndexOfProperty("xAutoscaleAdjustEnabled");
    ui.xAutoscaleAdjustEnabled->setChecked(mo->property(idx).read(d_plot).toBool());
    idx = getIndexOfProperty("xAutoscaleAdjustment");
    ui.xAutoscaleAdjustment->setValue(mo->property(idx).read(d_plot).toDouble());

    /* y autoscale adjustment */
    idx = getIndexOfProperty("yAutoscaleAdjustEnabled");
    ui.yAutoscaleAdjustEnabled->setChecked(mo->property(idx).read(d_plot).toBool());
    idx = getIndexOfProperty("yAutoscaleAdjustment");
    ui.yAutoscaleAdjustment->setValue(mo->property(idx).read(d_plot).toDouble());

    idx = getIndexOfProperty("xAxisLogScale");
    ui.xAxisLogScale->setChecked(mo->property(idx).read(d_plot).toBool());
    ui.xAxisLinearScale->setChecked(!mo->property(idx).read(d_plot).toBool());
    idx = getIndexOfProperty("yAxisLogScale");
    ui.yAxisLogScale->setChecked(mo->property(idx).read(d_plot).toBool());
    ui.yAxisLinearScale->setChecked(!mo->property(idx).read(d_plot).toBool());

    /* set point visible. This is available only for TPlotLightMarker  */
    if(d_plot->inherits("TPlotLightMarker") && d_plot->property("m_setPointEnabled").isValid())
        ui.setPointEnabled->setChecked(d_plot->property("m_setPointEnabled").toBool());
    ui.setPointEnabled->setVisible(d_plot->inherits("TPlotLightMarker"));



    idx = getIndexOfProperty("titleOnCanvasEnabled");
    ui.titleOnCanvasEnabled->setChecked(mo->property(idx).read(d_plot).toBool());

    idx = getIndexOfProperty("syncRefresh");
    ui.syncRefresh->setChecked(mo->property(idx).read(d_plot).toBool());

    idx = getIndexOfProperty("timeScaleEnabled");
    ui.gbDataBufSize->setTitle(mo->property(idx).read(d_plot).toBool() ? "Set time interval [seconds]" :
                               "Set data buffer size [points on the x axis]");

    ui.sbBufferSize->setSuffix(mo->property(idx).read(d_plot).toBool() ? " [s]" : "");

    ui.sbBufferSize->setToolTip(mo->property(idx).read(d_plot).toBool() ? "Limit the x axis to the number of seconds specified" :
                                "Limit the x axis to the number of points specified");

    initAppearenceSettings();

    /* disable spin boxes and check box for x and y if not in manual mode */
    ui.xLowerBound->setEnabled(ui.rbXManual->isChecked());
    ui.xUpperBound->setEnabled(ui.rbXManual->isChecked());
    /* Y */
    ui.yLowerBound->setEnabled(ui.rbYManual->isChecked());
    ui.yUpperBound->setEnabled(ui.rbYManual->isChecked());

    ui.xAutoscaleAdjustment->setEnabled(ui.xAutoscaleAdjustEnabled->isChecked() && ui.rbXAuto->isChecked());
    ui.yAutoscaleAdjustment->setEnabled(ui.yAutoscaleAdjustEnabled->isChecked() && ui.rbYAuto->isChecked());
    ui.xAutoscaleAdjustEnabled->setEnabled(ui.rbXAuto->isChecked());
    ui.yAutoscaleAdjustEnabled->setEnabled(ui.rbYAuto->isChecked());

    QDoubleValidator *doubleValidator = new QDoubleValidator(this);

    /* finally, create connections */
    connect(ui.rbXManual, SIGNAL(toggled(bool)), this, SLOT(xAdjustmentChanged()));
    connect(ui.rbXAuto, SIGNAL(toggled(bool)), this, SLOT(xAdjustmentChanged()));
    connect(ui.rbYManual, SIGNAL(toggled(bool)), this, SLOT(yAdjustmentChanged()));
    connect(ui.rbYAuto, SIGNAL(toggled(bool)), this, SLOT(yAdjustmentChanged()));
    connect(ui.xAxisLogScale, SIGNAL(toggled(bool)), this, SLOT(propertyChanged()));
    connect(ui.yAxisLogScale, SIGNAL(toggled(bool)), this, SLOT(propertyChanged()));

    foreach(QLineEdit *le, findChildren<QLineEdit *>(QRegExp("\\b(x|y).*")))
    {
	le->setValidator(doubleValidator);
	connect(le, SIGNAL(textChanged(const QString&)), this, SLOT(propertyChanged()));
    }
    foreach(QDoubleSpinBox *sb, findChildren<QDoubleSpinBox *>(QRegExp("\\b(x|y).*")))
	connect(sb, SIGNAL(valueChanged(double)), this, SLOT(propertyChanged()));
    foreach(QCheckBox *cb, findChildren<QCheckBox *>(QRegExp("\\b(x|y|setPointEnabled|colouredBackground|titleOnCanvasEnabled|syncRefresh).*")))
	connect(cb, SIGNAL(toggled(bool)), this, SLOT(propertyChanged()));
    connect(ui.pbApply, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui.cbBufferUnlimited, SIGNAL(toggled(bool)), ui.sbBufferSize, SLOT(setDisabled(bool)));
    connect(ui.xAutoscaleAdjustEnabled, SIGNAL(toggled(bool)), ui.xAutoscaleAdjustment, SLOT(setEnabled(bool)));
    connect(ui.yAutoscaleAdjustEnabled, SIGNAL(toggled(bool)), ui.yAutoscaleAdjustment, SLOT(setEnabled(bool)));
    connect(ui.pbRememberAppearanceSettings, SIGNAL(clicked()), this, SLOT(saveStyles()));

    idx = getIndexOfProperty("dataBufferSize");
    ui.cbBufferUnlimited->setChecked(mo->property(idx).read(d_plot).toInt() == -1);
    ui.sbBufferSize->setValue(mo->property(idx).read(d_plot).toInt());
}

void EPlotConfigurationWidget::initAppearenceSettings()
{
    QList<QwtPlotCurve *> curves = d_plot->curves();
    for(int i = 0; i < curves.size(); i++)
	ui.cbCurves->insertItem(i, curves.at(i)->title().text());
    connectAppearenceWidgets();
    curveSelectionChanged(0); /* initializes appearence settings of first curve */
}

void EPlotConfigurationWidget::xAdjustmentChanged()
{
    qprintf("\e[1;31mxAdjustmentChanged\e[0m\n");
    foreach(QLineEdit *w, findChildren<QLineEdit *>(QRegExp("\\b(xL|xU).*")))
	w->setEnabled(ui.rbXManual->isChecked());
    ui.xAutoscaleAdjustEnabled->setEnabled(ui.rbXAuto->isChecked());
    ui.xAutoscaleAdjustment->setEnabled(ui.rbXAuto->isChecked() && ui.xAutoscaleAdjustEnabled->isChecked());
    foreach(QDoubleSpinBox *sb, ui.gbX->findChildren<QDoubleSpinBox *>(QRegExp("\\bx.*")))
	sb->setValue(getPropertyAsDouble(sb->objectName()));
    foreach(QLineEdit *le, ui.gbX->findChildren<QLineEdit *>(QRegExp("\\bx.*")))
	le->setText(QString("%1").arg(getPropertyAsDouble(le->objectName())));
}

void EPlotConfigurationWidget::yAdjustmentChanged()
{
    qprintf("yAdjustmentChanged() entered\n");
    foreach(QLineEdit *w, ui.gbY->findChildren<QLineEdit *>(QRegExp("\\b(yU|yL).*")))
	w->setEnabled(ui.rbYManual->isChecked());
    ui.yAutoscaleAdjustEnabled->setEnabled(ui.rbYAuto->isChecked());
    ui.yAutoscaleAdjustment->setEnabled(ui.rbYAuto->isChecked() && ui.yAutoscaleAdjustEnabled->isChecked());

    foreach(QLineEdit *le, ui.gbY->findChildren<QLineEdit *>(QRegExp("\\by.*")))
	le->setText(QString("%1").arg(getPropertyAsDouble(le->objectName())));
    foreach(QDoubleSpinBox *sb, ui.gbY->findChildren<QDoubleSpinBox *>(QRegExp("\\by.*")))
	sb->setValue(getPropertyAsDouble(sb->objectName()));
}

int EPlotConfigurationWidget::getIndexOfProperty(const QString& name)
{
    const QMetaObject *mo = d_plot->metaObject();
    int idx;
    idx = mo->indexOfProperty(qstoc(name));
    qDebug() << "getting property " << name << " from " << d_plot;
    if(idx < 0)
        pwarn("error reading property \"%s\" from object \"%s\"", qstoc(name), qstoc(parent()->objectName()));
    return idx;
}

void EPlotConfigurationWidget::writeProperty(const QString& name, const QVariant& val)
{
    bool ret = false;
    int idx = getIndexOfProperty(name);
    if(idx >= 0)
    {
        const QMetaObject *mo = d_plot->metaObject();
        QMetaProperty mp = mo->property(idx);
        ret = mp.write(d_plot, val);
    }
    if(!ret)
	perr("error setting double property \"%s\"", qstoc(name));
}

double EPlotConfigurationWidget::getPropertyAsDouble(const QString& name)
{
    double ret = 0.0;
    int idx = getIndexOfProperty(name);
    if(idx >= 0)
    {
	const QMetaObject *mo = d_plot->metaObject();
	QMetaProperty mp = mo->property(idx);
	ret = mp.read(d_plot).toDouble();
    }
    return ret;
}

void EPlotConfigurationWidget::curveSelectionChanged(int)
{
    /* disable connections to avoid loops */
    ui.cbCurves->disconnect(SIGNAL(currentIndexChanged(int)));
    QList<QwtPlotCurve *> curves = d_plot->curves();
    foreach(QwtPlotCurve *c, curves)
    {
	if(c->title().text() == ui.cbCurves->currentText())
	{
            QPen cPen = c->pen();
            QPalette p = ui.curveColor->palette();
            p.setColor(QPalette::Button, cPen.color());
            ui.curveColor->setPalette(p);
            ui.sbCurveWidth->setValue(cPen.width());
            ui.sbAlpha->setValue(cPen.color().alpha());
            QwtPlotCurve::CurveStyle style = c->style();
            ui.cbCurveStyle->setCurrentIndex((int) style + 1);
            EPlotCurve *epc = dynamic_cast<EPlotCurve*>(c);
            if(epc)
                ui.cbshowYValuesEnabled->setChecked(epc->property("showYValuesEnabled").toBool());
            break;
	}
    }
    /* restore connections */
    connect(ui.cbCurves, SIGNAL(currentIndexChanged(int)), this, SLOT(curveSelectionChanged(int)));
}

void EPlotConfigurationWidget::curveStyleChanged()
{
#if QWT_VERSION >= 0x060000 /* CurveStyle enum starts from -1 in qwt 6 */
 d_plot->setCurveStyle(ui.cbCurves->currentText(), static_cast<QwtPlotCurve::CurveStyle>(ui.cbCurveStyle->currentIndex() - 1));
#else
    d_plot->setCurveStyle(ui.cbCurves->currentText(), (QwtPlotCurve::CurveStyle) ui.cbCurveStyle->currentIndex());
#endif

   /* remove from the current text the argin, if present, because the curve has the name of the tango source.
   * which does not contain argins.
   */
    EPlotCurve *crv = d_plot->curve(ui.cbCurves->currentText()/*.remove(QRegExp("\\(.*\\)"))*/);
    if(crv)
    {
        QColor color(ui.curveColor->palette().color(QPalette::Button));
        color.setAlpha(ui.sbAlpha->value());
        QPen p(color);
	p.setWidth(ui.sbCurveWidth->value());
        p.setCapStyle(Qt::FlatCap);
        crv->setProperty("showYValuesEnabled", ui.cbshowYValuesEnabled->isChecked());
	crv->setPen(p);
    }
    else
	perr("EPlotConfigurationWidget::curveStyleChanged(): no EPlotCurve found having name \"%s\".\n"
             "   Please use EPlotCurves instead of QwtPlotCurves in qtango plots.", qstoc(ui.cbCurves->currentText()));
}

void EPlotConfigurationWidget::saveStyles()
{
    QString id;
    QSettings s;
    for(int i = 0; i < ui.cbCurves->count(); i++)
    {
	QString curveTitle = ui.cbCurves->itemText(i);
	id = QString("%1_EPLOTLIGHT_CURVE_%2").arg(QCoreApplication::instance()->applicationName()).arg(curveTitle);
	EPlotCurve *crv = d_plot->curve(curveTitle);
	if(crv)
	{
            s.setValue(id + "_COLOR", crv->pen().color());
            s.setValue(id + "_COLOR_ALPHA", crv->pen().color().alpha());
            s.setValue(id + "_STYLE", crv->style());
            s.setValue(id + "_WIDTH", crv->pen().width());
            s.setValue(id + "_DISPLAY_YVALUES", crv->property("showYValuesEnabled").toBool());
	}
    }
}

void EPlotConfigurationWidget::editCurveColor()
{
    QColor c = QColorDialog::getColor(ui.curveColor->palette().color(QPalette::Button), this);
    if(c.isValid())
    {
	QPalette p = ui.curveColor->palette();
	p.setColor(QPalette::Button, c);
	ui.curveColor->setPalette(p);
    }
}

void EPlotConfigurationWidget::propertyChanged()
{
    qDebug() << "EPlotConfigurationWidget::propertyChanged()" << sender()->objectName() << "sender " << sender();
    d_changedProperties.insert(sender()->objectName());
}

void EPlotConfigurationWidget::apply()
{
    bool ok;
    /* Y section */
    if(ui.rbYAuto->isChecked())
	d_plot->setYAutoscale(true);
    else if(ui.rbYAutoConf->isChecked())
	QMetaObject::invokeMethod(d_plot, "restoreBoundsToAutoConfigurationValues", Qt::DirectConnection);
    else
    {
        d_plot->setYAutoscale(false);
	if(d_changedProperties.contains("yLowerBound") && ui.yLowerBound->text().toDouble(&ok) && ok)
            writeProperty("yLowerBound", ui.yLowerBound->text().toDouble());
	if(d_changedProperties.contains("yUpperBound") && ui.yUpperBound->text().toDouble(&ok) && ok)
            writeProperty("yUpperBound", ui.yUpperBound->text().toDouble());
    }
    /* auto scale adjustment */
    if(d_changedProperties.contains("yAutoscaleAdjustEnabled"))
    {
	writeProperty("yAutoscaleAdjustEnabled", ui.yAutoscaleAdjustEnabled->isChecked());
	/* also update the value */
	writeProperty("yAutoscaleAdjustment", ui.yAutoscaleAdjustment->value());
    }
    else if(d_changedProperties.contains("yAutoscaleAdjustment"))
	writeProperty("yAutoscaleAdjustment", ui.yAutoscaleAdjustment->value());

    /* X section */
    if(ui.rbXAuto->isChecked())
      d_plot->setXAutoscale(true);
    else if(ui.rbXManual->isChecked())
    {
        d_plot->setXAutoscale(false);
	if(d_changedProperties.contains("xLowerBound") && ui.xLowerBound->text().toDouble(&ok) && ok)
            writeProperty("xLowerBound", ui.xLowerBound->text().toDouble());
	if(d_changedProperties.contains("xUpperBound") && ui.xUpperBound->text().toDouble(&ok) && ok)
            writeProperty("xUpperBound", ui.xUpperBound->text().toDouble());
    }

    /* auto scale adjustment */
    if(d_changedProperties.contains("xAutoscaleAdjustEnabled"))
    {
	writeProperty("xAutoscaleAdjustEnabled", ui.xAutoscaleAdjustEnabled->isChecked());
	/* also update the value */
	writeProperty("xAutoscaleAdjustment", ui.xAutoscaleAdjustment->value());
    }
    else if(d_changedProperties.contains("xAutoscaleAdjustment"))
	writeProperty("xAutoscaleAdjustment", ui.xAutoscaleAdjustment->value());

    curveStyleChanged();

    /* log/linear scales */
    if(d_changedProperties.contains("xAxisLogScale"))
	writeProperty("xAxisLogScale", ui.xAxisLogScale->isChecked());
    if(d_changedProperties.contains("yAxisLogScale"))
	writeProperty("yAxisLogScale", ui.yAxisLogScale->isChecked());

    /* set point of read/write attributes enabled or not */
    if(d_changedProperties.contains("setPointEnabled"))
        writeProperty("setPointEnabled", ui.setPointEnabled->isChecked());

    /* coloured background */
    if(d_changedProperties.contains("colouredBackground"))
	writeProperty("colouredBackground", ui.colouredBackground->isChecked());

    /* title on canvas */
    if(d_changedProperties.contains("titleOnCanvasEnabled"))
	writeProperty("titleOnCanvasEnabled", ui.titleOnCanvasEnabled->isChecked());

    /* sync refresh property */
    if(d_changedProperties.contains("syncRefresh"))
    {
        writeProperty("syncRefresh", ui.syncRefresh->isChecked());
    }
    /* data buffer size */
    if(!ui.cbBufferUnlimited->isChecked())
	d_plot->setDataBufferSize(ui.sbBufferSize->value());
    else
	d_plot->setDataBufferSize(-1);

    d_changedProperties.clear();
}

EPlotConfigurationDialog::EPlotConfigurationDialog(QWidget *parent) : QDialog(parent)
{
    setModal(false);
    setAttribute(Qt::WA_DeleteOnClose);
    QHBoxLayout *lo = new QHBoxLayout(this);
    EPlotConfigurationWidget *confWidget = new EPlotConfigurationWidget(parent);
    lo->addWidget(confWidget);
}









