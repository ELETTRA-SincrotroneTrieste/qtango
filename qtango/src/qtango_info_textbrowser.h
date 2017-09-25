#ifndef QTANGO_INFO_TEXTBROWSER_H
#define QTANGO_INFO_TEXTBROWSER_H

#include <tinfo_textbrowser.h>

/* (C) 2009 Giacomo Strangolino
 *
 */

class QTangoInfoTextBrowserPrivate;

/** \brief a class to display information about a qtango widget 
 * 
 * A read only text browser directly inherited from QTangoCore's 
 * TInfoTextBrowser, used to display information and meta information
 * about tango sources/targets available through QTangoCore and tango database.
 * 
 * If qtango widgets have the property enterLeaveEventsEnabled, inherited by
 * InfoWidget::enterLeaveEventsEnabled, then when the mouse hovers the widget
 * some useful information is automatically displayed in this text browser.
 * 
 * It is possible to configure each widget so that there is a delay between the
 * mouse enter event and the information on the text browser.
 *
 * When the mouse leaves the widget, the text browser is cleared.
 */
class QTangoInfoTextBrowser : public TInfoTextBrowser
{
  Q_OBJECT
  public:
	QTangoInfoTextBrowser(QWidget *parent);
  
  protected:
	
  /* reimplemented to eat QTextBrowser::setSource() */
  void setSource(const QUrl &name) { Q_UNUSED (name); }
	
  private:
  QTangoInfoTextBrowserPrivate *d_ptr;
};

#endif
