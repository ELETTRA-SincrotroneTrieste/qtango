#ifndef GLOBAL_EXT_TIMER_H
#define GLOBAL_EXT_TIMER_H

#include <QThread>
#include <QEvent>
#include <qtangocore_common.h>

class QTimer;

class GlobalRefreshExternalTimer : public QThread
{
	Q_OBJECT
	public: 
		GlobalRefreshExternalTimer(QObject *parent, int milliseconds);
		
		void setInterval(int ms);
		int interval();
		
		void stopRefreshing();
		
		bool running();
		
	protected:
		
		void run();
		
	private:
		int d_timerInterval;
		QTimer *d_timer;
		bool d_timerOff;
		
	private slots:
		void emitTimeout();
		
	signals:
		void timeout();
		void changeInterval(int);
		void stopTimer();
};












#endif 
