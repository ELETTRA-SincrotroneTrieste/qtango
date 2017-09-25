#include "tspectrumbutton.h"
#include <QMessageBox>
#include <QTimer>

TSpectrumButton::TSpectrumButton(QWidget *parent) : QPushButton(parent), SpectrumWriter(this), QTangoWidgetCommon(this)
{
  /* simply connect the clicked() signal to the writeValues SLOT */
  connect(this, SIGNAL(clicked()), this, SLOT(execute()));
  setAutoConfiguration(true); /* to obtain TangoConfigurationParameters in handle and export them for info text browser */
  setText("Apply Values");
  d_configured = false;
}

void TSpectrumButton::execute()
{
  /* simply execute the execute() method of the SpectrumWriter class */
  SpectrumWriter::execute();
}

void TSpectrumButton::activate()
{
  QString notFound;
  QList<QObject *>dataProxies;
  foreach(QString s, d_objectNameList)
  {
	SimpleDataProxy *dp = NULL;
	QWidget *parent = parentWidget();
	QWidget *widget;
	while(parent != NULL && dp == NULL)
	{
	  qDebug() << "parent children" << parent->findChildren<QWidget*>();
	  qprintf("TSpectrumButton::activate(): searching in parent \"%s\"\n", qstoc(parent->objectName()));
	  widget = parent->findChild<QWidget *>(s);
	  if(widget)
	  {
		dp = dynamic_cast<SimpleDataProxy* >(widget);
		if(dp)
		  qprintf("TSpectrumButton::activate(): found %s\n", qstoc(widget->objectName()));
		else
		  qprintf("TSpectrumButton::activate():  %s isn't a SimpleDataPRoxy!\n", qstoc(widget->objectName()));
	  }
	  parent = parent->parentWidget();
	}
	if(!dp) /* no data proxy with the provided name */
	{
	  notFound = s;
	  break;
	}
	else
	  dataProxies << widget;
  }
  if(!notFound.isEmpty() && !d_objectNameList.isEmpty())
  {
	perr("SimpleDataProxy with name \"%s\" not found", qstoc(notFound));
	QMessageBox::critical(this, "Error: object not found", QString("The object with name \"%1\" was not found "
	  "in one of the parents of\n\"%2\" or it was not a SimpleDataProxy\n"
	  "Note: this is normal if you are previewing on the designer").arg(notFound).arg(objectName()));
  }
  else if(!d_objectNameList.isEmpty()) /* all objects were found */
  {
	setElements(dataProxies);
	/* since this will be a designer plugin, and the calling order of setObjectNameList and setTargets is not 
	 * guaranteed, we have to call setTargets() on the SpectrumWriter after setObjectNameList().
	 * On the other hand, if setTargets() is called by the designer _after_ setElements(), there will be 
	 * a double call to setTargets(), which the library manages by itself.
	 */
	if(!d_targets.isEmpty() && SpectrumWriter::targets() != d_targets)
	{
	  pinfo("SpectrumButton's setObjectNameList calls set targets (\"%s\") on the writer \"%s\"", qstoc(targets()), qstoc(objectName()));
	  SpectrumWriter::setTargets(targets());
	}
	else
	  pinfo("SpectrumButton: setObjectNameList: targets not already configured for \"%s\"", qstoc(objectName()));
  }
}

void TSpectrumButton::setTargets(const QString &targets)
{
  qprintf("SpectrumButton:set targets con %s\n", qstoc(targets));
  if(targets != d_targets)
  {
	d_targets = targets;
	/* activate only if object name list is not empty */
	if(!d_objectNameList.isEmpty())
	  QTimer::singleShot(1000, this, SLOT(activate()));
  }
}

void TSpectrumButton::setObjectNameList(const QStringList &objNameList)
{ 
  if(objNameList != d_objectNameList)
  {
	qprintf("SpectrumButton: list: "); qslisttoc(objNameList);
	d_objectNameList = objNameList;
	QTimer::singleShot(1000, this, SLOT(activate()));
  }
}

void TSpectrumButton::addElement(QObject *proxy)
{
  connect(proxy, SIGNAL(destroyed(QObject *)), this, SLOT(dataProxyDeleted(QObject *)));
  SpectrumWriter::addElement(proxy);
}

void TSpectrumButton::setElements(const QList <QObject *>& list)
{
  foreach(QObject *o, list)
	  connect(o, SIGNAL(destroyed(QObject *)), this, SLOT(dataProxyDeleted(QObject *)));
  SpectrumWriter::setElements(list);
}

void TSpectrumButton::dataProxyDeleted(QObject *o)
{
  if(d_objectNameList.contains(o->objectName()))
  {
	pinfo("TSpectrumButton: object \"%s\" deleted: removing from list", qstoc(o->objectName()));
	d_objectNameList.removeAll(o->objectName());
	removeElement(o);
  }
}
