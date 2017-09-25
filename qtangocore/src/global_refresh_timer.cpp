#include "global_refresh_timer.h"
#include <QTimer>
#include <QtDebug>
#include <QTime>
#include <macros.h>
#include <Stats>

GlobalRefreshExternalTimer::GlobalRefreshExternalTimer(QObject *parent, int milliseconds) : QThread(parent)
{
        setObjectName("Global Refresh Timer");
	d_timer = NULL;
	d_timerOff = false;
	d_timerInterval = milliseconds;
	qDebug() << "GlobalRefreshExternalTimer::GlobalRefreshExternalTimer() -- costruttore --- " << this << QThread::currentThread();
}

void GlobalRefreshExternalTimer::emitTimeout()
{
	qDebug() << "GlobalRefreshExternalTimer::emitTimeout()" << this << QThread::currentThread() << QTime::currentTime().toString("hh:mm:ss:zzz");
	/* emitted inside thread because the slot was directly connected inside run() */
	emit timeout();
}

void GlobalRefreshExternalTimer::run()
{
	ADD_READ_STAT_MSG("GlobalRefreshTimer", "starting thread", STATS_OBJINFO(this));
	qDebug() << "GlobalRefreshExternalTimer::run() timer creatinig and living here" << this << QThread::currentThread();
	d_timer = new QTimer(0);
	d_timer->setInterval(d_timerInterval);
	d_timer->setSingleShot(false);
	/* since the connection is direct, the signal is emitted in this thread. The user of the 
	 * timeout signal will have to use queued connection.
	 */
	connect(d_timer, SIGNAL(timeout()), this, SLOT(emitTimeout()), Qt::DirectConnection);
	/* the signals in the following two connects are emitted in the main thread, so queue the slot execution,
	 * since d_timer lives in this thread.
	 */
	connect(this, SIGNAL(changeInterval(int)), d_timer, SLOT(start(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(stopTimer()), d_timer, SLOT(stop()), Qt::QueuedConnection);
	if(!d_timerOff)
		d_timer->start();
	exec();
	pok("global refresh timer: stopping");
	d_timer->stop();
	delete d_timer;
}

bool GlobalRefreshExternalTimer::running()
{
	if(d_timer)
		return d_timer->isActive();
	return false;
}

int GlobalRefreshExternalTimer::interval()
{
	return d_timerInterval;
}

/* also starts timer triggering changeInterval() signal ! */
void GlobalRefreshExternalTimer::setInterval(int ms)
{
	pinfo("setting global refresh external timer interval from %d to %d", d_timerInterval, ms);
	d_timerInterval = ms;
	emit changeInterval(ms);
	d_timerOff = false;
}

void GlobalRefreshExternalTimer::stopRefreshing()
{
	/* might be invoked before timer starts */
	d_timerOff = true;  /* prevents timer start in run() */
	if(d_timer != NULL && d_timer->isActive())
	{
		pok("stopping external refresh trigger");
		emit stopTimer();
	}
	else
		pwarn("external refresh is not active: not stopping it");
}


