#ifndef TEXT_BROWSER
#define TEXT_BROWSER

#include <QTextBrowser>

class TextBrowser : public QTextBrowser
{
   Q_OBJECT
   public:
      TextBrowser(QWidget *parent);
      
   public slots:
      void initHtml();
};

#endif
