#include "treaderwriter.h"
#include <QHBoxLayout>
#include <QTimer>
#include <TLineEdit>
#include "treaderpushbutton.h"
#include <TSpinBox>
#include <TComboBox>
#include <TDoubleSpinBox>
#include <TNumeric>
#include <QMouseEvent>
#include <leftclick_with_modifiers_eater.h>

TReaderWriter::TReaderWriter(QWidget *parent) : TLabel(parent), QTangoComProxyWriter(this)
{
    d_dsb = NULL;
    d_le = NULL;
    d_sb = NULL;
    d_comboBox = NULL;
    d_writerWidget = NULL;
    d_tNumeric = NULL;
    d_pbApply = NULL;
    d_readOnly = false;
    d_zoomOnHoverEnabled = false;
    d_zoomEnabled = true;
    d_zoomPercentage = 130;
    d_writerSingleStep = 1.0;
    d_inZoom = false;
    d_writerType = Auto;
    d_writerPosition  = AutoPosition;
    d_randomId = qrand();
    d_pbEdit = new TReaderPushButton(parentWidget());
    LeftClickWithModifiersEater *lcwme = new LeftClickWithModifiersEater(this);
    lcwme->setObjectName("LeftClickWithModifiersEater");

    //   d_pbEdit->setVisible(false);
    d_pbEdit->setMinimumSize(QSize(10, 10));
    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    /* we need auto configuration */
    TLabel::setAutoConfiguration(true);

    connect(QTangoComProxyReader::qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), this,
            SLOT(configure(const TangoConfigurationParameters *)));
    connect(d_pbEdit, SIGNAL(toggled(bool)), this, SLOT(editToggled(bool)));
    connect(d_pbEdit, SIGNAL(toggled(bool)), this, SIGNAL(editButtonToggled(bool)));
    connect(d_pbEdit, SIGNAL(clicked(bool)), this, SIGNAL(editButtonClicked()));
    connect(QTangoComProxyReader::qtangoComHandle(), SIGNAL(connectionFailed()), this, SLOT(disable()));
    /* the writer might impart a command with an argout */
    connect(QTangoComProxyWriter::qtangoComHandle(), SIGNAL(newData(const QString&, const TVariant& )), this,
            SIGNAL(newDataAvailable(const QString&, const TVariant& )));

    d_objName = objectName();
    /* pointing hand cursor over TLabel please */
    setCursor(QCursor(Qt::PointingHandCursor));

    setMouseTracking(true); /* for enter/leave events */
    setHdbActionEnabled(true); /* Hdb action */
    /* 4.2.2: necessary to auto configure the writer as TComboBox in configure():
     * TangoConfigurationParameters *cp passed as argument must have the hasValues
     * returning true.
     */
    QTangoComProxyReader::setDesiredAttributePropertyList(QStringList() << "values");

}

QString TReaderWriter::writerObjectName()
{
    return QString("TRW_WRITER%1_%2").arg(objectName()).arg(d_randomId);
}

void TReaderWriter::clearTargets()
{ 
    d_writerTargets.clear();
    QTangoComProxyWriter::clearTargets();
    QTangoComProxyWriter *writer = dynamic_cast<QTangoComProxyWriter *>(d_writerWidget);
    if(writer)
        writer->clearTargets();
}

QString TReaderWriter::targets()
{
    return d_writerTargets;
}

void TReaderWriter::setTargets(QString targets)
{
    qprintf("\e[1;32mTReaderWriter::setTargets %s\e[0m\n", qstoc(targets));
    /* TReaderWriter::setTargets() sets d_writerTargets and setTargets on the com proxy writer */
    d_writerTargets = targets;
    /* then setup the target of the writer */
    if(targets.isEmpty()) /* target will be equal to the source */
    {
        /* clear the writer targets and (re)set the writer target equal to the source */
        pinfo("(re)setting the target of the reader writer \"%s\" to its source (\"%s\")", qstoc(objectName()), qstoc(source()));
        d_setTargets(source());
    }
    else /* a target different from the source is requested */
        d_setTargets(d_writerTargets);
}

/* This does not change d_writerTargets .
 * Applies the target on the writer if the writer was configured.
 */
void TReaderWriter::d_setTargets(QString targets)
{
    if(!targets.isEmpty())
    {
	if(targets.contains(";") && targets.split(";", QString::SkipEmptyParts).size() > 1)
            perr("TReaderWriter \"%s\": multiple targets are not accepted: setting only first target\n", qstoc(objectName()));
	targets.remove(QRegExp(";.*"));
	
	targets = manipTarget(targets);

	QTangoComProxyWriter *writer = dynamic_cast<QTangoComProxyWriter *>(d_writerWidget);
	if(writer)
	{
            writer->setTargets(targets);
        }
        QTangoComProxyWriter::setTargets(targets);
    }
}

QString TReaderWriter::manipTarget(const QString& target)
{
    QString targetManip = target;
    if(targetManip.contains(QRegExp("\\(.*\\)"))) /* the user provided a customized target with one or more argins */
    {
	/* see if it is necessary to subsitute the reference to the writer part inside the target argin */
	targetManip.replace("%writer%", "&" + writerObjectName());
    }
    else
    {
	/* simple target provided: e.g. test/device/1/double_scalar */
	targetManip = QString("%1(&%2)").arg(targetManip).arg(writerObjectName());
    }
    return targetManip;
}

void TReaderWriter::setWriterType(WriterType t)
{
    if(t != d_writerType)
    {
	d_writerType = t;
	pinfo("reconfiguring TReaderWriter \"%s\" with writer type %d\n", qstoc(objectName()), t);
	if(QTangoComProxyReader::configured())
	{
            unsetSource();
            setSource(source());
	}
    }
}

void TReaderWriter::configure(const TangoConfigurationParameters *cp)
{
    QTangoComProxyWriter *writer = NULL;
    QString  target;
    WriterType writerType = LineEdit;

    if(d_sb)
    {
        delete d_sb;
        d_sb = NULL;
    }
    else if(d_dsb)
    {
        delete d_dsb;
        d_dsb = NULL;
    }
    else if(d_le)
    {
        delete d_le;
        d_le = NULL;
    }
    else if(d_comboBox)
    {
        delete d_comboBox;
        d_comboBox = NULL;
    }
    else if(d_tNumeric)
    {
        delete d_tNumeric;
        d_tNumeric = NULL;
    }
    if(d_pbApply)
    {
        delete d_pbApply;
        d_pbApply = NULL;
    }

    d_pbApply = new QPushButton(parentWidget());
    LeftClickWithModifiersEater *lcwme = findChild<LeftClickWithModifiersEater *>("LeftClickWithModifiersEater");
    d_pbApply->installEventFilter(lcwme);

    if(cp->dataFormat() == Tango::SCALAR)
    {
        /* QTango 3.1 engineeriing notation: leave full freedom to the user */
        if( (d_writerType == Auto && (cp->format().contains("eng") || cp->dataType() == DEV_STRING )) ||
            d_writerType == LineEdit)
        {
            d_le = new TLineEdit(parentWidget());
            writerType = LineEdit;
            writer = d_le;
            d_writerWidget = d_le;
            d_le->setAlignment(alignment());
            d_le->setObjectName(writerObjectName());
            //		  d_le->setScaleMode(ELineEdit::None);
            pok("configured TReaderWriter's writer \"%s\" as line edit due to engineeriing format \"%s\"",
                qstoc(objectName()), qstoc(cp->format()));
            d_pbApply->setStyleSheet("QPushButton { margin:1px; margin-top:2px; margin-bottom:2px; border:1px solid #8f8f91; border-radius:4px; background-color:white; min-width:15px; max-width:15px; }");
        }
        else if((d_writerType == Auto && cp->hasValues() && cp->values().size() > 0)
                || d_writerType == ComboBox)
            /* with attribute property "values" set, configure a combo box */
        {
            d_comboBox = new TComboBox(parentWidget());
            /* do not want auto configuration to clear items */
            d_comboBox->setAutoConfiguration(false);
            /* indexMode property taken from free property */
            d_comboBox->setIndexMode(property("comboIndexMode").isValid() &&
                property("comboIndexMode").toBool());
            writer = d_comboBox;
            writerType = ComboBox;
            d_writerWidget = d_comboBox;
            d_comboBox->setObjectName(writerObjectName());
            d_pbApply->setStyleSheet("QPushButton { margin:2px; border:1px solid #8f8f91; border-radius:4px; background-color:white; min-width:15px; max-width:15px; }");
            pok("configured writer as a combobox with default attribute configuration \"values\"");
        }
        else if(d_writerType == Auto || d_writerType == Numeric)
        {
            d_tNumeric = new TNumeric(parentWidget());
            writerType = Numeric;
            d_tNumeric->setFrameShape(QFrame::Box);
            d_tNumeric->setFrameShadow(QFrame::Sunken); /* like TLabel */
            d_tNumeric->setAutoFillBackground(true); /* avoid transparency among the digits when overlapping */
            writer = d_tNumeric;
            d_writerWidget = d_tNumeric;
            d_tNumeric->setObjectName(writerObjectName());
            d_tNumeric->setMinimumHeight(40);
            d_tNumeric->setStyleSheet("QFrame { background-color:white;}");
            d_pbApply->setStyleSheet("QPushButton {margin:1px; border:1px solid #8f8f91; border-radius:4px; background-color:white;  min-width:15px; max-width:15px; }");
            pok("configured writer as TNumeric");
        }
        else if(d_writerType == DoubleSpinBox)
        {
            d_dsb = new TDoubleSpinBox(parentWidget());
            writerType = DoubleSpinBox;
            writer = d_dsb;
            d_writerWidget = d_dsb;
            d_dsb->setObjectName(writerObjectName());
            // d_dsb->setScaleMode(EDoubleSpinBox::None);
            d_pbApply->setStyleSheet("QPushButton { margin:2px; border:1px solid #8f8f91; border-radius:4px; background-color:white; min-width:15px; max-width:15px; }");
            pok("configured writer as double spin box");
        }
        else if(d_writerType == SpinBox) /* QSpinBox for other types (integers) */
        {
            d_sb = new TSpinBox(parentWidget());
            writerType = SpinBox;
            d_sb->setObjectName(writerObjectName());
            d_sb->setSingleStep(1);
            //		  d_sb->setScaleMode(ESpinBox::None);
            d_writerWidget = d_sb;
            writer = d_sb;
            d_pbApply->setStyleSheet("QPushButton { margin:2px; border:1px solid #8f8f91; border-radius:4px; background-color:white; min-width:15px; max-width:15px; }");
            pok("configured writer as int spin box");
        }
        else
        {
            perr("TReaderWriter \"%s\": unable to configure writer", objinfo(this));
            return;
        }
        d_pbApply->setIcon(QIcon(":icons/dialog-ok-apply.png"));
        d_pbApply->setText("");
        d_pbApply->setFlat(false);
        d_pbApply->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        d_pbApply->setToolTip("Click to apply the changes");
        d_pbApply->setObjectName(QString("TRW_PUSHBUTTON%1_TARGET<%2>").arg(objectName()).arg(target));
        /* change style of apply button, if specified */
        if(!d_applyButtonStyleSheet.isEmpty())
            d_pbApply->setStyleSheet(d_applyButtonStyleSheet);

        connect(d_pbApply, SIGNAL(clicked()), this, SLOT(execute()));

        if(d_writerWidget)
        {
            d_writerWidget->setFont(font());
            /* change stylesheet if specified */
            if(!d_writerStyleSheet.isEmpty())
                d_writerWidget->setStyleSheet(d_writerStyleSheet);

            if(d_writerTargets.isEmpty()) /* setTargets() has not been called yet */
            {
                d_setTargets(source());
                pinfo("TReaderWriter \"%s\": no targets specified: assuming \"%s\"", qstoc(objectName()), qstoc(source()));
            }
            else /* d_writerTargets is not empty: surely setTargets() was called before */
            {
                pinfo("TReaderWriter \"%s\": targets previously configured: \"%s\"",  qstoc(objectName()), qstoc(targets()));
                writer->setTargets(d_writerTargets);
            }
            /* insert now the items in the combo box with the values of the desired attribute
             * properties. This must be done after setTargets() because TComboBox's setTargets
             * clears all the items. Notice also that auto configuration has been set to false
             * since it clears the items too.
             */
            if(writerType == ComboBox && cp->hasValues() && cp->values().size() > 0)
            {
                d_comboBox->insertItems(0, cp->values());
            }
        }
        qprintf("\e[1;32msetting targets \"%s\" on apply button and writer : %s\e[0m\n", qstoc(targets()), qstoc(writerObjectName()));
        editToggled(false);
    }

    QString desc = "";
    if (cp != NULL && cp->descriptionIsSet()) {
        desc.append(cp->description());
    }
    if(cp != NULL && cp->maxIsSet() && cp->minIsSet()) {
        desc.append("\n(min: "+ QString("%1").arg(cp->minValue()) + " max: "+ QString("%1").arg(cp->maxValue()) + ")");
    }
    setWhatsThis(desc);
    /* emit writerCreated signal to inform the user that the TReaderWriter has been configured */
    emit writerCreated(d_writerWidget, writerType);
}

void TReaderWriter::execute()
{
    QTangoComProxyWriter::execute();
    d_pbEdit->setChecked(false);
    emit applyClicked();
}

void TReaderWriter::setZoomEnabled(bool en)
{
    d_zoomEnabled = en;
    if(d_writerType == Auto && QTangoComProxyReader::configured())
    {
	pinfo("zoom mode changed and reader writer \"%s\" already configured: reconfiguring...", qstoc(objectName()));;
	unsetSource();
	setSource(source());
    }
}

void TReaderWriter::editToggled(bool en)
{
    if(en && d_writerWidget)
    {
        if(d_zoomEnabled && !d_inZoom)
            zoom(true);
        d_pbEdit->setIcon(QIcon(":icons/window-close.png"));
        d_pbEdit->setToolTip("Click to close the settings section");
        showWriter();
        repositionAll();
    }
    else if(d_writerWidget)
    {
        d_writerTargets.isEmpty() ? d_pbEdit->setIcon(QIcon(":icons/configure.png")) :
                d_pbEdit->setIcon(QIcon(":icons/configure-differentTarget.png"));
        d_pbEdit->setToolTip("Click to show the settings section");
        d_writerWidget->hide();
        if(d_pbApply)
            d_pbApply->hide();
        d_pbEdit->hide();
        if(d_zoomEnabled && d_inZoom)
            zoom(false);
    }
}

void TReaderWriter::showWriter()
{
    if(d_writerWidget)
    {
        d_writerWidget->show();
        if(d_pbApply)
        {
            d_pbApply->show();
            d_pbApply->raise();
        }
    }
}

void TReaderWriter::showEditButton()
{
    if(d_writerWidget && !d_pbEdit->isVisible() && !d_readOnly)
    {
        editToggled(d_writerWidget->isVisible());
        d_pbEdit->show();
        d_pbEdit->raise();
        repositionAll();
    }
}

void TReaderWriter::enterEvent(QEvent *e)
{
    ///   if(d_zoomEnabled && d_zoomOnHoverEnabled)
    /// 	zoom(true);
    showEditButton();
    QWidget::enterEvent(e);
}

void TReaderWriter::leaveEvent(QEvent *e)
{
    ///   if(d_zoomEnabled && d_zoomOnHoverEnabled)
    /// 	zoom(false);
    if(d_pbApply && !d_pbApply->isVisible())
        d_pbEdit->scheduleHide();
    QWidget::leaveEvent(e);
}

void  TReaderWriter::mouseMoveEvent(QMouseEvent *e)
{
    showEditButton();
    QWidget::mouseMoveEvent(e);
}

void TReaderWriter::mousePressEvent(QMouseEvent *e)
{
    if(d_zoomEnabled && !d_inZoom)
        zoom(true);
    if(e->button() == Qt::LeftButton)
        d_pbEdit->setChecked(true);

    TLabel::mousePressEvent(e);
}

void TReaderWriter::repositionAll()
{
    int applyWidth = 16;
    /* do calculations only if elements are visible */
    if(d_writerWidget)
    {
        int height = this->height();
        /* TNumeric cannot have a very small height */
        if(d_tNumeric != NULL && height < 48)
            height = 48;
        d_pbEdit->setMinimumHeight(this->height());
        d_pbEdit->setMaximumHeight(this->height());
        d_writerWidget->setMinimumHeight(height);
        d_writerWidget->setMaximumHeight(height);
        /* writer widget shorter by the pbApply width minus... */
        applyWidth = d_pbApply->width();
        d_writerWidget->setMinimumWidth(this->width() - applyWidth);
        d_writerWidget->setMaximumWidth(this->width() -  applyWidth);
        d_pbApply->setMaximumHeight(d_writerWidget->height());
        d_pbApply->setMinimumHeight(d_writerWidget->height());

        /* position the edit push button according to the geometry of the reader writer and the width
		 * of the apply button, if present, or the default applyWidth
		 */
        d_pbEdit->move(mapToParent(rect().topRight() - QPoint(applyWidth, 0)));
        qDebug() << mapToParent(rect().topRight() - QPoint(applyWidth, 0)) << "TReaderWriter rect " << geometry() <<
                "pb edit geometry " << d_pbEdit->geometry() << "pbEdit visible" << d_pbEdit->isVisible();

        /* d_pbApply does not scale with rescale because spin box or label height is constant */
        /* 1. parent widget contains writer below reader */
        if(( parentWidget() && (d_writerPosition == AutoPosition && parentWidget()->rect().contains(
                mapToParent(QPoint(rect().bottomLeft().x(),
                                   rect().bottomLeft().y() + d_writerWidget->height())))) ) || d_writerPosition == South)
        {
            d_writerWidget->move(mapToParent(rect().bottomLeft()));
            /* 1a. parent widget contains edit and apply buttons at reader's right */
            d_pbApply->move(mapToParent(rect().bottomRight() - QPoint(applyWidth, 0)));
        }
        /* 2. parent widget does not contain writer below reader */
        else if(parentWidget() && (d_writerPosition == AutoPosition || d_writerPosition == North))
        {
            /* move the writer up */
            d_writerWidget->move(mapToParent(rect().topLeft()) - QPoint(0, d_writerWidget->height()));
            /* 2a. parent contains apply and edit buttons at readers'right */
            d_pbApply->move(mapToParent(rect().topRight() - QPoint(applyWidth, d_writerWidget->height())));
        }
        else if(parentWidget() && d_writerPosition == East) /* place on the reader's right */
        {
            d_writerWidget->move(mapToParent(rect().topRight() /*+ QPoint(1, 1)*/));
            d_pbApply->move(mapToParent(rect().topRight() + QPoint(d_writerWidget->width(), 0)));
        }
        else if(parentWidget() && d_writerPosition == West) /* place on the reader's right */
        {
            d_writerWidget->move(mapToParent(rect().topLeft() - QPoint(d_writerWidget->width(), 0)));
            d_pbApply->move(mapToParent(rect().topLeft() - QPoint(d_writerWidget->width() + d_pbApply->width(), 0)));
        }
    }
}

void TReaderWriter::hideEvent(QHideEvent *e)
{
    /* reader hide event is guaranteed and managed by QTangoWCommon, inherited by TLabel */
    d_pbEdit->setChecked(false);
    ESimpleLabel::hideEvent(e);
}

bool TReaderWriter::event(QEvent *e)
{
    if(e->type() == QEvent::Show)
        d_savedSize = size();
    if(d_writerWidget && e->type() == QEvent::WindowDeactivate)
        d_pbEdit->setChecked(false);
    return TLabel::event(e);
}

int TReaderWriter::configButtonHideTimeout()
{
    return d_pbEdit->timerInterval();
}

void TReaderWriter::setConfigButtonHideTimeout(int to)
{
    d_pbEdit->setTimerInterval(to);
}

void TReaderWriter::setWriterSingleStep(double s)
{
    if(d_dsb)
    {
        d_dsb->setSingleStep(s);
        d_writerSingleStep = s;
    }
    else if(d_sb)
    {
        d_sb->setSingleStep(s);
        d_writerSingleStep = s;
    }
}

void TReaderWriter::zoom(bool z)
{
    if(parentWidget() && z && !parentWidget()->layout() && !d_inZoom && d_pbEdit && d_writerWidget && d_pbApply)
    {
	QRect r = geometry();
	/* QSize operator * rounds to the nearest ingeger: save the current size */
	d_savedSize = QSize(size());
	r.setSize(r.size() * (d_zoomPercentage / 100));
	setGeometry(r);
	/* if font scale is disabled, we must zoom the font manually here */
	if(fontScaleMode() == ESimpleLabel::None)
	{
            QFont f = font();
            double pointSizeF = f.pointSizeF() * (d_zoomPercentage / 100);
            f.setPointSizeF(pointSizeF);
            setFont(f);
            /* the writer will have the same font size */
            d_writerWidget->setFont(f);
            if(d_tNumeric) /* bug!? setFont() does not have effect if a stylesheet is set! */
		d_tNumeric->setStyleSheet(QString("QFrame { background-color:white; font-size:%1pt; }").arg(f.pointSizeF()));
	}
	else
	{
            FontScalingWidget *fsw = dynamic_cast<FontScalingWidget *>(d_writerWidget);
            if(fsw && fsw->scaleMode() !=  (int) fontScaleMode())
		fsw->setScaleMode(fontScaleMode());
            else if(d_tNumeric && !d_tNumeric->digitsFontScaleEnabled())
		d_tNumeric->setDigitsFontScaleEnabled(true);
	}
	d_inZoom = true;
	{
            this->raise();
            d_pbEdit->raise();
            d_writerWidget->raise();
            d_pbApply->raise();
	}
    }
    else if(parentWidget() && !parentWidget()->layout() && d_inZoom)
    {
	QRect r = geometry();
	r.setSize(d_savedSize);
	/* if font scale is disabled, we must zoom the font manually here */
	if(fontScaleMode() == ESimpleLabel::None)
	{
            QFont f = font();
            double pointSizeF = f.pointSizeF() / (d_zoomPercentage / 100);
            f.setPointSizeF(pointSizeF);
            setFont(f);
            if(d_tNumeric) /* bug!? setFont() does not have effect if a stylesheet is set! */
		d_tNumeric->setStyleSheet(QString("QFrame { background-color:white; font-size:%1pt; }").arg(pointSizeF));
	}
	setGeometry(r);
        d_inZoom = false;
    }
}

QList<QTangoCommunicationHandle*> TReaderWriter::handleList() const
{
    QList<QTangoCommunicationHandle*> hlist;
    hlist << QTangoComProxyReader::qtangoComHandle() << QTangoComProxyWriter::qtangoComHandle();
    return hlist;
}

void TReaderWriter::setApplyButtonStyleSheet(const QString &style)
{
    d_applyButtonStyleSheet = style;
    if(d_pbApply)
	d_pbApply->setStyleSheet(style);
}

void TReaderWriter::setWriterStyleSheet(const QString &style)
{
    d_writerStyleSheet = style;
    if(d_writerWidget)
	d_writerWidget->setStyleSheet(style);
}


