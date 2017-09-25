#include "tsplashscreen.h"
#include <QPixmap>
#include <QApplication>

TSplashScreen::TSplashScreen(QWidget *parent, const QPixmap & pixmap, Qt::WindowFlags fl) :
  QSplashScreen(parent, pixmap, fl)
{
  QPixmap pix(300,40);
  pix.fill(qApp->palette().color(QPalette::Window));
  setPixmap(pix);
  QFont f = font();
  f.setPointSize(12);
  setFont(f);
  showMessage("Connecting to devices...", Qt::AlignCenter);
  show();
}



