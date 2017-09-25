#include "tplot.h"
#include <QtDebug>

TPlot::TPlot(QWidget *parent, bool auto_conf) 
	: EPlot(parent), 
	QTangoComProxyReader(this),
	QTangoWidgetCommon(this)
{
	setHideEventEnabled(false);
	setAutoConfiguration(true);
	replot();
	setDisabled(true);
	setHideEventEnabled(false);
	connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)));
	connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), 
		 this, SLOT(autoConfigure(const TangoConfigurationParameters *)));
}

void TPlot::autoConfigure(const TangoConfigurationParameters *cp)
{
  QString aTitle;
  if(!cp->label().isEmpty())
	aTitle = cp->label();
  else
	aTitle = source();
  if(cp->displayUnitIsSet())
	aTitle += QString (" [%1]").arg(cp->displayUnit());
  QwtText t = title();
  QFont f  = t.font();
  f.setPointSize(11);
  t.setFont(f);
  t.setText(aTitle);
  setTitle(t);
}


void TPlot::refresh(const TVariant &v)
{
	qDebug() << "TrendPlot::refresh";
	if (v.quality() == ATTR_INVALID)
	{
		qDebug() << "quality == ATTR_INVALID";
		setDisabled(true);
		for (int i = 0; i < curve.size(); i++)
			curve[i]->setPen(QPen(Qt::gray));
		replot();
		return;
	}
	else if (!isEnabled())
	{
		qDebug() << "quality != ATTR_INVALID";
		setEnabled(true);
		for (int i = 0; i < curve.size(); i++)
			curve[i]->setPen(QPen(curve_colors[i]));
	}
	
	if (v.canConvertToDouble())
	{
		qDebug() << "Double";
		if (v.getAttributeInfo().writable == READ)
			insertData(v.toDouble(true), v.timeReadRef());
		else
		{
			vector<double> data;
			/* toDouble() with true if read, with false if we want set point */
			data.push_back(v.toDouble(true));
			data.push_back(v.toDouble(false));
			insertData(data, v.timeReadRef());
			QVector<QwtPlotCurve*> cs = GetCurves();
			if (cs.size() > 1)
			{
				/*QPen p = cs[1]->pen();
				p.setWidth(2);
				p.setColor(Qt::red);
				cs[1]->setPen(p);*/
				if (legend() == 0)
				{
					cs[0]->setTitle("Read");
					cs[1]->setTitle("Set");
					insertLegend(new QwtLegend());
				}
			}
			//bg->referenceValue = getSetDataAsDouble();
		}
	}
	else if (v.canConvertToDoubleVector())
	{
		qDebug() << "Double vector";
		QVector<double> vect = v.toDoubleVector(true);
		insertData(vect, v.timeReadRef());
                //qDebug() << "y: " << vect << "x: " << v.timeRead();
	}
        else
        {
                setReaderToolTip("wrong data type");
                setDisabled(true);
        }
	replot();
}
