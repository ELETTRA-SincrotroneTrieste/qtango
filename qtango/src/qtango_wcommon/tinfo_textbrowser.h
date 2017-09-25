#ifndef POPUP_TEXT_BROWSER_H
#define POPUP_TEXT_BROWSER_H

#include <QTextBrowser>

class QTimer;
class QTangoWidgetCommon;
class ProxyInterface;
class InfoWidget;
class TInfoTextBrowserPrivate;

/** \brief a text browser used to display html formatted information on the tango source represented 
 *         by a QTango widget or custom descriptions personalized by the programmer.
 *
 * This is a class exported also as a Qt Designer plugin.
 * It receives text updates via the <em>InfoNotificationCenter</em> class, which is a singleton
 * where each TInfoTextBrowser registers itself at creation time.
 * Create an instance of this class, or drag and drop it from the designer, to have a working text
 * browser displaying html information for every QTango widget in your application.
 * Alt+Left click of the mouse on a widget also instantiates a TInfoTextBrowser with the widget
 * information.
 * @see InfoWidget
 * @see InfoNotificationCenter
 *
 * InfoWidget defines an interface to update messages on the TInfoTextBrowser when the mouse enters
 * a QTango widget.
 */
class TInfoTextBrowser : public QTextBrowser
{
  Q_PROPERTY(QString defaultMessage  READ defaultMessage WRITE setDefaultMessage DESIGNABLE true)
  
  Q_OBJECT
  public:
	
	enum pages { MAIN, AINFO };
	
	TInfoTextBrowser(QWidget *parent, Qt::WindowFlags wf = 0);
	~TInfoTextBrowser();
	
	/** \brief tells the TInfoTextBrowser to refresh pointing at the passed object
	 *
	 * @param object the object to retrieve information from. It <strong>must</strong> be a
	 * ProxyInterface and a QTangoWidgetCommon.
	 */
	void linkToSource(QObject *object);

	/** \brief provides the default message displayed when the info browser is not linked to any source
	 *
	 * @return the default html formatted message displayed when the info browser is not linked to any source.
	 *
	 * You can change it with setDefaultMessage()
	 * @see setDefaultMessage
	 *
	 */
	QString defaultMessage() { return d_defaultMessage; }
	
  public slots:
	
	/* Impossible to disable */
	void setDisabled(bool) { QTextBrowser::setDisabled(false); }
	
	void setEnabled(bool) { QTextBrowser::setEnabled(true); }
	
	void processClick(const QUrl&);
	
	/** \brief change the info browser default html text
	 *
	 * @param s the html formatted string to display as default message when the browser
	 *          is not linked to any tango point
	 */
	void setDefaultMessage(const QString &s) { d_defaultMessage = s; }
	
	/** \brief sets the html string s inserting it into html and body tags and providing a
	 *         stylesheet.
	 *
	 * Reimplements QTextEdit::setHtml(), adding a header to include a stylesheet and inserting
	 * the html formatted parameter s into html and body tags.
	 *
	 * @param s the html formatted string to set into the browser.
	 */
	void setHtml(const QString &s);

        /** \brief unlinks the current linked object from the browser info updates.
         *
         */
        void unlink();

	
  protected slots:
	/* reimplemented to eat QTextBrowser::setSource() */
	virtual void setSource(const QUrl &name);
	
	/* update the text according to the current page */
	void update();
	
  protected:
	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	
  private:
	QTimer *d_updateTimer;
	
	QTangoWidgetCommon *d_wCommon;
	ProxyInterface *d_proxyInterface;
	InfoWidget *d_infoWidget;
	
	QString menu();
	void d_setHtml(const QString& s);
	QString d_defaultMessage;
	int d_page;
	
  private slots:
	void updateTextMain();
	void updateTextAttributeInfo();

private:
    TInfoTextBrowserPrivate *d_ptr;
	
};

#endif
