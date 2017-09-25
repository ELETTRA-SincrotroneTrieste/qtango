#ifndef ANALYZER_PLOT_H
#define ANALYZER_PLOT_H

#include <eplotcurve.h>
#include <eplotlight_marker.h>
#include "read_data.h"
#include <elettracolors.h>
#include <qwt_symbol.h>

class AnalyzerCurve : public EPlotCurve
{
	Q_OBJECT
	public:
		AnalyzerCurve(QObject *parent, const QString &title,
			ReadData& rd);
		ReadData readData() { return d_data; }
				
	private:
		ReadData d_data;
};


class AnalyzerPlot  : public EPlotLightMarker
{
	Q_OBJECT
	public:
		AnalyzerPlot(const QwtText &title, QWidget *parent);
		
		void createCurves(QList<ReadData> &data);
		void removeCurves();
		void setCurveStyle(QString curveName, QwtPlotCurve::CurveStyle, QwtSymbol::Style, double penWidth);
		
   signals:
               void elementInfo(const QString&);
	       void entered(); /* emitted in enterEvent() */
	       
	public slots:
		void showEvents(bool);
		void showAll(bool);
		void showLegend(bool);
		
	protected:
		QString markerText(const QwtPlotCurve *relatedCurve, const int index);
		void enterEvent(QEvent *e);
		void legendItemChecked(bool);
		
   private slots:
               void plotZoomed(const QwtDoubleRect &);
	private:
		void colourCurve(AnalyzerCurve *);
		
		QList<QColor> d_colors;
		short d_colorIndex;
		QwtLegend *d_legend;
};

#endif
