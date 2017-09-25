#include "twidgetgroup.h"
#include <com_proxy_reader.h>
#include <QTimer>
#include <QObject>
#include <QMessageBox>

TWidgetGroup::TWidgetGroup(QWidget *parent) :  QGroupBox(parent), QTangoWidgetCommon(this), ProxyInterface()
{
	d_readersGroup = new ReadersGroup(this);
	d_designerMode = false;
        setTitle("Widget Group");
}

void TWidgetGroup::setDesignerMode(bool en)
{
  foreach(QObject *reader, d_readersGroup->readers())
  {
	if(en)
	  pinfo("setting designer mode on reader \"%s\"", qstoc(reader->objectName()));
	if(dynamic_cast<QTangoComProxyReader *> (reader))
	  dynamic_cast<QTangoComProxyReader *> (reader)->setDesignerMode(en);
  }
  d_designerMode = en;
}

void TWidgetGroup::activate()
{
  QString notFound;
  QList<QWidget *> readers;
  foreach(QString s, d_objectNameList)
  {
	QWidget *suitableReaderWidget = NULL;
	QWidget *parent = parentWidget();
	while(parent != NULL && suitableReaderWidget == NULL)
	{
	  suitableReaderWidget = parent->findChild<QWidget *>(s);
	  if(dynamic_cast<QTangoComProxyReader* >(suitableReaderWidget))
	  {
		qprintf("found %s\n", qstoc(suitableReaderWidget->objectName()));
		QTangoWidgetCommon *wCommon = dynamic_cast<QTangoWidgetCommon *>(suitableReaderWidget);
		if(wCommon)
		{
		  pinfo("disabling drag and drop, copy and paste and view trend on child readers");
		  wCommon->setDropEnabled(false);
		  wCommon->setCopyActionEnabled(false);
		  wCommon->setPasteActionEnabled(false);
		  wCommon->setViewTrendEnabled(false);
		}
	  }
	  else
		suitableReaderWidget = NULL;
	  parent = parent->parentWidget();
	}
	if(!suitableReaderWidget) /* no data proxy with the provided name */
	{
	  notFound = s;
	  break;
	}
	else
	  readers << suitableReaderWidget;
  }
  if(!notFound.isEmpty() && !d_objectNameList.isEmpty())
  {
	perr("QTangoComProxyReader with name \"%s\" not found", qstoc(notFound));
	QMessageBox::critical(this, "Error: object not found", QString("The object with name \"%1\" was not found "
	  "in one of the parents of\n\"%2\" or it was not a QTangoComProxyReader\n"
	  "Note: this might be normal if you are previewing on the designer").arg(notFound).arg(objectName()));
  }
  else if(!d_objectNameList.isEmpty()) /* all objects were found */
  {
	for(int i = 0; i < d_objectNameList.size() && i < readers.size(); i++)
	  d_readersGroup->addReader(readers.at(i), i);
	
	/* since this will be a designer plugin, and the calling order of setObjectNameList and setSource is not 
	 * guaranteed, we have to call setSource() on the ReadersGroup after setObjectNameList().
	 * On the other hand, if setSource() is called by the designer _after_ ReadersGroup::addReader(), there will be 
	 * a double call to setSource(), which the library manages by itself.
	 */
	if(!d_source.isEmpty())
	{
	  pinfo("TWidgetGroup's setObjectNameList calls set source (\"%s\") on the writer \"%s\"", qstoc(source()), qstoc(objectName()));
	  d_readersGroup->setSource(d_source);
	}
	else
	  pinfo("TWidgetGroup: setObjectNameList: source not already configured for \"%s\"", qstoc(objectName()));
  }
}

void TWidgetGroup::setSource(const QString &source)
{
  if(source != d_source)
  {
	d_source = source;
	/* activate only if object name list is not empty */
	if(!d_objectNameList.isEmpty())
	  QTimer::singleShot(1000, this, SLOT(activate()));
  }
}

void TWidgetGroup::setObjectNameList(const QStringList &objNameList)
{ 
  if(objNameList != d_objectNameList)
  {
	d_objectNameList = objNameList;
	QTimer::singleShot(1000, this, SLOT(activate()));
  }
}


