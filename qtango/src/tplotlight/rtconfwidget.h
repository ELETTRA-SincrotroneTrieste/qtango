#ifndef RTCONFWIDGET_H
#define RTCONFWIDGET_H

#include "ui_rtconfwidget.h"

class RtConfWidgetPrivate;

class RtConfWidget : public QWidget
{
  Q_OBJECT
  public:
	  RtConfWidget(QWidget *parent);
	  
  private slots:
	void modeChanged(int m); 
	void apply();
	
  private:
	/* initializes widgets from Qt metaProperties of the parent */
	void init();
	
  Ui_RtConfWidget ui;
  QWidget *d_plotWidget;

  RtConfWidgetPrivate *d_ptr;
};

#endif
