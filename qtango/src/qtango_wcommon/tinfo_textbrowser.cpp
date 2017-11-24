#include "tinfo_textbrowser.h"
#include <auto_configuration.h>
#include <communication_handle.h>
#include <QTimer>
#include <QStringList>
#include <QSettings>
#include <proxy_interface.h>
#include <infowidget.h> /* for additional info text */
#include "qtango_wcommon.h"
#include "info_notification_center.h"
#include <macros.h>
#include <QScrollBar>
#include <QDesktopServices>
#include <QCoreApplication>


TInfoTextBrowser::TInfoTextBrowser(QWidget *parent, Qt::WindowFlags wf) : QTextBrowser(parent)
{
  QSettings s;
  d_wCommon = NULL;
  d_proxyInterface = NULL;
  d_page = MAIN;
  
  if(wf != 0)
	setWindowFlags(wf);
  if(wf | Qt::Window)
	resize(QSize(364, 496));
  /* register ourselves to receive mouse enter/leave events from the info notification center */
  InfoNotificationCenter::instance()->registerInfoBrowser(this);
  /* refresh timer: setup */
  d_updateTimer = new QTimer(this);
  d_updateTimer->setSingleShot(false);
  d_updateTimer->setInterval(1000);
  
  setSearchPaths(s.value("CSS_PATH", QStringList() << "/runtime/share/qtango/stylesheets").toStringList());
  
  d_defaultMessage = "<h4>Informational panel</h4><p>move the mouse cursor over a <cite>qtango</cite> "
	"widget to display information about the tango point</p>";
  
  /* signal/slot connections */
  connect(this, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(processClick(const QUrl&)));
  connect(d_updateTimer, SIGNAL(timeout()), this, SLOT(updateTextMain()));
  d_setHtml(d_defaultMessage);
  setEnabled(true);
}

void TInfoTextBrowser::setSource(const QUrl &)
{
  
}

void TInfoTextBrowser::linkToSource(QObject *object)
{
  /* immediately unlink when an object is destroyed */
  connect(object, SIGNAL(destroyed()), this, SLOT(unlink()));
  d_page = MAIN;
  d_wCommon = dynamic_cast<QTangoWidgetCommon *>(object);
  d_proxyInterface = dynamic_cast<ProxyInterface *>(object);
  d_infoWidget = dynamic_cast<InfoWidget *>(object);
  if(d_proxyInterface)
	setWindowTitle(d_proxyInterface->tangoPoint());
  if(isVisible())
  {
	update();
	d_updateTimer->start();
  }
}

void TInfoTextBrowser::update()
{
  if(d_page == MAIN)
	updateTextMain();
  else if(d_page == AINFO)
	updateTextAttributeInfo();
}

void TInfoTextBrowser::unlink()
{
  d_wCommon = NULL;
  d_proxyInterface = NULL;
  d_infoWidget = NULL;
  d_updateTimer->stop();
  d_setHtml(d_defaultMessage);
}

void TInfoTextBrowser::updateTextMain()
{
  QString html;

  if(d_proxyInterface)
	html += d_proxyInterface->htmlHandleRepresentation();
  
  if(d_wCommon && d_wCommon->sourceMetadata())
	html +=  d_wCommon->sourceMetadata()->toHtml();
  
  if(d_infoWidget && d_infoWidget->additionalInfo() != QString())
	html += d_infoWidget->additionalInfo();
  
  TInfoTextBrowser::d_setHtml(html);
}

void TInfoTextBrowser::updateTextAttributeInfo()
{
  QString html;
  if(d_proxyInterface)
  {
	html += QString("<h3 align=\"center\">%1</h3>").arg(d_proxyInterface->tangoPoint());
	html += d_proxyInterface->htmlAttributeInfoRepresentation();
  }
  d_setHtml(html);
}

void TInfoTextBrowser::d_setHtml(const QString &s)
{
  QString text = s + menu();
  TInfoTextBrowser::setHtml(text);

}

void TInfoTextBrowser::setHtml(const QString &s)
{
  QString styleSheet = "<link rel=\"stylesheet\" href=\"tinfo.css\" type=\"text/css\" />";
  QString header = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html>"
  "<head><meta name=\"qrichtext\" content=\"1\" />";
  
  QString newS = header + styleSheet +  
  "</head>\n<body>"
   + s + 
  "</body>"
  "</html>";
  int scrollBarValue = verticalScrollBar()->value();
  QTextBrowser::setHtml(newS);
  verticalScrollBar()->setValue(scrollBarValue);
}

QString TInfoTextBrowser::menu()
{
  QString menu = "<h4>More resources</h4><ul>";
  if(d_page == MAIN && d_proxyInterface && d_wCommon)
  {
	if(HANDLE(d_proxyInterface) && HANDLE(d_proxyInterface)->configured())
	{
	  if(!d_proxyInterface->tangoPoint().contains("->"))
		menu += "<li><a href=\"ainfo\" title=\"attribute info\">attribute info</a></li>";
	  if(d_wCommon->helperApplicationEnabled() && !d_wCommon->helperApplication().isEmpty())
	  {
		menu += QString("<li><a href=\"helperapplication\" title=\"Launch associated helper application\">helper application (%1)</a></li>").
		  arg(d_wCommon->helperApplication());
	  }
	  if(d_wCommon->viewTrendEnabled())
		menu += "<li><a href=\"viewtrend\" title=\"View trend\">view trend</a></li>";
	}
  }
  else if(d_page != MAIN)
	menu += "<li><a href=\"main\" title=\"back to previous page\">back</a></li>";
  
  menu += "</ul>";
  
  if(d_page == MAIN)
  {
	QVariant v;
	menu += "<h4>Application info:</h4>\n";
	menu += "<ul>\n";
	QCoreApplication *a = qApp;
	v = a->property("applicationName");
	if(v.isValid())
	  menu += "<li>Name: <em>" + a->property("applicationName").toString() + "</em>;</li>\n";
        v = a->property("version");
	if(v.isValid())
          menu += "<li>version: <em>" + a->property("version").toString() + "</em>;</li>\n";
    menu += "<li>QTango library version: <em>" + QString(QTANGO_VERSION_STR) + "</em>;</li>\n";
	/* executable name */
	menu += QString("<li>executable: <cite>%1</cite>;</li>").arg(a->arguments().at(0));
	menu += "</ul><h4>Author info:</h4>\n<ul>";
	/* author name */
	v = a->property("author");
	if(v.isValid())
	  menu += "<li>name: <strong>" + a->property("author").toString() + "</strong>;</li>";
	/* author information */
	/* Office */
	v = a->property("office");
	if(v.isValid())
	  menu += "<li>office location: <cite>" + a->property("office").toString() + "</cite>;</li>";
	/* Phone number */
	v = a->property("phone");
	if(v.isValid())
	  menu += "<li>phone number: <cite>" + a->property("phone").toString() + "</cite>;</li>";
	/* mail */
	v = a->property("mail");
	if(v.isValid())
	  menu += QString("<li>send an <a href=\"mailto:%1\" title=\"send an email to %1\">email</a> to the author;</li>")
		.arg(a->property("mail").toString());
	
	v = a->property("hwReferent");
	if(v.isValid())
	  menu += "<li>Hardware referent: <cite>" + a->property("hwReferent").toString() + "</cite>;</li>";
		
	if(getenv("BUGZILLA_URL"))
	{
	  menu += QString("<li>to report a <em>bug</em> or make a <em>request</em> create a <a href=\"%1\" title=\"submit a ticket to bugzilla\">ticket</a>;</li>").arg(
		getenv("BUGZILLA_URL"));
	}
	/* end author information */
	
	menu += "</ul>\n";
  }
  menu += "<p class=\"bottom\"><a href=\"clearinfo\" title=\"clear the information displayed\">clear info</a></p>";
  
  return menu;
}

void TInfoTextBrowser::processClick(const QUrl& url)
{
  QString s = url.toString();
  disconnect(d_updateTimer, 0, this, 0);
  if(s == "ainfo")
  {
	d_page = AINFO;
	connect(d_updateTimer, SIGNAL(timeout()), this, SLOT(updateTextAttributeInfo()));
  }
  else if(s == "main")
  {
	d_page = MAIN;
	connect(d_updateTimer, SIGNAL(timeout()), this, SLOT(updateTextMain()));
  }
  else if(s == "helperapplication" && d_wCommon->helperApplicationEnabled() && !d_wCommon->helperApplication().isEmpty())
	d_wCommon->launchApplication();
  else if(s == "viewtrend" && d_wCommon->viewTrendEnabled())
	d_wCommon->createTrendPlot();
  else if(s.startsWith("http:") || s.startsWith("mailto:"))
	QDesktopServices::openUrl(url);
  else if(s == "clearinfo")
  {
	unlink();
	d_setHtml(d_defaultMessage);
  }
}

TInfoTextBrowser::~TInfoTextBrowser()
{
  pinfo("TInfoTextBrowser deleted: unregistering from info notification center");
  InfoNotificationCenter::instance()->unregisterInfoBrowser(this);
}

void TInfoTextBrowser::showEvent(QShowEvent *e)
{
  if(d_wCommon != NULL && d_proxyInterface != NULL)
  {
	update();
	d_updateTimer->start();
  }
  QTextBrowser::showEvent(e);
}
	
void TInfoTextBrowser::hideEvent(QHideEvent *e)
{
  if(d_updateTimer->isActive())
	d_updateTimer->stop();
  QTextBrowser::hideEvent(e);
}

