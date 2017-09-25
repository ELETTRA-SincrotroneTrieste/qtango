#ifndef EPLOTLIGHT_MARKER
#define EPLOTLIGHT_MARKER

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

#include "eplotlight.h"
#include <qwt_plot_item.h>
#include <QPainter>

class QwtPlotPicker;
class QwtPlotMarker;
class EPlotCurve;
class EPlotLightMarkerPrivate;

/** this is for private use of the EPlotLightMarker 
 */
class Arrow: public QwtPlotItem
{
  friend class EPlotLightMarker;
private:
    Arrow(const QwtText &title=QwtText()) : QwtPlotItem(title) {}
    void draw(QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRectF &) const;
        QwtPlotCurve *m_currentClosestCurve;
        int m_currentClosestPoint;

    QPointF begin, end;
};

/** \brief This class provides a plot based on the efficient EPlotLight, to which it adds markers on the canvas.
 *
 * Using or inheriting this widget provides markers to your plot. Clicking on a point over the plot
 * canvas produces a marker displaying text in a rectangle.
 * The default text displayed represents the values of the x and y values of the curve closest to 
 * the clicked point.
 *
 * Overwriting markerText() method can be useful (on an inherited plot) to personalize the text 
 * displayed on the marker.
 */
class EPlotLightMarker : public EPlotLight
{
  Q_OBJECT
  public:
    
    /** 
     * \brief The constructor of a simple plot with zooming and scrolling capabilities with a plot marker
     * @param parent the parent widget
     */
    EPlotLightMarker(QWidget *parent);
    
    /** 
     * \brief The constructor of a simple plot with zooming and scrolling capabilities with a plot marker
     * @param parent the parent widget
     * @param title the title of the plot
     */
    EPlotLightMarker(const QwtText &title, QWidget *parent);
    
    /**
     * \brief Change the aspect of the marker: render flags, color, font, background brush, background pen...
     * @param text the QwtText used to personalize the markers.
     * You might want to reimplement markerText() below to personalize the text each time a 
     * marker is updated.
     */
    void setMarkerLabel(const QwtText &text);

    /** \brief returns the list of curves that are intersect the curve in the point (x,y)
      *
      * @return QSet of QwtPlotCurve of the curves that overlap in the point (x,y)
      * @param x the value of the x axis of the desired point
      * @param y the value of the y axis of the desired point
      * @param curve the curve we want to test whether it intersects other curves in point x
      *
      * The return value won't contain the curve passed as second argument.
      * So the returned set might be an empty set.
      *
      */
    QSet<QwtPlotCurve *> intersectingCurves(const double x, const double y, const QwtPlotCurve* curve);

public slots:
    virtual void refresh();

    /** \brief returns the QwtPlotPicker used to intercept the mouse events and
      *        display the QwtPlotMarker.
      *
      * @return the QwtPlotPicker
      * @see plotMarker
      * @see markerArrow
      */
    QwtPlotPicker *plotPicker() const { return _picker; }

    /** \brief returns the QwtPlotMarker used to draw the curve x and y value when the
      *        plot is clicked
      *
      * @see markerArrow
      * @see plotPicker
      * @return the QwtPlotMarker
      */
    QwtPlotMarker *plotMarker() const { return _marker; }

    /** \brief returns the arrow drawn from the curve closest point and the marker.
      *
      * @see plotMarker
      * @see plotPicker
      */
    Arrow *markerArrow() const { return _arrow; }

    void moveCurveToYRight(QwtPlotCurve *c, bool yr);

  signals:
	void plotClicked(const QPoint& pt, QwtPlotCurve *closestCurve, double x, double y, const QPoint& curvePoint);
	
    protected slots:
    void showLabel(const QPolygon &);
	void updateLabel(QwtPlotCurve *closestCurve, int closestPoint);
	int findClosestPoint(QPoint p, QwtPlotCurve **closestCurve);
	void hideMarker();
    void plotZoomed(const QRectF&);
	
  protected:
    
    /**
     * \brief Personalize the marker text when a user clicks on the plot area.
     * @param relatedCurve: the closest curve to the point where the user clicked.
     * @param index the index value. To retrieve the x value of the curve, call relatedCurve->x(index)
     *          and to obtain the y value, relatedCurve->y(index).
     * @return a QString used by updateLabel() to fill in the marker.
     *
     * relatedCurve is not null when invoked by updateLabel().
     */
    virtual QString markerText(const QwtPlotCurve *relatedCurve, const int index);
    
    virtual void mouseReleaseEvent(QMouseEvent *ev);

    virtual void keyPressEvent(QKeyEvent *ke);
	
  private:
    void init();
    
	QwtPlotPicker* 	_picker;
	QwtPlotMarker* 	_marker;
	Arrow* 		_arrow;

    EPlotLightMarkerPrivate *d_ptr;
};



#endif
