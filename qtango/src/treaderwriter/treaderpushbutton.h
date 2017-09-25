#ifndef TREADER_PUSH_BUTTON_H
#define TREADER_PUSH_BUTTON_H

#include <QDialog>
#include <QPushButton>

class QTimer;

class TReaderPushButton : public QPushButton
{
	Q_OBJECT
	public:
		TReaderPushButton(QWidget *parent);

		void delayHide();
		
		void setTimerInterval(int to);
		int timerInterval();
		
	public slots:
		void scheduleHide();
		
	protected:
		void enterEvent(QEvent *e);
		void leaveEvent(QEvent *e);
		
	private:
		QTimer *d_hideTimer;
		
	private slots:
		void hideIfUnchecked();
};


#endif
