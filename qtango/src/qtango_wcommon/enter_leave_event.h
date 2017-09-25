#ifndef ENTER_EVENT_H
#define ENTER_EVENT_H

#include <QObject>

class QTimer;
class EnterLeaveEventPrivate;

/** \brief this class  manages enter and leave events on widgets.
 *
 * This class is used to detect mouse enter and leave events on QTango widgets.
 * It is used by the InfoWidget class.
 * 
 * @see InfoWidget
 */
class EnterLeaveEvent : public QObject
{
  Q_OBJECT
  public:
	EnterLeaveEvent(QObject *parent);
	~EnterLeaveEvent();
	
	int enterDelay() { return d_enterDelay; }
	void setEnterDelay(int d) { d_enterDelay = d; }
	
  protected:
	bool eventFilter(QObject *obj, QEvent *event);

  protected slots:
	void triggerLinkEvent();
	
  private:
	QTimer *d_enterDelayTimer;
	int d_enterDelay;

    EnterLeaveEventPrivate *d_ptr;
};

#endif
