#ifndef COPYPASTE_XML_H
#define COPYPASTE_XML_H

#include <qtango_wcommon.h>
#include <QString>
#include <QDomElement>

class CopyPasteXml 
{
  public:
	CopyPasteXml();
	
	/** \brief encodes qtango widget common into an xml string.
	 *
	 * This can be reimplemented to add properties into the xml document.
	 * Retrieve widgetElement() and appendChildren to it if you want to 
	 * encode more properties into the xml document, after calling 
	 * CopyPasteXml::encode() first.
	 * This method sets widgetElement.
	 */
	virtual QString encode(QTangoWidgetCommon *w);
	
	/** \brief Given the xml text and the property name, returns the property value inside the tag.
	 *
	 * @param xml the xml text to decode
	 * @param name the Qt property name to look for inside xml
	 *
	 */
	QString getProperty(const QString& xml, const QString &name);
	
	QDomElement widgetElement() { return d_widgetElement; }
	
	QStringList propertyList(const QString& xml);
	
  private:
	QDomElement d_widgetElement;
};


#endif
