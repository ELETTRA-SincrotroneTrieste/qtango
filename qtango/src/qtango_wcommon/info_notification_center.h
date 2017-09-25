#ifndef INFO_NOTIFICATION_CENTER
#define INFO_NOTIFICATION_CENTER

#include "html_notification_center.h"

class ProxyInterface;
class QObject;
class TInfoTextBrowser;
class QEvent;

class InfoNotificationCenter : public HtmlNotificationCenter
{
  public:
	static InfoNotificationCenter *instance(QObject *parent = NULL);
	
	/** \brief register a TInfoTextBrowser into the list of browsers that have to 
	 * be notified when a HtmlNotifEvent is received.
	 */
	void registerInfoBrowser(TInfoTextBrowser *ti );
	
	void unregisterInfoBrowser(TInfoTextBrowser *ti);
	
  protected:
	bool event(QEvent *e);
	
  private:
	InfoNotificationCenter(QObject *parent);
	QList<TInfoTextBrowser *>d_browsers;
	
	static InfoNotificationCenter *_instance;
};

#endif
