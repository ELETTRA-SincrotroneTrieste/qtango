#ifndef _EPLOT_H
#define _EPLOT_H


#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_double_rect.h>
#include <scrollzoomer.h>
#include <QDateTime>
#include <QMenu>
#include <QVector>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <vector>
#include <qwt_plot.h>
#include <right_click_menuable.h>

#define DEPRECATED(func) func __attribute__ ((deprecated))

class Zoomer;

class QLineEdit;

/** Time scale. Inherits from the class QwtScaleDraw, a class for drawing scales,
 * linear or logarithmic.
 * This is a specific scale studied to represent the time domain scale.
 */
class TimeScaleDraw : public QwtScaleDraw
{
public:
	TimeScaleDraw(){};

	virtual QwtText label(double v) const;
/*	{
		QDateTime d;
		d.setTime_t((int)v);
		return d.toString("hh:mm.ss");
	}*/
};


class MousePressEater : public QObject
{
	Q_OBJECT
	
	
	public:
		MousePressEater(QObject* obj) : QObject(obj) {  manualmode = false; }
		void setManual(bool m) { manualmode = m; }
	protected:

	bool eventFilter(QObject *o, QEvent *e);

	private:
		bool manualmode;
};

class ScaleConfigurator : public QDialog
{
	Q_OBJECT
	public:
		ScaleConfigurator(QWidget * parent, bool autoscalex, bool autoscaley, 
			double xmin, double xmax, double ymin, double ymax, 
			bool scrolling_enabled);
		double xMin();
		double xMax();
		double yMin();
		double yMax();
		bool autoScaleX() { return cbx->isChecked(); }
		bool autoScaleY() { return cby->isChecked(); }
		QPushButton *pbapply;
	public slots:
		void applyClicked() { emit apply(); }

	signals:
		void apply();
	private:
		QCheckBox *cbx, *cby;
		QLineEdit *startx, *endx, *starty, *endy;
};


/** \brief a plot/chart widget
 * EPlot is able to display two dimensional charts given a scalar
 * (time domain) or a vector of data.
 */
class EPlot : public QwtPlot, public RightClickMenuable
{
Q_OBJECT

public:

	/** 
	 * The following is used when one has to draw the plot from a matrix.
	 * If scrolling is true, then the widget will plot one curve for each
	 * value, otherwise data will not be appended and just the current values
	 * will be displayed in one curve.
	 */
	Q_PROPERTY(bool scrolling READ scrollingEnabled WRITE setScrollingEnabled)
	Q_PROPERTY(double xScaleFactor READ xScaleFactor WRITE setXScaleFactor DESIGNABLE true)
	Q_PROPERTY(double yScaleFactor READ yScaleFactor WRITE setYScaleFactor DESIGNABLE true)
	
	enum  PlotState { PLOT_PAUSED = 0, CLICK_PAUSE, PLOT_RUNNING };
	
	/** 
	 * The constructor: accepts a pointer to the parent widget 
	 */
        DEPRECATED(EPlot(QWidget * = 0,  bool legend_enabled = false ));

	/** 
	 * Sets the buffer dimension
	 *
	 * @param b buffer size (in seconds), -1 means infinite buffer.
	 */
	void setBufSize(int b) { bufsiz = b; }

	/** 
	 * Returns the buffer size 
	 * 
	 * @return the buffer size in seconds.
	 */
	int bufSize() { return bufsiz; }
	

	void setScrollingEnabled(bool s) { scrolling = s; init(); }
	bool scrollingEnabled() { return scrolling; }

	/** Scales the x scale factor by the value represented by the double
	 * passed as parameter.
	 */
	void setXScaleFactor(double s) { xscalefact = s; }
	
	/** Returns the x scale factor which multiplies the x axis elements */
	double xScaleFactor()  { return xscalefact; }

	/** Personalize the x-axis vith your vector v */
	void setXAxisVector(std::vector<double> v);

	/** To have the x-axis vector personalized with your absissa values
	 * as described in the setXAxisVector() call, you have first 
	 * to set to false the auto generation of the x axis with this
	 * method.
	 */
	void setXAxisVectorAutoGeneration(bool auto_gen = true);

	/** Setting this to true, makes your x axis be filled with 
	 * the date taken from the vector setup as the x-axis 
	 */
	void displayXAxisVectorAsDate(bool xasdate = false);
	/** Returns true if the x axis is being interpreted as 
	 * a vector of dates.
	 */
	bool xAxisVectorAsDate() { return xasdate; }

	/** Returns true if the x axis is automatically setup to 
	 * match with the y data being plotted.
	 */
	bool xAxisVectorAutoGeneration() { return xautogen; }

	/** The parameter passed as double is the value by which the y data
	 * vector or value is multiplied before being plotted on the widget.
	 */
	void setYScaleFactor(double s) { yscalefact = s; }
	
	/** Returns the y scale factor, by which each y value on the y axis is
	 * mutliplied before being plotted on the widget.
	 */
	double yScaleFactor() { return yscalefact; }

	/** The vector passed as parameter will be added to the 
	 * y values to be displayed.
	 * Be careful to fill in such array with a number of elements
	 * equal to the size of the y data vector.
	 * Note that if also an y scale factor is provided, first 
	 * the y offset is applied and then the result is 
	 * scaled.
	 */
	void setYOffset(QVector<double> v) { yoffset = v; }

	/** Returns the vector representing the offset which is
	 * added to the y data vector.
	 */
	QVector <double> yOffset() { return yoffset; }

	/** Inserts data into the plot, accepting a single value.
	 *  If a QDateTime is not passed, time values are initialized
	 *  to the current date and time.
 	 */
	void insertData(double value, const struct timeval* tv = NULL);
	
	/** Inserts data into the plot, accepting a vector of values.
 	 * This function behaves differently depending on the scrolling
 	 * being enabled or not.
 	 */
	void insertData(QVector<double> & v, const struct timeval* tv = NULL );

	/* This one is like the above function, but accepts a standard vector
	 * instead of a QVector. It is provided for convenience.
	 */
	void insertData(std::vector<double> &v, const struct timeval* tv = NULL );
	
	void EnableGridX(bool on) { plotgrid->enableX(on); }
	
	void EnableGridY(bool on) { plotgrid->enableY(on); }
	
	/** If the parameter is set to true, then the user sets the scale manually
	 * and all zoom and autoscaling is disabled.
	 * If manual mode is enabled, then during the refresh process, the autoScale
	 * is not set for the yLeft axis.
	 * If you call setAxisScale() on the EPlot, then that scale will be fixed
	 * and if a value goes above the yLeft limit, then it will not be 
	 * correctly shown.
	 * Otherwise, the yLeft axis will be auto scaled.
	 * For example:
	 * case 1:
	 * -- The scale is always fixed and goes between 0 and 4.
	 *    If you want to change it, you have to explicitly call
	 *    setAxisScale() again.
	 * EPlot *ep = new EPlot(this);
	 * ep->enableManualMode(true);
	 * ep->setAxisScale(QwtPlot::yLeft, 0, 4);
	 *
	 * case 2:
	 * -- Not calling setAxisScale() causes the yLeft axis
	 *    to be auto scaled at each insertData() call.
	 * EPlot *ep = new EPlot(this);
	 * ep->enableManualMode(true);
	 * ....
	 *
	 */
	void enableManualMode(bool manumo);
	
	/** Tells if the manual mode is enabled (returns true) or disabled
	 *  (returns false)
	 */
	bool manualModeEnabled() { return manualmode; }
	
	/** Sets the curve pen. index is the index of the curve whose pen
	 *  has to be modified.
	 */
	void SetCurvePen(QPen p, int index);

	/** Sets the curve name, which will be displayed in the legend.
	 * The index specifies the curve to be named, starting from 0.
	 * If the legend is not large enough to contain all the titles
	 * you gave, remember to impose a minimum size to it calling
	 * setMinimumSize(QSize(x, y) ); on the legend widget.
	 */
	void SetCurveName(QString name, int index);

	/** Returns the vector containing all the curves allocated 
	 * inside the EPlot.
	 */
	QVector<QwtPlotCurve*> GetCurves() { return curve; }

	/** Returns a pointer to the legend so that one can
	 * access and modify it.
	 * The legend is always created and attached inside the
	 * EPlot constructor, to avoid an endless loop that happens
	 * if one inserts manually a legend after having attached
	 * a curve to the plot.
	 * The loop is actually avoided inserting the legend -before-
	 * each QwtPlotCurve element.
	 *
	 * If one wants the legend, he must pass true as third parameter
	 * in the EPlot constructor.
	 * By default, the legend is not created.
	 * It is not possible to hide or show a legend already created
	 * and inserted because the qwt implementation of QwtLegend 
	 * calls show() on the legend item everytime a curve is being plotted.
	 * For this, at EPlot creation, one has to decide if he wants the legend
	 * or not.
	 * If the legend is not created and one then calls GetLegend(), he will
	 * obtain a NULL pointer. So always be careful when calling a method
	 * on GetLegend().
	 */
	QwtLegend *GetLegend() { return legenda; }

	/** This one returns true if the legend has been 
	 * created at EPlot construction.
	 */
	bool LegendEnabled() { return legend_enabled; }

	/** If the user selects a manual scale from the popup, the scale is no more
	 * changed in any point in the code and the zoom is disabled.
	 * This does not affect any other parameter such as manual mode or scrolling.
	 * When the user returns to auto mode, everything returns as it was initialized
	 */
	bool userDefinedXScale() { return user_defined_xscale; }
	
	bool userDefinedYScale() { return user_defined_yscale; }

	
signals:
	/** This one is emitted when the x axis scale changes, except in the
	 * case it changes after a zoom operation.
	 */
	void xAxisScaleChanged(double x1, double x2);
	
	/** This one is emitted when the x axis scale changes, except in the
	 * case it changes after a zoom operation.
	 */

	void yAxisScaleChanged(double y1, double y2);
	/** Signals that the plot has been zoomed and provides also the 
	 * new x axis scale.
	 */
	void EPlotZoomed(const QwtDoubleRect &r, double x1, double x2);
	
	/** Signals that the plot has been zoomed and provides also the 
	 * new x axis scale and y scale.
	 */
	void EPlotZoomed(const QwtDoubleRect &r, double x1, double x2, double y1, double y2);

	/** Signals that the plot has been zoomed and provides the zoom rectangle. */
	void EPlotZoomed(const QwtDoubleRect &r);
	
protected:

	virtual QMenu* rightClickMenu() { return popupmenu; }
  
//#if 0
	void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
//#endif
	bool event(QEvent* );

	QVector<QwtPlotCurve*> curve;
	QVector<QColor>	curve_colors;

	bool refresh_enabled;
	Zoomer *zoomer;

protected slots:

	void changeXAxisScale(double x1, double x2);
	void changeYAxisScale(double y1, double y2);
	void startPauseAcq();	
	void EPlotBeenZoomed(const QwtDoubleRect &r);
	void save();
	void print();
	void bufSizeDialog();
	void configureScalesDialog();
	void applyScaleChanges();


private:

	void init();
	void display();
	void refresh();

	void setEPlotAxisScale(int axis, double start, double end);
	void insert_data(double value, const struct timeval* tv );

	void insert_data(QVector<double> &, const struct timeval* tv );
	
	double xscalefact, yscalefact;
	
	bool ScaleChanged(double xinterval);

	QVector<double> xdata, xdatamanual, yoffset;
	
	QVector<QVector<double> > ydata;

	bool  xasdate, xautogen;
	/* Buffer size and scrolling */
	int bufsiz;
	bool scrolling, left_button_pressed;
	QwtPlotGrid *plotgrid;
	
	/* The name of the widget */
	QString wname;	
	/* Popup menu to set buffer size */
	QMenu* popupmenu;
	QAction* start_pause_act;
	PlotState state; /* Paused, running */
	MousePressEater *mpe;

	QwtLegend* legenda;
	ScaleConfigurator *sc;
	bool about_to_zoom, manualmode, legend_enabled;
	bool user_defined_xscale, user_defined_yscale; /* set to true by the configuration dialog */
	bool manual_mode_save;
	double user_defined_x1, user_defined_x2;
};









































#endif

