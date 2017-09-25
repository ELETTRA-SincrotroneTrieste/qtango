#ifndef QTANGO_DIALOG_H
#define QTANGO_DIALOG_H

#include <QDialog>
#include "widget_cleaner.h"

/** \brief create a dialog widget for your application optimized for use with the QTangoCore library
 *
 * This class is a QDialog optimized for QTangoCore.
 * Prefer using this class rather than the default QDialog, since it provides an optimized interface
 * to QTangoCore, especially in the context of the critical shutdown phase.
 * You are not compeled to use QTangoDialog for your widgets: the tasks performed by QTangoDialog
 * concern shutdown procedures that are also implemented inside TUtil when the QCoreApplication sends the
 * `aboutToQuit' signal. But while QCoreApplication's aboutToQuit() signal is a <em>last minute</em> 
 * opportunity to perform cleanup operations on  the QTango application, QTangoDialog design actuates
 * cleanup procedures inside the closeEvent, reimplemented from QDialog.
 * <h3>Usage</h3>
 * <p>If QTangoDialog is not integrated into Qt4 designer forms, create a QDialog with the designer and then
 * right clicking on it, before adding any other graphical elements, and <em>Promote it</em> to <em>QTangoDialog</em>.
 * The <em>include</em> file is a <em>global include</em> installed with the QTango distribution.
 * 
 */
class QTangoDialog : public QDialog, public QTangoWidgetCleaner
{
	Q_OBJECT
		
	public: 
	
		QTangoDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
		
		virtual void closeEvent(QCloseEvent * event);
};


#endif
