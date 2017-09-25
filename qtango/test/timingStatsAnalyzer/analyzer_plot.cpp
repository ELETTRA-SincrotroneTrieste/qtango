#include "analyzer_plot.h"
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <QtDebug>
#include <ezoomer.h>
#include <QResizeEvent>

AnalyzerCurve::AnalyzerCurve(QObject *parent, const QString &title,
	      ReadData& rd) : EPlotCurve(parent, title), d_data(rd)
{
	
}

AnalyzerPlot::AnalyzerPlot(const QwtText &title, QWidget *parent) 
	: EPlotLightMarker(title, parent)
{
	d_legend = new QwtLegend(this);
	d_legend->setItemMode(QwtLegend::CheckableItem);
	insertLegend(d_legend, QwtPlot::BottomLegend);
	d_colorIndex = 0;
	d_colors << KRED90SAT << KDARKGREEN << KBLUE << KDARKCYAN
			<< KMAGENTA << KDARKPINK << KDARKVIOLET << KDARKYELLOW << KGRAY << 
			KCAMEL << KDARKWATER << KPINKPIG << Qt::black << Qt::blue << Qt::green
			<< KDARKMAROON << KCYAN << KVERYDARKBLUE<< KVERYDARKVIOLET;
	setAxisLabelRotation(QwtPlot::xBottom, -90);
	setAxisTitle(QwtPlot::xBottom, "Seconds");
	setAxisTitle(QwtPlot::yLeft, "Millisedonds");
	setAxisMaxMinor(QwtPlot::xBottom, 10);
	setAxisMaxMajor(QwtPlot::xBottom, 20);
	connect(zoomer, SIGNAL(zoomed(const QwtDoubleRect &)), this, SLOT(plotZoomed(const QwtDoubleRect &)));
}

QString AnalyzerPlot::markerText(const QwtPlotCurve *relatedCurve, const int index)
{
	QString txt = "Error";
	QwtPlotCurve *crv = (QwtPlotCurve *) relatedCurve;
	AnalyzerCurve *ac = dynamic_cast<AnalyzerCurve *>(crv);
	if(ac)
	{
		double x = relatedCurve->x(index);
		unsigned msec = (unsigned) relatedCurve->y(index);
		int times = (int) x;
		unsigned h = times/3600;
		unsigned m = (times - h * 3600) / 60;
		unsigned secs = times - h * 3600 - m * 60;
		txt = QString("%1\n%2:%3:%4:%5").arg(ac->readData().name).
				arg(h, 2, 10, QChar('0')).
				arg(m, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'))
				.arg(msec, 3, 10, QChar('0'));
		
		QList<Timestamp> tlist = ac->readData().findTimestamp(h, m, secs, msec);
		if(tlist.size())
		{
		  Timestamp t = tlist.first();
		  if(t.event)
			   txt += "\nEvent";
		  if(t.error)
			   txt += "\nError";
		  if(t.message != QString())
		     txt += "\n" + t.message/*.replace("\\s", " ")*/;
		  txt += QString("\nElement %1").arg(t.index);
		}
		QString info;
		foreach(Timestamp t, tlist)
		{
		   info += QString(
			 "<ul><strong>%1</strong><li>element: %2</li><li>Comment: %3</li><li>Timestamp: <em>%4:%5:%6:%7</em></li><li>Object: <cite>%8</cite></li></ul>").arg(t.name).
			 arg(t.index).arg(t.message).arg(h, 2, 10, QChar('0')).
			 arg(m, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'))
			 .arg(msec, 3, 10, QChar('0')).arg(t.objinfo);
		}
	     emit elementInfo(info);
	}
	return txt;
}

void AnalyzerPlot::plotZoomed(const QwtDoubleRect &)
{
   /* get rid of the nasty disabled vertical scrollBar, if it appears after zoom */
   if(!zoomer->zoomRectIndex() && zoomer->verticalScrollBar())
      zoomer->verticalScrollBar()->setVisible(false);
}

void AnalyzerPlot::createCurves(QList<ReadData> &data)
{
	double xLowerBound, xUpperBound, yLowerBound, yUpperBound;
	QString curveName;
	foreach(ReadData rd, data)
	{
		curveName = rd.name;
		AnalyzerCurve *crv = new AnalyzerCurve(this, curveName, rd);
		addCurve(curveName, crv);
		colourCurve(crv);
		crv->setStyle(QwtPlotCurve::Dots);
		QWidget *w = legend()->find(crv);
		QwtLegendItem *li = dynamic_cast<QwtLegendItem *>(w);
		if(li)
			li->setChecked(true);
		QList<Timestamp> ts = rd.timestamps;
		foreach(Timestamp t, ts)
		{
			double x = t.hours * 3600.0 + t.min * 60.0 + t.sec;
			double y = (double) t.msec;
			appendData(curveName, x, y);
		}
	}
	/* EPlotLightBase method, calculates lower and upper values of each curve, both
	 * in x and in y values, and rescales both xBottom and yLeft expanding the axes of
	 * a certain percentage. Does not call replot()
	*/
	EPlotLightBase::adjustScales(0.1);
        replot();
	zoomer->setZoomBase();
	/* sometimes appears a vertical scrollbar, disabled.. */
	if(zoomer->verticalScrollBar())
	   zoomer->verticalScrollBar()->setVisible(false);
}

void AnalyzerPlot::showLegend(bool s)
{
   if(!s && d_legend)
   {
      delete d_legend;
      d_legend = NULL;
   }
   else if(s && !d_legend)
   {
      d_legend = new QwtLegend(this);
      d_legend->setItemMode(QwtLegend::CheckableItem);
      insertLegend(d_legend, QwtPlot::BottomLegend);
      showAll(true);
     
   }
   QResizeEvent *re = new QResizeEvent(size()*0.99, size());
   resizeEvent(re);
}

void AnalyzerPlot::showAll(bool a)
{
   QList<QwtPlotCurve *> crvs = curves();
   foreach(QwtPlotCurve *pc, crvs)
   {
      if(d_legend)
      {
         QWidget *w = d_legend->find(pc);
         QwtLegendItem *li = dynamic_cast<QwtLegendItem *>(w);
         li->setChecked(a);
      }
      pc->setVisible(a);
   }
   replot();
}

void AnalyzerPlot::colourCurve(AnalyzerCurve *c)
{
	int size = d_colors.size();
	int i = d_colorIndex % size;
	QColor col = d_colors.at(i);
	c->setPen(QPen(QBrush(col), 4));
	d_colorIndex++;
}

void AnalyzerPlot::showEvents(bool s)
{
	QList<QwtPlotCurve *> crvs = curves();
	AnalyzerCurve *ac;
	foreach(QwtPlotCurve *pc, crvs)
	{
		QWidget *w = legend()->find(pc);
		QwtLegendItem *li = dynamic_cast<QwtLegendItem *>(w);
		if(li && ((ac = dynamic_cast<AnalyzerCurve *>(pc)) != NULL) &&  ac->readData().timestamps.size() > 0)
		{
			if(s)
			{
				ac->setVisible(s);
				w->setVisible(s);
			}
			else
			{
				ac->setVisible(!ac->readData().timestamps.at(0).event);	
				w->setVisible(!ac->readData().timestamps.at(0).event);	
			}
		}
	}
	replot();
}

void AnalyzerPlot::legendItemChecked(bool c)
{
	QList<QwtPlotCurve *> crvs = curves();
	foreach(QwtPlotCurve *pc, crvs)
	{
		QWidget *w = legend()->find(pc);
		QwtLegendItem *li = dynamic_cast<QwtLegendItem *>(w);
		if(li)
		{
			pc->setVisible(li->isChecked());		
		}
	}
	replot();
	QwtPlot::legendItemChecked(c);
}

void AnalyzerPlot::setCurveStyle(QString curveName, QwtPlotCurve::CurveStyle s, QwtSymbol::Style ssty,
	double penWidth)
{
	QList<QwtPlotCurve *> crvs = curves();
	foreach(QwtPlotCurve *c, crvs)
	{
		if(c->title().text() == curveName)
		{
			
			QPen p = c->pen();
			QwtSymbol sym(ssty, QBrush(p.color()), QPen(Qt::black), QSize(penWidth * 5,penWidth * 4));
			c->setSymbol(sym);
			c->setStyle(s);
			p.setWidth(penWidth);
			c->setPen(p);
			replot();
		}
	}
}

void AnalyzerPlot::removeCurves()
{
	QList<QwtPlotCurve *> crvs = curves();
	foreach(QwtPlotCurve *c, crvs)
	{
		QString crvname = c->title().text();
		removeCurve(crvname);
	}
}

void AnalyzerPlot::enterEvent(QEvent *e)
{
   emit entered();
   QWidget::enterEvent(e);
}

