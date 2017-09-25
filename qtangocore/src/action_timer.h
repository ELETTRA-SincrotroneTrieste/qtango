#ifndef ACTION_TIMER_H
#define ACTION_TIMER_H

#include <QTimer>

/** \brief a timer used by TAction, to prevent intervals of 0 milliseconds.
 *
 */
class ActionTimer : public QTimer
{
  Q_OBJECT
  public:
	ActionTimer(QObject *parent); /* sets a default interval of 1000 ms */
	void setInterval(int msec);
	
  public slots:
	void start(int msec); 
	void start() { QTimer::start(); }
};

#endif
