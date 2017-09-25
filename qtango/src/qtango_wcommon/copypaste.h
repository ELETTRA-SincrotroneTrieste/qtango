#ifndef COPY_PASTE_H
#define COPY_PASTE_H

class QWidget;
class CopyPastePrivate;

#include <QString>

/* unescaped source regexp */
/*
  (([A-Za-z_0-9\.]*[:]{1}[0-9]+[/])?(([A-Za-z_0-9\.]+/[A-Za-z_0-9\.]+/[A-Za-z_0-9\.]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9\.]+)|(\$[1-9]*([/]{1,1}|[->]{2})[A-Za-z_0-9\.]+)){1}([\(]{1}[&A-Za-z_0-9\.,]+[\)]{1})?[;]?){1,}
 */

#define SOURCE_REGEXP "(([A-Za-z_0-9\\.]*[:]{1}[0-9]+[/])?(([A-Za-z_0-9\\.]+/[A-Za-z_0-9\\.]+/[A-Za-z_0-9\\.]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9\\.]+)|(\\$[1-9]*([/]{1,1}|[->]{2})[A-Za-z_0-9\\.]+)){1}([\\(]{1}[&A-Za-z_0-9\\.,]+[\\)]{1})?[;]?){1,}"

class CopyPaste
{
  public:
	
	CopyPaste(QWidget *parent);
	
	void copy();
        void copyPlainText();
	void paste();
	
	void setCopyActionEnabled(bool en) { d_copyActionEnabled = en; }
	void setPasteActionEnabled(bool en) { d_pasteActionEnabled = en; };
	bool copyActionEnabled() { return d_copyActionEnabled; }
	bool pasteActionEnabled() { return d_pasteActionEnabled; }
	
  private:
	bool d_copyActionEnabled, d_pasteActionEnabled;
	QWidget *d_parentWidget;
	
	QString textInClipboard();

    CopyPastePrivate *d_ptr;
};


#endif
