#include "copypaste_xml.h"
#include <macros.h>
#include <QDomDocument>
#include <QtXml/QDomElement>
#include <proxy_interface.h>
#include <source_metadata.h>
#include <QMetaObject>
#include <QStringList>
#include <QObject>

CopyPasteXml::CopyPasteXml()
{
  
}

QString CopyPasteXml::encode(QTangoWidgetCommon *w)
{
  QString xml;
  /* see if the parent is a reader or a writer */
  ProxyInterface *iProxy = dynamic_cast<ProxyInterface *>(w);
  QObject *object = dynamic_cast<QObject *>(w);
  SourceMetadata *sourceMetadata = w->sourceMetadata();

  if(!object || !iProxy || !sourceMetadata)
  {
	perr("CopyPasteXml: error: not a proxyInterface (0x%p) or an object (0x%p) or a sourceMetadata (0x%p)",
		 iProxy, object, sourceMetadata);
	return QString();
  }
  QString objType = object->metaObject()->className();
  QString tangoSrc = iProxy->tangoPoint();
  QDomDocument doc("qtangoCopyPasteXml");
  QDomProcessingInstruction processingInstruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
  doc.appendChild(processingInstruction);
  QDomElement widgetEl = doc.createElement("widget");
  doc.appendChild(widgetEl);
  widgetEl.setAttribute("class", objType);
  widgetEl.setAttribute("name", object->objectName());
  
  /* save d_widgetElement */
  d_widgetElement = widgetEl;
  
  /* repeat class and object name into properties to allow simple use of getProperty() */
  const QStringList propertyNames = QStringList() << "source" << "owner" << "objectName" << "class";
  const QStringList propertyValues = QStringList() << tangoSrc << sourceMetadata->owner() 
	<< object->objectName() << objType;
  
  for(int i = 0; i < propertyNames.size(); i++)
  {
	QString propertyName = propertyNames.at(i);
	QDomElement prop = doc.createElement("property");
	prop.setAttribute("name", propertyName);
    widgetEl.appendChild(prop);
	QDomElement propStringEl = doc.createElement("string");
    prop.appendChild(propStringEl);
	QDomText txt = doc.createTextNode(propertyValues.at(i));
	propStringEl.appendChild(txt);
  }
  /* get metadata and set mime data */
  xml = doc.toString();
  return xml;
}

QString CopyPasteXml::getProperty(const QString &xml, const QString &propertyName)
{
  QDomDocument doc("qtangoCopyPasteXml");
  if(doc.setContent(xml))
  {
	QDomNodeList propertyNodes = doc.elementsByTagName("property");
	for(int i = 0; i < propertyNodes.size(); i++)
	{
	  QDomElement element = propertyNodes.at(i).toElement();
	  if(!element.isNull() && element.hasAttribute("name") && element.attribute("name") == propertyName)
	  {
		QDomElement stringEl = propertyNodes.at(i).firstChildElement("string");
		if(!stringEl.isNull())
		  return stringEl.text();
	  }
	}
  }
  return QString();
}

QStringList CopyPasteXml::propertyList(const QString& xml)
{
  QStringList properties;
  QDomDocument doc("qtangoCopyPasteXml");
  if(doc.setContent(xml))
  {
	QDomNodeList propertyNodes = doc.elementsByTagName("property");
	for(int i = 0; i < propertyNodes.size(); i++)
	{
	  QDomElement element = propertyNodes.at(i).toElement();
	  if(!element.isNull() && element.hasAttribute("name"))
		properties << element.attribute("name");
	}
  }
  return properties;
}


