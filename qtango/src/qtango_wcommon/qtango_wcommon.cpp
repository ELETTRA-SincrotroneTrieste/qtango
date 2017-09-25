#include <proxy_interface.h>
#include <communication_handle.h>
#include <com_proxy_reader.h>
#include <com_proxy_writer.h>
#include "drag_drop_event.h"
#include "right_click_event.h"
#include "left_click_event.h"
#include "hide_event.h"
#include "show_event.h"
#include "copypaste_xml.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>
#include <QString>
#include <QMetaObject>
#include <QDateTime>
#include <QMetaProperty>

#include "qtango_wcommon.h"

QTangoWidgetCommon::QTangoWidgetCommon(QWidget *parent) :
  QTangoHelperApplication(parent),
  QTangoViewTrend(parent),
  CopyPaste(parent),
  ShowHide(parent),
  InfoWidget(parent),
  QTangoWhatsThis(parent),
  QTangoHdb()
{
  Q_ASSERT(parent != NULL);
  /* By default, communication status is allowed to modify the enabled property of the widget */
  d_wForceDisabled = true;
  d_parentWidget = parent;
  d_dropEnabled = false;
  d_sourceMetadata = new SourceMetadata(parent);
  d_dropEvent = NULL;
  setDropEnabled(d_dropEnabled);
  /* drag event */
  DragEvent *dragEvent = new DragEvent(d_parentWidget);
  d_parentWidget->installEventFilter(dragEvent);
  /* right click event: initializes itself to copy enabled and paste disabled */
  rClickEvent = new RightClickEvent(this);
  d_parentWidget->installEventFilter(rClickEvent);
  /* left click event */
  LeftClickEvent *leftClickEvent = new LeftClickEvent(d_parentWidget);
  d_parentWidget->installEventFilter(leftClickEvent);
  
  /* manage show and hide events */
  ShowEvent *showEvent = new ShowEvent(d_parentWidget);
  d_parentWidget->installEventFilter(showEvent);
  HideEvent *hideEvent = new HideEvent(d_parentWidget);
  d_parentWidget->installEventFilter(hideEvent);


}

QTangoWidgetCommon::~QTangoWidgetCommon()
{
    delete  d_sourceMetadata;
    delete rClickEvent;
}

void QTangoWidgetCommon::enableWidget()
{
    d_parentWidget->setEnabled(true);
}

void QTangoWidgetCommon::disableWidget()
{
    d_parentWidget->setDisabled(true);
}

bool QTangoWidgetCommon::dragEnabled() const
{
    return d_parentWidget->findChild<DragEvent *>() != NULL;
}

void QTangoWidgetCommon::setDragEnabled(bool en)
{
    DragEvent *dragEvent = d_parentWidget->findChild<DragEvent *>();
    if(en && !dragEvent)
    {
        dragEvent = new DragEvent(d_parentWidget);
        d_parentWidget->installEventFilter(dragEvent);
    }
    else if(!en && dragEvent != NULL)
    {
        d_parentWidget->removeEventFilter(dragEvent);
        delete dragEvent;
    }
}

/* this is called from the QTango designer plugins to enable drops in design mode
 */
void QTangoWidgetCommon::enableDrops()
{
    if(!d_dropEnabled && d_parentWidget) /* if not already installed the Drop event */
    {
        d_parentWidget->installEventFilter(new DropEvent(d_parentWidget));
    }
}

void QTangoWidgetCommon::setDropEnabled(bool en)
{
  d_dropEnabled = en;
  /* let's find out whether we are in designer mode */
  bool designerMode = false;
  QTangoComProxyReader *reader = dynamic_cast<QTangoComProxyReader*>(this);
  QTangoComProxyWriter *writer = dynamic_cast<QTangoComProxyWriter*>(this);

  if(reader)
      designerMode = reader->designerMode();
  else if(writer)
      designerMode = writer->designerMode();

  /* if not in designer mode, install the drop event filter.
   * Otherwise, drops are always enabled to allow fast configuration of
   * sources and targets, even from jive >= 4.12
   */
  if(!designerMode)
  {
      if(d_dropEnabled && !d_dropEvent && d_parentWidget)
            d_dropEvent = new DropEvent(d_parentWidget);
      if(d_dropEnabled && d_parentWidget)
            d_parentWidget->installEventFilter(d_dropEvent);
      else if(d_dropEvent)
            d_parentWidget->removeEventFilter(d_dropEvent);
  }
}

QString QTangoWidgetCommon::formatCopyText()
{
  QString xml;
  CopyPasteXml xmlEncoder;
  /* see if the parent is a reader or a writer */
  xml = xmlEncoder.encode(this);
  return xml;
}

void QTangoWidgetCommon::changeTangoPoint(const QString &source)
{
    /* use metaobject + properties instead of dynamic_cast */
  if(d_parentWidget->metaObject()->indexOfProperty("source") > -1)
      d_parentWidget->setProperty("source", source);
  else if(d_parentWidget->metaObject()->indexOfProperty("targets") > -1)
      d_parentWidget->setProperty("targets", source);
}

/* the default implementation updates sourceMetaData and returns the tango source */
QString QTangoWidgetCommon::decodePasteData(const QString& xml)
{
  QString source, owner, objectName;
  CopyPasteXml xmlDecoder;
  source = xmlDecoder.getProperty(xml, "source");
  owner = xmlDecoder.getProperty(xml, "owner");
  objectName = xmlDecoder.getProperty(xml, "objectName");

  if(!owner.isEmpty())
	sourceMetadata()->changeOwner(owner);
  if(!objectName.isEmpty())
	sourceMetadata()->changeObjectName(objectName);

  /* the decoder could not decode the pasted data: maybe it contained simply a source */
  if(source.isEmpty() && xml.contains(QRegExp(SOURCE_REGEXP)))
	source = xml;
  pinfo("QTangoWidgetCommon::decodePasteData: decoded source \"%s\" owner \"%s\" obj name: \"%s\" class \"%s\"",
	qstoc(source), qstoc(owner), qstoc(objectName), qstoc(xmlDecoder.getProperty(xml, "class")));
  return source;
}



