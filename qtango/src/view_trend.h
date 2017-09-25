/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino		   *
*   giacomo.strangolino@elettra.trieste.it   *
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

/* $Id: view_trend.h,v 1.3 2013-04-30 07:34:03 giacomo Exp $ */


#ifndef QTANGO_VIEW_TREND
#define QTANGO_VIEW_TREND

class QObject;
class TrendPlot;
class QTangoViewTrendPrivate;

class QTangoViewTrend
{
	public:
		QTangoViewTrend(QObject* iProxy);

		/** \brief returns true if view trend is enabled.
		 *
		 * A true return value does not mean that the plot will be shown: it depends on
		 * canPlot().
		 */
		bool viewTrendEnabled() { return d_trend; }
		
		/** \brief enables or disables the view trend property.
		 *
		 * @see canPlot
		 * @see createTrendPlot
		 * @see viewTrendEnabled
		 */
		void setViewTrendEnabled(bool en) { d_trend = en; }
		
		/** \brief the trend plot will display coloured bands corresponding to alarms and warning thresholds.
		 *
		 * This works if auto configuration is possible for the scalar attribute associated.
		 * @see setTrendColouredBackground
		 */
		bool trendColouredBackground() { return d_colouredBackground; }
		
		/** \brief configure the trend plot to display coloured bands corresponding to alarms and warning thresholds.
		 *
		 * This works if auto configuration is possible for the scalar attribute associated.
		 * @see trendColouredBackground
		 */
		void setTrendColouredBackground(bool en) { d_colouredBackground = en; }
		
		/* trend */
		/** \brief the default implementation returns true.
		 * 
		 * The expected behaviour for a subclass implementing QTangoViewTrend is that it is possible to 
		 * plot data, if the tango source point is set.
		 */
        virtual bool canPlot();
		
		/** \brief creates and displays a trend plot. When closed, it continues reading and is not destroyed.
		 *
		 * <strong>Note</strong> the view trend plot will disable show/hide events, to provide continuous data
		 * to the plot, even if one of the widgets which share the qtango source is hidden.
		 */
		virtual void createTrendPlot();
		
	protected:  
		TrendPlot *plot;
	
	private:
		QObject *d_parentObject;
		bool d_trend;
		bool d_colouredBackground;

        QTangoViewTrendPrivate *d_ptr;
};

#endif

