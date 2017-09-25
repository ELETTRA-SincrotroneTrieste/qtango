#include "tsimplelabel.h"

TSimpleLabel::TSimpleLabel(QWidget *parent) : ESimpleLabel(parent),
	QTangoComProxyReader(this),
	QTangoWidgetCommon(this)
{
    if(d_scaleMode == 0)
	  setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    d_displayFullSource = false;
   setText("Simple Label");
   setRefreshMode(MANUAL_REFRESH);
   setAutoConfiguration(true);
   connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)));
}

void TSimpleLabel::setDisplayFullSource(bool full)
{
    d_displayFullSource = full;
    if(qtangoComHandle()->configured())
        read(); /* refresh source and unit */
}

void TSimpleLabel::setSource(QString source)
{

  /* setSource on reader to get auto configuration and extract unit, if
   * set on the device attribute.
   */
  QTangoComProxyReader::setSource(source);
}

QString TSimpleLabel::m_getShortSource(const QString &source)
{
    QString txt = QString("syntax error on source \"%1\"").arg(source);
    if(source.contains("->"))
    {
      QStringList sl = source.split("->", QString::SkipEmptyParts);
      if(sl.size() == 2)
          txt = sl.last();
    }
    else if(source.count("/") == 3)
    {
      QStringList sl = source.split("/", QString::SkipEmptyParts);
      if(sl.size() == 4)
            txt = sl.last();
    }
    return txt;
}

void TSimpleLabel::refresh(const TVariant &v)
{
    AttributeInfo ai = v.getAttributeInfo();
    if(d_displayFullSource)
          setText(v.tangoPoint());
    else if(ai.label != "")
        setText(QString::fromStdString(ai.label));
    else
    {
        QString txt = m_getShortSource(v.tangoPoint());
        setText(txt);
    }
    /* add display unit */
    setText(text() + " [" + QString::fromStdString(ai.display_unit) + "]");
    qprintf("TSimpleLabel::refresh std %s disp %s min %s max %s \n",
           ai.standard_unit.c_str(), ai.display_unit.c_str(),
           ai.min_value.c_str(), ai.max_value.c_str());
}
