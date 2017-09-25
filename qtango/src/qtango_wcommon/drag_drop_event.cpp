#include <com_proxy_reader.h>
#include <com_proxy_writer.h>
#include <elettracolors.h>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QWidget>
#include <QPainter>
#include <QStringList>
#include <QApplication>

#include "drag_drop_event.h"
#include "source_metadata.h"
#include "qtango_wcommon.h"
#include "copypaste_xml.h"

DragEvent::DragEvent(QObject *parent) : QObject(parent)
{
  if(qobject_cast<QWidget *>(parent))
  {
	qobject_cast<QWidget *>(parent)->setMouseTracking(true);
  }
}

bool DragEvent::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
  if(event->type() == QEvent::DragEnter)
  {
	QDragEnterEvent *dev = static_cast<QDragEnterEvent *>(event);
	dev->acceptProposedAction();
	  return true; /* stop handling event here */
  }
  else if(event->type() == QEvent::MouseButtonPress)
  {
	QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	if(ev && ev->button() == Qt::LeftButton && ev->modifiers() == Qt::NoModifier)
	  d_dragStartPos = ev->pos();  
  }
  else if(event->type() == QEvent::MouseMove)
  {
	QMouseEvent *ev = static_cast<QMouseEvent *>(event);
	QWidget *parentWidget = dynamic_cast<QWidget *>(parent());
	if ( !parentWidget || !ev || !(ev->buttons() & Qt::LeftButton) || ev->modifiers() != Qt::NoModifier)
         return false;
     if ((ev->pos() - d_dragStartPos).manhattanLength() < QApplication::startDragDistance())
         return false;

     QDrag *drag = new QDrag(parentWidget);
     QMimeData *mimeData = new QMimeData;
	 QString data;
	
	 QTangoWidgetCommon *qtwCommon = dynamic_cast<QTangoWidgetCommon *>(parent());
	 if(qtwCommon)
	 {
	   data = qtwCommon->formatCopyText();
	   mimeData->setText(data);
	   drag->setMimeData(mimeData);
	   QString source;
	   CopyPasteXml xmlDec;
	   source = xmlDec.getProperty(data, "source");
	   QFont::insertSubstitutions("Sans Serif", QStringList() << "Arial Black" << "Verdana" << "Monospace");
	   QFont f = parentWidget->font();
	   f.setBold(true);
	   f.setFamily("Sans Serif");
	   f.setPointSize(12);
	   QFontMetrics fontMetrics(f);
	   QSize fontSize(fontMetrics.width(source), fontMetrics.height());
	   QPixmap dragPix(fontSize);
	   QColor pixColor = Qt::white;
	   pixColor.setAlpha(50);
	   dragPix.fill(pixColor);
	   QPainter painter(&dragPix);
	   painter.setRenderHints(QPainter::TextAntialiasing);
	   painter.setPen(QPen(KBLUE));
	   painter.setFont(f);
	   painter.drawText(dragPix.rect(), Qt::AlignLeft|Qt::AlignVCenter, source);
	   drag->setPixmap(dragPix);
       drag->exec(Qt::CopyAction | Qt::MoveAction);
	   return true;
	 }
  }
  return false; /* do not filter out the event */
}

DropEvent::DropEvent(QObject *parent) : QObject(parent)
{
  if(qobject_cast<QWidget *>(parent))
  {
	qobject_cast<QWidget *>(parent)->setAcceptDrops(true);
  }
}

bool DropEvent::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
  if(event->type() == QEvent::Drop && parent())
  {
	QDropEvent *dropEvent = static_cast<QDropEvent *>(event);
	QString source;
	QStringList data;
	QTangoWidgetCommon *wCommon;
	
	/* dropEvent->source() NULL means that the drag was started from another application.
	 * parentWidget is the widget that created the drag event (see DragEvent::eventFilter()): it must
	 * be different from our parentWidget: i.e. refuse drops coming from ourselves.
	 */
	if(dropEvent  &&  (dropEvent->source() == NULL || dropEvent->source() != dynamic_cast<QWidget *>(parent())))
	{
	  if(dropEvent->mimeData()->hasText())
	  {
		source = dropEvent->mimeData()->text();
		wCommon = dynamic_cast<QTangoWidgetCommon *>(parent());
		if(wCommon)
		{
		  source = wCommon->decodePasteData(source);
		  wCommon->changeTangoPoint(source);
		}
	  }
	 
	  return true;
	} 
	else
	  pinfo("DropEvent::eventFilter(): refusing drop: source 0x%p\n", dropEvent->source());
  }

    return false;
}




