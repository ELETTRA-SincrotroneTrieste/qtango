#include <QEvent>
#include <QApplication>
#include <QVector>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDateTime>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QWidget>

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QMenu>
#include <QLineEdit>

#include <QtDebug>
#include <qwt_plot_layout.h>
#include <qwt_scale_engine.h>

#include <sys/time.h>
#include <time.h>
#include <vector>
#include <iostream>
#include "eplot.h"
#include "eplotlight/ezoomer.h"
#include <elettracolors.h>
#include <macros.h>

#define MAX_CURVES 64

using namespace std;
class EPlot;

QwtText TimeScaleDraw::label(double v) const
{
        QDateTime d;
        d.setTime_t((int) v );
	return d.toString("hh:mm.ss");
}

/* The constructor of the class EPlot */
EPlot::EPlot(QWidget *p, bool show_legend) 
	: QwtPlot(p), RightClickMenuable(), xscalefact(1), yscalefact(1), bufsiz(-1)
{
	pwarn("EPlot is no more maintained in qtcontrols/qtango distribution since version 3.1.0.");
	pwarn("Please substitute it with EPlotLight(Marker)/TPlotLightMarker as soon as possible.");
	scrolling = true;
	refresh_enabled = true;
	state = PLOT_RUNNING;
	setFrameStyle(QFrame::NoFrame);
	setLineWidth(0);
	setCanvasLineWidth(2);
//	plotLayout()->setAlignCanvasToScales(true);
	plotLayout()->setCanvasMargin(0, QwtPlot::yLeft);
	plotLayout()->setCanvasMargin(0, QwtPlot::yRight);
	/* Popup menu to set buffer size, print or save data. */
	popupmenu = new QMenu("Plot", this);
	start_pause_act = popupmenu->addAction("Pause Acquisition", this, SLOT(startPauseAcq()));
	popupmenu->addSeparator();
	popupmenu->addAction("Set Buffer Size...", this, SLOT(bufSizeDialog() ) );
	popupmenu->addSeparator();
	popupmenu->addAction("Save Data", this, SLOT(save()));
	popupmenu->addAction("Print", this, SLOT(print()));
	popupmenu->addSeparator();
	popupmenu->addAction("Configure scales", this, SLOT(configureScalesDialog()));
	/* the colours of the curve */
	curve_colors << EColor(Elettra::darkBlue) << EColor(Elettra::red) << EColor(Elettra::orange) 
		<< EColor(Elettra::darkViolet);
	curve_colors << EColor(Elettra::pinkPig) << EColor(Elettra::darkGreen) << QColor(Elettra::maroon) 
		<< EColor(Elettra::cyan);
	curve_colors << EColor(Elettra::darkWater) << EColor(Elettra::violet) << EColor(Elettra::almostBlack)
		<< EColor(Elettra::blueGray);
	curve_colors << EColor(Elettra::darkPink) << EColor(Elettra::blue) << QColor(Elettra::darkMaroon) 
		<< EColor(Elettra::darkCyan);

	user_defined_xscale = user_defined_yscale =  manualmode = manual_mode_save = false;
	xasdate = false;
	xautogen = true;
	left_button_pressed = false;
	mpe = new MousePressEater(this);
	about_to_zoom = false;

	/* white background */
	setCanvasBackground(Qt::white);
	plotgrid = new QwtPlotGrid;
	plotgrid->setPen(QPen(Qt::DotLine));
	plotgrid->attach(this);
	EnableGridX(true);
	EnableGridY(true);
	replot();
	zoomer = new Zoomer(canvas());
	zoomer->setRubberBandPen(QPen(Qt::red, 2, Qt::DotLine));
	zoomer->setTrackerPen(Qt::NoPen);
		
	setEnabled(true);
	
	init();
	
	legenda = NULL;
	
	if(show_legend)
	{
		legenda = new QwtLegend();
		insertLegend(legenda, QwtPlot::RightLegend);
		legend_enabled = true;
	}
	else
		legend_enabled = false;

	ydata << QVector<double>();

	connect(zoomer, SIGNAL(zoomed(const QwtDoubleRect&) ), this, SLOT(EPlotBeenZoomed(const QwtDoubleRect&) ) );

}

/** Inserts data into the plot, accepting a single value.
  *  If a QDateTime is not passed, time values are initialized
  *  to the current date and time.
  */
void  EPlot::insertData(double value, const struct timeval* tv )
{
	//qDebug("insertData: %f", value);
	double x1 = 0, x2 = 0;
	if(user_defined_xscale) /* save the x scale */
	{
#if QWT_VERSION >= 0x050200
		x1 = axisScaleDiv(QwtPlot::xBottom)->lowerBound();
		x2 = axisScaleDiv(QwtPlot::xBottom)->upperBound();
#else
		x1 = axisScaleDiv(QwtPlot::xBottom)->lBound();
		x2 = axisScaleDiv(QwtPlot::xBottom)->hBound();
#endif
	}
	
	insert_data(value, tv);
	
	if(user_defined_xscale)
		setAxisScale(QwtPlot::xBottom, user_defined_x1, user_defined_x2);

}


/** Inserts data into the plot, accepting a vector of values.
  * This function behaves differently depending on the scrolling
  * being enabled or not.
  */
void EPlot::insertData(QVector<double> & v, const struct timeval* tv )
{
	insert_data(v, tv);

	if(user_defined_xscale)
		setAxisScale(QwtPlot::xBottom, user_defined_x1, user_defined_x2);

}

/** This one is like the above function, but accepts a standard vector
 * instead of a QVector. It is provided for convenience.
 */
void EPlot::insertData(std::vector<double> &v, const struct timeval * tv)
{
	QVector<double> qvect = QVector<double>::fromStdVector(v);

	insert_data(qvect, tv);

	if(user_defined_xscale)
		setAxisScale(QwtPlot::xBottom, user_defined_x1, user_defined_x2);


	
}

void EPlot::SetCurveName(QString name, int index)
{
	if(index < curve.size() )
		 curve[index] ->setTitle(name);
	else
		qDebug() << "index " << index << " out of range (there are " << curve.size() << " curves.";

}

void EPlot::SetCurvePen(QPen p, int index)
{
	if(index < curve.size() )
		curve[index]->setPen(p);
	else
		qDebug() << "index " << index << " out of range (there are " << curve.size() << " curves.";
}
/* PRIVATE FUNCTIONS */

void EPlot::init()
{
	if (scrolling)
	{
		setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
		setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	}
	else
	{
		setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw());
		setAxisLabelRotation(QwtPlot::xBottom, 0.0);
	}
	setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
}


void EPlot::enableManualMode(bool en)
{
	manual_mode_save = manualmode = en;
	mpe->setManual(en);
	if(en)
	{
		canvas()->installEventFilter(mpe);
		setCursor(Qt::ArrowCursor);
	}
	else
	{
		canvas()->removeEventFilter(mpe);
		setCursor(Qt::CrossCursor);
	}
}

void EPlot::setXAxisVector(std::vector<double> v)
{
	unsigned int i;
	/* Prepare xdatamanual, which will be used in the 
	 * insert_data.
	 */
	xdatamanual.clear();
	for(i = 0; i < v.size(); i++)
		xdatamanual.push_back(v[i]);
	/* Setup the scale for xBottom axis */
	if(xdatamanual.size() > 0)
		setEPlotAxisScale(QwtPlot::xBottom, xdatamanual[0], 
				xdatamanual[xdatamanual.size() - 1]);
}
 
/* Slot to change x axis */
void EPlot::changeXAxisScale(double x1, double x2)
{
	double start, end;
	qApp->processEvents();
#if QWT_VERSION >= 0x050200
	start = axisScaleDiv(QwtPlot::xBottom)->lowerBound();
	end = axisScaleDiv(QwtPlot::xBottom)->upperBound();
#else	
	start = axisScaleDiv(QwtPlot::xBottom)->lBound();
	end = axisScaleDiv(QwtPlot::xBottom)->hBound();
#endif
	if(x1 != start || x2 != end)
	{
		setEPlotAxisScale(QwtPlot::xBottom, x1, x2); /* calls emit() */
	}
}

/* Slot that propagates the zoomed() signal emitted by the zoomer */
void EPlot::EPlotBeenZoomed(const QwtDoubleRect &r)
{
	qApp->processEvents();
#if QWT_VERSION >= 0x050200
		emit EPlotZoomed(r,axisScaleDiv(QwtPlot::xBottom)->lowerBound(), axisScaleDiv(QwtPlot::xBottom)->upperBound()); 
#else
	emit EPlotZoomed(r, axisScaleDiv(QwtPlot::xBottom)->lBound(), axisScaleDiv(QwtPlot::xBottom)->hBound());
#endif
}

/* Slot to change y axis scale */
void EPlot::changeYAxisScale(double y1, double y2)
{
	setAxisScale(QwtPlot::yLeft, y1, y2);
	emit yAxisScaleChanged(y1, y2);
}

void EPlot::setXAxisVectorAutoGeneration(bool autog)
{
	xautogen = autog;
}

void EPlot::displayXAxisVectorAsDate(bool x_as_date)
{
	xasdate = x_as_date;
	if(xasdate)
	{
		setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
		setAxisLabelRotation(QwtPlot::xBottom, -50.0);
	}
	else
	{
		setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw());
		setAxisLabelRotation(QwtPlot::xBottom, 0.0);
	}
	setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
}

bool MousePressEater::eventFilter(QObject *o, QEvent *e)
{
	if( ( e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonRelease) && manualmode )
	{
		QMouseEvent* mev = (QMouseEvent *)e;
		if(mev->button() == Qt::LeftButton || mev->button() == Qt::MidButton)
			return true;
	}
	return QObject::eventFilter(o, e);

}

void EPlot::insert_data(double newdata, const struct timeval* tv)
{
	
	struct timeval tval;
	/* Avoid updating the plot when scrolling is enabled and the state is PAUSED.
         * The lack of this check causes the zoom to behave badly: while we are zooming
         * the plot is updated and the paused screenshot changes!
         * This issue doesn't appear when scrolling is enabled because the data is stored
         * continuously inside the plot.
         */
        if(!scrolling && state == EPlot::PLOT_PAUSED)
                return;

	

	/* check if curves data are empty and eventually reattach them */
	if (curve.size() == 0)
	{
		QwtPlotCurve *c = new QwtPlotCurve(QString("%1").arg(curve.size() ) );
		c->setPen(QPen(curve_colors[0]));
		c->attach(this);
		curve << c;
	}

	ydata[0].push_back(newdata * yscalefact);
	
	if(tv == NULL)
		gettimeofday(&tval, NULL);
	else
		memcpy(&tval, tv, sizeof(struct timeval) );
	xdata.push_back( (double) (tval.tv_sec + tval.tv_usec * 0.000001) );

	while ((bufsiz != -1) && ((xdata.last() - xdata.front() ) > bufsiz) )
	{
		ydata[0].pop_front();
		xdata.pop_front();
	}
		
	curve[0]->setRawData(&xdata[0], &(ydata[0])[0], xdata.size());

	
	if (refresh_enabled)
		QApplication::postEvent(this, new QEvent(QEvent::User));
}

/* Inserts data into the plot, accepting a vector of values.
 * This function behaves differently depending on the scrolling
 * being enabled or not.
 */
void EPlot::insert_data(QVector<double> &newDataVector, const struct timeval* tv)
{
	qDebug() << "insert_data con QVector";
	int s, i, xdatasiz = 0;
	struct timeval tval;
	
	/* See the comment in the function above */
        if(!scrolling && state == EPlot::PLOT_PAUSED)
                return;
	
	/* check if curves data are empty and eventually reattach them */
/*	if (curve.size() == 0)
	{
		QwtPlotCurve *c = new QwtPlotCurve("Plot.");
		c->setPen(QPen(curve_colors[0]));
		c->attach(this);
		curve << c;
	}
*/	
	s = MAX_CURVES;
	if (newDataVector.size() < MAX_CURVES)
		s = newDataVector.size();
	
	/* Apply y offset to the y data if required and the y offset vector is setup
	 * correctly.
	 */
	if(yoffset.size() != 0 && newDataVector.size() != yoffset.size() )
		qDebug() << "The yOffset vector(long " << yoffset.size() <<
			") must be as long as the y data vector (long " << newDataVector.size() << 
			")! The offset won't be applied!";
	else if(yoffset.size() == newDataVector.size() )
	{
		for(i = 0; i < newDataVector.size(); i++)
			newDataVector[i] += yoffset[i];
	}

	if (scrolling)
	{
		qDebug() << "scrolling";
		while (ydata.size() < s)
			ydata.push_back(QVector<double>());
		
		for (i = 0; i < ydata.size(); i++)
			ydata[i].push_back(newDataVector[i] * yscalefact);
		
		if(tv == NULL)
			gettimeofday(&tval, NULL);
		else
			memcpy(&tval, tv, sizeof(struct timeval) );
	
		xdata.push_back( (double) (tval.tv_sec + tval.tv_usec * 0.000001) );
		
		qDebug() << "data" << ctime(&tval.tv_sec);

		while ((bufsiz != -1) && (xdata.size() > bufsiz))
		{
			for (i = 0; i < ydata.size(); i++)
				ydata[i].pop_front();
			xdata.pop_front();
		}
	}
	else
	{
		qDebug() << "no scrolling";
		if(yscalefact != 1)
		{
			QVector<double> datacopy;
			datacopy.resize(newDataVector.size() );
			for(i = 0; i < datacopy.size(); i++)
				datacopy[i] = newDataVector[i] * yscalefact;
			ydata[0] = datacopy;
		}
		else
			ydata[0] = newDataVector;

		if(xautogen || xdata.size() != ydata[0].size() )
		{
			xdata.clear();
			for (i = 0; i < ydata[0].size(); i++)
				xdata.push_back(i * xscalefact);
			
			if(!xautogen && xdata.size() != ydata[0].size() )
			{
				qDebug() << "X axis auto generation is disabled but the lenght of " ;
				qDebug() <<  " the x vector provided [" << xdata.size() << "] is different ";
				qDebug() <<  " from the y vector length [" << ydata[0].size() << "]";
				qDebug() << "If you call setXAxisVectorAutoGeneration() to false, you have";
				qDebug() << "to call setAxisVector() explicitly at least once to provide the x axis elements.";
			}
		}
		else
		{
			/* Leave xdata untouched: it should have been set manually. */
		}
	}
	
	while (curve.size() < ydata.size())
	{
		QwtPlotCurve *c = new QwtPlotCurve(QString("%1").arg(curve.size() ) );
		c->setPen(QPen(curve_colors[curve.size() % curve_colors.size()]));
		c->attach(this);
		curve.push_back(c);
	}
	
	qDebug() << "numero di curve: " << curve.size();

	
	for (i = 0; i < curve.size(); i++)
	{
		xdatasiz = ydata[i].size(); /* initialize xdatasiz to this, then we will correct it if
					     * xdatamanual.size() != ydata[i].size()
					     */
		if(!xautogen && (xdatamanual.size() != ydata[i].size() ) )
		{
			/* The number of elements to plot has to be the 
			 * minimum between the x and y vectors size
			 */
			qDebug() << "WARNING: the size of the x vector (" << xdatamanual.size()  <<
				") is not equal to the size of the y vector (" << 
				ydata[i].size() << ") at the " <<
				i + 1 << " position.";
			qDebug() << "The plot will not appear with all the elements expected";
			if(xdatamanual.size() >= ydata[i].size() )
				xdatasiz = ydata[i].size();
			else
				xdatasiz = xdatamanual.size();
			/* Finally, resize xdatamanual, so that the display() will receive
			 * consistent data.
			 */
			xdatamanual.resize(xdatasiz);
		}
		if(!xautogen && !scrolling) /* x manual is allowed only without scrolling enabled */
			curve[i]->setRawData(&xdatamanual[0], &(ydata[i])[0], xdatasiz);
		else
			curve[i]->setRawData(&xdata[0], &(ydata[i])[0], xdata.size());
	}
	qDebug() << "refresh_enabled" << refresh_enabled;
	if (refresh_enabled)
		QApplication::postEvent(this, new QEvent(QEvent::User));
}					
	
void EPlot::display()
{
	QwtDoubleRect r;
	double offset;
	int i;
	QVector <double> x_data;
	x_data.clear();

	/* x_data will contain the right vector, depending on
	 * scrolling and autogen enabled or not.
	 */
	if(!scrolling && !xautogen)
	{
		for(i = 0; i < xdatamanual.size(); i++)
			x_data.push_back(xdatamanual[i] );
		if(xasdate)
		{
			
		}
	}
	else
	{
		for(i = 0; i < xdata.size(); i++)
			x_data.push_back(xdata[i]);
	}

	if (!x_data.size())
		return;

	
	if (state == EPlot::PLOT_RUNNING /*&& !manualmode*/)
	{
		if (! zoomer->zoomRectIndex()) /* we are not inside the zoom */
		{
//			qDebug() << "! zoomer->zoomRectIndex()";
			/* Adjust the scale just when needed */
			if(!user_defined_xscale && ( !scrolling && !manualmode && ScaleChanged( x_data[x_data.size() - 1] -  x_data[0])))
				setEPlotAxisScale(QwtPlot::xBottom, x_data[0], x_data[x_data.size() - 1]);
#if QWT_VERSION >= 0x050200
			else if(!user_defined_xscale && (axisScaleDiv(QwtPlot::xBottom)->lowerBound() != x_data[0] 
				|| axisScaleDiv(QwtPlot::xBottom)->upperBound() != x_data[x_data.size() - 1] ))	
#else
			else if(!user_defined_xscale && (axisScaleDiv(QwtPlot::xBottom)->lBound() != x_data[0] 
				|| axisScaleDiv(QwtPlot::xBottom)->hBound() != x_data[x_data.size() - 1] ))
#endif
			{
				// qDebug() << "Aggiorno la scala perche` risulta diversa";
				QwtPlot::setAxisScale(QwtPlot::xBottom, x_data[0], x_data[x_data.size() - 1]);
 				emit xAxisScaleChanged(x_data[0], x_data[x_data.size() - 1]);
			}
//			else
//				qDebug() << "Non aggiorno la scala perche` non serve!";
			/* Disable explicit setAxisAutoScale 
			 * in manualmodei: see enableManualMode() 
			 * comment in eplot.h. 
			 */
			if(!manualmode && !user_defined_yscale) 
				setAxisAutoScale(QwtPlot::yLeft);
			replot();

		}
		else if (scrolling ) /* zoom with scrolling enabled */
		{
	//		qDebug() << "scrolling";
			r = zoomer->zoomBase();
			offset = x_data[x_data.size()-1] - x_data[x_data.size()-2];
			if ((bufsiz != -1) || ((x_data.last()-x_data.front()) < bufsiz))
				r.setRight(r.right()+offset);
			else
				r.moveRight(r.right()+offset);
			
			zoomer->setZoomBase(r);
			zoomer->moveBy(offset, 0);
			replot();
		}
		else /* zoom with scrolling disabled */
		{
	//		qDebug() << "else";
			/* Modifica del 19 aprile: questa istruzione causa l'autoscale della Y anche nello zoom,
        		 * annullando l'effetto dello zoom sull'asse stesso
	         	*/
	//		if(!manualmode) 
	//			setAxisAutoScale(QwtPlot::yLeft);

			replot();	
		}
			
	}
}

bool EPlot::ScaleChanged(double xinterval)
{
	QwtDoubleRect zrect = zoomer->zoomBase();
	double x1, x2;
	x1 = zrect.left();
	x2 = zrect.right();
//	if(xinterval != x2 - x1)
//		qDebug() << "effettivamente la scala e` cambiata: x1: " << x1 << ", x2: " << x2 <<
//			", xinterval: " << xinterval;
	return xinterval != x2 - x1;
	
}

void  EPlot::setEPlotAxisScale(int id, double start, double end)
{
	QwtDoubleRect zrect = zoomer->zoomBase();
	QwtDoubleRect newrect;
//	QwtPlot::setAxisScale(id, start, end);
	if(id == QwtPlot::xBottom) /* x axis */
		newrect.setRect(start, zrect.top(), end - start, zrect.bottom() - zrect.top() );
	else if(id == QwtPlot::yLeft)
		newrect.setRect(zrect.left(), start, zrect.right() - zrect.left(), end - start);
	zoomer->setZoomBase(newrect);
	zoomer->zoom(0);
	QwtPlot::setAxisScale(id, start, end);
	/* signal that scales have changed */
	if(id == QwtPlot::yLeft)
		emit yAxisScaleChanged(start, end);
	else
		emit xAxisScaleChanged(start, end);
}

bool EPlot::event(QEvent *e)
{
	if (e->type() == QEvent::User)
	{
		display();
		return true;
	}
	else
		return QwtPlot::event(e);
}

void EPlot::refresh()
{
	QApplication::postEvent(this, new QEvent(QEvent::User));
}

void EPlot::mousePressEvent(QMouseEvent *ev)
{
	/* Pause refreshing if state is running and put the
	 * state to CLICK_PAUSE, so that at mouse release
	 * we can restore the state to PLOT_RUNNING
	 */
	if(ev->button() == Qt::LeftButton && !manualmode)
	{
		QRect r;
		if (state == EPlot::PLOT_RUNNING)
			state = EPlot::CLICK_PAUSE;
		about_to_zoom = false;
		left_button_pressed = true;
		/* zoomer->setZoomBase() is now called inside the mouse move event.
		 */
		if (! zoomer->zoomRectIndex())
			zoomer->setZoomBase( );

	}
	else if (ev->button() == Qt::RightButton)
		popupmenu->popup(ev->globalPos());
	
//		QwtPlotPicker::mousePressEvent(ev);
}


void EPlot::mouseReleaseEvent(QMouseEvent *ev)
{
	if(ev->button() == Qt::LeftButton)
	{
		if(state == EPlot::CLICK_PAUSE)
			state = EPlot::PLOT_RUNNING;
	}
}

void EPlot::startPauseAcq()
{
	if (state == EPlot::PLOT_PAUSED)
	{
		state = EPlot::PLOT_RUNNING;
		start_pause_act->setText("Pause Acquisition");
	}

	else if (state == EPlot::PLOT_RUNNING)
	{
		state = EPlot::PLOT_PAUSED;
		start_pause_act->setText("Resume Acquisition");
	}
}


void EPlot::save()
{
	int size, i, j;
	QDateTime d;
	QString s = QFileDialog::getSaveFileName(this, "Choose a filename to save under", QDir::homePath(), "Text file (*.txt)");
	if (!s.isEmpty())
	{
		QFile f(s);
		if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::warning(this, "Error", "Can't create file", QMessageBox::Ok, 0, 0); 
			return;
		}
		QTextStream out(&f);
		out << fixed;
		out.setRealNumberPrecision(10);
		out << QDateTime::currentDateTime().toString() << '\n';
		size = xdata.size();
		for (i = 0; i < size; i++)
		{
			d.setTime_t((int)xdata[i]);
			out << d.toString() << '\t' << xdata[i] << '\t';
			for (j = 0; j < ydata.size(); j++)
				out << ydata[j][i] << '\t';
			out << '\n';
		}
		f.close();
	}		
}

void EPlot::print()
{
	QPrinter printer;
	QString docName = title().text();
	if ( docName.isEmpty() )
	{
		docName.replace (QRegExp (QString::fromLatin1 ("\n")), tr (" -- "));
		printer.setDocName (docName);
	}
	printer.setCreator("QTango");
	QPrintDialog dialog(&printer);
	if ( dialog.exec() )
	{
		QwtPlotPrintFilter filter;
		if ( printer.colorMode() == QPrinter::GrayScale )
		{
			filter.setOptions(QwtPlotPrintFilter::PrintAll & ~QwtPlotPrintFilter::PrintBackground);
		}
		QwtPlot::print(printer, filter);
	}
}

void EPlot::bufSizeDialog()
{
	QDialog *dlg = new QDialog(this);
	dlg->setWindowTitle("Buffer Size");
	
	QLabel *l = new QLabel("Buffer Size [s]:", dlg);
	QSpinBox *s = new QSpinBox(dlg);
	s->setToolTip("-1 means no size limit");
	s->setMinimum(-1);
	s->setMaximum(60*60*24);
	s->setValue(bufSize());
	
	QPushButton *close = new QPushButton("OK", dlg);
	QPushButton *cancel = new QPushButton("Cancel", dlg);
	connect(close, SIGNAL(clicked()), dlg, SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), dlg, SLOT(reject()));

	QGridLayout *grid = new QGridLayout(dlg);
	grid->addWidget(l, 0, 0);
	grid->addWidget(s, 0, 1);
	grid->addWidget(close, 1, 0);
	grid->addWidget(cancel, 1, 1);
	
	if (dlg->exec() == QDialog::Accepted)
		setBufSize(s->value());

	delete dlg;
}



void EPlot::applyScaleChanges()
{
	if(!sc->autoScaleX())
	{
		user_defined_x1 = sc->xMin();
		user_defined_x2 = sc->xMax();
		if(user_defined_x2 <= user_defined_x1)
		{
			QMessageBox::information(this, "Error", QString("The end value %1 is minor or equal than %2!").
					arg(user_defined_x2).arg(user_defined_x1));
			return;
		}
		user_defined_xscale = true;
		mpe->setManual(true);
		canvas()->installEventFilter(mpe);
		setCursor(Qt::ArrowCursor);
		manualmode = true;	
		setAxisScale(QwtPlot::xBottom, sc->xMin(), sc->xMax());
	}
	else
	{
		user_defined_xscale = false;
		if(!manual_mode_save && sc->autoScaleY()) /* before personalizing the scales manual mode was true */
			enableManualMode(false);

	}

	if(!sc->autoScaleY())
	{
		if(sc->yMax() <= sc->yMin())
		{
			QMessageBox::information(this, "Error", QString("The end value %1 is minor or equal than %2!").
					arg(sc->yMax()).arg(sc->yMin()));
			return;
		}

		user_defined_yscale = true;
		mpe->setManual(true);
		canvas()->installEventFilter(mpe);
		setCursor(Qt::ArrowCursor);
		manualmode = true;	

		setAxisScale(QwtPlot::yLeft, sc->yMin(), sc->yMax());
	}
	else
	{
		user_defined_yscale = false;
		if(!manual_mode_save && sc->autoScaleX()) /* before personalizing the scales manual mode was true */
			enableManualMode(false);

	}
	replot();
}

void EPlot::configureScalesDialog()
{
  #if QWT_VERSION >= 0x050200
        double xstart = axisScaleDiv(QwtPlot::xBottom)->lowerBound();
	double xend = axisScaleDiv(QwtPlot::xBottom)->upperBound();
	double ystart = axisScaleDiv(QwtPlot::yLeft)->lowerBound();
	double yend = axisScaleDiv(QwtPlot::yLeft)->upperBound();
#else
	double xstart = axisScaleDiv(QwtPlot::xBottom)->lBound();
	double xend = axisScaleDiv(QwtPlot::xBottom)->hBound();
	double ystart = axisScaleDiv(QwtPlot::yLeft)->lBound();
	double yend = axisScaleDiv(QwtPlot::yLeft)->hBound();
#endif
	sc = new ScaleConfigurator(this, userDefinedXScale(),
		userDefinedYScale(),
		xstart, xend, ystart, yend, scrolling);

	connect(sc, SIGNAL(apply()), this, SLOT(applyScaleChanges()));

	if(sc->exec() == QDialog::Accepted)
	{
		applyScaleChanges();
	}
	delete sc;

		

}

double ScaleConfigurator::xMin()
{
	bool ok;
	double ret;
	ret = startx->text().toDouble(&ok);
	if(!ok)
	{
		QMessageBox::information(0, "Error", QString("The value \"%1\" is not a valid double").arg(startx->text()));
		ret = 0;
	}
	return ret;
}

double ScaleConfigurator::xMax()
{
	bool ok;
	double ret;
	ret = endx->text().toDouble(&ok);
	if(!ok)
	{
		QMessageBox::information(0, "Error", QString("The value \"%1\" is not a valid double").arg(endx->text()));
		ret = 0;
	}
	return ret;

}

double ScaleConfigurator::yMin()
{
	bool ok;
	double ret;
	ret = starty->text().toDouble(&ok);
	if(!ok)
	{
		QMessageBox::information(0, "Error", QString("The value \"%1\" is not a valid double").arg(starty->text()));
		ret = 0;
	}
	return ret;

}

double ScaleConfigurator::yMax()
{
	bool ok;
	double ret;
	ret = endy->text().toDouble(&ok);
	if(!ok)
	{
		QMessageBox::information(0, "Error", QString("The value \"%1\" is not a valid double").arg(endy->text()));
		ret = 0;
	}
	return ret;
}




ScaleConfigurator::ScaleConfigurator(QWidget *par, bool autoscalex, bool autoscaley,
        double xmin, double xmax, double ymin, double ymax, bool scrolling_enabled) : QDialog(par)
{
	setWindowTitle("Configure the scales");
	QGroupBox* gbx = new QGroupBox(this);
	QGroupBox* gby = new QGroupBox(this);
	QPushButton *pbacc = new QPushButton(this);
	pbacc->setText("Ok");
	QPushButton *pbcancel = new QPushButton(this);
	pbcancel->setText("Cancel");
	QPushButton *pbapply = new QPushButton(this);
	pbapply->setText("Apply");
	
	/* Check boxes */
	cbx = new QCheckBox(gbx);
	cby = new QCheckBox(gby);
	QGridLayout *lox, *loy, *loglobal;
	lox = new QGridLayout(gbx);
	loy = new QGridLayout(gby);
	loglobal = new QGridLayout(this);

	/* Double spin boxes */
	startx = new QLineEdit(gbx);
	endx = new QLineEdit(gbx);
	
	lox->addWidget(cbx, 0, 0, 1, 1);
	lox->addWidget(startx, 0, 2, 1, 1);
	lox->addWidget(endx, 0, 4, 1, 1);

	
	starty = new QLineEdit(gby);
	endy = new QLineEdit(gby);

	loy->addWidget(cby, 0, 0, 1, 1);
	loy->addWidget(starty, 0, 2, 1, 1);
	loy->addWidget(endy, 0, 4, 1, 1);

	loglobal->addWidget(gbx, 0, 0, 1, 5);
	loglobal->addWidget(gby, 1, 0, 1, 5);
	loglobal->addWidget(pbapply, 2, 2, 1, 1);
	loglobal->addWidget(pbacc, 2, 3, 1, 1);
	loglobal->addWidget(pbcancel, 2, 4, 1, 1);

	cbx->setText("Auto Scale");
	cby->setText("Auto Scale");

	if(scrolling_enabled)
	{
		gbx->setTitle("X axis (not available in this scrolling mode)");
		gbx->setDisabled(true);
	}
	else
		gbx->setTitle("X Axis");
	
	gby->setTitle("Y Axis");

	connect(cbx, SIGNAL(toggled(bool)), startx, SLOT(setDisabled(bool)));
	connect(cbx, SIGNAL(toggled(bool)), endx, SLOT(setDisabled(bool)));

	connect(cby, SIGNAL(toggled(bool)), starty, SLOT(setDisabled(bool)));
	connect(cby, SIGNAL(toggled(bool)), endy, SLOT(setDisabled(bool)));

	connect(pbacc, SIGNAL(clicked()), this, SLOT(accept()));
	connect(pbcancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(pbapply, SIGNAL(clicked()), this, SLOT(applyClicked()));

	cbx->setChecked(!autoscalex);
	cby->setChecked(!autoscaley);
	startx->setText(QString("%1").arg(xmin));
	endx->setText(QString("%1").arg(xmax));

	starty->setText(QString("%1").arg(ymin));
	endy->setText(QString("%1").arg(ymax));

}



