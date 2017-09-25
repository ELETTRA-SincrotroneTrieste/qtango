#include <tutil.h>
#include "tlogbutton.h"
#include <tlogdialog.h>

TLogButton::TLogButton(QWidget* parent) : QPushButton(parent)
{
/*	w = new TLogDialog(this);
	w->changeStack(true);*/
	setText("Show Logs");
	connect(this, SIGNAL(clicked()), this, SLOT(showLogs()));
}

void TLogButton::showLogs()
{
	QApplication::postEvent(TUtil::instance(), new QShowLogsEvent(true));
}

TLogButton::~TLogButton()
{
}

