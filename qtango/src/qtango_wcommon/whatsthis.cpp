#include "whatsthis.h"
#include <QEvent>
#include <QHelpEvent>
#include <proxy_interface.h>
#include <tinfo_textbrowser.h>
#include <QWhatsThis>

bool WhatsThisEventFilter::eventFilter(QObject *obj, QEvent *e)
{
  if(e->type() == QEvent::WhatsThis)
  {
	QWhatsThis::enterWhatsThisMode();
	QHelpEvent *he = static_cast<QHelpEvent *>(e);
	ProxyInterface *proxyInterface = dynamic_cast<ProxyInterface *>(obj);
	TInfoTextBrowser *ptb = new TInfoTextBrowser(dynamic_cast<QWidget *>(obj), Qt::Window);
	ptb->setAttribute(Qt::WA_DeleteOnClose);
	if(proxyInterface)
	  ptb->setWindowTitle(proxyInterface->tangoPoint());
	ptb->move(he->globalPos());
	ptb->linkToSource(parent());
	ptb->show();
	QWhatsThis::leaveWhatsThisMode();
	return true; /* prevent the event be handled further */
  }
  return false;
}

QTangoWhatsThis::QTangoWhatsThis(QWidget *parent)
{
  Q_ASSERT(parent != NULL);
  d_widget = parent;
  WhatsThisEventFilter * whatsThisFilter = new WhatsThisEventFilter(parent);
  parent->installEventFilter(whatsThisFilter);
}