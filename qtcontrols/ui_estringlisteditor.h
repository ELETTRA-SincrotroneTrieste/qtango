/*
*********************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*********************************************************************
*/

/********************************************************************************
** Form generated from reading UI file 'estringlisteditor.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ESTRINGLISTEDITOR_H
#define UI_ESTRINGLISTEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_StringListEditor
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QLabel *textLabel;
    QLineEdit *itemTextLineEdit;
    QToolButton *tdbBrowserButton;
    QListWidget *listWidget;
    QHBoxLayout *hboxLayout1;
    QToolButton *newItemButton;
    QToolButton *deleteItemButton;
    QSpacerItem *spacerItem1;
    QToolButton *moveItemUpButton;
    QToolButton *moveItemDownButton;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem2;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *StringListEditor)
    {
        if (StringListEditor->objectName().isEmpty())
            StringListEditor->setObjectName(QStringLiteral("StringListEditor"));
        StringListEditor->resize(340, 237);
        vboxLayout = new QVBoxLayout(StringListEditor);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        groupBox = new QGroupBox(StringListEditor);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(0);
#ifndef Q_OS_MAC
        hboxLayout->setContentsMargins(0, 0, 0, 0);
#endif
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        textLabel = new QLabel(groupBox);
        textLabel->setObjectName(QStringLiteral("textLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textLabel->sizePolicy().hasHeightForWidth());
        textLabel->setSizePolicy(sizePolicy);

        hboxLayout->addWidget(textLabel);

        itemTextLineEdit = new QLineEdit(groupBox);
        itemTextLineEdit->setObjectName(QStringLiteral("itemTextLineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(itemTextLineEdit->sizePolicy().hasHeightForWidth());
        itemTextLineEdit->setSizePolicy(sizePolicy1);

        hboxLayout->addWidget(itemTextLineEdit);

        tdbBrowserButton = new QToolButton(groupBox);
        tdbBrowserButton->setObjectName(QStringLiteral("tdbBrowserButton"));

        hboxLayout->addWidget(tdbBrowserButton);


        gridLayout->addLayout(hboxLayout, 2, 0, 1, 1);

        listWidget = new QListWidget(groupBox);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        gridLayout->addWidget(listWidget, 0, 0, 1, 2);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
        newItemButton = new QToolButton(groupBox);
        newItemButton->setObjectName(QStringLiteral("newItemButton"));

        hboxLayout1->addWidget(newItemButton);

        deleteItemButton = new QToolButton(groupBox);
        deleteItemButton->setObjectName(QStringLiteral("deleteItemButton"));

        hboxLayout1->addWidget(deleteItemButton);

        spacerItem1 = new QSpacerItem(16, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);


        gridLayout->addLayout(hboxLayout1, 1, 0, 1, 1);

        moveItemUpButton = new QToolButton(groupBox);
        moveItemUpButton->setObjectName(QStringLiteral("moveItemUpButton"));

        gridLayout->addWidget(moveItemUpButton, 1, 1, 1, 1);

        moveItemDownButton = new QToolButton(groupBox);
        moveItemDownButton->setObjectName(QStringLiteral("moveItemDownButton"));

        gridLayout->addWidget(moveItemDownButton, 2, 1, 1, 1);


        vboxLayout->addWidget(groupBox);

        hboxLayout2 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout2->setSpacing(6);
#endif
        hboxLayout2->setContentsMargins(0, 0, 0, 0);
        hboxLayout2->setObjectName(QStringLiteral("hboxLayout2"));
        spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem2);

        okButton = new QPushButton(StringListEditor);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout2->addWidget(okButton);

        cancelButton = new QPushButton(StringListEditor);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout2->addWidget(cancelButton);


        vboxLayout->addLayout(hboxLayout2);

#ifndef QT_NO_SHORTCUT
        textLabel->setBuddy(itemTextLineEdit);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(listWidget, newItemButton);
        QWidget::setTabOrder(newItemButton, deleteItemButton);
        QWidget::setTabOrder(deleteItemButton, moveItemUpButton);
        QWidget::setTabOrder(moveItemUpButton, moveItemDownButton);
        QWidget::setTabOrder(moveItemDownButton, itemTextLineEdit);
        QWidget::setTabOrder(itemTextLineEdit, okButton);
        QWidget::setTabOrder(okButton, cancelButton);

        retranslateUi(StringListEditor);
        QObject::connect(okButton, SIGNAL(clicked()), StringListEditor, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), StringListEditor, SLOT(reject()));

        QMetaObject::connectSlotsByName(StringListEditor);
    } // setupUi

    void retranslateUi(QDialog *StringListEditor)
    {
        StringListEditor->setWindowTitle(QApplication::translate("StringListEditor", "Edit Targets", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("StringListEditor", "Edit String List", Q_NULLPTR));
        textLabel->setText(QApplication::translate("StringListEditor", "&Text   ", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        itemTextLineEdit->setToolTip(QApplication::translate("StringListEditor", "Set Item Text", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        tdbBrowserButton->setText(QApplication::translate("StringListEditor", "...", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        listWidget->setToolTip(QApplication::translate("StringListEditor", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Valid formats are:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for attributes: <span style=\" font-weight:600;\">tango/device/server/attribute</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for commands: <span style=\" font-weight:600;\">tango/device/server-&gt;command</span></p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent"
                        ":0px;\">You can also specify a Tango Database:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for attributes: <span style=\" font-weight:600;\">host:port/tango/device/server/attribute</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">for commands: <span style=\" font-weight:600;\">host:port/tango/device/server-&gt;command</span></p><p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        newItemButton->setToolTip(QApplication::translate("StringListEditor", "New Item", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        newItemButton->setText(QApplication::translate("StringListEditor", "&New Item", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        deleteItemButton->setToolTip(QApplication::translate("StringListEditor", "Delete Item", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        deleteItemButton->setText(QApplication::translate("StringListEditor", "&Delete Item", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        moveItemUpButton->setToolTip(QApplication::translate("StringListEditor", "Move Item Up", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        moveItemUpButton->setText(QApplication::translate("StringListEditor", "U", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        moveItemDownButton->setToolTip(QApplication::translate("StringListEditor", "Move Item Down", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        moveItemDownButton->setText(QApplication::translate("StringListEditor", "D", Q_NULLPTR));
        okButton->setText(QApplication::translate("StringListEditor", "OK", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("StringListEditor", "Cancel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class StringListEditor: public Ui_StringListEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ESTRINGLISTEDITOR_H
