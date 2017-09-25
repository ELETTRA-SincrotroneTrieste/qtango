#include "elineedit.h"
#include <QEvent>  
#include <QStyleOptionFrame>
#include <QStyle>
#include <QtDebug>

ELineEdit::ELineEdit(QWidget *parent) : QLineEdit(parent), FontScalingWidget(this)
{               
  d_rescaleFontOnTextChanged = true;
  connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(rescaleFont(const QString&)));
}

bool ELineEdit::event(QEvent *e)
{
  if(e->type() == QEvent::Resize || e->type() == QEvent::Show)
	FontScalingWidget::rescaleFont(text(), calculateTextSpace());
  return QLineEdit::event(e);
}

void ELineEdit::setText(const QString &txt)
{
  QLineEdit::setText(txt); 
  FontScalingWidget::rescaleFont(txt, d_savedTextSpace);
}

QSize ELineEdit::calculateTextSpace()
{
  qDebug() << "ELineEdit::labelSize(): button size " << size();
  QStyleOptionFrame labelStyleOption;
  initStyleOption(&labelStyleOption);
  d_savedTextSpace = style()->subElementRect(QStyle::SE_LineEditContents, &labelStyleOption, this).size();
  qDebug() << "ELineEdit::labelSize(): LABEL EFFECTIVE SIZE :" << d_savedTextSpace;
  return d_savedTextSpace;
}


void ELineEdit::rescaleFont(const QString& newText)
{
  if(d_rescaleFontOnTextChanged)
	FontScalingWidget::rescaleFont(newText, d_savedTextSpace);
}

QSize ELineEdit::sizeHint() const
{   
  if(!fontScaleEnabled())
	return QLineEdit::sizeHint();
   QFont f = font();
   f.setPointSize(10);
   QFontMetrics fm(f);
   int w = fm.width(text());
   /* add some pixels for a little border */
   w += 6;
   return QSize(w, 26);
}

QSize ELineEdit::minimumSizeHint() const
{
  if(!fontScaleEnabled())
	return QLineEdit::minimumSizeHint();
  return sizeHint();
}

