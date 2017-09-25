#ifndef QTANGO_WHATSTHIS_H
#define QTANGO_WHATSTHIS_H

#include <QObject>

class QWidget;
class QTangoWhatsThisPrivate;

class WhatsThisEventFilter : public QObject
{
  Q_OBJECT
  public:
    WhatsThisEventFilter(QObject *parent) : QObject(parent) {}
  
  protected:
	virtual bool eventFilter(QObject *obj, QEvent *e);
	
};

class QTangoWhatsThis 
{
  public:
	QTangoWhatsThis(QWidget *parent);
	
  private:

	QWidget *d_widget;

    QTangoWhatsThisPrivate *d_ptr;
};

#endif
