#ifndef QTANGO_MAINWINDOW_H
#define QTANGO_MAINWINDOW_H

#include <QMainWindow>
#include "widget_cleaner.h"

/** \brief create a main window for your application optimized for use with the QTangoCore library
 *
 * This class is a QMainWindow optimized for QTangoCore.
 * Prefer using this class rather than the default QMainWindow, since it provides an optimized interface
 * to QTangoCore, especially in the context of the critical shutdown phase.
 * You are not compeled to use QTangoMainWindow for your widgets: the tasks performed by QTangoMainWindow
 * concern shutdown procedures that are also implemented inside TUtil when the QCoreApplication sends the
 * `aboutToQuit' signal. But while QCoreApplication's aboutToQuit() signal is a <em>last minute</em> 
 * opportunity to perform cleanup operations on  the QTango application, QTangoMainWindow design actuates
 * cleanup procedures inside the closeEvent, reimplemented from QMainWindow.
 * <h3>Usage</h3>
 * <p>If QTangoMainWindow is not integrated into Qt4 designer forms, create a QMainWindow with the designer and then
 * right clicking on it, before adding any other graphical elements, and <em>Promote it</em> to <em>QTangoMainWindow</em>.
 * The <em>include</em> file is a <em>global include</em> installed with the QTango distribution.
 * </p>
 */
class QTangoMainWindow : public QMainWindow, public QTangoWidgetCleaner
{
	Q_OBJECT
		
	public: 
	
		QTangoMainWindow( QWidget * parent = 0, Qt::WindowFlags f = 0 );
		
		virtual void closeEvent(QCloseEvent * event);
};


#endif
