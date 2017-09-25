#include "pointeditor.h"
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QtDebug>

PointEditor::PointEditor(QWidget *parent, const QString &defaultTxt) : QDialog(parent)
{

    QGridLayout *lo = new QGridLayout(this);
    cb = new QComboBox(this);
    le = new QLineEdit(this);
    QLabel *label = new QLabel(this);
    label->setObjectName("textLabel");
    QPushButton *pb = new QPushButton(this);
    QPushButton *pbCancel = new QPushButton(this);
    cbox = new QCheckBox(this);
    cbox->setText("command");
    connect(cbox, SIGNAL(toggled(bool)), this, SLOT(commandSwitch(bool)));
    cb->insertItems(0, QStringList() << "no wildcard" << "$1" << "$2" << "$3" << "$4" << "$5");
    cb->setCurrentIndex(1);

    label->setText("Set the tango point");
    pb->setText("Apply");
    pbCancel->setText("Cancel");

    connect(pb, SIGNAL(clicked()), this, SLOT(accept()));
    connect(pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));

    le->setText(defaultTxt);
    d_savedPoint = d_point = defaultTxt;

    setWindowTitle("Edit tango source or target");
    lo->addWidget(label, 0, 0, 1, 5);
    lo->addWidget(cbox, 1, 0, 1, 1);
    lo->addWidget(cb, 1, 1, 1, 2);
    lo->addWidget(le, 2, 0, 1, 5);
    lo->addWidget(pb, 3, 3, 1, 1);
    lo->addWidget(pbCancel, 3, 4, 1, 1);

    /* by default, place a wildcard (first argument, "$1") */
    comboChanged(1);
}

void PointEditor::accept()
{
    d_point = le->text();
    return QDialog::accept();
}

void PointEditor::comboChanged(int i)
{
    cbox->setEnabled(i != 0);
    if(i > 0)
    {
        QString wildcard = cb->currentText();
        QString point = le->text();
        QStringList parts;
        point.contains("->") ? (parts = point.split("->", QString::SkipEmptyParts)) : (parts = point.split("/", QString::SkipEmptyParts));
        QString entity;
        if(parts.size() > 1)
            entity = parts.last();
        else if(parts.size() == 1)
            entity = parts.first();
        else
            entity = "";

        if(cbox->isChecked()) /* probably the attribute name */
            le->setText(wildcard + "->" + entity);
        else
            le->setText(wildcard + "/" + entity);
    }
    else
        le->setText(d_savedPoint);
}

void PointEditor::commandSwitch(bool cmd)
{
    QString point = le->text();
    qDebug() << "commandSwitch: " << point << cmd;
    if(point.contains("->") && !cmd)
        point.replace("->", "/");
    else if(point.count("/") == 1 && cmd)
        point.replace("/", "->");
    else if(point.count("/") > 1 && cmd)
    {
        int idx = point.lastIndexOf("/");
        point.replace(idx, 1, "->");
    }
    le->setText(point);
}

