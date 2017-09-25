#ifndef RIGHT_CLICK_EVENT_H
#define RIGHT_CLICK_EVENT_H

#include <QObject>

class QTangoWidgetCommon;
class RightClickEventPrivate;

class RightClickEvent : public QObject
{
  Q_OBJECT
  public:
	RightClickEvent(QTangoWidgetCommon *wCommon);
	
  protected slots:
	void copy();
        void copyPlainText();
	void paste();
	void launchHelperApplication();
	void viewTrend();
	void openHdbLink();
	void showInfoBrowser();
	
   protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
  private:
    QTangoWidgetCommon *d_parentWidgetCommon;

    RightClickEventPrivate *d_ptr;
	
};


#endif
