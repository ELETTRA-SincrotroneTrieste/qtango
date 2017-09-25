#ifndef TIMING_WIDGET_H
#define TIMING_WIDGET_H

#include <QWidget>

class TimingWidget : public QWidget
{
	Q_OBJECT
	public:
		TimingWidget(QWidget *parent);
		void setSource(QString s);
		
	protected slots:
		void changeRefresh();
		void enableWaitRefresh(bool);
	
	private:

};


#endif
