#ifndef QTANGO_WIDGET_H
#define QTANGO_WIDGET_H

#include <QWidget>
#include "widget_cleaner.h"

/** \brief create a widget for your application optimized for use with the QTangoCore library
 *
 * This class is a QWidget optimized for QTangoCore.
 * Prefer using this class rather than the default QWidget, since it provides an optimized interface
 * to QTangoCore, especially in the context of the critical shutdown phase.
 * You are not compeled to use QTangoWidget for your widgets: the tasks performed by QTangoWidget
 * concern shutdown procedures that are also implemented inside TUtil when the QCoreApplication sends the
 * `aboutToQuit' signal. But while QCoreApplication's aboutToQuit() signal is a <em>last minute</em> 
 * opportunity to perform cleanup operations on  the QTango application, QTangoWidget design actuates
 * cleanup procedures inside the closeEvent, reimplemented from QWidget.
 * <h3>Usage</h3>
 * <p>If QTangoWidget is not integrated into Qt4 designer forms, create a QWidget with the designer and then
 * right clicking on it, before adding any other graphical elements, and <em>Promote it</em> to <em>QTangoWidget</em>.
 * The <em>include</em> file is a <em>global include</em> installed with the QTango distribution.
 * </p>
 */
class QTangoWidget : public QWidget, public QTangoWidgetCleaner
{
	Q_OBJECT
		
	public: 
	
		QTangoWidget( QWidget * parent = 0, Qt::WindowFlags f = 0 );
		
		virtual void closeEvent(QCloseEvent * event);
};


#endif
