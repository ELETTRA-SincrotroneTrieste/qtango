#include <QMouseEvent>
#include <QMenu>
#include <com_proxy_reader.h>
#include <right_click_menuable.h> /* qtcontrols */
#include "source_metadata.h"
#include "qtango_wcommon.h"
#include "right_click_event.h"

RightClickEvent::RightClickEvent(QTangoWidgetCommon *wCommon)
{
  d_parentWidgetCommon = wCommon;
}

void RightClickEvent::copy()
{
  CopyPaste *copy = static_cast<CopyPaste *>(d_parentWidgetCommon);
  copy->copy();
}

void RightClickEvent::copyPlainText()
{
  CopyPaste *copy = static_cast<CopyPaste *>(d_parentWidgetCommon);
  copy->copyPlainText();
}

void RightClickEvent::paste()
{
  CopyPaste *paste = static_cast<CopyPaste *>(d_parentWidgetCommon);
  paste->paste();
}

void RightClickEvent::launchHelperApplication()
{
  static_cast<QTangoHelperApplication *>(d_parentWidgetCommon)->launchApplication();
}

void RightClickEvent::viewTrend()
{
   QTangoViewTrend *viewTrend = static_cast<QTangoViewTrend *>(d_parentWidgetCommon);
   ShowHide *showHide = static_cast<ShowHide *>(d_parentWidgetCommon);
   /* disable hide events when a trend plot is created */
   pinfo("right click event: view trend: trend plot disables show hide events in its correlated widget");
   showHide->setHideEventEnabled(false);
   if(viewTrend->canPlot())
	 viewTrend->createTrendPlot();
   else
	 perr("view trend said: cannot plot the trend. Is data type supported?");
}

void RightClickEvent::openHdbLink()
{
  QTangoHdb* qtangoHdb = dynamic_cast<QTangoHdb *>(d_parentWidgetCommon);
  ProxyInterface *proxyInterface = dynamic_cast<ProxyInterface *>(d_parentWidgetCommon);
  if(proxyInterface && proxyInterface->handleList().size() == 1 && 
	  !proxyInterface->tangoPoint().contains("->") && qtangoHdb && qtangoHdb->hdbActionEnabled())
	if(!qtangoHdb->openLink(proxyInterface->tangoPoint()))
	  perr("RightClickEvent::openHdbLink(): failed to open link for \"%s\"", qstoc(proxyInterface->tangoPoint()));
}

void RightClickEvent::showInfoBrowser()
{
  InfoWidget *iw = static_cast<InfoWidget *>(d_parentWidgetCommon);
  if(iw)
	iw->showInfoBrowser();
}
	
/* the core processing of the right click: take each component, get its configuration and 
 * create actions. Return true to eat the right click event.
 */
bool RightClickEvent::eventFilter(QObject *obj, QEvent *event)
{
  
  QMouseEvent *mouseEvent;
  if(event->type() == QEvent::MouseButtonPress && ((mouseEvent = static_cast<QMouseEvent *>(event)) != NULL)
	&& mouseEvent->button() == Qt::RightButton)
  {
	printf("RightClickEvent::eventFilter(QObject *obj, QEvent *event)\n");
        QMenu *menu;
	QString applicationName;
	QTangoHelperApplication *hApp = static_cast<QTangoHelperApplication *>(d_parentWidgetCommon);
	QTangoViewTrend *wTrendIface = static_cast<QTangoViewTrend *>(d_parentWidgetCommon);
	CopyPaste *copyPaste = static_cast<CopyPaste *>(d_parentWidgetCommon);
	QTangoHdb* qtangoHdb = static_cast<QTangoHdb *>(d_parentWidgetCommon);
	RightClickMenuable *widgetWithRClick = dynamic_cast<RightClickMenuable *>(obj); /* ! obj */
	ProxyInterface *proxyInterface = dynamic_cast<ProxyInterface *>(d_parentWidgetCommon);
	menu = new QMenu("Actions", dynamic_cast<QWidget *>(d_parentWidgetCommon));
	
	/* if the parent widget inherits RightClickMenuable, it must implement rightClickMenu() method ,
	 * that we must use to display its custom right click menu. This menu is placed first, and then
	 * QTango (core) specific right click menu is placed.
	 */
	if(widgetWithRClick && widgetWithRClick->rightClickMenu())
	{
	  QList<QAction *>actions = widgetWithRClick->rightClickMenu()->actions();
	  foreach(QAction *action, actions)
		menu->addAction(action);
	  if(actions.size())
		menu->addSeparator();
	}
	else if(!widgetWithRClick)
	  qprintf("\e[1;35mnot right click menuable \"%s\"\e[0m\n", qstoc(obj->objectName()));
	/* qtangocore right click menu */
	if (wTrendIface->viewTrendEnabled() && wTrendIface->canPlot())
        menu->addAction("View Trend...", this, SLOT(viewTrend()));
	else if(wTrendIface->viewTrendEnabled() && !wTrendIface->canPlot())
	  pwarn("cannot plot trend for the data type represented by this widget. Sorry :(");
	else if(!wTrendIface->viewTrendEnabled())
	  pwrn("view trend not enabled: if you desire this functionality, please call setViewTrendEnabled() on this widget");

	if (hApp->helperApplicationEnabled())
		menu->addAction("Helper Application...", this, SLOT(launchHelperApplication()));
 
	if(copyPaste->copyActionEnabled())
        {
	  menu->addAction("Copy source", this, SLOT(copy()));
          menu->addAction("Copy source (plain text)", this, SLOT(copyPlainText()));
        }
	if(copyPaste->pasteActionEnabled())
	  menu->addAction("Paste source", this, SLOT(paste()));
	
        if(proxyInterface &&
	  !proxyInterface->tangoPoint().contains("->") && qtangoHdb->hdbActionEnabled())
	{
                QAction *hdbAction = menu->addAction("View Historical Db", this, SLOT(openHdbLink()));
                hdbAction->setToolTip(QString("Opens a web browser showing attribute history for \n\"%1\",\nif "
                  "archiving has been started for it.").arg(proxyInterface->tangoPoint()));
	}
	if(proxyInterface)
	{
	  menu->addSeparator();
	  menu->addAction("Info...", this, SLOT(showInfoBrowser()));
	}
	/* show menu even if empty: maybe the reimplementation of rightClickMenuAboutToShow() 
	 * in a subclass adds some actions.
	 */
	if(widgetWithRClick)
	  widgetWithRClick->rightClickMenuAboutToShow();
	menu->exec(mouseEvent->globalPos());
        delete menu;
	return true; /* the event is eaten */
  }
  return false;
}




