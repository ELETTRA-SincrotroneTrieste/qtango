#ifndef EPLOT_CURVE_H
#define EPLOT_CURVE_H

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

#include <qwt_plot_curve.h>
#include "ecurvedata.h"

/* rtti value will be QwtPlotItem::Rtti_PlotUserItem + RTTI_CURVE_OFFSET */
#define RTTI_CURVE_OFFSET 10

class EPlotCurvePrivate;

class EPlotCurve : public QObject, public QwtPlotCurve
{
  Q_PROPERTY(bool showYValuesEnabled READ showYValuesEnabled WRITE setShowYValuesEnabled)
  Q_OBJECT
  public:

    enum State { Normal, Warning, Alarm, Invalid };

    /*! Constructor */
    EPlotCurve(QObject *parent);
    
    /** 
     * \brief Constructor of EPlotCurve.
     * @param title the title of the curve.
     */
    EPlotCurve(QObject *parent, const QwtText &title);
    
     /** 
     * \brief Constructor of EPlotCurve.
     * @param title the title of the curve.
     */
    EPlotCurve(QObject *parent, const QString &title);
    
    ~EPlotCurve();
    
    /** \brief appends x axis data and y axis data of a given size to the curve data.
     * @param x pointer to a double array of x axis data of size count
     * @param y pointer to a double array of y axis data of size count
     * @param count number of elements of the x and y arrays
     *
     * One may consider calling updateRawData() before drawing the curve
     */
    void appendData(double *x, double *y, int count);
    
    void setData(const QVector< double > &xData, const QVector< double > &yData);
	
	void popFront();
    
    /** \brief takes its data and sets it as raw data on the curve for subsequent drawing
     *
     */
    void updateRawData();
    
	void setTimeInterval(int t);
	
	int timeInterval() const;
    
    bool vectorUpdateMode() { return d_vectorUpdateMode; }

    /** to force vector update mode */
    void setVectorUpdateMode(bool vum) { d_vectorUpdateMode = vum; }
    
    void clearData();
	
	int count() const { return d_data->count(); }

        State state() const { return m_state; }

        void setState(State s) { m_state = s; }
    
    /* \brief Draws error markers when state is Invalid or y values as labels when the option
     * showYValuesEnabled is enabled.
     *
     * When there is enough available space, y values can be drawn near the curve points.
     * The space is calculated on the basis of the number of points currently displayed on
     * the screen, that may vary depending on the zoom active or not. This means that a curve
     * with many points can be zoomed to show its y values in the zoom rect.
     *
     * If the state of the curve is Invalid, then some triangular markers are drawn.
     */
    void drawCurve(QPainter *p, int style,
                                   const QwtScaleMap &xMap,
                                   const QwtScaleMap &yMap,
                                   const QRectF &canvasRect,
                                   int from, int to) const;

    virtual int rtti() const { return Rtti_PlotUserItem + RTTI_CURVE_OFFSET; }

    int pointsCountInInterval(double start, double end) const;

    double x(int index) const;

    double y(int index) const;

    size_t size() const;

public slots:

    void setShowYValuesEnabled(bool show) { m_showYValuesEnabled = show; }

    bool showYValuesEnabled() const { return m_showYValuesEnabled; }

  protected:

  private:
    void init();
    int d_bufSize; /* no more needed: managed by EPlotLight */
    CurveData *d_data;
    bool d_vectorUpdateMode;
    bool m_showYValuesEnabled;
    State m_state;

    EPlotCurvePrivate *d_ptr;
};



#endif


