#include "showhide.h"
#include <QWidget>
#include <macros.h>
#include <com_proxy_reader.h>

ShowHide::ShowHide(QWidget *parent)
{
	d_parentWidget = parent;
	Q_ASSERT(d_parentWidget != NULL);
	d_hideEventEnabled = false;
}

void ShowHide::setHideEventEnabled(bool en)
{
   QTangoComProxyReader *reader = dynamic_cast<QTangoComProxyReader *>(d_parentWidget);
   if(reader)
	 reader->enableHideEvents(en);
   d_hideEventEnabled = en;
}








