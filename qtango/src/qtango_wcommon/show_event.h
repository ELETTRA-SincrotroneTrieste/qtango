#ifndef SHOW_EVENT_H
#define SHOW_EVENT_H

#include <QObject>
class ShowEventPrivate;

class ShowEvent : public QObject 
{
  Q_OBJECT
  public:
	ShowEvent(QObject *parent);
	
  protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
  private:
    ShowEventPrivate *d_ptr;
};


#endif
