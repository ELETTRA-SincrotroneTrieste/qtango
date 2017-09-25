#ifndef LEFT_CLICK_EVENT
#define LEFT_CLICK_EVENT

#include <QObject>
#include <QString>

class QTangoWidgetCommon;
class LeftClickEventPrivate;

class LeftClickEvent : public QObject
{
  Q_OBJECT
  public:
	LeftClickEvent(QObject *parent);
	
  protected slots:
	
  protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
  private:
    QTangoWidgetCommon *d_parentWidgetCommon;
	
    LeftClickEventPrivate *d_ptr;
};



#endif
