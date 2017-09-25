#include "tledbooleditor.h"
#include <QColorDialog>
#include <QMetaProperty>
#include <macros.h>

#include <tango.h>

TLedBoolEditor::TLedBoolEditor(QColor tc, QColor fc, QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

// // 	?? ui.tLedTrue->setType(Tango::DEV_BOOLEAN);
	ui.tLedTrue->setTrueColor(tc);
	ui.tLedTrue->setFalseColor(fc);
// 	Tango::DeviceAttribute attr;
// 	attr << true;
// 	ui.tLedTrue->setTangoValue(attr);
	ui.tLedTrue->setColor(tc);

// // 	?? ui.tLedFalse->setType(Tango::DEV_BOOLEAN);
	ui.tLedFalse->setTrueColor(tc);
	ui.tLedFalse->setFalseColor(fc);
// 	attr << false;
// 	ui.tLedFalse->setTangoValue(attr);
	ui.tLedFalse->setColor(fc);

	connect(ui.pushColorTrue, SIGNAL(clicked()), this, SLOT(updateColorTrue()));
	connect(ui.pushColorFalse, SIGNAL(clicked()), this, SLOT(updateColorFalse()));
}

void TLedBoolEditor::updateColorTrue()
{
	QColor newColor = QColorDialog::getColor(ui.tLedTrue->trueColor());
	if (newColor.isValid())
	{
		ui.tLedFalse->setTrueColor(newColor);
		ui.tLedTrue->setTrueColor(newColor);
		Tango::DeviceAttribute attr;
		Tango::DevBoolean data = true;
		attr << data;
		ui.tLedTrue->setColor(newColor);
	}
}

void TLedBoolEditor::updateColorFalse()
{
	QColor newColor = QColorDialog::getColor(ui.tLedFalse->falseColor());
	if (newColor.isValid())
	{
		ui.tLedFalse->setFalseColor(newColor);
		ui.tLedTrue->setFalseColor(newColor);
		Tango::DeviceAttribute attr;
		Tango::DevBoolean data = false;
		attr << data;
		ui.tLedFalse->setColor(newColor);
	}

}

void TLedBoolEditor::accept()
{
  emit propertyChanged("trueColor", ui.tLedTrue->trueColor());
  emit propertyChanged("falseColor", ui.tLedFalse->falseColor());
   /* save properties on parent, if  */
  QObject *parent = parentWidget();
  if(parent)
  {
	const QMetaObject *mo = parent->metaObject();
	QStringList properties = QStringList() << "trueColor" << "falseColor";
	QList<QVariant> values = QList<QVariant>() <<ui.tLedTrue->trueColor() << ui.tLedFalse->falseColor();
	
	for(int i = 0; i < properties.size() && i < values.size(); i++)
	{
	  int index = mo->indexOfProperty(properties.at(i).toStdString().c_str());
	  if(index >= 0)
	  {
		QMetaProperty mp = mo->property(index);
		if(! (mp.isValid() && mp.write(parent, values.at(i))))
		 perr("tled bool editor: cannot write property \"%s\" on object \"%s\"", mp.name(), qstoc(parent->objectName()));
	  }
	  else
	  {
		perr("tled bool editor: index %d for property \"%s\": stopping", index, qstoc(properties.at(i)));
		break;
	  }
	}
  }
  else
	pinfo("tled bool editor: parent is null and properties are not automatically set");
  return QDialog::accept();
}




