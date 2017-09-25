#include "treaderpushbutton.h"

#include <QHBoxLayout>
#include <QTimer>

TReaderPushButton::TReaderPushButton(QWidget *parent) : QPushButton(parent)
{
	setIcon(QIcon(":icons/configure.png"));
	setFlat(false);
	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	setCheckable(true);
	setChecked(false);
	setStyleSheet("QPushButton { border:none; border-radius:0px; background-color:transparent; max-width:16px; }");
	setToolTip("Click to change the value");
	d_hideTimer = new QTimer(this);
	d_hideTimer->setSingleShot(true);
	d_hideTimer->setInterval(1000);
	connect(d_hideTimer, SIGNAL(timeout()), this, SLOT(hideIfUnchecked()));
	d_hideTimer->start();
	setHidden(true);
	setCursor(QCursor(Qt::PointingHandCursor));
}

void TReaderPushButton::setTimerInterval(int to)
{
	d_hideTimer->setInterval(to);
}

int TReaderPushButton::timerInterval()
{
	return d_hideTimer->interval();
}

void TReaderPushButton::enterEvent(QEvent *)
{
	d_hideTimer->stop();
// 	QWidget::enterEvent(e);
}

void TReaderPushButton::leaveEvent(QEvent *)
{
	d_hideTimer->start();
// 	QWidget::leaveEvent(e);
}

void TReaderPushButton::hideIfUnchecked()
{
	if(!isChecked())
	{
		hide();
	}
}

void TReaderPushButton::delayHide()
{
	d_hideTimer->stop();
}

void TReaderPushButton::scheduleHide()
{
	d_hideTimer->start();
}


