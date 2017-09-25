#include "tlabelbooleditor.h"
#include <QColorDialog>
#include <QMetaProperty>
#include <macros.h>

TLabelBoolEditor::TLabelBoolEditor(QString ts, QString fs, QColor tc, QColor fc, QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	ui.lineEditTrue->setText(ts);
	ui.lineEditFalse->setText(fs);

	QPalette p;
	p = ui.pushColorTrue->palette();
	p.setColor(QPalette::Button, tc);
	ui.pushColorTrue->setPalette(p);
	
	p = ui.pushColorFalse->palette();
	p.setColor(QPalette::Button, fc);
	ui.pushColorFalse->setPalette(p);
	
	connect(ui.lineEditTrue, SIGNAL(textChanged(QString)), this, SLOT(updateEditor(QString)));
	connect(ui.lineEditFalse, SIGNAL(textChanged(QString)), this, SLOT(updateEditor(QString)));
	connect(ui.pushColorTrue, SIGNAL(clicked()), this, SLOT(updateColorTrue()));
	connect(ui.pushColorFalse, SIGNAL(clicked()), this, SLOT(updateColorFalse()));
	
	updateEditor();
}

void TLabelBoolEditor::updateEditor()
{
	ui.eLabelTrue->setText(ui.lineEditTrue->text());
	ui.eLabelFalse->setText(ui.lineEditFalse->text());
	
	QPalette p;
	p = ui.eLabelTrue->palette();
	p.setColor(QPalette::Base, ui.pushColorTrue->palette().color(QPalette::Button));
	ui.eLabelTrue->setPalette(p);
	
	p = ui.eLabelFalse->palette();
	p.setColor(QPalette::Base, ui.pushColorFalse->palette().color(QPalette::Button));
	ui.eLabelFalse->setPalette(p);
}

void TLabelBoolEditor::updateColorTrue()
{
	QColor newColor = QColorDialog::getColor(ui.pushColorTrue->palette().color(QPalette::Button));
	if (newColor.isValid())
	{
		QPalette p;
		p = ui.pushColorTrue->palette();
		p.setColor(QPalette::Button, newColor);
		ui.pushColorTrue->setPalette(p);
		updateEditor();
	}
}

void TLabelBoolEditor::updateColorFalse()
{
	QColor newColor = QColorDialog::getColor(ui.pushColorFalse->palette().color(QPalette::Button));
	if (newColor.isValid())
	{
		QPalette p;
		p = ui.pushColorFalse->palette();
		p.setColor(QPalette::Button, newColor);
		ui.pushColorFalse->setPalette(p);
		updateEditor();
	}
}

void TLabelBoolEditor::accept()
{
  emit propertyChanged("trueColor", ui.pushColorTrue->palette().color(QPalette::Button));
  emit propertyChanged("falseColor", ui.pushColorFalse->palette().color(QPalette::Button));
  emit propertyChanged("trueString", ui.lineEditTrue->text());
  emit propertyChanged("falseString", ui.lineEditFalse->text());
   /* save properties on parent, if  */
  QObject *parent = parentWidget();
  if(parent)
  {
	const QMetaObject *mo = parent->metaObject();
	QStringList properties = QStringList() << "trueColor" << "falseColor" << "trueString" << "falseString";
	QList<QVariant> values = QList<QVariant>() <<ui.pushColorTrue->palette().color(QPalette::Button) << 
	 ui.pushColorFalse->palette().color(QPalette::Button) << ui.lineEditTrue->text() <<
	 ui.lineEditFalse->text();
	
	for(int i = 0; i < properties.size() && i < values.size(); i++)
	{
	  int index = mo->indexOfProperty(properties.at(i).toStdString().c_str());
	  if(index >= 0)
	  {
		QMetaProperty mp = mo->property(index);
		if(! (mp.isValid() && mp.write(parent, values.at(i))))
		 perr("tlabel bool editor: cannot write property \"%s\" on object \"%s\"", mp.name(), qstoc(parent->objectName()));
	  }
	  else
	  {
		perr("tlabel bool editor: index %d for property \"%s\": stopping", index, qstoc(properties.at(i)));
		break;
	  }
	}
  }
  else
	pinfo("tlabel bool editor: parent is null and properties are not automatically set");
  return QDialog::accept();
}



