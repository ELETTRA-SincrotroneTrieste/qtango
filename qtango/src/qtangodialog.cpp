#include "qtangodialog.h"
#include <macros.h>

QTangoDialog::QTangoDialog( QWidget * parent, Qt::WindowFlags f) :
	QDialog(parent, f), QTangoWidgetCleaner()
{

}

void QTangoDialog::closeEvent(QCloseEvent * event)
{
	pok("qtango widget closing: cleaning qtango core...");
	clean();
	QDialog::closeEvent(event);
}




