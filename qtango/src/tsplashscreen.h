#ifndef TSPLASHSCREEN_H
#define TSPLASHSCREEN_H

#include <QSplashScreen>

class TSplashScreen : public QSplashScreen
{
  Q_OBJECT
  public:
	TSplashScreen(QWidget * parent = NULL, const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0 );
};


#endif
