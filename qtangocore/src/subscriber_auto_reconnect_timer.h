#ifndef SUBSCRIBER_AUTORECONNECT_TIMER_H
#define SUBSCRIBER_AUTORECONNECT_TIMER_H

#include <QTimer>
#include <QList>
#include <QString>

class SubscriberAutoReconnectTimer : public QTimer
{
  Q_OBJECT
  public:
	SubscriberAutoReconnectTimer(QObject *parent, const QString& source);
	~SubscriberAutoReconnectTimer();
	
	void setTimeouts(const QList<int> &tos) { d_timeos = tos; }
	int attemptsNo() { return d_attemptNo; }
	void setSourceName(const QString &sname) { d_sourceName = sname; }
  
  public slots:
	void scheduleRetry();
	void unscheduleRetry();
	
  signals:
	void retry();
	
  private slots:
	void slotTimeout();
	
  private:
	int d_attemptNo;
	QList<int> d_timeos;
    QString d_sourceName;
};

#endif
