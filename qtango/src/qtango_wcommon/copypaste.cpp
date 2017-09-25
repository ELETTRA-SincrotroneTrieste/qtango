#include "copypaste.h"
#include "qtango_wcommon.h"
#include <QApplication>
#include <QClipboard>
#include <QDomDocument>
#include <QWidget>
#include <macros.h>

CopyPaste::CopyPaste(QWidget *parent)
{
  d_copyActionEnabled = true;
  d_pasteActionEnabled = false;
  d_parentWidget = parent;
}

QString CopyPaste::textInClipboard()
{
  QString txt;
  QClipboard *clipB = QApplication::clipboard();
  txt = clipB->text();
  /* perform a minimal validation */
  QDomDocument doc("CopyPaste");
  if(doc.setContent(txt))
  {
	if(txt.contains("source"))
	  return txt;
  }
  else if(txt.contains(QRegExp(SOURCE_REGEXP)))
	return txt;
	
  return QString();
}

void CopyPaste::copyPlainText()
{
  QClipboard *clip;
  ProxyInterface *pri = dynamic_cast<ProxyInterface *>(d_parentWidget);
  QString cpTxt;
  if(pri)
  {
        cpTxt = pri->tangoPoint();
        clip = QApplication::clipboard();
        clip->setText(cpTxt);
  }
}


void CopyPaste::copy()
{
  QClipboard *clip;
  QTangoWidgetCommon *qtw = dynamic_cast<QTangoWidgetCommon *>(d_parentWidget);
  QString cpTxt;
  if(qtw)
  {
	cpTxt = qtw->formatCopyText();
	clip = QApplication::clipboard();
	clip->setText(cpTxt);
  }
}

void CopyPaste::paste()
{
  QTangoWidgetCommon *qtw = dynamic_cast<QTangoWidgetCommon *>(d_parentWidget);
  QString txt = textInClipboard();
  if(!txt.isEmpty() && qtw)
  {
	QString source = qtw->decodePasteData(txt);
	if(!source.isEmpty())
	{
	  pinfo("CopyPaste: paste: changing tango point to \"%s\"", qstoc(source));
	  qtw->changeTangoPoint(source);
	}
  }
  else
	pinfo("CopyPaste: paste: not changing tango point");
}








