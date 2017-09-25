#include "widget_cleaner.h"
#include <tutil.h>

void QTangoWidgetCleaner::clean()
{
	TUtil::instance()->cleanup();	
}
