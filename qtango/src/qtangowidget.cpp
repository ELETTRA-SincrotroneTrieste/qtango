#include "qtangowidget.h"
#include <macros.h>

QTangoWidget::QTangoWidget( QWidget * parent, Qt::WindowFlags f) :
	QWidget(parent, f), QTangoWidgetCleaner()
{

}

void QTangoWidget::closeEvent(QCloseEvent * event)
{
	pok("qtango widget closing: cleaning qtango core...");
	clean();
	QWidget::closeEvent(event);
}




