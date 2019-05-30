#ifndef EPLOTLIGHT_BASE_H
#define EPLOTLIGHT_BASE_H


/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	   			  *
*   delleceste@gmail.com		   				  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_date_scale_draw.h>

class Zoomer;

class EPlotLightBasePrivate;

class EScaleEngine : public QwtLinearScaleEngine
{
  public:
        EScaleEngine();

  protected:
	virtual void divideInterval(double interval, int numSteps) const;
        virtual void autoScale (int maxNumSteps, double &x1, double &x2, double &stepSize) const;
	virtual QwtScaleDiv divideScale (double x1, double x2, int maxMajSteps, int maxMinSteps, double stepSize=0.0);
  private:
	
};

class EScaleWidget : public QwtScaleWidget
{
  public:
    EScaleWidget(QWidget *parent=NULL) : QwtScaleWidget(parent) {}

  protected:
	virtual void scaleChange();
  private:
	
};

class ETimeScaleDraw : public QwtDateScaleDraw
{
  public:
    ETimeScaleDraw(Qt::TimeSpec timeSpec= Qt::LocalTime	) : QwtDateScaleDraw(timeSpec)
    {
        setDateEnabled(false);

    }

    virtual QwtText label(double v) const;

    void setDateEnabled(bool en)
    {
        if(en)
        {
            setDateFormat( QwtDate::Millisecond, "hh:mm:ss:zzz\nddd dd MMM" );
            setDateFormat( QwtDate::Second, "hh:mm:ss\nddd dd MMM" );
            setDateFormat( QwtDate::Minute, "hh:mm\nddd dd MMM" );
            setDateFormat( QwtDate::Hour, "hh:mm\nddd dd MMM" );
            setDateFormat( QwtDate::Day, "ddd dd MMM" );
            setDateFormat( QwtDate::Week, "Www" );
            setDateFormat( QwtDate::Month, "MMM" );
        }
        else
        {
            setDateFormat( QwtDate::Millisecond, "hh:mm:ss:zzz" );
            setDateFormat( QwtDate::Second, "hh:mm:ss" );
            setDateFormat( QwtDate::Minute, "hh:mm" );
            setDateFormat( QwtDate::Hour, "hh:mm" );
            setDateFormat( QwtDate::Day, "ddd dd MMM" );
            setDateFormat( QwtDate::Week, "Www" );
            setDateFormat( QwtDate::Month, "MMM" );
        }
        m_dateEnabled = en;
    }

    bool dateEnabled() const { return m_dateEnabled; }

private:
    bool m_dateEnabled;
};

class ShiftClickEater : public QObject
{
    Q_OBJECT
public:
    ShiftClickEater(QObject *parent) : QObject(parent) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

/** \brief A simple plot with zooming and scrolling capabilities
 *
 * This class provides a simple plot able to zoom and scroll the curves.
 * It should be used with the EPlotCurve as curves to take advantage of 
 * the zooming features.
 * You might want to look at the refresh() method to learn how zoom works.
 *
 * EPlotLightBase - based plots only manage an Y left and X bottom axes.
 * Should you need other features, you have to use QwtPlot interface in order to add
 * scales other than xBottom and yLeft.
 *
 * <h3>X axis scaling</h3>
 * <p> X axis scaling is managed internally by EPlotLightBase. This means that you should 
 * use EPlotLightBase methods to correctly manage x axis autoscaling.
 * Remember that when you manually set either the <em>upper and/or lower bounds of the x scale</em>,
 * x axis autoscale is disabled. The X axis lower and upper bounds will remain fixed to the values 
 * provided. Zoom scrolling is disabled in that scenario.
 * </p>
 * <h3>Y axis scaling</h3>
 * <p>
 * Since Y axis scaling does not affect the zoom behaviour, Y axis autoscaling is managed through QwtPlot
 * setAxisAutoscale() and setAxisScale().
 * </p>
 * @see setYLowerBound()
 * @see setYUpperBound()
 *  
 * @see setXLowerBound()
 * @see setXUpperBound()
 * 
 * The methods above disable x axis autoscale. To enable it again, you have to call setXAxisAutoscale(true).
 */
class EPlotLightBase : public QwtPlot
{
  Q_OBJECT

  Q_PROPERTY(bool xAxisAutoscale READ xAxisAutoscaleEnabled WRITE setXAxisAutoscaleEnabled)
  Q_PROPERTY(bool yAxisAutoscale READ yAxisAutoscaleEnabled WRITE setYAxisAutoscaleEnabled)
  Q_PROPERTY(bool alignCanvasToScales READ alignCanvasToScalesEnabled WRITE setAlignCanvasToScalesEnabled)
  Q_PROPERTY(double yUpperBound READ yUpperBound WRITE setYUpperBound)
  Q_PROPERTY(double yLowerBound READ yLowerBound WRITE setYLowerBound)
  Q_PROPERTY(double xUpperBound READ xUpperBound WRITE setXUpperBound)
  Q_PROPERTY(double xLowerBound READ xLowerBound WRITE setXLowerBound)
  Q_PROPERTY(bool xAutoscaleAdjustEnabled READ xAutoscaleAdjustEnabled WRITE setXAutoscaleAdjustEnabled)
  Q_PROPERTY(bool yAutoscaleAdjustEnabled READ yAutoscaleAdjustEnabled WRITE setYAutoscaleAdjustEnabled)
  Q_PROPERTY(double xAutoscaleAdjustment READ xAutoscaleAdjustment WRITE setXAutoscaleAdjustment)
  Q_PROPERTY(double yAutoscaleAdjustment READ yAutoscaleAdjustment WRITE setYAutoscaleAdjustment)
  Q_PROPERTY(bool titleOnCanvasEnabled  READ titleOnCanvasEnabled WRITE setTitleOnCanvasEnabled)
  Q_PROPERTY(bool xAxisLogScale READ xAxisLogScale WRITE setXAxisLogScale)
  Q_PROPERTY(bool yAxisLogScale READ yAxisLogScale WRITE setYAxisLogScale)
  Q_PROPERTY(bool zoomDisabled READ zoomDisabled WRITE setZoomDisabled)
    
  Q_PROPERTY(CurveStyle curvesStyle READ curvesStyle WRITE setCurvesStyle)
  Q_ENUMS(CurveStyle)
  
  public:
	
	/* directly taken from QwtPlotCurve CurveStyle */
	enum CurveStyle  { NoCurve,  Lines,  Sticks,  Steps,  Dots,  UserCurve = 100 };
	
    /** 
     * \brief The constructor of a simple plot with zooming and scrolling capabilities
     * @param parent the parent widget
     */
    EPlotLightBase(QWidget *parent);
    
    /** 
     * \brief The constructor of a simple plot with zooming and scrolling capabilities
     * @param parent the parent widget
     * @param title the title of the plot
     */
    EPlotLightBase(const QwtText &title, QWidget *parent);
    
    /** 
     * \brief Enables x axis interpretation as a timestamp and produces date/time labels in the axis itself
     * @param enable enables date/time interpretation if true.
     */
    void setTimeScaleDrawEnabled(bool enable);
    
    /**
     * @return time scale interpretation in the x axis
     */
    bool timeScaleDrawEnabled();

    /** \brief returns true if the time scale draw writes the day on the date label
      *
      */
    bool dateOnTimeScaleDrawEnabled() const;

    /** \brief enables or disables the date on the x axis scale draw
      *
      * @param en true the day is shown on the scale draw.
      * @param en false only the time is displayed on the scale.
      */
    void setDateOnTimeScaleDrawEnabled(bool en);

    
    /** \brief calls setAxisScale on the X and Y axis. The bounds are calculated reading the limits from 
     *         all the curves and expanding both axes of a percentage.
     *
     *  This method takes all curves, calculates the minimum and maximum values of all the x and y values,
     *  then calculates the x and y interval and expands them of the given percentage.
     *  replot() is not called at the end.
     * 
     * @param percent the percentage by which the x and y axes will be expanded.
     * @return true if the operation succeeds
     *         false if the curve size is 0 or 1 (cannot determine max x and y values)
     */
    bool adjustScales(double percent = 0.0, bool do_replot = true);
    
    bool adjustXScale(double percent = 0.0, bool do_replot = true);
    bool adjustYScale(double percent = 0.0, bool do_replot = true);
	
	bool setAxisScaleIfNeeded(int axisId, double min, double max, double step=0);

    CurveStyle curvesStyle() {return d_curvesStyle; }
	
	bool titleOnCanvasEnabled() { return d_titleOnCanvasEnabled; }
	
    /** \brief calls QwtPlotLayout::setAlignCanvasToScales
     *
     * Aligns the plot canvas to  the axis scales. By default it is disabled.
     *
     */
    void setAlignCanvasToScalesEnabled(bool en);
    
    bool alignCanvasToScalesEnabled();
 
    bool xAxisAutoscaleEnabled() { return d_xAutoscale; }
    bool yAxisAutoscaleEnabled() { return d_yAutoscale; }
    
    double yUpperBound();
    double yLowerBound();

	bool xAxisLogScale();
	bool yAxisLogScale();
	
	void setXAxisLogScale(bool l);
	void setYAxisLogScale(bool l);
	
    double xUpperBound();
    double xLowerBound();
    
    void enableTrackerText(bool en);
    bool trackerTextEnabled();

    /** \brief returns false if the zoom is enabled (the default), true otherwise.
      *
      * @see setZoomDisabled
      */
    bool zoomDisabled() const;
    
    QList<QwtPlotCurve *>curves();
    
  public slots:
    /** \brief refreshes the plot, taking care of the zoom state.
     *
     * The default behaviour of the refresh method depends on the setData method of the EPlotCurves:<ul>
     * <li>
     *  a. data on the EPlotCurve is refreshed adding only one element at a time (via EPlotCurve::appendData()
     *     or via EPlotLight::appendData() ):<br/>
     *    when <em>zoomed</em>, the zoom rectangle is moved by an offset equal to the delta value between the 
     *    curve last x value and the second last x value, so that the zoom <em>scrolls</em> when data is 
     *    updated.
     * </li>
     * <li>
     * b. data on the EPlotCurve is refreshed by passing an x and y vector each time:<br/>
     * 	  <em>it is assumed that the size of the vectors passed remains constant</em> over time and so
     *    the zoom rectangle <em>is not scrolled</em>.
     * </li>
     * </ul>
     * <h3>Note</h3>
     * <p>
     * If the curves in the plot are not of type EPlotCurve, then the zoom is <em>never scrolled</em> (case a).
     * </p>
     * You can reimplement the refresh method if you want to give a different behaviour to your plot.
     *
     * @see EPlotLight::setData()
     * @see EPlotLight::appendData()
     * @see EPlotLightBase::refresh()
     */
    virtual void refresh();
	
    /** \brief enables or disables x axis autoscale
     *
     * This is internally managed by EPlotLightBase and you need to call EPlotLightBase::refresh() to correctly
     * replot the graph after you update the curve data.
     */
    void setXAxisAutoscaleEnabled(bool en);
    
    /** \brief enables Y axis autoscale
     *
     * @param en true calls directly QwtPlot::setAxisAutoscale(QwtPlot::yLeft), letting QwtPlot manage the y axis scale.
     * @param en false simply takes the current lower and upper bounds and sets the scales via QwtPlot::setAxisScale()
     *           passing to the method the current bounds. You might want to call setYLowerBound and setYUpperBound then.
     * As a valid alternative, call setYLowerBound and setYUpperBound to disable y axis autoscale and set their values at once.
     * Moreover, you can directly use the QwtPlot::setAxisScale(QwtPlot::yLeft, min, max) to change the scales at once.
     * This method is provided for convenience mostly to <em>enable</em> the y axis autoscale.
     * @see extendYLowerBound
     * @see extendYUpperBound
     * @see setYUpperBound
     * @see setYLowerBound
     */
    void setYAxisAutoscaleEnabled(bool en);
    
     /** \brief extends the lower bound of the Y axis
      *
      * Disables Y axis autoscale on the plot.
      */
     void extendYLowerBound(double l);
     
     /** \brief extends the upper bound of the Y axis 
      *
      * Disables Y axis autoscale on the plot.
      */
     void extendYUpperBound(double u);
    
     /** \brief changes the Y lower bound
      *
      * brings the y lower bound to the value <em>l</em>.
      * Disables Y autoscale.
      */
     void setYLowerBound(double l);
     
     /** \brief changes the Y upper bound
      *
      * brings the y upper bound to the value <em>l</em>.
      * Disables Y autoscale.
      */
     void setYUpperBound(double u);
     
     /** \brief sets x axis lower bound 
      *
      * NOTE that it disables x axis autoscale. You might consider setting also x upper bound
      * @see setXUpperBound
      */
     void setXLowerBound(double l);
     
      /** \brief sets x axis upper bound 
      *
      * NOTE that it disables x axis autoscale. You might consider setting also x lower bound
      * @see setXLowerBound
      */
     void setXUpperBound(double u);
     
     /** \brief adjusts the x axis scale bounds enlarging it of the percentage d
      *
      * if d is different from 0.0, then at refresh time the scales are adjusted according to the value
      * of d. 
      * <strong>Note</strong>: inside the refresh() method, this is called for the x axis if and only if
      * xAxisAutoscale is enabled, and for the y axis if and only if the yAxisAutoscale is enabled.
      * You are free to call it wherever in your code to adjust the scales.
      *
      */
     void setXAutoscaleAdjustment(double d);
     double xAutoscaleAdjustment() {return d_xAutoscaleAdjustment; }
     
     /** \brief adjusts the y axis scale bounds enlarging it of the percentage d
      *
      * if yAutoscaleAdjustEnabled is true, then at refresh time the scales are adjusted according to the value
      * of d. 
      * <strong>Note</strong>: inside the refresh() method, this is called for the x axis if and only if
      * xAxisAutoscale is enabled, and for the y axis if and only if the yAxisAutoscale is enabled.
      * You are free to call it wherever in your code to adjust the scales.
      *
      * @see setXAutoscaleAdjustEnabled
      * @see setYAutoscaleAdjustEnabled
      */
     void setYAutoscaleAdjustment(double d);
     double yAutoscaleAdjustment() {return d_yAutoscaleAdjustment; }
     
     bool xAutoscaleAdjustEnabled() { return d_xAutoscaleAdjustEnabled; }
     bool yAutoscaleAdjustEnabled() { return d_yAutoscaleAdjustEnabled; }
	
	 void setTitleOnCanvasEnabled(bool en);
     
     void setXAutoscaleAdjustEnabled(bool a);
     void setYAutoscaleAdjustEnabled(bool a);
	 
	 /** \brief changes all the curves style 
	  * @param curveStyle is taken from Qwt curve styles.
	  */
	 void setCurvesStyle(CurveStyle curveStyle);

         /** \brief Enables or disables the zooming capabilities.
           *
           * Zoom is always enabled in EPlotLight classes. To disable it, call this method with a
           * true parameter. It will install an event filter on the shift-click event.
           */
         void setZoomDisabled(bool);

         /**
          * \brief scans all curves looking for minimum and maximum values among the x values.
          *
          * scans all curves data and stores in the xMin and xMax pointers the smallest value
          * and the greatest value from all curves. This can be used to retrieve the scale
          * interval via the getScaleInterval method call.
          */
         void getXBoundsFromCurves(double *xMin, double *xMax);

         /** \brief returns the scale interval according to the minimum and maximum values passed.
           *
           * If the property xAutoscaleAdjustmentEnabled is true, the interval returned is enlarged according
           * to the value of the property xAutoscaleAdjustment.
           *
           * If xMax < xMin, -1 is returned.
           */
         double getXScaleInterval(double xMin, double xMax);
	 
  protected:
    Zoomer* zoomer;
	
	/** \brief returns the plot title.
	 * 
	 * This is called if titleOnCanvasEnabled is true. Subclasses must reimplement this method 
	 * to provide a title drawn on the plot canvas. For an example, see TPlotLightMarker reimplementation.
	 * This default implementation provides no title.
	 */
	virtual QString plotTitle() { return QString(); }
	
	/** updates the title on the canvas */
	virtual void drawCanvas(QPainter *p);

  protected slots:
	void displayZoomHint();
	void eraseZoomHint();
    
  private:
    void init();

    ETimeScaleDraw *d_timeScaleDraw;
    CurveStyle d_curvesStyle;
    bool d_scheduleAdjustScales;
    bool d_xAutoscale;
    bool d_yAutoscale;
    bool d_xAutoscaleAdjustEnabled, d_yAutoscaleAdjustEnabled;
    double d_xAutoscaleAdjustment, d_yAutoscaleAdjustment;
    bool d_titleOnCanvasEnabled, d_displayZoomHint;

    EPlotLightBasePrivate *d_ptr;
};



#endif
