/********************************************************************************
** Form generated from reading UI file 'configuration.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGURATION_H
#define UI_CONFIGURATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EPlotConfigurationWidget
{
public:
    QGridLayout *gridLayout_4;
    QGroupBox *gbX;
    QGridLayout *gridLayout;
    QRadioButton *rbXAutoConf;
    QRadioButton *rbXAuto;
    QRadioButton *rbXManual;
    QLabel *labelXLb;
    QLineEdit *xLowerBound;
    QLabel *labelXUb;
    QLineEdit *xUpperBound;
    QDoubleSpinBox *xAutoscaleAdjustment;
    QSpacerItem *horizontalSpacer_3;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QRadioButton *xAxisLinearScale;
    QRadioButton *xAxisLogScale;
    QCheckBox *xAutoscaleAdjustEnabled;
    QGroupBox *gbY;
    QGridLayout *gridLayout_2;
    QRadioButton *rbYAutoConf;
    QRadioButton *rbYAuto;
    QRadioButton *rbYManual;
    QLabel *labelYLb;
    QLineEdit *yLowerBound;
    QLabel *labelYUb;
    QLineEdit *yUpperBound;
    QCheckBox *yAutoscaleAdjustEnabled;
    QDoubleSpinBox *yAutoscaleAdjustment;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QRadioButton *yAxisLinearScale;
    QRadioButton *yAxisLogScale;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *colouredBackground;
    QGroupBox *gboxAppearence;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QComboBox *cbCurves;
    QPushButton *curveColor;
    QLabel *label_5;
    QComboBox *cbCurveStyle;
    QLabel *label_6;
    QDoubleSpinBox *sbCurveWidth;
    QLabel *label_4;
    QSpinBox *sbAlpha;
    QCheckBox *cbshowYValuesEnabled;
    QGroupBox *gbDataBufSize;
    QHBoxLayout *horizontalLayout;
    QCheckBox *cbBufferUnlimited;
    QSpinBox *sbBufferSize;
    QCheckBox *setPointEnabled;
    QCheckBox *titleOnCanvasEnabled;
    QPushButton *pbApply;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *canvasPaintCached;
    QCheckBox *canvasPaintPacked;
    QCheckBox *syncRefresh;
    QPushButton *pbRememberAppearanceSettings;

    void setupUi(QWidget *EPlotConfigurationWidget)
    {
        if (EPlotConfigurationWidget->objectName().isEmpty())
            EPlotConfigurationWidget->setObjectName(QStringLiteral("EPlotConfigurationWidget"));
        EPlotConfigurationWidget->resize(535, 829);
        gridLayout_4 = new QGridLayout(EPlotConfigurationWidget);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gbX = new QGroupBox(EPlotConfigurationWidget);
        gbX->setObjectName(QStringLiteral("gbX"));
        gridLayout = new QGridLayout(gbX);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        rbXAutoConf = new QRadioButton(gbX);
        rbXAutoConf->setObjectName(QStringLiteral("rbXAutoConf"));
        rbXAutoConf->setLayoutDirection(Qt::RightToLeft);

        gridLayout->addWidget(rbXAutoConf, 0, 0, 1, 2);

        rbXAuto = new QRadioButton(gbX);
        rbXAuto->setObjectName(QStringLiteral("rbXAuto"));
        rbXAuto->setLayoutDirection(Qt::RightToLeft);

        gridLayout->addWidget(rbXAuto, 0, 2, 1, 1);

        rbXManual = new QRadioButton(gbX);
        rbXManual->setObjectName(QStringLiteral("rbXManual"));
        rbXManual->setLayoutDirection(Qt::RightToLeft);

        gridLayout->addWidget(rbXManual, 0, 3, 1, 1);

        labelXLb = new QLabel(gbX);
        labelXLb->setObjectName(QStringLiteral("labelXLb"));
        labelXLb->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(labelXLb, 1, 0, 1, 1);

        xLowerBound = new QLineEdit(gbX);
        xLowerBound->setObjectName(QStringLiteral("xLowerBound"));

        gridLayout->addWidget(xLowerBound, 1, 1, 1, 1);

        labelXUb = new QLabel(gbX);
        labelXUb->setObjectName(QStringLiteral("labelXUb"));
        labelXUb->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(labelXUb, 1, 2, 1, 1);

        xUpperBound = new QLineEdit(gbX);
        xUpperBound->setObjectName(QStringLiteral("xUpperBound"));

        gridLayout->addWidget(xUpperBound, 1, 3, 1, 1);

        xAutoscaleAdjustment = new QDoubleSpinBox(gbX);
        xAutoscaleAdjustment->setObjectName(QStringLiteral("xAutoscaleAdjustment"));

        gridLayout->addWidget(xAutoscaleAdjustment, 2, 3, 2, 1);

        horizontalSpacer_3 = new QSpacerItem(81, 26, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 4, 0, 1, 1);

        widget_2 = new QWidget(gbX);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_3 = new QHBoxLayout(widget_2);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(widget_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label_2);

        xAxisLinearScale = new QRadioButton(widget_2);
        xAxisLinearScale->setObjectName(QStringLiteral("xAxisLinearScale"));
        xAxisLinearScale->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_3->addWidget(xAxisLinearScale);

        xAxisLogScale = new QRadioButton(widget_2);
        xAxisLogScale->setObjectName(QStringLiteral("xAxisLogScale"));
        xAxisLogScale->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_3->addWidget(xAxisLogScale);


        gridLayout->addWidget(widget_2, 4, 1, 1, 3);

        xAutoscaleAdjustEnabled = new QCheckBox(gbX);
        xAutoscaleAdjustEnabled->setObjectName(QStringLiteral("xAutoscaleAdjustEnabled"));
        xAutoscaleAdjustEnabled->setLayoutDirection(Qt::RightToLeft);

        gridLayout->addWidget(xAutoscaleAdjustEnabled, 3, 2, 1, 1);


        gridLayout_4->addWidget(gbX, 0, 0, 1, 4);

        gbY = new QGroupBox(EPlotConfigurationWidget);
        gbY->setObjectName(QStringLiteral("gbY"));
        gridLayout_2 = new QGridLayout(gbY);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        rbYAutoConf = new QRadioButton(gbY);
        rbYAutoConf->setObjectName(QStringLiteral("rbYAutoConf"));
        rbYAutoConf->setLayoutDirection(Qt::RightToLeft);

        gridLayout_2->addWidget(rbYAutoConf, 0, 0, 1, 2);

        rbYAuto = new QRadioButton(gbY);
        rbYAuto->setObjectName(QStringLiteral("rbYAuto"));
        rbYAuto->setLayoutDirection(Qt::RightToLeft);

        gridLayout_2->addWidget(rbYAuto, 0, 2, 1, 1);

        rbYManual = new QRadioButton(gbY);
        rbYManual->setObjectName(QStringLiteral("rbYManual"));
        rbYManual->setLayoutDirection(Qt::RightToLeft);

        gridLayout_2->addWidget(rbYManual, 0, 3, 1, 1);

        labelYLb = new QLabel(gbY);
        labelYLb->setObjectName(QStringLiteral("labelYLb"));
        labelYLb->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(labelYLb, 1, 0, 1, 1);

        yLowerBound = new QLineEdit(gbY);
        yLowerBound->setObjectName(QStringLiteral("yLowerBound"));

        gridLayout_2->addWidget(yLowerBound, 1, 1, 1, 1);

        labelYUb = new QLabel(gbY);
        labelYUb->setObjectName(QStringLiteral("labelYUb"));
        labelYUb->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(labelYUb, 1, 2, 1, 1);

        yUpperBound = new QLineEdit(gbY);
        yUpperBound->setObjectName(QStringLiteral("yUpperBound"));

        gridLayout_2->addWidget(yUpperBound, 1, 3, 1, 1);

        yAutoscaleAdjustEnabled = new QCheckBox(gbY);
        yAutoscaleAdjustEnabled->setObjectName(QStringLiteral("yAutoscaleAdjustEnabled"));
        yAutoscaleAdjustEnabled->setLayoutDirection(Qt::RightToLeft);

        gridLayout_2->addWidget(yAutoscaleAdjustEnabled, 3, 1, 1, 2);

        yAutoscaleAdjustment = new QDoubleSpinBox(gbY);
        yAutoscaleAdjustment->setObjectName(QStringLiteral("yAutoscaleAdjustment"));

        gridLayout_2->addWidget(yAutoscaleAdjustment, 3, 3, 1, 1);

        widget = new QWidget(gbY);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_3);

        yAxisLinearScale = new QRadioButton(widget);
        yAxisLinearScale->setObjectName(QStringLiteral("yAxisLinearScale"));
        yAxisLinearScale->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_2->addWidget(yAxisLinearScale);

        yAxisLogScale = new QRadioButton(widget);
        yAxisLogScale->setObjectName(QStringLiteral("yAxisLogScale"));
        yAxisLogScale->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_2->addWidget(yAxisLogScale);


        gridLayout_2->addWidget(widget, 5, 1, 1, 3);

        horizontalSpacer_2 = new QSpacerItem(81, 26, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 5, 0, 1, 1);

        colouredBackground = new QCheckBox(gbY);
        colouredBackground->setObjectName(QStringLiteral("colouredBackground"));
        colouredBackground->setLayoutDirection(Qt::RightToLeft);

        gridLayout_2->addWidget(colouredBackground, 4, 0, 1, 4);


        gridLayout_4->addWidget(gbY, 1, 0, 1, 4);

        gboxAppearence = new QGroupBox(EPlotConfigurationWidget);
        gboxAppearence->setObjectName(QStringLiteral("gboxAppearence"));
        gridLayout_3 = new QGridLayout(gboxAppearence);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label = new QLabel(gboxAppearence);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        cbCurves = new QComboBox(gboxAppearence);
        cbCurves->setObjectName(QStringLiteral("cbCurves"));

        gridLayout_3->addWidget(cbCurves, 0, 1, 1, 1);

        curveColor = new QPushButton(gboxAppearence);
        curveColor->setObjectName(QStringLiteral("curveColor"));

        gridLayout_3->addWidget(curveColor, 0, 2, 1, 1);

        label_5 = new QLabel(gboxAppearence);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_5, 2, 1, 1, 1);

        cbCurveStyle = new QComboBox(gboxAppearence);
        cbCurveStyle->setObjectName(QStringLiteral("cbCurveStyle"));

        gridLayout_3->addWidget(cbCurveStyle, 2, 2, 1, 1);

        label_6 = new QLabel(gboxAppearence);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_6, 3, 1, 1, 1);

        sbCurveWidth = new QDoubleSpinBox(gboxAppearence);
        sbCurveWidth->setObjectName(QStringLiteral("sbCurveWidth"));
        sbCurveWidth->setValue(1);

        gridLayout_3->addWidget(sbCurveWidth, 3, 2, 1, 1);

        label_4 = new QLabel(gboxAppearence);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_4, 1, 1, 1, 1);

        sbAlpha = new QSpinBox(gboxAppearence);
        sbAlpha->setObjectName(QStringLiteral("sbAlpha"));
        sbAlpha->setMaximum(255);
        sbAlpha->setValue(230);

        gridLayout_3->addWidget(sbAlpha, 1, 2, 1, 1);

        cbshowYValuesEnabled = new QCheckBox(gboxAppearence);
        cbshowYValuesEnabled->setObjectName(QStringLiteral("cbshowYValuesEnabled"));
        cbshowYValuesEnabled->setLayoutDirection(Qt::RightToLeft);

        gridLayout_3->addWidget(cbshowYValuesEnabled, 4, 1, 1, 2);


        gridLayout_4->addWidget(gboxAppearence, 2, 0, 1, 4);

        gbDataBufSize = new QGroupBox(EPlotConfigurationWidget);
        gbDataBufSize->setObjectName(QStringLiteral("gbDataBufSize"));
        horizontalLayout = new QHBoxLayout(gbDataBufSize);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        cbBufferUnlimited = new QCheckBox(gbDataBufSize);
        cbBufferUnlimited->setObjectName(QStringLiteral("cbBufferUnlimited"));
        cbBufferUnlimited->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout->addWidget(cbBufferUnlimited);

        sbBufferSize = new QSpinBox(gbDataBufSize);
        sbBufferSize->setObjectName(QStringLiteral("sbBufferSize"));
        sbBufferSize->setMinimum(5);
        sbBufferSize->setMaximum(999999999);
        sbBufferSize->setValue(600);

        horizontalLayout->addWidget(sbBufferSize);


        gridLayout_4->addWidget(gbDataBufSize, 3, 0, 1, 1);

        setPointEnabled = new QCheckBox(EPlotConfigurationWidget);
        setPointEnabled->setObjectName(QStringLiteral("setPointEnabled"));
        setPointEnabled->setLayoutDirection(Qt::RightToLeft);

        gridLayout_4->addWidget(setPointEnabled, 4, 1, 1, 3);

        titleOnCanvasEnabled = new QCheckBox(EPlotConfigurationWidget);
        titleOnCanvasEnabled->setObjectName(QStringLiteral("titleOnCanvasEnabled"));
        titleOnCanvasEnabled->setLayoutDirection(Qt::RightToLeft);

        gridLayout_4->addWidget(titleOnCanvasEnabled, 5, 1, 1, 3);

        pbApply = new QPushButton(EPlotConfigurationWidget);
        pbApply->setObjectName(QStringLiteral("pbApply"));

        gridLayout_4->addWidget(pbApply, 8, 3, 1, 1);

        groupBox = new QGroupBox(EPlotConfigurationWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        canvasPaintCached = new QCheckBox(groupBox);
        canvasPaintCached->setObjectName(QStringLiteral("canvasPaintCached"));
        canvasPaintCached->setLayoutDirection(Qt::RightToLeft);

        verticalLayout->addWidget(canvasPaintCached);

        canvasPaintPacked = new QCheckBox(groupBox);
        canvasPaintPacked->setObjectName(QStringLiteral("canvasPaintPacked"));
        canvasPaintPacked->setLayoutDirection(Qt::RightToLeft);

        verticalLayout->addWidget(canvasPaintPacked);

        syncRefresh = new QCheckBox(groupBox);
        syncRefresh->setObjectName(QStringLiteral("syncRefresh"));
        syncRefresh->setLayoutDirection(Qt::RightToLeft);

        verticalLayout->addWidget(syncRefresh);


        gridLayout_4->addWidget(groupBox, 6, 0, 1, 4);

        pbRememberAppearanceSettings = new QPushButton(EPlotConfigurationWidget);
        pbRememberAppearanceSettings->setObjectName(QStringLiteral("pbRememberAppearanceSettings"));

        gridLayout_4->addWidget(pbRememberAppearanceSettings, 8, 2, 1, 1);


        retranslateUi(EPlotConfigurationWidget);

        QMetaObject::connectSlotsByName(EPlotConfigurationWidget);
    } // setupUi

    void retranslateUi(QWidget *EPlotConfigurationWidget)
    {
        EPlotConfigurationWidget->setWindowTitle(QApplication::translate("EPlotConfigurationWidget", "Plot Configuration", Q_NULLPTR));
        gbX->setTitle(QApplication::translate("EPlotConfigurationWidget", "X scale adjustment", Q_NULLPTR));
        rbXAutoConf->setText(QApplication::translate("EPlotConfigurationWidget", "from auto configuration", Q_NULLPTR));
        rbXAuto->setText(QApplication::translate("EPlotConfigurationWidget", "auto", Q_NULLPTR));
        rbXManual->setText(QApplication::translate("EPlotConfigurationWidget", "manual", Q_NULLPTR));
        labelXLb->setText(QApplication::translate("EPlotConfigurationWidget", "Lower Bound:", Q_NULLPTR));
        labelXUb->setText(QApplication::translate("EPlotConfigurationWidget", "Upper Bound:", Q_NULLPTR));
        xAutoscaleAdjustment->setSuffix(QApplication::translate("EPlotConfigurationWidget", "%", Q_NULLPTR));
        label_2->setText(QApplication::translate("EPlotConfigurationWidget", "Scale engine:", Q_NULLPTR));
        xAxisLinearScale->setText(QApplication::translate("EPlotConfigurationWidget", "Linear", Q_NULLPTR));
        xAxisLogScale->setText(QApplication::translate("EPlotConfigurationWidget", "Logarithmic", Q_NULLPTR));
        xAutoscaleAdjustEnabled->setText(QApplication::translate("EPlotConfigurationWidget", "Adjustment", Q_NULLPTR));
        gbY->setTitle(QApplication::translate("EPlotConfigurationWidget", "Y scale adjustment", Q_NULLPTR));
        rbYAutoConf->setText(QApplication::translate("EPlotConfigurationWidget", "from auto configuration", Q_NULLPTR));
        rbYAuto->setText(QApplication::translate("EPlotConfigurationWidget", "auto", Q_NULLPTR));
        rbYManual->setText(QApplication::translate("EPlotConfigurationWidget", "manual", Q_NULLPTR));
        labelYLb->setText(QApplication::translate("EPlotConfigurationWidget", "Lower Bound:", Q_NULLPTR));
        labelYUb->setText(QApplication::translate("EPlotConfigurationWidget", "Upper Bound:", Q_NULLPTR));
        yAutoscaleAdjustEnabled->setText(QApplication::translate("EPlotConfigurationWidget", "Adjustment", Q_NULLPTR));
        yAutoscaleAdjustment->setSuffix(QApplication::translate("EPlotConfigurationWidget", "%", Q_NULLPTR));
        label_3->setText(QApplication::translate("EPlotConfigurationWidget", "Scale engine:", Q_NULLPTR));
        yAxisLinearScale->setText(QApplication::translate("EPlotConfigurationWidget", "Linear", Q_NULLPTR));
        yAxisLogScale->setText(QApplication::translate("EPlotConfigurationWidget", "Logarithmic", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        colouredBackground->setToolTip(QApplication::translate("EPlotConfigurationWidget", "If the plot is reading an attribute, and the attribute\n"
"has been configured with minimum, maximum, warning\n"
"and alarm thresholds, then it is possible to colour \n"
"the background of the plot according to those \n"
"thresholds.", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        colouredBackground->setText(QApplication::translate("EPlotConfigurationWidget", "Coloured Background according to warn and alarm thresholds", Q_NULLPTR));
        gboxAppearence->setTitle(QApplication::translate("EPlotConfigurationWidget", "Curves Appearance", Q_NULLPTR));
        label->setText(QApplication::translate("EPlotConfigurationWidget", "Curve:", Q_NULLPTR));
        curveColor->setText(QApplication::translate("EPlotConfigurationWidget", "Color", Q_NULLPTR));
        label_5->setText(QApplication::translate("EPlotConfigurationWidget", "Style:", Q_NULLPTR));
        cbCurveStyle->clear();
        cbCurveStyle->insertItems(0, QStringList()
         << QApplication::translate("EPlotConfigurationWidget", "No Curve", Q_NULLPTR)
         << QApplication::translate("EPlotConfigurationWidget", "Lines", Q_NULLPTR)
         << QApplication::translate("EPlotConfigurationWidget", "Sticks", Q_NULLPTR)
         << QApplication::translate("EPlotConfigurationWidget", "Steps", Q_NULLPTR)
         << QApplication::translate("EPlotConfigurationWidget", "Dots", Q_NULLPTR)
        );
        label_6->setText(QApplication::translate("EPlotConfigurationWidget", "Width:", Q_NULLPTR));
        label_4->setText(QApplication::translate("EPlotConfigurationWidget", "Alpha Channel:", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        cbshowYValuesEnabled->setToolTip(QApplication::translate("EPlotConfigurationWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">If this box is checked, then the associated curve will display the value of the Y coordinate beside the curve point. This might be useful when the curve represents</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a little amount of data.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        cbshowYValuesEnabled->setText(QApplication::translate("EPlotConfigurationWidget", "Display Y Values on the curve", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        gbDataBufSize->setToolTip(QApplication::translate("EPlotConfigurationWidget", "Change the number of desired points on the X axis", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        gbDataBufSize->setTitle(QApplication::translate("EPlotConfigurationWidget", "X Axis Data Buffer Size", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        cbBufferUnlimited->setToolTip(QApplication::translate("EPlotConfigurationWidget", "Values are never removed from the X axis", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        cbBufferUnlimited->setText(QApplication::translate("EPlotConfigurationWidget", "Unlimited", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        setPointEnabled->setToolTip(QApplication::translate("EPlotConfigurationWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">If the plot is reading Tango READ WRITE attributes,</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">then you can toggle the <span style=\" font-weight:600;\">set point</span> visibility on or </p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">off with this check box. This does not apply to READ</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; mar"
                        "gin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">ONLY attributes or commands.</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">The changes have effect on the next replot.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        setPointEnabled->setText(QApplication::translate("EPlotConfigurationWidget", "Set Points visible for read/write attributes", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        titleOnCanvasEnabled->setToolTip(QApplication::translate("EPlotConfigurationWidget", "Shows the name of the curves on the plot canvas if the box is checked.", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        titleOnCanvasEnabled->setText(QApplication::translate("EPlotConfigurationWidget", "Show the curve names on the plot canvas", Q_NULLPTR));
        pbApply->setText(QApplication::translate("EPlotConfigurationWidget", "Apply Changes", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("EPlotConfigurationWidget", "Advanced Performance Settings", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        canvasPaintCached->setToolTip(QApplication::translate("EPlotConfigurationWidget", "This is an advanced option: you should not need to change this value.\n"
"It may affect plot performance.", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        canvasPaintCached->setText(QApplication::translate("EPlotConfigurationWidget", "Enable Paint Cached Property on Canvas", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        canvasPaintPacked->setToolTip(QApplication::translate("EPlotConfigurationWidget", "This is an advanced option: you should not need to change this value.\n"
"It may affect plot performance.", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        canvasPaintPacked->setText(QApplication::translate("EPlotConfigurationWidget", "Enable Paint Packed Property on Canvas", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        syncRefresh->setToolTip(QApplication::translate("EPlotConfigurationWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:7pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">The refresh of the curves is performed on the trigger of a </p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">timer. This improves performance for plots with more than</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one curve and with high refresh rate or a big amount of points</p>\n"
"<p align=\"justify\" style=\" margin-top:0px; margin-bottom:0px; margin-"
                        "left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">on the curves.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        syncRefresh->setText(QApplication::translate("EPlotConfigurationWidget", "Enable Sync Refresh", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        pbRememberAppearanceSettings->setToolTip(QApplication::translate("EPlotConfigurationWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Clicking this button will save the <span style=\" font-weight:600;\">Curves Appearance</span> properties.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Note:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">the sav"
                        "ed properties for the curves are:</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">- <span style=\" font-weight:600;\">colour</span> (included alpha channel);</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">- <span style=\" font-weight:600;\">style</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">- <span style=\" font-weight:600; font-style:italic;\">pen width</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">- the<span style=\" font-weight:600;\"> Display Y Values</span><span style=\" font-style:italic;\"> </span>property.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>\n"
""
                        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">The configuration is saved in the user wide system settings.</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        pbRememberAppearanceSettings->setText(QApplication::translate("EPlotConfigurationWidget", "Save Curves Appearence", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EPlotConfigurationWidget: public Ui_EPlotConfigurationWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGURATION_H
