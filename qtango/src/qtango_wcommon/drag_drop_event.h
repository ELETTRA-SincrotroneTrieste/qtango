#ifndef DRAG_DROP_EVENT_H
#define DRAG_DROP_EVENT_H

#include <QObject>
#include <QStringList>
#include <macros.h>
#include <QEvent>
#include <QPoint>

class DragEventPrivate;
class DropEventPrivate;

class DragEvent : public QObject 
{
  Q_OBJECT
  public:
	DragEvent(QObject *parent);
	
  protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
  private:
	QPoint d_dragStartPos;

    DragEventPrivate *d_ptr;
};

class DropEvent : public QObject 
{
  Q_OBJECT
  public:
	DropEvent(QObject *parent);
	
  protected:
	
	/** \brief the drag/drop event filter
	 *
	 * Drag and drop events are managed here. Finally, in the drop case, changeSource() is invoked, to
	 * actually set the source or target. 
	 * @see changeSource
	 * You might want to reimplement changeSource() in a derived class to specialize the setSource/Target,
	 * as demonstrated in QTango TPlotLightMarker implementation.
	 */
	bool eventFilter(QObject *obj, QEvent *event);
	
  private:

    DropEventPrivate *d_ptr;
	
};

#endif
