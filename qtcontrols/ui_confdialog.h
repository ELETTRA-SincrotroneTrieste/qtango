/********************************************************************************
** Form generated from reading UI file 'confdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFDIALOG_H
#define UI_CONFDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ConfDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *palette_groupBox;
    QGridLayout *gridLayout_3;
    QRadioButton *bw_radioButton;
    QLabel *palette_BW_label;
    QRadioButton *c_radioButton;
    QLabel *palette_C_label;
    QRadioButton *jet_radioButton;
    QLabel *palette_jet_label;
    QRadioButton *hot_radioButton;
    QLabel *palette_hot_label;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ConfDialog)
    {
        if (ConfDialog->objectName().isEmpty())
            ConfDialog->setObjectName(QStringLiteral("ConfDialog"));
        ConfDialog->resize(399, 300);
        verticalLayout = new QVBoxLayout(ConfDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        palette_groupBox = new QGroupBox(ConfDialog);
        palette_groupBox->setObjectName(QStringLiteral("palette_groupBox"));
        gridLayout_3 = new QGridLayout(palette_groupBox);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        bw_radioButton = new QRadioButton(palette_groupBox);
        bw_radioButton->setObjectName(QStringLiteral("bw_radioButton"));
        bw_radioButton->setLayoutDirection(Qt::RightToLeft);

        gridLayout_3->addWidget(bw_radioButton, 0, 0, 1, 1);

        palette_BW_label = new QLabel(palette_groupBox);
        palette_BW_label->setObjectName(QStringLiteral("palette_BW_label"));
        palette_BW_label->setFrameShape(QFrame::StyledPanel);
        palette_BW_label->setFrameShadow(QFrame::Plain);
        palette_BW_label->setPixmap(QPixmap(QString::fromUtf8(":/icons/paletteBW.png")));
        palette_BW_label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(palette_BW_label, 0, 1, 1, 1);

        c_radioButton = new QRadioButton(palette_groupBox);
        c_radioButton->setObjectName(QStringLiteral("c_radioButton"));
        c_radioButton->setLayoutDirection(Qt::RightToLeft);
        c_radioButton->setChecked(true);

        gridLayout_3->addWidget(c_radioButton, 1, 0, 1, 1);

        palette_C_label = new QLabel(palette_groupBox);
        palette_C_label->setObjectName(QStringLiteral("palette_C_label"));
        palette_C_label->setFrameShape(QFrame::StyledPanel);
        palette_C_label->setPixmap(QPixmap(QString::fromUtf8(":/icons/paletteC.png")));

        gridLayout_3->addWidget(palette_C_label, 1, 1, 1, 1);

        jet_radioButton = new QRadioButton(palette_groupBox);
        jet_radioButton->setObjectName(QStringLiteral("jet_radioButton"));
        jet_radioButton->setLayoutDirection(Qt::RightToLeft);

        gridLayout_3->addWidget(jet_radioButton, 2, 0, 1, 1);

        palette_jet_label = new QLabel(palette_groupBox);
        palette_jet_label->setObjectName(QStringLiteral("palette_jet_label"));
        palette_jet_label->setFrameShape(QFrame::StyledPanel);
        palette_jet_label->setPixmap(QPixmap(QString::fromUtf8(":/icons/paletteJet.png")));

        gridLayout_3->addWidget(palette_jet_label, 2, 1, 1, 1);

        hot_radioButton = new QRadioButton(palette_groupBox);
        hot_radioButton->setObjectName(QStringLiteral("hot_radioButton"));
        hot_radioButton->setLayoutDirection(Qt::RightToLeft);

        gridLayout_3->addWidget(hot_radioButton, 3, 0, 1, 1);

        palette_hot_label = new QLabel(palette_groupBox);
        palette_hot_label->setObjectName(QStringLiteral("palette_hot_label"));
        palette_hot_label->setFrameShape(QFrame::StyledPanel);
        palette_hot_label->setPixmap(QPixmap(QString::fromUtf8(":/icons/paletteHot.png")));

        gridLayout_3->addWidget(palette_hot_label, 3, 1, 1, 1);


        verticalLayout->addWidget(palette_groupBox);

        buttonBox = new QDialogButtonBox(ConfDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Close);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(ConfDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ConfDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ConfDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ConfDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfDialog)
    {
        ConfDialog->setWindowTitle(QApplication::translate("ConfDialog", "Dialog", Q_NULLPTR));
        palette_groupBox->setTitle(QApplication::translate("ConfDialog", " palette ", Q_NULLPTR));
        bw_radioButton->setText(QApplication::translate("ConfDialog", "BW", Q_NULLPTR));
        bw_radioButton->setProperty("paletteName", QVariant(QApplication::translate("ConfDialog", "bw", Q_NULLPTR)));
        palette_BW_label->setText(QString());
        c_radioButton->setText(QApplication::translate("ConfDialog", "&C", Q_NULLPTR));
        c_radioButton->setProperty("paletteName", QVariant(QApplication::translate("ConfDialog", "c", Q_NULLPTR)));
        palette_C_label->setText(QString());
        jet_radioButton->setText(QApplication::translate("ConfDialog", "&Jet", Q_NULLPTR));
        jet_radioButton->setProperty("paletteName", QVariant(QApplication::translate("ConfDialog", "jet", Q_NULLPTR)));
        palette_jet_label->setText(QString());
        hot_radioButton->setText(QApplication::translate("ConfDialog", "Ho&t", Q_NULLPTR));
        hot_radioButton->setProperty("paletteName", QVariant(QApplication::translate("ConfDialog", "hot", Q_NULLPTR)));
        palette_hot_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ConfDialog: public Ui_ConfDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFDIALOG_H
