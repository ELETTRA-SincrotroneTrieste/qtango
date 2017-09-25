#ifndef HTML_NOTIFICATION_CENTER_H
#define HTML_NOTIFICATION_CENTER_H

#include <QObject>
#include <QEvent>

class QObject;

class ProxyNotificationEvent : public QEvent
{
  public:
	enum events { ProxyLinkEvent = QEvent::User + 1,  ProxyUnlinkEvent };
	
	ProxyNotificationEvent(events t, QObject *o);
	
	QObject *object() { return d_object; }
	
  private:
	QObject *d_object;
};


class HtmlNotificationCenter : public QObject
{
  Q_OBJECT
  public:
	HtmlNotificationCenter(QObject *parent);
	
	
};


#endif

