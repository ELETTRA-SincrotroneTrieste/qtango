#include "infowidget.h"
#include <proxy_interface.h>
#include "tinfo_textbrowser.h"
#include <QWidget>
#include <QCursor>
#include <QCoreApplication>

InfoWidget::InfoWidget(QWidget *parent)
{
  Q_ASSERT(parent);
  d_enterLeaveEvent = new EnterLeaveEvent(parent);
  d_parentWidget = parent;
  setEnterLeaveEventsEnabled(true);
}

void InfoWidget::setEnterLeaveEventsEnabled(bool en)
{
  d_eventsEnabled = en;
  if(en)
	d_parentWidget->installEventFilter(d_enterLeaveEvent);
  else
  {
	/* It is always safe to remove an event filter, even during event filter activation */
	d_parentWidget->removeEventFilter(d_enterLeaveEvent);
  }  
}

bool InfoWidget::enterLeaveEventsEnabled()
{
  return d_eventsEnabled;
}

void InfoWidget::showInfoBrowser()
{
  ProxyInterface *proxyInterface = dynamic_cast<ProxyInterface *>(d_parentWidget);
  TInfoTextBrowser *ptb = QCoreApplication::instance()->findChild<TInfoTextBrowser *>();
  if(!ptb)
	ptb = new TInfoTextBrowser(d_parentWidget, Qt::Window);
//   ptb->setAttribute(Qt::WA_DeleteOnClose);
  if(proxyInterface)
	ptb->setWindowTitle(proxyInterface->tangoPoint());
  if(ptb->windowTitle().isEmpty()) /* another window title: better than nothing */
	ptb->setWindowTitle(QString("Tango info for \"%1\"").arg(d_parentWidget->objectName()));
  ptb->move(QCursor::pos());
  ptb->show(); /* first show */
  ptb->linkToSource(d_parentWidget); /* then link to source */
}
