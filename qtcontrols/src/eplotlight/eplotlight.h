#ifndef EPLOTLIGHT_H
#define EPLOTLIGHT_H

#include <QMap>
#include "eplotlight_base.h"
#include <right_click_menuable.h>

class EPlotCurve;


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

class QMenu;
class EPlotLightPrivate;

/**
 * \brief a plot derived from EPlotLightBase that provides easy curve management.
 *
 * @see EPlotLightBase base class
 * @see EPlotLightBase::refresh() method
 * 
 * 
 */
class EPlotLight : public EPlotLightBase, public RightClickMenuable
{
  Q_OBJECT
    
  Q_PROPERTY(bool timeScaleEnabled READ timeScaleDrawEnabled() WRITE setTimeScaleDrawEnabled)
  Q_PROPERTY(bool dateOnTimeScaleDrawEnabled READ dateOnTimeScaleDrawEnabled WRITE setDateOnTimeScaleDrawEnabled)
  Q_PROPERTY(int dataBufferSize READ dataBufferSize WRITE setDataBufferSize)

//   Q_PROPERTY(int timeInterval READ timeInterval WRITE setTimeInterval)
  
  public:
    /** 
     * \brief The constructor of a simple plot with zooming and scrolling capabilities
     * @param parent the parent widget
     */
    EPlotLight(QWidget *parent);
    
    /** 
     * \brief The constructor of a simple plot with zooming and scrolling capabilities
     * @param parent the parent widget
     * @param title the title of the plot
     */
    EPlotLight(const QwtText &title, QWidget *parent);

    /** \brief Registers a new curve with the provided curveName 
     * @param curveName the name associated to the EPlotCurve curve.
     * @param curve the pointer to the EPlotCurve to add to the plot with the given curveName.
     *
     * Use addCurve() to add the curve to the plot, with the provided curveName. The curveName
     * has to be used to appendData to the EPlotCurve when new points need to be added.
     * If a curve already exists with curveName, then that curve is previously removed.
     *
     * The method automatically chooses and sets a pen on the curve added.
     * The style is then customizable via the right click dialog available for the plot.
     * If you want to
     * customize the curve pen, remember to call setPen() on the curve <em>after</em> calling
     * addCurve().
     * A value for the alpha channel of the colour of each curve is taken from the dynamic
     * property curvesAlphaChannel, also available as designer free property.
     *
     * <h3>
     * <strong>Important note</strong>:</h3>
     * <p>
     * It is recommended that the alpha channel is set to 255 (no transparency at all). Otherwise,
     * a lot of CPU is required to draw curves at high refresh rates and/or with many points.
     * </p>
     *
     *
     * When the method ends, the <em>curveAdded()</em> signal is emitted, with the just added
     * curve as parameter. On the connected slot you can change the curves appearance and style.
     * To later change the curves style, call the curve() method with the name of the desired
     * curve.
     */
    void addCurve(const QString& curveName, EPlotCurve *curve);
    
    /** \brief Removes the specified curve from the plot, deleting it.
     * @param curveName the name of the curve you want to remove.
     */
    void removeCurve(const QString &curveName);
    
    /** \brief sets a particular curve style for the specified curveName
     * @param curveName the name of the curve you want to change the style to
     * @param style the CurveStyle (NoCurve, Lines, Sticks, Steps, Dots, UserCurve) you want to apply to the curve curveName.
     * Should you want to change the style of all the curves in the plot in one shot, consider using
     * EPlotLightBase::setCurvesStyle().
     */
    void setCurveStyle(const QString &curveName, QwtPlotCurve::CurveStyle);
    
	/** \brief creates a curve with name curveName, adds it to the internal map of curves 
	 *
	 * @param curveName the name of the curve, in most cases the tango point.
	 *
	 * This method creates a new EPlotCurve, with name curveName.
	 * Then it calls addCurve() to add it internally to the list of curves.
	 * If the curve already exists, addCurve will remove the already existing curve.
	 * @see addCurve
	 */
	void createAndAddCurve(const QString &curveName);
	
    void removeData();
    
    /** \brief sets the data buffer size to b elements. When b is reached, data is removed from the front.
     * @param b the maximum size of the underlying data .
     *
     * For each curve of the plot, sets the buffer size of the data linked to the curve. This puts a limit
     * on the memory used by the plot.
     */
    void setDataBufferSize(int b);
    
    /** \brief returns the data buffer size.
     * @return the number of elements of the underlying data size.
     */
    int dataBufferSize() { return d_bufSiz; }
    
    int timeInterval  ()const;
	
	void setTimeInterval(int t);
    
    bool yAutoscale() { return axisAutoScale(QwtPlot::yLeft); }
    
    public slots:
    
     /**
     *  \brief Initialize data with x- and y-arrays (explicitly shared)
     * @param curveName the name of the curve you want to set data on
     * @param xData x array of data to set on the curve
     * @param yData y array of data to set on the curve
     *
     * Calls QwtPlotCurve's setData() on the curve having the specified name.<br/>
	 * If axis autoscale is enabled (x or y), each time you call setData() it is necessary to call refresh()
	 * to set zoom base with the correct value. Otherwise the zoom might not behave as expected.
	 * Plots such as TPlotLightMarker and derived do this for you in their refresh methods.
     */
     virtual void setData(const QString& curveName, const QVector< double > &xData, const QVector< double > &yData);
    
    /** \brief Appends data to the curve with the given curveName, using the efficient draw() method
     *
     * @param curveName the curve name that identifies the curve you want to append data to. The curveName
     * must be the string you assigned when you called addCurve(). It is not necessary that the curveName is
     * the same name as the curve title provided when the curve was created.
     * @param x the x value to append to the x axis.
     * @param y the y value to append to the y axis.
     * This method adds one x and y point to the curve with curveName, using the low level draw() method of
     * the QwtPlotCurve, providing an efficient update of the plot.<br/>
	 * If axis autoscale is enabled (x or y), each time you call setData() it is necessary to call refresh()
	 * to set zoom base with the correct value. Otherwise the zoom might not behave as expected.
	 * Plots such as TPlotLightMarker and derived do this for you in their refresh methods.
     */
    virtual void appendData(const QString& curveName, double x, double y); 
    
    /** \brief Appends size data to the curve with the given curveName, using the efficient draw() method
     *
     * @param curveName the curve name that identifies the curve you want to append data to. The curveName
     * must be the string you assigned when you called addCurve(). It is not necessary that the curveName is
     * the same name as the curve title provided when the curve was created.
     * @param x pointer to the x value(s) to append to the x axis.
     * @param y pointer to the y value(s) to append to the y axis.
     * @param size number of elements contained in the data pointed by x and y.
     *
     * This method adds size x and y points to the curve with curveName, using the low level draw() method of
     * the QwtPlotCurve, providing an efficient update of the plot.<br/>
	 * If axis autoscale is enabled (x or y), each time you call setData() it is necessary to call refresh()
	 * to set zoom base with the correct value. Otherwise the zoom might not behave as expected.
	 * Plots such as TPlotLightMarker and derived do this for you in their refresh methods.
     */
    virtual void appendData(const QString& curveName, double *x, double *y, int size);
     
    /** \brief the same as setXAxisAutoscaleEnabled, provided for convenience and for backwards compatibility
     */
    void setXAutoscale(bool a) { EPlotLightBase::setXAxisAutoscaleEnabled(a); }
    
    /** \brief the same as setYAxisAutoscaleEnabled, provided for convenience and for backwards compatibility
     */
    void setYAutoscale(bool a) { EPlotLightBase::setYAxisAutoscaleEnabled(a); }
	
    /** \brief toggles y values visibility for each curve on the canvas.
      *
      * @param show true y values are shown on each curve
      * @param show false y values are not shown (the default)
      *
      * <strong>Note</strong>: this looks for all current curves in the plot and
      * applies the showYValuesEnabled on each of them. Curves that will be created
      * later need to be manually updated with this method.
      */
    void setShowYValuesEnabled(bool show);

    /** \brief returns true if Y values are shown, false otherwise.
      *
      * @see setShowYValuesEnabled
      */
    bool showYValuesEnabled() const;
	
    /** \brief Clears data on the plot, removing values from the current curves.
	 *
	 * Clears data on the plot, and the acquisition restarts.
	 */
    void clearPlot();
	
	/** \brief Returns the curve with the name provided
	 *
	 * @param name the name of the curve you want to obtain. When using TPlotLightMarker subclass,
     *             the name of a curve is equal to the name of the tango point. A case insensitive
     *             comparison is done between name and the name of the curves.
     *
     * @return the curve with the given name (case insensitive match), NULL if there's no curve with
     *         that name.
	 *
	 */
	EPlotCurve *curve(const QString& name);
	
	QString curveName(EPlotCurve *curve);

    /** \brief Returns true if the plot has the curve with the given name, case insensitive search
     *
     * @return true if the plot has the curve with the provided name (case insensitive comparison)
     *         false otherwise.
     */
    bool hasCurveIgnoreCase(const QString& name) const;

    /**
     * @brief getCurveIgnoreCase returns the curve with the given name, looking for the name using
     *        a case insensitive search. This is the same as the curve method above.
     *
     * @param name the name of the curve to look for
     *
     * @return the curve with the given name, NULL if no curve in the plot has the given name.
     */
    EPlotCurve *getCurveIgnoreCase(const QString& name);
    
  signals:
	/** \brief This signal is emitted when a new curve is added to the plot.
	 *
	 * This signal can be useful when you want to catch the creation and adding of a new curve
	 * to the plot. You can then modify the characteristics of the curve right after it is added
	 * to the plot.
	 */
	void curveAdded(EPlotCurve *);
	
	/** \brief This signal is emitted whenever a curve is removed from the plot via removeCurve.
	 *
	 * When a curve is detached from the plot, in the removeCurve() method, this signal is emitted.
	 * Right after the signal is emitted, the curve is deleted. So be sure that you connect this signal
	 * via a Qt::DirectConnection to your slot.
	 */
	void curveRemoved(EPlotCurve *);
	
  protected:
	virtual QMenu* rightClickMenu() { return d_rightClickMenu; }

	
  protected slots:
    virtual void dataUpdated() {};
	
	void print();
	
	/* save an image */
	void snapshot();
	
	void copyImage();
	
	/** \brief dumps the curves into a csv file.
	 * 
	 * The columns are in the form xAxisValues1, yAxisValues1, ... xAxisValuesN, yAxisValuesN.
	 */
	void saveData();
	
	/** \brief executes the EPlotConfigurationDialog to configure the appearence of the plot.
	 *
	 * Reimplement this method in subclasses to personalize the behaviour of the configuration action, for
	 * instance using another configuration dialog, as done in QTango TRealtimePlot.
	 */
	virtual void configurePlot();
	
    
  private:
    void init();
    int d_bufSiz;
    QMap<QString, EPlotCurve*> d_curvesMap;
    QMenu *d_rightClickMenu;
	QList<QColor> d_colors;

    EPlotLightPrivate *d_ptr;
};



#endif
