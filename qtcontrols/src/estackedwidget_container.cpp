#include "estackedwidget_container.h"
#include <macros.h>
#include <QtDebug>
#include <QPaintEvent>


EStackedWidgetContainer::EStackedWidgetContainer(QWidget *parent) : QWidget(parent)
{
  
}

void EStackedWidgetContainer::paintEvent(QPaintEvent *e)
{
  qDebug() << "rectangle to paint: " << e->rect();
  qprintf("ordered widgets number: %d\n", d_orderedWidgets.size());
  qDebug() << "ordered widgets: " << d_orderedWidgets;
  for(int i = 0; i < d_orderedWidgets.size() - 1; i++)
  {
  	qprintf("\e[0;35mstacking widget \"%s\" under widget \"%s\"\e[0m\n", qstoc(d_orderedWidgets.at(i)->objectName()), qstoc(d_orderedWidgets.at(i+1)->objectName()));
	
  	d_orderedWidgets.at(i)->stackUnder(d_orderedWidgets.at(i+1));
	}
  return QWidget::paintEvent(e);
}

void EStackedWidgetContainer::addWidget(QWidget * w, int z)
{
  d_widgetsMap.insert(w, z);
  updateOrderedList();
}

int EStackedWidgetContainer::widgetPriority(QWidget *w)
{
	if(d_widgetsMap.contains(w))
		return d_widgetsMap.value(w);
	return -1;
}

void EStackedWidgetContainer::setWidgetPriority(QWidget *w, int p)
{
	if(d_widgetsMap.contains(w))
	{
		d_widgetsMap[w] = p;
		updateOrderedList();
	}
	else
		pwarn("widget \"%s\" not present, cannot change its priority", qstoc(w->objectName()));
}

void EStackedWidgetContainer::updateOrderedList()
{
	QList<int> zetas = d_widgetsMap.values();
 	qSort(zetas.begin(), zetas.end());
  	d_orderedWidgets.clear();
  	foreach(int i, zetas)
  		d_orderedWidgets << d_widgetsMap.key(i);
}

