#ifndef HIDE_EVENT_H
#define HIDE_EVENT_H

#include <QObject>

class HideEventPrivate;

class HideEvent : public QObject 
{
  Q_OBJECT
  public:
	HideEvent(QObject *parent);
	
  protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
  private:
    HideEventPrivate *d_ptr;
};

#endif
