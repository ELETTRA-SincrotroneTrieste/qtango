#include "spectrumwriter.h"
#include "spectrumwriter_conf_helper.h"
#include <QVariant>
#include <QVector>
#include <QObject>
#include <QtDebug>

SpectrumWriter::~SpectrumWriter()
{
	delete d_cfgHelper;
}

SpectrumWriter::SpectrumWriter(QObject *parent) : QTangoComProxyWriter(parent)
{
  d_cfgHelper = new ConfigHelper();
  setAutoConfiguration(true);
  QObject::connect(qtangoComHandle(), SIGNAL
		(attributeAutoConfigured(const TangoConfigurationParameters *)), d_cfgHelper,
		  SLOT(configure(const TangoConfigurationParameters *)));
}

QList<TVariant> SpectrumWriter::execute(QVariant v)
{
  Q_UNUSED(v);
  QList <QVariant> values;
  QList<TVariant> ret;
  foreach(QObject * obj, d_proxyList)
  {
	SimpleDataProxy *sdp = dynamic_cast<SimpleDataProxy *>(obj);
    if(sdp)
        values << QVariant(sdp->getData());
  }
  if(values.size())
  {
  	QVariant variant(values);
	qDebug() << "execute con " << values;
	ret = QTangoComProxyWriter::execute(variant);
  }
  return ret;
}

void SpectrumWriter::addElement(QObject *proxy)
{
  if(dynamic_cast<SimpleDataProxy *>(proxy))
	d_proxyList.append(proxy);
  else
	perr("SpectrumWriter::addElement(): cannot add an object (\"%s\") which is not a SimpleDataProxy",
	  qstoc(proxy->objectName()));
}

void SpectrumWriter::setElements(const QList <QObject *>& list)
{
  QList <QObject *>copyCheck;
  foreach(QObject *o, list)
  {
	SimpleDataProxy *sdp = dynamic_cast<SimpleDataProxy *>(o);
	if(sdp)
	  copyCheck << o;
	else
	  perr("SpectrumWriter::setElements(): cannot add an object (\"%s\") which is not a SimpleDataProxy",
		qstoc(o->objectName()));
  }
  d_proxyList = copyCheck;
}

void SpectrumWriter::removeElement(QObject *proxy)
{
  if(d_proxyList.contains(proxy))
	d_proxyList.removeAll(proxy);
}

void SpectrumWriter::clearTargets()
{
  d_cfgHelper->clearElements();
  QTangoComProxyWriter::clearTargets();
}

void SpectrumWriter::setTargets(QString target)
{
	qprintf("SpectrumWriter: setto targets \"%s\"\n", qstoc(target));
	if(!d_proxyList.isEmpty())
	{
	  d_cfgHelper->setAttributeName(target);
	  QList <SimpleDataProxy *>proxyList;
	  foreach(QObject *o, d_proxyList)
		proxyList << dynamic_cast<SimpleDataProxy *>(o);
	  d_cfgHelper->registerElements(proxyList);
	  QTangoComProxyWriter::setTargets(target);
	}
	else
	  pwarn("SpectrumWriter: proxy list is empty: won't configure any target now");
}

