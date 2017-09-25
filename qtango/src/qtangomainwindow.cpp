#include "qtangomainwindow.h"
#include <macros.h>

QTangoMainWindow::QTangoMainWindow( QWidget * parent, Qt::WindowFlags f) :
	QMainWindow(parent, f), QTangoWidgetCleaner()
{

}

void QTangoMainWindow::closeEvent(QCloseEvent * event)
{
	pok("qtango widget closing: cleaning qtango core...");
	clean();
	QMainWindow::closeEvent(event);
}




