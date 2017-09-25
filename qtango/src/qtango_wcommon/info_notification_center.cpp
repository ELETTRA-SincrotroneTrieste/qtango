#include "info_notification_center.h"
#include "tinfo_textbrowser.h"
#include <proxy_interface.h>
#include <macros.h>
#include <QtDebug>

InfoNotificationCenter *InfoNotificationCenter::_instance = NULL;

InfoNotificationCenter::InfoNotificationCenter(QObject *parent) : HtmlNotificationCenter(parent)
{
  
}

InfoNotificationCenter* InfoNotificationCenter::instance(QObject *parent)
{
  if(!_instance)
	_instance = new InfoNotificationCenter(parent);
  return _instance;
}

void InfoNotificationCenter::registerInfoBrowser(TInfoTextBrowser *ti )
{
  pok("info notification center: registering info browser \"%s\"", qstoc(ti->objectName()));
  d_browsers << ti;
}

void InfoNotificationCenter::unregisterInfoBrowser(TInfoTextBrowser *ti)
{
   pok("info notification center: unregistering info browser \"%s\"", qstoc(ti->objectName()));
   d_browsers.removeAll(ti);
}

bool InfoNotificationCenter::event(QEvent *e)
{
  if(e->type() == (int) ProxyNotificationEvent::ProxyLinkEvent || e->type() == (int) ProxyNotificationEvent::ProxyUnlinkEvent)
  {
	ProxyNotificationEvent *pne = static_cast<ProxyNotificationEvent *>(e);
    if(pne && e->type() == (int) ProxyNotificationEvent::ProxyLinkEvent)
	{
	  foreach(TInfoTextBrowser *tb, d_browsers)
	  {
		tb->linkToSource(pne->object());
	  }
	}
    else if(pne && e->type() == (int) ProxyNotificationEvent::ProxyUnlinkEvent)
	{
	  foreach(TInfoTextBrowser *tb, d_browsers)
	  {
		tb->unlink();
	  }
	}
	return true; /* recognized and processed */
  }
  return QObject::event(e);
}

