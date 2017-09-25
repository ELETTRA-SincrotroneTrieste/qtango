#include "plot_com_proxy_reader.h"
#include "tplotlight_marker.h"
#include <tlog.h>
#include <device_factory.h>
#include <QDateTime>
#include <QtDebug>

PlotComProxyReader::PlotComProxyReader(QObject *parent, QString name) : QObject(parent), QTangoComProxyReader(parent)
{
	setPeriod(1000);
	setRefreshMode( AUTO_REFRESH);
	d_name = name;
	d_filledFromHistory = false;
        enableHideEvents(false);
	connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)));
	connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)),
		this, SLOT(sourceConfigured(const TangoConfigurationParameters *)));
}

void PlotComProxyReader::d_triggerRefresh(const TVariant &v)
{
	emit newData(v);
}

void PlotComProxyReader::refresh(const TVariant &v)
{
  if(!d_filledFromHistory)
  {
	TPlotRefreshConnector *plotRC = dynamic_cast<TPlotRefreshConnector *>(parent());
	/* if not already filled in auto configuration slot, try to configure here at first refresh */
	if(plotRC && plotRC->fillFromHistoryEnabled() && !d_filledFromHistory)
	 fillFromHistory(v.getAttributeInfo());
  }
  d_triggerRefresh(v);
}

void PlotComProxyReader::disconnectNewDataSignal()
{
  disconnect(qtangoComHandle(), SIGNAL(newData(const TVariant&)));
}

void PlotComProxyReader::connectNewDataSignal()
{
  /* avoid multiple connects */
  disconnect(qtangoComHandle(), SIGNAL(newData(const TVariant&)));
  /* connect */
  connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)));
}

void PlotComProxyReader::sourceConfigured(const TangoConfigurationParameters *cp)
{
	qDebug() << "PlotComProxyReader::single source configured" <<cp->minValue() <<  cp->maxValue() ;
	if(cp->maxIsSet() && cp->minIsSet())
        {
            emit sourceUpperBound(cp->maxValue());
            emit sourceLowerBound(cp->minValue());
	}
	emit sourceConfigurationAvailable(cp);
	TPlotRefreshConnector *plotRC = dynamic_cast<TPlotRefreshConnector *>(parent());
	if(plotRC && plotRC->fillFromHistoryEnabled() && !d_filledFromHistory)
	  fillFromHistory(cp->attributeInfoEx());
}

 
void PlotComProxyReader::fillFromHistory(const Tango::AttributeInfo &ainfo)
{
	long depth = 10;
	QString devName, entityName;
	QString src = realSource();
	if(src.count("/") == 3 && src.split("/", QString::SkipEmptyParts).size() == 4)
	{
	  entityName = src.split("/", QString::SkipEmptyParts).last();
	  devName = src.remove("/" + entityName);
	  printf("Fill from history: \e[1;32m%s %s\e[0m\n", qstoc(devName), qstoc(entityName));
	}
	QStringList args;
	string s_device_name = devName.toStdString();
	string s_attr_command = entityName.toStdString();
	if(ainfo.data_format == Tango::SCALAR)
	{
		DbData prop;
		prop.push_back(DbDatum("attr_poll_ring_depth"));
		prop.push_back(DbDatum("poll_ring_depth"));
		try {
			DeviceProxy *device = DeviceFactory::instance()->create(devName);
			device->get_property(prop);
			if ((prop[0].is_empty() == false))
			{
				vector<string> temp;
				prop[0] >> temp;
				QVector<string> attr_poll = QVector<string>::fromStdVector(temp);
				int found = attr_poll.indexOf(s_attr_command);
				if (found != -1)
					depth = QString::fromStdString(attr_poll[found+1].c_str()).toLong();
				else if (prop[1].is_empty() == false)
					prop[1] >> depth;
			}
			else if (prop[1].is_empty() == false)
				prop[1] >> depth;
			vector<DeviceAttributeHistory> *hist;
            pinfo("attribute history for source \"%s\" has depth depth: %ld\n", ainfo.name.c_str(), depth);
			hist = device->attribute_history(s_attr_command, depth);
			for (unsigned int i = 0; i < (*hist).size(); i++)
			{
				TVariant historyElement((*hist)[i], ainfo, realSource());
				d_triggerRefresh(historyElement);
			}
			if((*hist).size() && dynamic_cast<EPlotLightBase *>(parent()))
			  dynamic_cast<EPlotLightBase *>(parent())->refresh();
			/* It is the caller responsibility to delete memory -- tango manual -- */
			delete hist;
		} 
		catch (DevFailed &e) 
		{ 
			TLog failLog(e);
			TUtil::instance()->addLog(failLog.toTimeErrlistPair(), failLog.formatMessage());
			perr("PlotComProxyReader: failed to get attribute history for \"%s\": this won't be available",
				      qstoc(source()));
		}
	}
	else
	  perr("data type is not attribute and data format is not SCALAR: fill from history impossible");
	d_filledFromHistory = true;
}








