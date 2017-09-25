#include "ttextbrowser.h"
#include <QPalette>
#include <configuration.h>
#include <QScrollBar>

TTextBrowser::TTextBrowser(QWidget *parent) : QTextBrowser(parent),
	QTangoComProxyReader(this),
	QTangoWidgetCommon(this)
{
  setViewTrendEnabled(false);
  setHelperApplicationEnabled(false);
  setHideEventEnabled(true); /* generally enabled for readers */
  d_plainText = true;
  connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)),
	Qt::DirectConnection);
}

void TTextBrowser::refresh(const TVariant &v)
{
  d_hScrollPos = horizontalScrollBar()->value();
  d_vScrollPos = verticalScrollBar()->value();
  QColor color;
  Config *conf = Config::instance();
  QPalette pal = palette();
  if(v.message() != toolTip())
	 setReaderToolTip(v.message());
	
  switch (v.quality())
  {
	 case ATTR_VALID:
	  color = conf->qualityColor(ATTR_VALID);
	  break;
	 case ATTR_ALARM:
	  color = conf->qualityColor(ATTR_ALARM);
	  break;
	 case ATTR_WARNING:
	  color = conf->qualityColor(ATTR_WARNING);
	  break;
	 case ATTR_CHANGING:
	  color = conf->qualityColor(ATTR_CHANGING);
	  break;
	 case ATTR_INVALID:
	  if (palette().color(QPalette::Background) != Qt::white)
	  {
	   if (palette().color(backgroundRole()) != Qt::white)
	   {
			QPalette pal = palette();
			pal.setColor(backgroundRole(), Qt::white);
			setPalette(pal);
	   }
	  }
	  setReaderToolTip(v.message());
	  if(!d_plainText)
		setHtml(QString("<strong>Attribute \"%1\" is not valid</strong>").arg(QTangoComProxyReader::realSource()));
	  else
		setText(QString("Attribute \"%1\" is not valid").arg(QTangoComProxyReader::realSource()));
	  horizontalScrollBar()->setValue(d_hScrollPos);
	  verticalScrollBar()->setValue(d_vScrollPos);
	  return;
  }
  if ((pal.color(backgroundRole()) != color))
  {
	   pal.setColor(backgroundRole(), color);
	   setPalette(pal);
  }
  if (v.canConvertToStringVector())
  {
	  QVector<QString> strings = v.toStringVector();
	  QString str;
	  QString txt;
	  if(!d_plainText)
	  {
		txt = "<ul>\n";
		foreach (QString s, strings)
		  txt += "<li>" + s + "</li>\n";
		txt += "</ul>\n";
		setHtml(txt);
	  }
	  else
	  {
		foreach (QString s, strings)
		{
		  txt += s;
		  if(!s.endsWith("\n"))
			txt += "\n";
		}
		setText(txt);
	  }
  }
  else if (v.canConvertToString())
  {
	  d_plainText ? setText(v.toString()) : setHtml(v.toString());
  }
	else
	{
		setReaderToolTip("wrong data type");
		if(!d_plainText)
		  setHtml(QString("<strong>Attribute \"%1\": wrong data type</strong>").arg(QTangoComProxyReader::realSource()));
		else
		  setText(QString("Attribute \"%1\": wrong data type").arg(QTangoComProxyReader::realSource()));
	}
	horizontalScrollBar()->setValue(d_hScrollPos);
	verticalScrollBar()->setValue(d_vScrollPos);
}


