#include "fontscalingwidget.h"
#include <macros.h>
#include <QEvent>
#include <QtDebug>

#define FONT_SIZE_TOLERANCE_MARGIN 		3 /* pixel */
#define MIN_FONT_SIZE 					8

FontScalingWidget::FontScalingWidget(QWidget *parent)
{
  Q_ASSERT(parent != NULL);
  d_widget = parent;
#ifdef SCALABLE_FONTS_ENABLED_BY_DEFAULT
  d_scaleMode = 1;
#else
  d_scaleMode = 0;
#endif
  d_lateralBorderWidth = 2;
  d_botTopBorderWidth = 2;
  d_fontScaleFactor = 1.0;
  d_savedFont = d_widget->font();
}

void FontScalingWidget::setBotTopBorderWidth(double pixValue) 
{ 
  d_botTopBorderWidth = pixValue;
  if(d_scaleMode == 1 || d_scaleMode == 2) /* update */
	rescaleFont(text(), calculateTextSpace());
}

void FontScalingWidget::setLateralBorderWidth(double pixValue) 
{ 
  d_lateralBorderWidth = pixValue;
  if(d_scaleMode == 1 || d_scaleMode == 2) /* update */
	rescaleFont(text(), calculateTextSpace());
}

void FontScalingWidget::setScaleMode(int mode)
{
  if(mode == 1 || mode == 2) /* save the font for future restore */
  {
	d_savedFont = d_widget->font();
	rescaleFont(text(), calculateTextSpace());
  }
  else if((mode != 1 && mode != 2) 
	&& (d_scaleMode == 1 || d_scaleMode == 2))
  {
	d_widget->setFont(d_savedFont);
  }
  d_scaleMode = mode;
}

double FontScalingWidget::calculateFontPointSizeF(const QString& text, const QSize &size)
{
  QFontMetrics fmint = d_widget->fontMetrics();
  QFontMetricsF fm(fmint); 
  QFont f = d_widget->font();
  QString longestLine;
  double txtWidth;
  double txtHeight;
  int linecnt = text.count("\n") + 1;
  if(linecnt > 1)
  {
	QStringList lines = text.split("\n");
	qSort(lines.begin(), lines.end(), FontScalingWidget::longerThan);
	qslisttoc(lines);
	longestLine = lines.first();
	txtHeight = fm.lineSpacing() * linecnt;
  }
  else 
  {
	longestLine = text; /* no newline */
	txtHeight = fm.height();
  }
  
  double borderH1 = size.height() - d_botTopBorderWidth;
  double borderH2 = borderH1 - d_botTopBorderWidth;
  
//    printf("\e[0;33m w %d w1 %.2f w2 %.2f -  h %d h1 %.2f h2 %.2f \e[0m\n",
// 		 size.width(), borderW1, borderW2, size.height(), borderH1, borderH2);
  
  qDebug() << d_widget << text << size;
  
  if(d_scaleMode == 2)
  {
	txtWidth = fm.width(longestLine);
	double borderW1 = size.width() - d_lateralBorderWidth;
	double borderW2 = borderW1 - d_lateralBorderWidth;
	
	if( (txtWidth < borderW1 && txtWidth > borderW2) && (txtHeight < borderH1 && txtHeight > borderH2))
	{
// 	  printf("\e[1;32m good: text w %.2f h %.2f\e[0m | borderW1: %.2f borderW2: %.2f borderH1: %.2f borderH2: %.2f\n", 
// 			 txtWidth, txtHeight, borderW1, borderW2, borderH1,borderH2 );
	}
	else
	{
	  while(txtWidth < borderW2 && txtHeight < borderH2)
	  {
		f.setPointSizeF(f.pointSizeF() + 0.5);
//   	  printf(" \e[1;35m ++ INCREASING font size \"%s\" :text \"%s\" width %.1f height %.1f - point size %.2f - w: %.2f h: %.2f\e[0m\n", 
//   			 qstoc(widget()->objectName()), qstoc(text),  txtWidth, txtHeight, f.pointSizeF(), w, h); 
		QFontMetricsF tmpFm(f);
		txtWidth = tmpFm.width(longestLine);
		txtHeight = linecnt * tmpFm.lineSpacing();
	  }
	  while((txtWidth >= borderW1 || txtHeight >= borderH1) && f.pointSizeF() > MIN_FONT_SIZE)
	  {
		f.setPointSizeF(f.pointSizeF() - 0.5);
// 		printf(" \e[1;36m -- DECREASING font size \"%s\" :text \"%s\" width %.1f height %.1f - point size %.2f - w: %.2f h: %.2f\e[0m\n", 
//   			 qstoc(widget()->objectName()), qstoc(text),  txtWidth, txtHeight, f.pointSizeF(), w, h); 
		QFontMetricsF tmpFm(f);
		txtWidth = tmpFm.width(longestLine);
		txtHeight = linecnt * tmpFm.lineSpacing();
	  }
	}
  }
  else /* scale according to width, length of the string, and height */
  {
	borderH2 = borderH1; /* move slightly up and down the border */
	if(txtHeight == borderH1/* && txtHeight > borderH2*/)
	{
// 	  printf("\e[1;32m good: text h %.2f\e[0m | borderH1: %.2f borderH2: %.2f pointSizeF %.2f, h: %.2f\n", 
// 			 txtHeight, borderH1,borderH2, f.pointSizeF(), h );
	}
	else
	{
	  while((txtHeight > borderH1) && f.pointSizeF() > MIN_FONT_SIZE)
	  {
		f.setPointSizeF(f.pointSizeF() - 0.5);
// 		printf(" \e[1;36m -- DECREASING font size \"%s\" :text \"%s\"  height %.1f - point size %.2f - h: %.2f\e[0m\n", 
//   			 qstoc(widget()->objectName()), qstoc(text),  txtHeight, f.pointSizeF(),  h); 
		QFontMetricsF tmpFm(f);
		txtHeight = linecnt * tmpFm.lineSpacing();
	  }
	  /* cycle above might have produced a text height slightly below the borderH1. With luck, we do not enter
	   * the next cycle (if txtHeight == borderH2 (==borderH1) after the cycle above). Otherwise we will increase
	   * the font one step more until txtHeight becomes > borderH2, but we should fit the tolerance specified with
	   * d_botTopBorderWidth.
	   */
	  while(txtHeight < borderH2)
	  {
		f.setPointSizeF(f.pointSizeF() + 0.5);
// 		printf(" \e[1;35m ++ INCREASING font size \"%s\" :text \"%s\" height %.1f - point size %.2f - h: %.2f\e[0m\n", 
//   			 qstoc(widget()->objectName()), qstoc(text), txtHeight, f.pointSizeF(), h); 
		QFontMetricsF tmpFm(f);
		txtHeight = linecnt * tmpFm.lineSpacing();
	  }
	}
	
  }
  return f.pointSizeF();
}

void FontScalingWidget::rescaleFont(const QString& text, const QSize &size)
{
  if((d_scaleMode != 1 && d_scaleMode != 2)  || size.width() < 8  || size.height() < 4)
	return;
  double fontSize = calculateFontPointSizeF(text, size);
  QFont f = d_widget->font();
  f.setPointSizeF(fontSize);
  d_widget->setFont(f);
}





