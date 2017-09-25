#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <qwt_plot_item.h>

class TangoConfigurationParameters;

class Background : public QwtPlotItem
{
public:
	Background();
	
	/** \brief enables/disables colors */
	void setColoured(bool en) { d_coloured = en; }
	bool coloured() { return d_coloured; }
	
	void configure(const TangoConfigurationParameters *cp);
	
	virtual int rtti() const;
    virtual void draw(QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRectF &) const;

	
	bool configured() const { return (d_errorLow != d_errorHigh) || (d_warningLow != d_warningHigh); }

	double referenceValue;
	
  private:
	bool d_coloured;
	double d_errorLow, d_warningLow, d_warningHigh, d_errorHigh, d_minValue, d_maxValue;
	
};


#endif
