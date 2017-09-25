#include "proxy_interface.h"
#include <communication_handle.h>
#include <enum2qstring.h>

ProxyInterface::ProxyInterface()
{
  
}

QList<QTangoCommunicationHandle*> ProxyInterface::handleList() const
{
  QList <QTangoCommunicationHandle*> hlist;
  if(qtangoComHandle())
	hlist << qtangoComHandle();
  return hlist;
}

QString ProxyInterface::htmlHandleRepresentation()
{
  QString html;
  RefreshMode refreshMode;
  unsigned short configured = 0;
  QList<QTangoCommunicationHandle *>hList = handleList();
  
  foreach(QTangoCommunicationHandle *h, hList)
  {
	if(h->configured())
	{
	  configured++;
	  /* NOTE: here we put html = ".." to re initialize the string */
	  html += isReader() ? QString("<h3>%1</h3>\n").arg(h->source()) : QString("<h3>%1</h3>\n").arg(h->sourcesAsSingleString());
	  const TangoConfigurationParameters* conf = h->tangoConfigurationParameters();
	  if(conf)
	  {
		if(conf->descriptionIsSet())
		  html += QString("<p><cite>%1</cite></p>").arg(conf->description());
		else
		  html += "<p><cite>description unavailable (from the device attribute properties)</cite></p>";
	  }
	  
	  /* connection status/info (<ul>) */
	  if(h->source() != QString())
	  {
		html += QString("<h4>Source connection status:</h4><ul><li><cite>%1</cite></li>").arg(h->connectionStatus());
		html += QString("<li>source: <cite>%1</cite></li>").arg(h->source());
		html += "<li>type: <strong>reader</strong></li>";
		html += QString("<li>refresh mode: <cite>%1</cite></li>").arg(TUtil::instance()->modeRepresentation(h->refreshMode(), true));
        refreshMode = h->refreshMode();
        if(refreshMode == POLLED_REFRESH)
		  html += QString("<li>polling period: <em>%1</em>ms</li>").arg(h->period());
        else if(refreshMode >= USER_EVENT_REFRESH && refreshMode <= ARCHIVE_EVENT_REFRESH)
        {
            html += "<ul><li>is zmq: ";
            if(h->isEventZmq())
                html += "yes</li></ul>";
            else
                html += "no</li></ul>";
        }
	  }
	  if(!h->sources().isEmpty())
	  {
		html += QString("<h4>Target(s) connection status:</h4><ul><li><cite>%1</cite></li>").arg(h->connectionStatus());
		html += QString("<li><em>targets</em>: <ul>");
		foreach(QString tgt, h->sources())
		  html += QString("<li><cite>%1</cite></li>").arg(tgt);
		html += "</ul></li>";
		html += "<li>type: <strong>writer</strong></li>";
		html += QString("<li>actions count: <em>%1</em></li>").arg(h->actionsCount());
	  }
	  
	  html += "</ul>";
	}
	else 
	{
	  if(!h->connectionStatus().isEmpty())
		html += QString("<p>%1</p>").arg(h->connectionStatus());
	  if(!h->connectionErrorMessage().isEmpty())
		html += QString("<p>%1</p>").arg(h->connectionErrorMessage());
	}
  }
//   if(!configured)
//   {
// 	html = "<h3>Source/target not configured yet</h3>";
// 	html += QString("<p><ul><li>%1</li></ul></p>").arg(h->connectionStatus());
//   }
  return html;
}

QString ProxyInterface::htmlAttributeInfoRepresentation()
{
  const TangoConfigurationParameters* conf = NULL;
  QString html;
  
  /* NOTE: look for qtangoComHandle() != NULL because the method is pure virtual and some implementation might
   * return NULL. This is the case of TPlotRefreshConnector in TPlotLightMarker.
   */
  if(qtangoComHandle() && (conf = qtangoComHandle()->tangoConfigurationParameters()) != NULL)
  {
	/* data type, format, unit */
	QString tfu = QString("<ul><li>data type: <em>%1</em></li><li>"
	  "data format: <em>%2</em></li><li>write type: <em>%3</em></li>"
	  "<li>label: <cite>%4</cite></li><li>format: <em>%5</em></li>"
	  "<li>unit: <cite>%6</cite></li></ul>")
	  .arg(Enum2String::CmdArgType2String((Tango::CmdArgType)conf->dataType())).
	  arg(Enum2String::AttrDataFormat2String(conf->dataFormat())).
	  arg(Enum2String::AttrWriteType2String(conf->writeType())).arg(conf->label()).arg(conf->format()).
	  arg(conf->displayUnit());
	
	html = tfu;  
	 
	AttributeAlarmInfo alarms = conf->attributeInfoEx().alarms;
	AttributeEventInfo events = conf->attributeInfoEx().events;
	/* get data structures */
	QString minmax = (conf->minIsSet() && conf->maxIsSet()) ? 
	  QString("<tr><td align=\"center\">Value</td><td align=\"center\">%1</td><td align=\"center\">%2</td></tr>").arg(conf->minValue()).arg(conf->maxValue()) :
	  "<tr><td align=\"center\">Value</td><td align=\"center\">Not set</td><td align=\"center\">Not set</td></tr>";
	/* format information into 2 html tables */
	html +=
	"<h4>Alarm info</h4>"
	"<div id=\"table1\">" + 
	QString("<table summary=\"%1\" border=\"1\" valign=\"center\" align=\"center\" width=\"90%\">").arg(tangoPoint())
	 + "<tr><td align=\"center\"></td><td align=\"center\">Min</td><td align=\"center\">Max</td></tr>"
	 + minmax +
	  
	QString("<tr><td align=\"center\">Warning</td><td align=\"center\">%1</td><td align=\"center\">%2</td></tr>").arg(QString::fromStdString(alarms.min_warning)).
	  arg(QString::fromStdString(alarms.max_warning)) +
	  
	 QString("<tr><td align=\"center\">Alarm</td><td align=\"center\">%1</td><td align=\"center\">%2</td></tr>").arg(QString::fromStdString(alarms.min_alarm)).
	  arg(QString::fromStdString(alarms.max_alarm)) +
	  
	 QString("<tr><td align=\"center\">Delta T</td><td colspan=\"2\">%1</td></tr>").arg(QString::fromStdString(alarms.delta_t)) +
	  
	 QString("<tr><td align=\"center\">Delta Val.</td><td colspan=\"2\">%1</td></tr>").arg(QString::fromStdString(alarms.delta_val)) +
    "</table></div>\n<div id=\"table2\">";
	
	html +=
	"<h4>Event info</h4>" +
	QString("<table summary=\"%1\" border=\"1\" valign=\"center\" align=\"center\" width=\"90%\">").arg(tangoPoint())
	+ "<tr><td align=\"center\"></td><td align=\"center\">Absolute</td><td align=\"center\">Relative</td><td align=\"center\">Period</td></tr>" +
	QString("<tr><td align=\"center\">Change</td><td align=\"center\">%1</td><td align=\"center\">%2</td><td align=\"center\">-</td></tr>").arg(QString::fromStdString(events.ch_event.abs_change)).
	  arg(QString::fromStdString(events.ch_event.rel_change)) +
	  
	QString("<tr><td align=\"center\">Archive</td><td align=\"center\">%1</td><td align=\"center\">%2</td><td align=\"center\">%3</td></tr>").arg(QString::fromStdString(events.arch_event.archive_abs_change)).
	  arg(QString::fromStdString(events.arch_event.archive_rel_change)).arg(QString::fromStdString(events.arch_event.archive_period)) +
	  
	 QString("<tr><td align=\"center\">Periodic</td><td align=\"center\">-</td><td align=\"center\">-</td><td align=\"center\">%1</td></tr>").arg(QString::fromStdString(events.per_event.period)) +
	"</table></div>";
  }
  else
	html += tangoPoint().isEmpty() ? "<p>configuration parameters unavailable</p>" : 
	  QString("<p>configuration parameters not available for <cite>%1</cite></p>").arg(tangoPoint());
  
  return html;
}







