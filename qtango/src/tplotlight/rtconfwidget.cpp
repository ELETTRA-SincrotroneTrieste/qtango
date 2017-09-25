#include "rtconfwidget.h"
#include "realtimeplot.h" /* for cast in the constructor */
#include <QMetaObject>
#include <QMetaProperty>
#include <macros.h>

RtConfWidget::RtConfWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	Q_ASSERT(qobject_cast<TRealtimePlot *>(parent));
	d_plotWidget = parent;
	init();
}

void RtConfWidget::apply()
{
  /* gather property indexes */
  QObject *o = d_plotWidget;
  int iMode, iDate1, iDate2, iPoints, iRefreshPeriod, iBunch1, iBunch2;
  const QMetaObject* mo = o->metaObject();
  iMode = mo->indexOfProperty("mode");
  iDate1 = mo->indexOfProperty("date1");
  iDate2 = mo->indexOfProperty("date2");
  iPoints = mo->indexOfProperty("numberOfPoints");
  iRefreshPeriod = mo->indexOfProperty("period");
  iBunch1 = mo->indexOfProperty("bunch1");
  iBunch2 = mo->indexOfProperty("bunch2");
  
  if(iMode >= 0 && iDate1 >= 0 && iDate2 >= 0 && iPoints >= 0 && iRefreshPeriod >= 0 && iBunch1 >= 0 && iBunch2 >= 0)
  {
	QMetaProperty mp;
	mp = mo->property(iDate1);
	mp.write(o, ui.dt1->dateTime());
	mp = mo->property(iDate2);
	mp.write(o, ui.dt2->dateTime());
	mp = mo->property(iPoints);
	mp.write(o, ui.sbPoints->value());
	mp = mo->property(iRefreshPeriod);
	mp.write(o, 1000.0/ui.sbFreq->value());
	printf("settato 1000.0/ui.sbFreq->value() su prop period: %.2f", mp.read(o).toDouble());
	mp = mo->property(iBunch1);
	mp.write(o, ui.sbBunch1->value());
	mp = mo->property(iBunch2);
	mp.write(o, ui.sbBunch2->value());
	/* last, write mode property. This must be the last property to set, because it triggers the 
	 * update of all the other properties on the plot.
	 */
	mp = mo->property(iMode);
	mp.write(o, ui.cbMode->currentIndex());
  }
  else
	perr("RtConfWidget: some qt meta properties are unavailable for (\"%s\")", objinfo(parentWidget()));
}


void RtConfWidget::modeChanged(int m)
{
  ui.stackedWidget->setCurrentIndex(m);
  if(m == 2)
  {
	int iDate1, iDate2;
	const QMetaObject *mo = d_plotWidget->metaObject();
	iDate1 = mo->indexOfProperty("date1");
	iDate2 = mo->indexOfProperty("date2");
	if(iDate1 >= 0 && iDate2 >= 0)
	{
	  QDateTime dt1 = mo->property(iDate1).read(d_plotWidget).toDateTime();
	  QDateTime dt2 = mo->property(iDate2).read(d_plotWidget).toDateTime();
	  if(dt1 == dt2) /* first time */
	  {
		ui.dt1->setDateTime(QDateTime::currentDateTime().addSecs(-1800));
		ui.dt2->setDateTime(QDateTime::currentDateTime());
	  }
	  else
	  {
		ui.dt1->setDateTime(dt1);
		ui.dt2->setDateTime(dt2);
	  }
	}
  }
}

void RtConfWidget::init()
{
	connect(ui.pbApply, SIGNAL(clicked()), this, SLOT(apply()));
	connect(ui.cbMode, SIGNAL(currentIndexChanged(int)), this, SLOT(modeChanged(int)));
    int iMode, iPoints, iRefreshPeriod, iBunch1, iBunch2;
	const QMetaObject* mo = d_plotWidget->metaObject();
	iMode = mo->indexOfProperty("mode");
	iPoints = mo->indexOfProperty("numberOfPoints");
	iRefreshPeriod = mo->indexOfProperty("period");
	iBunch1 = mo->indexOfProperty("bunch1");
	iBunch2 = mo->indexOfProperty("bunch2");
	if(iMode >= 0 &&  iPoints >= 0 && iRefreshPeriod >= 0 && iBunch1 >= 0 && iBunch2 >= 0)
	{
	  QMetaProperty mp;
	  ui.sbPoints->setValue(mo->property(iPoints).read(d_plotWidget).toInt());
	  ui.sbBunch1->setValue(mo->property(iBunch1).read(d_plotWidget).toInt());
	  ui.sbBunch2->setValue(mo->property(iBunch2).read(d_plotWidget).toInt());
	  ui.sbFreq->setValue(1000.0/mo->property(iRefreshPeriod).read(d_plotWidget).toDouble());
	  ui.cbMode->setCurrentIndex(mo->property(iMode).read(d_plotWidget).toInt());
	}
	else
	  perr("RtConfWidget::init(): failed to read properties of RealtimePlot:\n"
	   "   be sure to have created this widget correctly, with a RealtimePlot as parent");
}


