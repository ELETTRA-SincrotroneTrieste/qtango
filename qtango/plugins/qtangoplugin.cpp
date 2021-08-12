/* $Id: qtangoplugin.cpp,v 1.45 2014-08-26 14:55:11 giacomo Exp $ */
#include <tplotlight_marker.h>
#include <realtimeplot.h>
#include <qtangoplugin.h>
#include <QtDebug>
#include <QtPlugin>
#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowCursorInterface>
#include <QExtensionManager>
#include <QColorDialog>
#include <elettracolors.h>
#include <tpushbutton.h>
#include <tlabel.h>
#include <tsimplelabel.h>
#include <tapplynumeric.h>
#include <tnumeric.h>
#include <tlineargauge.h>
#include <tcirculargauge.h>
#include <tcheckbox.h>
#include <tcombobox.h>
#include <tlineedit.h>
#include <tcombobox.h>
#include <tspinbox.h>
#include <tdoublespinbox.h>
#include <tlogbutton.h>
#include <ttable.h>
#include <tpixmap.h>
#include <treaderwriter.h>
#include <twidgetgroup.h>
#include <qtango_info_textbrowser.h>
#include <tspectrumbutton.h>
#include <tpropertylabel.h>
#include <twidget.h>
#include <timage/timagewidget.h>

// #include <QTango>
#include <QTabWidget>

#include "editsourcedialog.h"
#include "edittargetdialog.h"
#include "tledbooleditor.h"
#include "tlabelbooleditor.h"
#include "ttablebooleditor.h"
#include "pointeditor.h"
#include <copypaste_xml.h>

//*///Added_code///
//TImageInterface::TImageInterface(QObject* parent) : CustomWidgetInterface(parent)
//{
//        d_name = "TImage";
//        d_include = "TImage";
//        d_icon = QPixmap(":pixmaps/timage.png");
//        d_domXml =
//        "<widget class=\"TImage\" name=\"timage\">\n"
//        " <property name=\"geometry\">\n"
//        "  <rect>\n"
//        "   <x>0</x>\n"
//        "   <y>0</y>\n"
//        "   <width>87</width>\n"
//        "   <height>28</height>\n"
//        "  </rect>\n"
//        " </property>\n"
//        "</widget>\n";

//}

//QWidget* TImageInterface::createWidget(QWidget* parent)
//{
//    TImage * image = new TImage(parent);
//    image->setDesignerMode(true);
//    image->enableDrops();
//    return image;
//}

//TImageSnpInterface::TImageSnpInterface(QObject* parent) : CustomWidgetInterface(parent)
//{
//        d_name = "TImageSnp";
//        d_include = "timagesnapshot.h";
//        d_icon = QPixmap(":pixmaps/timage.png");
//        d_domXml =
//        "<widget class=\"TImageSnapshot\" name=\"timagesnp\">\n"
//        " <property name=\"geometry\">\n"
//        "  <rect>\n"
//        "   <x>0</x>\n"
//        "   <y>0</y>\n"
//        "   <width>87</width>\n"
//        "   <height>28</height>\n"
//        "  </rect>\n"
//        " </property>\n"
//        "</widget>\n";

//}

//QWidget* TImageSnpInterface::createWidget(QWidget* parent)
//{
//    TImageSnapshot * image = new TImageSnapshot(parent);
//   // image->setDesignerMode(true);
//    image->enableDrops();
//    return image;
//}
///////Added_code_end/ */

DropEventFilter::DropEventFilter(QObject *parent) : QObject(parent)
{

}

bool DropEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::DragEnter && qobject_cast<TWidget *>(obj))
    {
      QDragEnterEvent *dev = static_cast<QDragEnterEvent *>(event);
      dev->acceptProposedAction();
        return true; /* stop handling event here */
    }
    else if(event->type() == QEvent::Drop)
    {
        QDropEvent *dropEvent = static_cast<QDropEvent *>(event);
        const QMimeData *mimeData = dropEvent->mimeData();
        QString text = mimeData->text();

        CopyPasteXml xmlDecoder;
        /* 1. try to extract source or target from an xml */
        QString point = xmlDecoder.getProperty(text, "source");
        if(point.isEmpty()) /* try with target */
            point = xmlDecoder.getProperty(text, "target");
        /* simple text source or target a/b/c/d or a/b/c->d */
        if(point.isEmpty() && text.contains(QRegExp(SOURCE_REGEXP)))
            point = text;

        if(!point.isEmpty())
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(obj);
            if(formWindow)
            {
                formWindow->clearSelection(true);
                formWindow->selectWidget(qobject_cast<QWidget *>(obj));


                if(qobject_cast<TWidget *>(obj))
                {
                    qDebug() << __FUNCTION__ << "drop in TWidget " << point << dropEvent->pos().x() << dropEvent->pos().y()
                             << "PARENT IS " << parent() << "parent paretn is " << parent()->parent();

                    qDebug() << __FUNCTION__ << formWindow << formWindow->cursor();
                //    QDesignerFormWindowCursorInterface *cursor = formWindow->cursor();


                    TLabel *label = new TLabel(qobject_cast<QWidget *>(obj));
                    label->setObjectName("TESTLABELFROM DESIGNER");
                    label->setSource(point);
                    qDebug() << __FUNCTION__ << "new label " << label << label->geometry() << "parent_>" << label->parent()
                             << "parent widget " << label->parentWidget() << label->source();
                    label->show();

                  //  label->move(dropEvent->pos().x(), dropEvent->pos().y());

                }
                else
                {

                    PointEditor pointEditor(qobject_cast<QWidget *>(obj), point);


                    /* TCheckBox for convenience sets sources and targets */
                    if(obj->metaObject()->indexOfProperty("targets") > -1 &&
                            obj->inherits("TCheckBox"))
                    {
                        pointEditor.textLabel()->setText("TCheckBox: set tango <b>source</b> and <b>target</b> point");
                        pointEditor.setWindowTitle("TCheckBox source and target editor");
                    }
                    else if(obj->metaObject()->indexOfProperty("source") > -1)
                    {
                        pointEditor.textLabel()->setText("Set tango <b>source</b> point on " + obj->objectName());
                        pointEditor.setWindowTitle(QString("%1 source editor").arg(obj->objectName()));
                    }
                    else if(obj->metaObject()->indexOfProperty("targets") > -1 && !obj->inherits("TCheckBox"))
                    {
                        pointEditor.textLabel()->setText("Set tango <b>target</b> point on " + obj->objectName());
                        pointEditor.setWindowTitle(QString("%1 target editor").arg(obj->objectName()));
                    }
                    pointEditor.exec();

                    if(pointEditor.result() == QDialog::Accepted)
                    {
                        if(obj->metaObject()->indexOfProperty("source") > -1)
                        {
                            formWindow->cursor()->setProperty("source", pointEditor.point());
                        }
                        else if(obj->metaObject()->indexOfProperty("targets") > -1)
                        {
                            formWindow->cursor()->setProperty("targets", pointEditor.point());
                        }
                        /* TCheckBox: set targets too! */
                        if(obj->metaObject()->indexOfProperty("targets") > -1 &&
                                obj->inherits("TCheckBox"))
                            formWindow->cursor()->setProperty("targets", pointEditor.point());
                    }
                }
            }
            else
                perr("formWindow is null (\"%s\")", qstoc(obj->objectName()));
        }
        else
            perr("point is empty (cannot set \"%s\" on \"%s\")", qstoc(text), qstoc(obj->objectName()));

        return true; /* always eat drop events */
    }
    return false;
}

CustomWidgetInterface::CustomWidgetInterface(QObject *parent): QObject(parent), d_isInitialized(false)
{
    QColor c;
    for (int i = 0; i < 48; i++)
    {
        c = EColor((Elettra::colors)i);
        if (c.isValid())
            QColorDialog::setStandardColor(i, c.rgb());
    }
}

void CustomWidgetInterface::initialize(QDesignerFormEditorInterface *formEditor)
{
    if (d_isInitialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    if (manager)
        manager->registerExtensions(new TaskMenuFactory(manager), Q_TYPEID(QDesignerTaskMenuExtension));

    d_isInitialized = true;
}

TSimpleLabelInterface::TSimpleLabelInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TSimpleLabel";
    d_include = "TSimpleLabel";
    d_icon = QPixmap(":pixmaps/simplelabel.png");
    d_domXml =
            "<widget class=\"TSimpleLabel\" name=\"tSimpleLabel\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>90</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TSimpleLabelInterface::createWidget(QWidget *parent)
{
    TSimpleLabel * label = new TSimpleLabel(parent);
    label->setDesignerMode(true);
    label->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(label);
    label->installEventFilter(dropEventFilter);
    return label;
}

TLabelInterface::TLabelInterface(QObject *parent): CustomWidgetInterface(parent)
{
    qDebug() << __FUNCTION__ << parent;
    d_name = "TLabel";
    d_include = "TLabel";
    d_icon = QPixmap(":pixmaps/tlabel.png");
    d_domXml =
            "<widget class=\"TLabel\" name=\"tLabel\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>90</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TLabelInterface::createWidget(QWidget *parent)
{
    TLabel * label = new TLabel(parent);
    qDebug() << __FUNCTION__<< "creating nwe TLabel and returning!!!!";
    label->setDesignerMode(true);
    label->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(label);
    label->installEventFilter(dropEventFilter);
    return label;
}

TPropertyLabelInterface::TPropertyLabelInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TPropertyLabel";
    d_include = "TPropertyLabel";
    d_icon = QPixmap(":pixmaps/propertyLabel.png");
    d_domXml =
            "<widget class=\"TPropertyLabel\" name=\"tPropertyLabel\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>90</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TPropertyLabelInterface::createWidget(QWidget *parent)
{
    TPropertyLabel * label = new TPropertyLabel(parent);
    DropEventFilter *dropEventFilter = new DropEventFilter(label);
    label->installEventFilter(dropEventFilter);
    return label;
}

TPixmapInterface::TPixmapInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TPixmap";
    d_include = "TPixmap";
    d_icon = QPixmap(":pixmaps/tpixmap.png");
    d_domXml =
            "<widget class=\"TPixmap\" name=\"tPixmap\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>64</width>\n"
            "   <height>64</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TPixmapInterface::createWidget(QWidget *parent)
{
    TPixmap *pixmap = new TPixmap(parent);
    pixmap->setDesignerMode(true);
    pixmap->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(pixmap);
    pixmap->installEventFilter(dropEventFilter);
    return pixmap;
}



TCheckBoxInterface::TCheckBoxInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TCheckBox";
    d_include = "TCheckBox";
    d_icon = QPixmap(":pixmaps/tcheckbox.png");
    d_domXml =
            "<widget class=\"TCheckBox\" name=\"tCheckBox\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>65</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TCheckBoxInterface::createWidget(QWidget *parent)
{
    TCheckBox *tCheckBox = new TCheckBox(parent);
    tCheckBox->QTangoComProxyReader::setDesignerMode(true);
    tCheckBox->QTangoComProxyWriter::setDesignerMode(true);
    tCheckBox->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tCheckBox);
    tCheckBox->installEventFilter(dropEventFilter);
    return tCheckBox;
}


TTableInterface::TTableInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TTable";
    d_include = "TTable";
    d_icon = QPixmap(":pixmaps/ttable.png");
    d_domXml =
            "<widget class=\"TTable\" name=\"tTable\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>160</width>\n"
            "   <height>90</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TTableInterface::createWidget(QWidget *parent)
{
    TTable *table = new TTable(parent);
    table->setDesignerMode(true);
    table->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(table);
    table->installEventFilter(dropEventFilter);
    return table;
}

TLedInterface::TLedInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TLed";
    d_include = "TLed";
    d_icon = QPixmap(":pixmaps/tled.png");
    d_domXml =
            "<widget class=\"TLed\" name=\"tLed\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>30</width>\n"
            "   <height>30</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TLedInterface::createWidget(QWidget *parent)
{
    TLed *led = new TLed(parent);
    led->enableDrops();
    led->setDesignerMode(true);
    DropEventFilter *dropEventFilter = new DropEventFilter(led);
    led->installEventFilter(dropEventFilter);
    return led;
}

TLineEditInterface::TLineEditInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TLineEdit";
    d_include = "TLineEdit";
    d_icon = QPixmap(":pixmaps/tlineedit.png");
    d_domXml =
            "<widget class=\"TLineEdit\" name=\"tLineEdit\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>60</width>\n"
            "   <height>23</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TLineEditInterface::createWidget(QWidget *parent)
{
    TLineEdit *tLineEdit  = new TLineEdit(parent);
    tLineEdit->setDesignerMode(true);
    tLineEdit->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tLineEdit);
    tLineEdit->installEventFilter(dropEventFilter);
    return tLineEdit;
}

TComboBoxInterface::TComboBoxInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TComboBox";
    d_include = "TComboBox";
    d_icon = QPixmap(":pixmaps/tcombobox.png");
    d_domXml =
            "<widget class=\"TComboBox\" name=\"tComboBox\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>90</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}
QWidget *TComboBoxInterface::createWidget(QWidget *parent)
{
    TComboBox* tCombo = new  TComboBox(parent);
    tCombo->setDesignerMode(true);
    tCombo->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tCombo);
    tCombo->installEventFilter(dropEventFilter);
    return tCombo;
}

TSpinBoxInterface::TSpinBoxInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TSpinBox";
    d_include = "TSpinBox";
    d_icon = QPixmap(":pixmaps/tspinbox.png");
    d_domXml =
            "<widget class=\"TSpinBox\" name=\"tSpinBox\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>90</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TSpinBoxInterface::createWidget(QWidget *parent)
{
    TSpinBox *tsb = new TSpinBox(parent);
    tsb->setDesignerMode(true);
    tsb->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tsb);
    tsb->installEventFilter(dropEventFilter);
    return tsb;
}

TDoubleSpinBoxInterface::TDoubleSpinBoxInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TDoubleSpinBox";
    d_include = "TDoubleSpinBox";
    d_icon = QPixmap(":pixmaps/tdoublespinbox.png");
    d_domXml =
            "<widget class=\"TDoubleSpinBox\" name=\"tDoubleSpinBox\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>90</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TDoubleSpinBoxInterface::createWidget(QWidget *parent)
{
    TDoubleSpinBox *tds = new TDoubleSpinBox(parent);
    tds->setDesignerMode(true);
    tds->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tds);
    tds->installEventFilter(dropEventFilter);
    return tds;
}



TLinearGaugeInterface::TLinearGaugeInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TLinearGauge";
    d_include = "TLinearGauge";
    d_icon = QPixmap(":pixmaps/tlineargauge.png");
    d_domXml =
            "<widget class=\"TLinearGauge\" name=\"tLinearGauge\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>40</width>\n"
            "   <height>100</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TLinearGaugeInterface::createWidget(QWidget *parent)
{
    TLinearGauge *linearGauge =  new TLinearGauge(parent);
    linearGauge->setDesignerMode(true);
    linearGauge->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(linearGauge);
    linearGauge->installEventFilter(dropEventFilter);
    return linearGauge;
}


TCircularGaugeInterface::TCircularGaugeInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TCircularGauge";
    d_include = "TCircularGauge";
    d_icon = QPixmap(":pixmaps/tcirculargauge.png");
    d_domXml =
            "<widget class=\"TCircularGauge\" name=\"tCircularGauge\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>100</width>\n"
            "   <height>100</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}



QWidget *TCircularGaugeInterface::createWidget(QWidget *parent)
{
    TCircularGauge *circularGauge = new TCircularGauge(parent);
    circularGauge->setDesignerMode(true);
    circularGauge->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(circularGauge);
    circularGauge->installEventFilter(dropEventFilter);
    return circularGauge;
}


TPlotLightMarkerInterface::TPlotLightMarkerInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TPlotLightMarker";
    d_include = "TPlotLightMarker";
    d_icon = QPixmap(":pixmaps/tplot.png");
    d_domXml =
            "<widget class=\"TPlotLightMarker\" name=\"tPlotLightMarker\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>250</width>\n"
            "   <height>200</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TPlotLightMarkerInterface::createWidget(QWidget *parent)
{
    TPlotLightMarker *tplMarker = new TPlotLightMarker(parent);
    tplMarker->setDesignerMode(true);
    tplMarker->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tplMarker);
    tplMarker->installEventFilter(dropEventFilter);
    return tplMarker;
}

TRealtimePlotInterface::TRealtimePlotInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TRealtimePlot";
    d_include = "TRealtimePlot";
    d_icon = QPixmap(":pixmaps/tplot.png");
    d_domXml =
            "<widget class=\"TRealtimePlot\" name=\"tRealtimePlot\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>250</width>\n"
            "   <height>200</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TRealtimePlotInterface::createWidget(QWidget *parent)
{
    TRealtimePlot *rtPlot = new TRealtimePlot(parent);
    rtPlot->setDesignerMode(true);
    rtPlot->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(rtPlot);
    rtPlot->installEventFilter(dropEventFilter);
    return rtPlot;
}


TNumericInterface::TNumericInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TNumeric";
    d_include = "TNumeric";
    d_icon = QPixmap(":pixmaps/tnumeric.png");
    d_domXml =
            "<widget class=\"TNumeric\" name=\"tNumeric\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>100</width>\n"
            "   <height>50</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TNumericInterface::createWidget(QWidget *parent)
{
    TNumeric *tNumeric = new TNumeric(parent, 4, 2);
    tNumeric->setDesignerMode(true);
    tNumeric->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tNumeric);
    tNumeric->installEventFilter(dropEventFilter);
    return tNumeric;
}


TApplyNumericInterface::TApplyNumericInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TApplyNumeric";
    d_include = "TApplyNumeric";
    d_icon = QPixmap(":pixmaps/tapplynumeric.png");
    d_domXml =
            "<widget class=\"TApplyNumeric\" name=\"tApplyNumeric\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>160</width>\n"
            "   <height>70</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TApplyNumericInterface::createWidget(QWidget *parent)
{
    TApplyNumeric *tApplyNumeric = new TApplyNumeric(parent);
    tApplyNumeric->setDesignerMode(true);
    tApplyNumeric->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tApplyNumeric);
    tApplyNumeric->installEventFilter(dropEventFilter);
    return tApplyNumeric;
}

TReaderWriterInterface::TReaderWriterInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TReaderWriter";
    d_include = "TReaderWriter";
    d_icon = QPixmap(":pixmaps/tlineedit.png");
    d_domXml =
            "<widget class=\"TReaderWriter\" name=\"tReaderWriter\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>90</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TReaderWriterInterface::createWidget(QWidget *parent)
{
    TReaderWriter *tReaderWriter = new TReaderWriter(parent);
    tReaderWriter->setDesignerMode(true);
    tReaderWriter->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tReaderWriter);
    tReaderWriter->installEventFilter(dropEventFilter);
    return tReaderWriter;
}


TWidgetGroupInterface::TWidgetGroupInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TWidgetGroup";
    d_include = "TWidgetGroup";
    d_icon = QPixmap(":pixmaps/twidgetgroup.png");
    d_domXml =
            "<widget class=\"TWidgetGroup\" name=\"tWidgetGroup\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>160</width>\n"
            "   <height>160</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TWidgetGroupInterface::createWidget(QWidget *parent)
{
    TWidgetGroup *tWidgetGroup = new TWidgetGroup(parent);
    tWidgetGroup->setDesignerMode(true);
    tWidgetGroup->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tWidgetGroup);
    tWidgetGroup->installEventFilter(dropEventFilter);
    return tWidgetGroup;
}

TWidgetInterface::TWidgetInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TWidget";
    d_include = "TWidget";
    d_icon = QPixmap(":pixmaps/twidgetgroup.png");
    d_domXml =
            "<widget class=\"TWidget\" name=\"tWidget\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>480</width>\n"
            "   <height>328</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TWidgetInterface::createWidget(QWidget *parent)
{
    TWidget *tWidget = new TWidget(parent);
    tWidget->setDesignerMode(true);
    tWidget->setAcceptDrops(true);
    DropEventFilter *dropEventFilter = new DropEventFilter(tWidget);
    tWidget->installEventFilter(dropEventFilter);
    return tWidget;
}

QTangoInfoTextBrowserInterface::QTangoInfoTextBrowserInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "QTangoInfoTextBrowser";
    d_include = "QTangoInfoTextBrowser";
    d_icon = QPixmap(":pixmaps/tinfobrowser.png");
    d_domXml =
            "<widget class=\"QTangoInfoTextBrowser\" name=\"qtangoInfoTextBrowser\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>250</width>\n"
            "   <height>300</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *QTangoInfoTextBrowserInterface::createWidget(QWidget *parent)
{
    QTangoInfoTextBrowser *infoTextBrowser = new QTangoInfoTextBrowser(parent);
    return infoTextBrowser;
}

TSpectrumButtonInterface::TSpectrumButtonInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TSpectrumButton";
    d_include = "TSpectrumButton";
    d_icon = QPixmap(":pixmaps/tpushbutton.png");
    d_domXml =
            "<widget class=\"TSpectrumButton\" name=\"tSpectrumButton\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>87</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}


QWidget *TSpectrumButtonInterface::createWidget(QWidget *parent)
{
    TSpectrumButton *tSpectrumButton = new TSpectrumButton(parent);
    tSpectrumButton->setDesignerMode(true);
    tSpectrumButton->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tSpectrumButton);
    tSpectrumButton->installEventFilter(dropEventFilter);
    tSpectrumButton->setText("Apply Values");
    return tSpectrumButton;
}

TPushButtonInterface::TPushButtonInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TPushButton";
    d_include = "TPushButton";
    d_icon = QPixmap(":pixmaps/tpushbutton.png");
    d_domXml =
            "<widget class=\"TPushButton\" name=\"tPushButton\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>87</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TPushButtonInterface::createWidget(QWidget *parent)
{
    TPushButton *tPushButton = new TPushButton(parent);
    tPushButton->setDesignerMode(true);
    tPushButton->enableDrops();
    DropEventFilter *dropEventFilter = new DropEventFilter(tPushButton);
    tPushButton->installEventFilter(dropEventFilter);
    return tPushButton;
}

TImageWidgetInterface::TImageWidgetInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TImageWidget";
    d_include = "TImageWidget";
    d_icon = QPixmap(":pixmaps/timagewidget.png");
    d_domXml =
            "<widget class=\"TImageWidget\" name=\"tImageWidget\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>87</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TImageWidgetInterface::createWidget(QWidget *parent)
{
    TImageWidget *tImageWidget = new TImageWidget(parent);
    DropEventFilter *dropEventFilter = new DropEventFilter(tImageWidget);
    tImageWidget->installEventFilter(dropEventFilter);
    return tImageWidget;
}



TLogButtonInterface::TLogButtonInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TLogButton";
    d_include = "TLogButton";
    d_icon = QPixmap(":pixmaps/tlogbutton.png");
    d_domXml =
            "<widget class=\"TLogButton\" name=\"tLogButton\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>87</width>\n"
            "   <height>28</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
}

QWidget *TLogButtonInterface::createWidget(QWidget *parent)
{
    TLogButton *tLogButton = new TLogButton(parent);
    return tLogButton;
}

/*

TApplyButtonInterface::TApplyButtonInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "TApplyButton";
    d_include = "TApplyButton";
    d_icon = QPixmap(":pixmaps/tapplybutton.png");
    d_domXml =
        "<widget class=\"TApplyButton\" name=\"t\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>87</width>\n"
        "   <height>28</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *TApplyButtonInterface::createWidget(QWidget *parent)
{
    return new TApplyButton(parent);
}
*/
CustomWidgetCollectionInterface::CustomWidgetCollectionInterface(QObject *parent): QObject(parent)
{
    d_plugins.append(new TSimpleLabelInterface(this));
    d_plugins.append(new TLabelInterface(this));
    d_plugins.append(new TTableInterface(this));
    d_plugins.append(new TLedInterface(this));
    d_plugins.append(new TLineEditInterface(this));
    d_plugins.append(new TComboBoxInterface(this));
    d_plugins.append(new TSpinBoxInterface(this));
    d_plugins.append(new TDoubleSpinBoxInterface(this));
    d_plugins.append(new TLinearGaugeInterface(this));
    d_plugins.append(new TCircularGaugeInterface(this));
    d_plugins.append(new TNumericInterface(this));
    d_plugins.append(new TApplyNumericInterface(this));
    d_plugins.append(new TPushButtonInterface(this));
    d_plugins.append(new TSpectrumButtonInterface(this));
    d_plugins.append(new TLogButtonInterface(this));
    //    d_plugins.append(new TApplyButtonInterface(this));
    d_plugins.append(new TCheckBoxInterface(this));
    d_plugins.append(new TPixmapInterface(this));
    d_plugins.append(new TReaderWriterInterface(this));
    d_plugins.append(new TPlotLightMarkerInterface(this));
    d_plugins.append(new TRealtimePlotInterface(this));
    d_plugins.append(new TWidgetGroupInterface(this));
    d_plugins.append(new QTangoInfoTextBrowserInterface(this));
    d_plugins.append(new TPropertyLabelInterface(this));
    d_plugins.append(new TWidgetInterface(this));
    d_plugins.append(new TImageWidgetInterface(this));
    //    d_plugins.append(new TImageInterface(this));
    //    d_plugins.append(new TImageSnpInterface(this));
}

QList<QDesignerCustomWidgetInterface*> CustomWidgetCollectionInterface::customWidgets(void) const
{
    return d_plugins;
}

/* TaskMenuFactory */

TaskMenuFactory::TaskMenuFactory(QExtensionManager *parent): QExtensionFactory(parent)
{
}

QObject *TaskMenuFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerTaskMenuExtension))
        return 0;

    if (qobject_cast<TLabel*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TSimpleLabel*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TTable*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TLed*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TLinearGauge*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TCircularGauge*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TPushButton*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TApplyNumeric*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TNumeric*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TLineEdit*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TDoubleSpinBox*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TSpinBox*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if(qobject_cast<TPropertyLabel *>(object))
        return new TaskMenuExtension((QWidget *)object, parent);

    else if (qobject_cast<TNumeric*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TCheckBox*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TPixmap*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TPlotLightMarker*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TRealtimePlot*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if(qobject_cast<TReaderWriter *>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    /* TComboBox to allow attribute configuration "values" property */
    else if(qobject_cast<TComboBox *>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if(qobject_cast<TWidgetGroup *>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if(qobject_cast<TSpectrumButton *>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    else if (qobject_cast<TImageWidget*>(object))
        return new TaskMenuExtension((QWidget*)object, parent);
    return QExtensionFactory::createExtension(object, iid, parent);
}
/* */

/* TaskMenuExtension */
TaskMenuExtension::TaskMenuExtension(QWidget *widget, QObject *parent): QObject(parent), d_widget(widget), d_editTangoAction(0), d_editAction(0), editSourceDialog(0)
{
    d_editTangoAction = new QAction(tr("Edit Tango Connection..."), this);
    d_editAction = new QAction(tr("Edit Properties..."), this);
    connect(d_editTangoAction, SIGNAL(triggered()), this, SLOT(editTango()));
    connect(d_editAction, SIGNAL(triggered()), this, SLOT(editAttributes()));
}

QList<QAction *> TaskMenuExtension::taskActions() const
{
    QList<QAction *> list;
    QString cname(d_widget->metaObject()->className());
    if ((cname == "TSimpleLabel") || (cname == "TLabel") || (cname == "TTable") || (cname == "TLed") || (cname == "TLinearGauge") || (cname == "TCircularGauge") || (cname == "TPlot") || (cname == "TApplyNumeric") || (cname == "TPushButton") || (cname == "TCheckBox") || cname == "TPixmap" || (cname == "TNumeric") || (cname == "TReaderWriter") || (cname == "TPlotLightMarker")  || (cname == "TRealtimePlot")||  (cname == "TComboBox") ||  (cname == "TLineEdit")
            ||   (cname == "TDoubleSpinBox") ||(cname == "TSpinBox") ||(cname == "TReaderWriter")
            || (cname == "TSpectrumButton") || (cname == "TWidgetGroup") ||
            (cname == "TPropertyLabel") || cname == "TImageWidget")
        list.append(d_editTangoAction);
    if ((cname == "TSimpleLabel") || (cname == "TLabel") || (cname == "TTable") || (cname == "TLed"))
        list.append(d_editAction);
    return list;
}

QAction *TaskMenuExtension::preferredEditAction() const
{
    return d_editTangoAction;
}

void TaskMenuExtension::setupSourceTargetDialog(QWidget *cb_widget)
{
    QString source = cb_widget->property("source").toString();
    QString targets = cb_widget->property("targets").toString();

    QDialog *w = new QDialog();
    //	QDialog *wt = new QDialog();

    QGridLayout *grid = new QGridLayout(w);
    QTabWidget *tw = new QTabWidget(w);

    EditSourceDialog *wins = new EditSourceDialog(0);
    EditTargetDialog *wint = new EditTargetDialog(0);
    tw->addTab(wins, "Source");
    tw->addTab(wint, "Target");
    tw->setCurrentIndex(0);

    /* Creating source dialog */
    wins->ui.okButton->setHidden(true);
    wins->ui.cancelButton->setHidden(true);
    wins->ui.lineEdit->setText(source);

    /* Creating target dialog */
    wint->ui.okButton->setHidden(true);
    wint->ui.cancelButton->setHidden(true);
    wint->ui.listWidget->addItems(targets.split(";",QString::SkipEmptyParts));

    grid->setRowStretch(0, 2);
    grid->addWidget(tw, 0, 0, 1, 2);

    QPushButton *okb = new QPushButton("OK", w);
    QPushButton *cancb = new QPushButton("Cancel", w);
    grid->addWidget(okb, 1, 0);
    grid->addWidget(cancb, 1, 1);
    connect(okb, SIGNAL(clicked() ), w, SLOT(accept() ) );
    connect(cancb, SIGNAL(clicked() ), w, SLOT(reject() ) );

    if (w->exec() == QDialog::Accepted)
    {
        QDesignerFormWindowInterface *formWindow = 0;
        formWindow = QDesignerFormWindowInterface::findFormWindow(d_widget);
        formWindow->cursor()->setProperty("source", wins->ui.lineEdit->text());

        QString targets;
        foreach (QListWidgetItem *it, wint->ui.listWidget->findItems("*", Qt::MatchWildcard))
        {
            targets.append(it->text());
            targets.append(";");
        }
        formWindow->cursor()->setProperty("targets", targets);
    }
    else
        qDebug() << "Caso non accettato!";

    delete wins;
    delete wint;
    delete w;
}

void TaskMenuExtension::editTango()
{
    QString stringaIniziale;
    bool edit_source = true;
    /* beware: TLabel after TReaderWriter because TReaderWriter IS a TLabel! */
    if (TTable *tdv = qobject_cast<TTable*>(d_widget))
        stringaIniziale = tdv->source();
    else if (TLed *tdv = qobject_cast<TLed*>(d_widget))
        stringaIniziale = tdv->source();
    else if (TLinearGauge *tdv = qobject_cast<TLinearGauge*>(d_widget))
        stringaIniziale = tdv->source();
    else if (TCircularGauge *tdv = qobject_cast<TCircularGauge*>(d_widget))
        stringaIniziale = tdv->source();
    else if (TPixmap *tdv = qobject_cast<TPixmap*>(d_widget))
        stringaIniziale = tdv->source();
    else if (qobject_cast<TCheckBox*>(d_widget))
    {
        setupSourceTargetDialog(d_widget);
        edit_source = false;
        return;
    }
    else if(qobject_cast<TReaderWriter *>(d_widget))
    {
        setupSourceTargetDialog(d_widget);
        edit_source = false;
        return;
    }
    /* TLabel after TReaderWriter! */
    else if (TLabel *tdv = qobject_cast<TLabel*>(d_widget))
        stringaIniziale = tdv->source();
    else if(TPropertyLabel *pv = qobject_cast<TPropertyLabel *>(d_widget))
        stringaIniziale = pv->source();
    else if (TSimpleLabel *tsl = qobject_cast<TSimpleLabel*>(d_widget))
        stringaIniziale = tsl->source();
    else if (TPushButton *tdv = qobject_cast<TPushButton*>(d_widget))
    {
        stringaIniziale = tdv->targets();
        qDebug() << "target restituiti da TPushButton: " << stringaIniziale;
        edit_source = false;
    }

    else if (TApplyNumeric *tdv = qobject_cast<TApplyNumeric*>(d_widget))
    {
        stringaIniziale = tdv->targets();
        edit_source = false;
    }
    else if (TNumeric *tdv = qobject_cast<TNumeric*>(d_widget))
    {
        stringaIniziale = tdv->targets();
        edit_source = false;
    }
    else if (TLineEdit *tle = qobject_cast<TLineEdit*>(d_widget))
    {
        edit_source = false;
        stringaIniziale = tle->targets();
    }
    else if (TSpinBox *tle = qobject_cast<TSpinBox*>(d_widget))
    {
        stringaIniziale = tle->targets();
        edit_source = false;
    }
    else if (TDoubleSpinBox *tdsb = qobject_cast<TDoubleSpinBox*>(d_widget))
    {
        stringaIniziale = tdsb->targets();
        edit_source = false;
    }
    else if(TSpectrumButton *tsb = qobject_cast<TSpectrumButton *>(d_widget))
    {
        stringaIniziale = tsb->targets();
        edit_source = false;
    }
    /* before TPlotLightMarker */
    else if (TRealtimePlot *rtp = qobject_cast<TRealtimePlot*>(d_widget))
        stringaIniziale = rtp->sources();
    else if (TPlotLightMarker *tdv = qobject_cast<TPlotLightMarker*>(d_widget))
        stringaIniziale = tdv->sources();
    else if(TComboBox *tcb = qobject_cast<TComboBox *>(d_widget))
        stringaIniziale = tcb->source();
    else if(TWidgetGroup *twg = qobject_cast<TWidgetGroup *>(d_widget))
        stringaIniziale = twg->source();
    else if (TImageWidget *tiw = qobject_cast<TImageWidget*>(d_widget))
        stringaIniziale = tiw->source();
    else
        return;

    if (edit_source)
    {
        if (editSourceDialog == 0)
            editSourceDialog = new EditSourceDialog();

        editSourceDialog->ui.lineEdit->setText(stringaIniziale);
        if (editSourceDialog->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(d_widget);
            formWindow->cursor()->setProperty("source", editSourceDialog->ui.lineEdit->text());
        }
    }
    else
    {
        EditTargetDialog *d = new EditTargetDialog();
        qDebug() << "stringa iniziale (prima di split): " << stringaIniziale;
        qDebug() << "listWidget: aggiungo: " << stringaIniziale.split(';');
        d->ui.listWidget->addItems(stringaIniziale.split(';'));
        if (d->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(d_widget);
            QString targets;
            foreach (QListWidgetItem *it, d->ui.listWidget->findItems("*", Qt::MatchWildcard))
            {
                targets.append(it->text() + ";");
            }
            qDebug() << "targets sarebbero:" << targets;
            qDebug() << "formWindow:" << formWindow;
            formWindow->cursor()->setProperty("targets", targets);
        }
    }
}

void TaskMenuExtension::editAttributes()
{
    if (TLabel *label = qobject_cast<TLabel*>(d_widget))
    {
        TLabelBoolEditor *w = new TLabelBoolEditor(label->trueString(), label->falseString(), label->trueColor(), label->falseColor());

        if (w->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(label);
            formWindow->cursor()->setProperty("trueString", w->ui.lineEditTrue->text());
            formWindow->cursor()->setProperty("falseString", w->ui.lineEditFalse->text());
            formWindow->cursor()->setProperty("trueColor", w->ui.pushColorTrue->palette().color(QPalette::Button));
            formWindow->cursor()->setProperty("falseColor", w->ui.pushColorFalse->palette().color(QPalette::Button));
        }
    }
    else if (TTable *table = qobject_cast<TTable*>(d_widget))
    {
        TTableBoolEditor *w = new TTableBoolEditor(table->readNumRows(), table->readNumColumns(), table->trueStrings(), table->falseStrings(), table->trueColors(), table->falseColors(), table->getDisplayMask());

        if (w->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(table);
            formWindow->cursor()->setProperty("numRows", w->ui.eFlagTrue->readNumRows());
            formWindow->cursor()->setProperty("numColumns", w->ui.eFlagTrue->readNumColumns());
            formWindow->cursor()->setProperty("trueStrings", w->ui.eFlagTrue->trueStrings());
            formWindow->cursor()->setProperty("falseStrings", w->ui.eFlagTrue->falseStrings());
            formWindow->cursor()->setProperty("trueColors", w->ui.eFlagTrue->trueColors());
            formWindow->cursor()->setProperty("falseColors", w->ui.eFlagTrue->falseColors());
            formWindow->cursor()->setProperty("displayMask", w->ui.lineMask->text());
        }
    }
    else if (TLed *led = qobject_cast<TLed*>(d_widget))
    {
        TLedBoolEditor *w = new TLedBoolEditor(led->trueColor(), led->falseColor());
        if (w->exec() == QDialog::Accepted)
        {
            QDesignerFormWindowInterface *formWindow = 0;
            formWindow = QDesignerFormWindowInterface::findFormWindow(led);
            formWindow->cursor()->setProperty("trueColor", w->ui.tLedTrue->trueColor());
            formWindow->cursor()->setProperty("falseColor", w->ui.tLedFalse->falseColor());
        }
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(QtControls, CustomWidgetCollectionInterface)
#endif

