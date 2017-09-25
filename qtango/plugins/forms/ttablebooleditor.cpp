#include "ttablebooleditor.h"
#include <QColorDialog>
#include <macros.h>
#include <QMetaObject>
#include <QMetaProperty>
#include <QtDebug>

TTableBoolEditor::TTableBoolEditor(int rows, int columns, QString ts, QString fs, QString tc, QString fc, QString dm, QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	ui.spinNumRows->setValue(rows);
	ui.spinNumColumns->setValue(columns);

	ui.lineEditTrue->setText(ts.split(";")[0]);
	ui.lineEditFalse->setText(fs.split(";")[0]);

	QPalette p;
	p = ui.pushColorTrue->palette();
	QColor c;
	c.setRgba(tc.split(";")[0].toUInt());
	p.setColor(QPalette::Button, c);
//	p.setColor(QPalette::Button, tc.toList()[0].value<QColor>());
	ui.pushColorTrue->setPalette(p);
	
	p = ui.pushColorFalse->palette();
	c.setRgba(fc.split(";")[0].toUInt());
	p.setColor(QPalette::Button, c);
//	p.setColor(QPalette::Button, fc.toList()[0].value<QColor>());
	ui.pushColorFalse->setPalette(p);

	ui.eFlagTrue->setNumRows(rows);
	ui.eFlagTrue->setNumColumns(columns);
	ui.eFlagTrue->setTrueStrings(ts);
	ui.eFlagTrue->setFalseStrings(fs);
	ui.eFlagTrue->setTrueColors(tc);
	ui.eFlagTrue->setFalseColors(fc);
	
	ui.eFlagFalse->setNumRows(rows);
	ui.eFlagFalse->setNumColumns(columns);
	ui.eFlagFalse->setTrueStrings(ts);
	ui.eFlagFalse->setFalseStrings(fs);
	ui.eFlagFalse->setTrueColors(tc);
	ui.eFlagFalse->setFalseColors(fc);

	ui.lineMask->setText(dm);
	
	connect(ui.spinNumRows, SIGNAL(valueChanged(int)), this, SLOT(updateFlags()));
	connect(ui.spinNumColumns, SIGNAL(valueChanged(int)), this, SLOT(updateFlags()));
//	connect(ui.checkApplyAll, SIGNAL(clicked()), this, SLOT(updateEditor()));
	connect(ui.spinRow, SIGNAL(valueChanged(QString)), this, SLOT(updateLabels()));
	connect(ui.spinColumn, SIGNAL(valueChanged(QString)), this, SLOT(updateLabels()));

	connect(ui.pushApply, SIGNAL(clicked()), this, SLOT(updateFlags()));

//	connect(ui.lineEditTrue, SIGNAL(textChanged(QString)), this, SLOT(updateEditor()));
//	connect(ui.lineEditFalse, SIGNAL(textChanged(QString)), this, SLOT(updateEditor()));
//	connect(ui.lineEditTrue, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(updateEditor()));
//	connect(ui.lineEditFalse, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(updateEditor()));
	connect(ui.pushColorTrue, SIGNAL(clicked()), this, SLOT(updateColorTrue()));
	connect(ui.pushColorFalse, SIGNAL(clicked()), this, SLOT(updateColorFalse()));
	
	updateLabels();
	updateFlags();
}

void TTableBoolEditor::updateFlags()
{
	ui.spinRow->setMaximum(ui.spinNumRows->value()-1);
	ui.spinColumn->setMaximum(ui.spinNumColumns->value()-1);

	if ((ui.eFlagTrue->readNumRows() != ui.spinNumRows->value()) || (ui.eFlagTrue->readNumColumns() != ui.spinNumColumns->value()))
	{
		ui.eFlagTrue->setNumRows(ui.spinNumRows->value());
		ui.eFlagTrue->setNumColumns(ui.spinNumColumns->value());
		
		ui.eFlagFalse->setNumRows(ui.spinNumRows->value());
		ui.eFlagFalse->setNumColumns(ui.spinNumColumns->value());
	}
	
	//ui.eFlagTrue->setUpdatesEnabled(false);
	//ui.eFlagFalse->setUpdatesEnabled(false);

	QList<unsigned int> m;
	QList<QVariant> tv, fv;
	for (int i = 0; i < ui.spinNumRows->value()*ui.spinNumColumns->value(); i++)
	{
		m << i;
		tv << true;
		fv << false;
	};
	ui.eFlagTrue->setDisplayMask(m);
	ui.eFlagTrue->setValue(tv);
	ui.eFlagFalse->setDisplayMask(m);
	ui.eFlagFalse->setValue(fv);


	if (ui.checkApplyAll->isChecked())
	{
		for (int i = 0; i < ui.spinNumRows->value()*ui.spinNumColumns->value(); i++)
		{
			ui.eFlagTrue->setBooleanDisplay(i, ui.lineEditFalse->text(), ui.lineEditTrue->text(), ui.pushColorFalse->palette().color(QPalette::Button), ui.pushColorTrue->palette().color(QPalette::Button));
			ui.eFlagFalse->setBooleanDisplay(i, ui.lineEditFalse->text(), ui.lineEditTrue->text(), ui.pushColorFalse->palette().color(QPalette::Button), ui.pushColorTrue->palette().color(QPalette::Button));
			/* this solves the refresh() problems while editing */
			ui.eFlagTrue->cells[i]->display();
			ui.eFlagFalse->cells[i]->display();
		}
	}
	else
	{
		ui.eFlagTrue->setBooleanDisplay(ui.spinRow->value()*ui.spinNumColumns->value() + ui.spinColumn->value(), ui.lineEditFalse->text(), ui.lineEditTrue->text(), ui.pushColorFalse->palette().color(QPalette::Button), ui.pushColorTrue->palette().color(QPalette::Button));
		ui.eFlagFalse->setBooleanDisplay(ui.spinRow->value()*ui.spinNumColumns->value() + ui.spinColumn->value(), ui.lineEditFalse->text(), ui.lineEditTrue->text(), ui.pushColorFalse->palette().color(QPalette::Button), ui.pushColorTrue->palette().color(QPalette::Button));
		/* this solves the refresh() problems while editing */
		ui.eFlagTrue->cells[ui.spinRow->value()*ui.spinNumColumns->value() + ui.spinColumn->value()]->display();
		ui.eFlagFalse->cells[ui.spinRow->value()*ui.spinNumColumns->value() + ui.spinColumn->value()]->display();
	}
	
}

void TTableBoolEditor::updateLabels()
{
	ui.lineEditTrue->setText(ui.eFlagTrue->cells[ui.spinRow->value()*ui.spinNumColumns->value() + ui.spinColumn->value()]->trueString());
	ui.lineEditFalse->setText(ui.eFlagFalse->cells[ui.spinRow->value()*ui.spinNumColumns->value() + ui.spinColumn->value()]->falseString());
	QPalette p;
	p = ui.pushColorTrue->palette();
        p.setColor(QPalette::Button, ui.eFlagTrue->cells[ui.spinRow->value()*ui.spinNumColumns->value() + ui.spinColumn->value()]->trueColor());
	ui.pushColorTrue->setPalette(p);
        p.setColor(QPalette::Button, ui.eFlagFalse->cells[ui.spinRow->value()*ui.spinNumColumns->value() + ui.spinColumn->value()]->falseColor());
	ui.pushColorFalse->setPalette(p);
}

void TTableBoolEditor::updateColorTrue()
{
	QColor newColor = QColorDialog::getColor(ui.pushColorTrue->palette().color(QPalette::Button));
	if (newColor.isValid())
	{
		QPalette p;
		p = ui.pushColorTrue->palette();
		p.setColor(QPalette::Button, newColor);
		ui.pushColorTrue->setPalette(p);
//		updateEditor();
	}
}

void TTableBoolEditor::updateColorFalse()
{
	QColor newColor = QColorDialog::getColor(ui.pushColorFalse->palette().color(QPalette::Button));
	if (newColor.isValid())
	{
		QPalette p;
		p = ui.pushColorFalse->palette();
		p.setColor(QPalette::Button, newColor);
		ui.pushColorFalse->setPalette(p);
//		updateEditor();
	}
}

void TTableBoolEditor::accept()
{
  emit propertyChanged("numRows", ui.eFlagTrue->readNumRows());
  emit propertyChanged("numColumns", ui.eFlagTrue->readNumColumns());
  emit propertyChanged("trueColors", ui.eFlagTrue->trueColors());
  emit propertyChanged("falseColors", ui.eFlagFalse->falseColors());
  emit propertyChanged("trueStrings", ui.eFlagTrue->trueStrings());
  emit propertyChanged("falseStrings", ui.eFlagFalse->falseStrings());
  
  /* save properties on parent, if  */
  QObject *parent = parentWidget();
  if(parent)
  {
	const QMetaObject *mo = parent->metaObject();
	QStringList properties = QStringList() << "numRows" << "numColumns" << "trueColors" <<  "falseColors"
	  << "trueStrings" << "falseStrings";
	QList<QVariant> values = QList<QVariant>() << ui.eFlagTrue->readNumRows() << 
	  ui.eFlagTrue->readNumColumns()   << ui.eFlagTrue->trueColors() << ui.eFlagFalse->falseColors() <<
	   ui.eFlagTrue->trueStrings()  << ui.eFlagFalse->falseStrings();
	
	for(int i = 0; i < properties.size() && i < values.size(); i++)
	{
	  int index = mo->indexOfProperty(properties.at(i).toStdString().c_str());
	  if(index >= 0)
	  {
		QMetaProperty mp = mo->property(index);
		if(! (mp.isValid() && mp.write(parent, values.at(i))))
		 perr("ttable bool editor: cannot write property \"%s\" on object \"%s\"", mp.name(), qstoc(parent->objectName()));
	  }
	  else
	  {
		perr("ttable bool editor: index %d for property \"%s\": stopping", index, qstoc(properties.at(i)));
		break;
	  }
	}
  }
  else
	pinfo("ttable bool editor: parent is null and properties are not automatically set");
  return QDialog::accept();
}



