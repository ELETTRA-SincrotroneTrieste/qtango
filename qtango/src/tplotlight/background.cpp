#include "background.h"
#include "configuration.h" /* color map for attribute quality colors */
#include <auto_configuration.h>
#include <qwt_scale_map.h>
#include <elettracolors.h>
#include <QtDebug>
#include <QPainter>
#include <macros.h>

Background::Background()
{
  d_errorLow = d_warningLow = d_warningHigh = d_errorHigh =  d_minValue =  d_maxValue = 0.0;
  d_coloured = false;
}
	
int Background::rtti() const 
{ 
	return QwtPlotItem::Rtti_PlotUserItem; 
}

void Background::configure(const TangoConfigurationParameters *cp)
{
//   printf("Background::configure - source \"%s\" warn set %d %d err set %d %d ", qstoc(cp->label()),
// 		 cp->mWarnIsSet(), cp->MWarnIsSet() ,cp->mErrIsSet(), cp->MErrIsSet() );
  /* red and yellow bands must be extended to intersect the alarm and warning thresholds 
   * of all sources.
   */
  if(cp->mErrIsSet() && cp->MErrIsSet())
  {
	if(d_errorLow == d_errorHigh)
	{
	  d_errorHigh = cp->maxError();
	  d_errorLow = cp->minError();
	}
	else
	{
	  if(cp->maxError() < d_errorHigh)
		d_errorHigh = cp->maxError();
	  if(cp->minError() > d_errorLow)
		d_errorLow = cp->minError();
	}
  }
  if(cp->mWarnIsSet() && cp->MWarnIsSet())
  {
	if(d_warningLow == d_warningHigh)
	{
	  d_warningHigh = cp->maxWarning();
	  d_warningLow = cp->minWarning();
	}
	else 
	{
	  if(cp->maxWarning() < d_warningHigh)
		 d_warningHigh = cp->maxWarning();
	  if(cp->minWarning() > d_warningLow)
		d_warningLow = cp->minWarning();
	}
  }
//   printf("\e[1;36m error high %.2f \e[1;33m warn high %.2f - warn low %.2f \e[1;31m err low %.2f\e[0m\n",
// 		  d_errorHigh, d_warningHigh, d_warningLow, d_errorLow);
}


void Background::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &) const
{
   if(!d_coloured || !configured())
      return;

	Config *cfg = Config::instance();
	QPen p;
	p.setColor(EColor(Elettra::red));
	p.setWidth(2);
// 	painter->setPen(p);
// 	painter->drawLine(xMap.p1(), yMap.transform(referenceValue), xMap.p2()-xMap.p1(), yMap.transform(referenceValue));

	/* warn colors: draw them if warning thresholds are configured (warningLow != warningHigh) and
	 * warningHigh < errorHigh for the upper band and warningLow > errorLow for the lower bound. This
	 * second condition applies if errorHigh != errorLow (i.e. alarm thresholds configured.
	 */
	QRectF r;
	QColor warn = cfg->qualityColor(ATTR_WARNING);
	warn.setAlpha(40);
	painter->setPen(warn);
	painter->setBrush(warn);
	/* make local copies to use to draw the rectangles */
	double errorLow = d_errorLow, warningLow = d_warningLow, errorHigh = d_errorHigh, warningHigh = d_warningHigh;
	if(d_warningLow != d_warningHigh)
	{
	  if(d_errorLow == d_errorHigh)
	  {
		errorLow = d_warningLow;
		errorHigh = d_warningHigh;
	  }
	  if(warningHigh <= errorHigh)
	  {
		/* draw warning upper band */
		r = QRectF(xMap.p1(), yMap.transform(errorHigh), xMap.p2()-xMap.p1(), yMap.transform(warningHigh) - yMap.transform(errorHigh));
		painter->drawRect(r);
	  }
	  if(warningLow >= errorLow)
	  {
		r = QRectF(xMap.p1(), yMap.transform(warningLow), xMap.p2()-xMap.p1(), yMap.transform(errorLow) - yMap.transform(warningLow));
		painter->drawRect(r);
	  }
	}

	/* alarm red bands: draw them if errorLow != errorHigh. We need not check warning thresholds here: warning bands
	 * have been drawn above if necessary.
	 */
	if(d_errorLow != d_errorHigh)
	{
	  QColor err = cfg->qualityColor(ATTR_ALARM);
	  err.setAlpha(40);
	  painter->setPen(err);
	  painter->setBrush(err);
	  r = QRectF(xMap.p1(), yMap.p2(), xMap.p2()-xMap.p1(), yMap.transform(errorHigh) - yMap.p2());
	  painter->drawRect(r);
	  r = QRectF(xMap.p1(), yMap.transform(errorLow), xMap.p2()-xMap.p1(), yMap.p1() - yMap.transform(errorLow));
	  painter->drawRect(r);
	}
// 	printf("\e[1;31m error high %.2f \e[1;33m warn high %.2f - warn low %.2f \e[1;31m err low %.2f\e[0m\n",
// 		  errorHigh, warningHigh, warningLow, errorLow);
}

