#ifndef INFO_WIDGET_H
#define INFO_WIDGET_H

#include "enter_leave_event.h"
#include <QString>

class QWidget;
class InfoWidgetPrivate;

/** \brief InfoWidget is a class inherited by QTangoWidgetCommon and is used to
 *         define an interface allowing to use TInfoTextBrowser text browser to
 *         display information about the tango source or widget specific description to
 *         be appended to the tango source information.
 *
 * The informational text is available by left clicking on a qtango widget while pressing
 * the <em>Alt</em> special key or by entering the widget itself, if enterLeaveEventsEnabled
 * property is set to true.
 * Each qtango widget is ready to display this information, and the description of the tango source
 * is automatically available through qtangocore. Should you like, you can add a widget specific
 * descriptive text calling setAdditionalInfo().
 * If enterLeaveEventsEnabled is set to true, then you can specify a delay, in milliseconds, 
 * representing the delay with which the information appears on the text browser after the 
 * mouse enters the widget.
 */
class InfoWidget
{
  public:
	InfoWidget(QWidget *parent);
	
	/** \brief configures the delay after which an enter event is considered and signaled
	 *
	 */
	void setEnterEventDelay(int d) { d_enterLeaveEvent->setEnterDelay(d); }
	
	/** \brief returns the enter event delay
	 *
	 * @see setEnterEventDelay
	 */
	int enterEventDelay() { return d_enterLeaveEvent->enterDelay(); }
	
	/** \brief enables or disables the enter/leave events detection on the widget
	 *
	 * @see enterLeaveEventsEnabled
	 */
	void setEnterLeaveEventsEnabled(bool en);
	
	/** \brief returns true if enter leave events are intercepted and used, false otherwise
	 *
	 * @see setEnterLeaveEventsEnabled
	 */
	bool enterLeaveEventsEnabled();
	
	/** \brief returns the string configured as additional info
	 *
	 * @see setAdditionalInfo
	 */
	QString additionalInfo() { return d_additionalInfo; }
	
	/** \brief Adds or changes the additional info property
	 *
	 * @param desc the string that is appended to the TInfoTextBrowser after the tango source information.
	 *
	 * @see additionalInfo
	 */
	void setAdditionalInfo(const QString& desc) { d_additionalInfo = desc; }
	
	void showInfoBrowser();

  private:
	QString d_additionalInfo;
	bool d_eventsEnabled;
	EnterLeaveEvent *d_enterLeaveEvent;
	QWidget *d_parentWidget;

    InfoWidgetPrivate *d_ptr;

};

#endif
