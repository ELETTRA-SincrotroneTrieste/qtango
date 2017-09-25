#include "esimplelabel.h"
#include <macros.h>
#include <QStyleOptionFrame>
#include <QStyle>
#include <QtDebug>
#include <QEvent>

ESimpleLabel::ESimpleLabel(QWidget *parent) : QLabel(parent), FontScalingWidget(this)
{
  if(fontScaleMode() != None)
	setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  else
	setAlignment(Qt::AlignRight|Qt::AlignVCenter);
}

ESimpleLabel::ESimpleLabel(const QString& text, QWidget *parent) : QLabel(text, parent), FontScalingWidget(this)
{
  if(fontScaleMode() != None)
	setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void ESimpleLabel::setText(const QString &txt)
{
  QLabel::setText(txt);
  if(fontScaleMode() == WidthAndHeight || fontScaleMode() == WidthAndHeightManaged)
  FontScalingWidget::rescaleFont(txt, d_savedTextSpace);
}

bool ESimpleLabel::event(QEvent *e)
{
  if(e->type() == QEvent::Resize || e->type() == QEvent::Show)
	FontScalingWidget::rescaleFont(text(), calculateTextSpace());
  return QLabel::event(e);
}

QSize ESimpleLabel::calculateTextSpace()
{
  /* contentsRect takes into account frame lineWidth and midLineWidth properties */
  d_savedTextSpace = contentsRect().size();
//   d_savedTextSpace.setWidth(d_savedTextSpace.width() - 2 * frameWidth() - 2 * midLineWidth());
//   d_savedTextSpace.setHeight(d_savedTextSpace.height() - 2 * frameWidth() - 2 * midLineWidth());
//   printf("ESimpleLabel: contents rect %dx%d, text space %dx%d\n", contentsRect().width(), contentsRect().height(),
// 		 d_savedTextSpace.width(), d_savedTextSpace.height());
  
  return d_savedTextSpace;
}

QSize ESimpleLabel::sizeHint() const
{
  if(!fontScaleEnabled())
  {
	return QLabel::sizeHint();
  }
 QFont f = font();
 f.setPointSize(4);
 QFontMetrics fm(f);
 int w = fm.width(text());
 int h = fm.height();
 /* add some pixels... */
 w += 2 * frameWidth() + 2 * midLineWidth();
 h += 2 * frameWidth() + 2 * midLineWidth();
 QSize size(w, h);
//  printf("ESimpleLabel \e[1;33msizeHint\e[0m \"%s\" returning size w %d h %d\n", qstoc(objectName()), size.width(), size.height());
 return size;
}

QSize ESimpleLabel::minimumSizeHint() const
{
  QSize size;
  if(!fontScaleEnabled())
	size = QLabel::minimumSizeHint();
  else
	size = sizeHint();
//   printf("ESimpleLabel \e[0;33mminimumSizeHint\e[0m \"%s\" returning size w %d h %d\n", qstoc(objectName()), size.width(), size.height());
  return size;
}