#include "realtimeplot.h"
#include "rtconfdialog.h"
#include <QRegExp>
#include <QMenu>

#define RTCOMMAND_ARGIN_REGEXP 		".*->.+(\\(\\d+,\\d+(,\\d+){0,3}\\))"
#define ARGIN_REGEXP				"\\(\\d+,\\d+(,\\d+){0,3}\\)"
/* the following is used to guess the mode, samples, bunch number start and end, date start and end.. 
 * not escaped is \((\d+),(\d+)(?:,(\d+))?(?:,(\d+))?(?:,(\d+))?\)
 */
#define ARGIN_CAPTURE_REGEXP		"\\((\\d+),(\\d+)(?:,(\\d+))?(?:,(\\d+))?(?:,(\\d+))?\\)"

TRealtimePlot::TRealtimePlot(QWidget *parent) : TPlotLightMarker(parent)
{
  /* initialize Qt properties values */
  setTimeScaleDrawEnabled(false);
  /* normally, realtime plot draws spectrum quantities: when hidden it is not necessary to refresh */
  setHideEventEnabled(true);
  d_nPoints = 1000;
  d_date1 = QDateTime::currentDateTime();
  d_date2 = QDateTime::currentDateTime();
  d_mode = 0;
  d_b1 = 0;
  d_b2 = 100;
  setPeriod(100);
  setBoundsFromAutoConfiguration(false);
  d_defaultArgin = "(0,1000)";
  /* autoscale enabled in EPlotLightBase */
  /* this disables x axis autoscale for a better refresh of the x axis */
  setAxisScale(QwtPlot::xBottom,  0, d_nPoints);
  replot(); /* for timeScaleDrawEnabled and axisScale */
}

bool TRealtimePlot::extractDefaultArginFromSourcesList(const QStringList &sources)
{
  /* as discussed in the documentation, the default argin is taken from the 
   * _last_ source containing an argin directive, so we start the cycle from
   * sources.size() -1 
   */
  for(int i = sources.size() - 1; i >= 0; i--)
  {
	pstep("TRealtimePlot \"%s\": processing source \"%s\" (%d/%d)", qstoc(objectName()), qstoc(sources.at(i)),
		  i, sources.size());
	QString s = sources.at(i);
	/* regexp: ".*->.+(\(\d+,\d+(,\d+){0,3}\))"  
	 * abc->def(0,10), abc->def(2,t1sec,t1usec,t2sec,t2usec)
	 * The regexp is not so strict... but trust the user and the programmer ;)
	 * (should check first argument included in [0;2] and number of arguments should 
	 * 2 or 5 (in timestamp mode)
	 */
	QRegExp re(RTCOMMAND_ARGIN_REGEXP); /* escaped */
	int pos = re.indexIn(s);
	if(pos > -1)
	{
	  QStringList caps = re.capturedTexts();
	  if(caps.size() > 1)
	  {
		pstep("TRealtimePlot \"%s\": choosing default argin \"%s\"", qstoc(objectName()), qstoc(caps.at(1)));
		d_defaultArgin = caps.at(1);
		updatePropertiesFromArgin();
		return true;
	  }
	} 
  }
  return false;
}

void TRealtimePlot::clearArginsFromSourcesList(QStringList &srcs)
{
  QStringList cleared;
  foreach(QString s, srcs)
  {
	if(s.contains(QRegExp(ARGIN_REGEXP)))
	{
	  QString orig = s;
	  s.remove(QRegExp(ARGIN_REGEXP));
	  pstep("TRealtimePlot \"%s\": removing argin from \"%s\": will become \"%s\"", qstoc(objectName()), 
		qstoc(orig), qstoc(s));
	}
	cleared << s;
  }
  srcs = cleared;
}

void TRealtimePlot::addPastedSource()
{
  pinfo("TRealtimePlot \"%s\": \e[1;32madding source\e[0m \"%s\"", qstoc(objectName()), qstoc(pastedSource()));
  if(sourceConfigured(pastedSource()) || sources().isEmpty())
  {
	perr("source \"%s\" already configured or sources list empty. Not adding it.", qstoc(pastedSource()));
	return;
  }
  QString currentSources = sources();
  currentSources += ";" + pastedSource();
  setSources(currentSources);
}

void TRealtimePlot::setSources(const QString &srcs)
{
  if(srcs.isEmpty())
	return;
  QString sources = srcs;
  QStringList srcList, srcListWithDefaultArgin;
  if(sources.contains(";"))
	srcList = sources.split(";", QString::SkipEmptyParts);
  else
	srcList << sources;
  /* check that the sources are commands */
  foreach(QString s, srcList)
  {
	if(!s.contains("->"))
	{
	  perr("TRealtimePlot: source \"%s\" is not syntactically correct to express commands", qstoc(s));
	  perr("TRealtimePlot accepts commands in the form: \"a/b/c->command\" or \"a/b/c->command(N,M)\"");
	  return;
	}
  }
  /* if called from setMode(), there will be no default argin among sources, because 
   * setMode() calls clearArginsFromSourcesList() before calling setSource().
   * setMode() actually manipulates d_defaultArgin according to the Qt properties 
   * mode/number of points, date1 date2, et cetera
   */
  bool hasArgin = extractDefaultArginFromSourcesList(srcList);
  if(!hasArgin)
	pinfo("TRealtimePlot \"%s\": no source has an argin specified", qstoc(objectName()));
  else
	pinfo("TRealtimePlot \"%s\": chosen argin \"%s\" for all the sources", qstoc(objectName()), qstoc(d_defaultArgin));
  
  /* remove argins from sources list */
  clearArginsFromSourcesList(srcList);
  
  QString newSrc;
  foreach(QString s, srcList)
  {
	newSrc = s + d_defaultArgin;
	srcListWithDefaultArgin << newSrc;
	pstep("TRealtimePlot \"%s\": source modified with default argin from \"%s\" to \"%s\"", qstoc(objectName()), 
		qstoc(s), qstoc(newSrc));
  }

  QString rtSrcString = srcListWithDefaultArgin.join(";");
  if(rtSrcString != TPlotLightMarker::sources())
    TPlotLightMarker::setSources(rtSrcString);
  else
      pinfo("TRealtimePlot.setSources: sources \"%s\" equal to \"%s\", not changing sources", qstoc(rtSrcString), qstoc(TPlotLightMarker::sources()));
}

void TRealtimePlot::configurePlot()
{
  RtConfDialog *conf = new RtConfDialog(this);
  conf->show();
}

void TRealtimePlot::updatePropertiesFromArgin()
{
  QRegExp re(ARGIN_CAPTURE_REGEXP);
  int pos = re.indexIn(d_defaultArgin);
  bool ok;
  if(pos > -1)
  {
	pok("ok processing argin \"%s\" to update properties", qstoc(d_defaultArgin));
	QStringList caps = re.capturedTexts();
	caps.removeAll(QString()); /* remove empty strings */
	qslisttoc(caps);
	if(caps.size() == 3) /* (0,NSamples) (+ captured string (see capturedTexts() doc!) ) */
	{
	  if(caps.at(1).toInt(&ok) == 0 && ok && caps.at(2).toInt(&ok) > 0 && ok)
	  {
		pinfo("TRealtimePlot \"%s\": detected mode 0 and number of samples %d", qstoc(objectName()), caps.at(2).toInt());
		d_mode = 0;
		d_nPoints = caps.at(2).toInt();
	  }
	  else
		perr("TRealtimePlot \"%s\": detected argin of type (A,B) but not good: \"%s\"", qstoc(objectName()), qstoc(d_defaultArgin));
	}
	else if(caps.size() == 4) /* (1,fromBunch,toBunch) + captured string */
	{
	  if(caps.at(1).toInt(&ok) == 1 && ok && caps.at(2).toInt(&ok) >= 0 && ok && caps.at(3).toInt(&ok) > 0 && ok)
	  {
		pinfo("TRealtimePlot \"%s\": detected mode 1 and bunches from %d to %d", qstoc(objectName()),
			  caps.at(2).toInt(), caps.at(3).toInt());
		d_mode = 1;
		d_b1 = caps.at(2).toInt();
		d_b2 = caps.at(3).toInt();
	  }
	  else
		perr("TRealtimePlot \"%s\": detected argin of type (A,B,C) but not good: \"%s\"", qstoc(objectName()), qstoc(d_defaultArgin));
	}
	else if(caps.size() == 6) /* (2,t1sec,t1msec,t2sec,t2msec) + captured string */
	{
	  if(caps.at(1).toInt(&ok) == 2 && ok && caps.at(2).toInt(&ok) >= 0 && ok && caps.at(3).toInt(&ok) > 0 && ok
		&& caps.at(4).toInt(&ok) > 0 && ok && caps.at(5).toInt(&ok) > 0 && ok)
	  {
		pinfo("TRealtimePlot \"%s\": detected mode 1 and bunches from %d to %d", qstoc(objectName()),
			  caps.at(2).toInt(), caps.at(3).toInt());
		d_mode = 2;
		d_date1 = QDateTime::fromTime_t(caps.at(2).toUInt());
		d_date1.addMSecs(caps.at(3).toUInt());
		d_date2 = QDateTime::fromTime_t(caps.at(4).toUInt());
		d_date2.addMSecs(caps.at(5).toUInt());
	  }
	}
	else
	  perr("TRealtimePlot \"%s\": detected argin of type (A,B,C,D,E) but not good: \"%s\"", qstoc(objectName()), qstoc(d_defaultArgin));
  }
  else
	perr("TRealtimePlot \"%s\":no match for regexp \"%s\" in \"%s\"", qstoc(objectName()), ARGIN_CAPTURE_REGEXP, qstoc(d_defaultArgin));
}

/** \brief Change the number of points (applies to Last N Samples mode)
 *
 * \note
 * After calling setMode and (or) setNumberOfPoints
 * remember to apply changes with updateParams
 *
 */
void TRealtimePlot::setNumberOfPoints(int n)
{
  d_nPoints = n;
  updateParams();
}

/** \brief set the start date (applies to timestamp mode)
 *
 */
void TRealtimePlot::setDate1(const QDateTime &d1)
{
    if(d_date1 != d1)
    {
        d_date1 = d1;
        updateParams();
    }
}

/** \brief set the stop date (applies to timestamp mode)
 *
 * @see setDate1
 */
void TRealtimePlot::setDate2(const QDateTime &d2)
{
    if(d_date2 != d2)
    {
        d_date2 = d2;
        updateParams();
    }
}

/** \brief set bunch 1 (in bunch mode)
 *
 * @see setBunch2
 */
void TRealtimePlot::setBunch1(int b)
{
    if(d_b1 != b)
    {
        d_b1 = b;
        updateParams();
    }
}

/** \brief set  bunch N (applies to bunch mode)
 *
 * @see setBunch1
 */
void TRealtimePlot::setBunch2(int b2)
{
    if(b2 != d_b2)
    {
        d_b2 = b2;
        updateParams();
    }
}

void TRealtimePlot::setMode(int m)
{
    if(m != d_mode)
    {
        d_mode = m;
        updateParams();
    }
}

/** \brief Update the source of the plot according to mode, period, number of points.
 *
 * This method is called from numberOfPoints, mode, bunch1, bunchN, date1 or date2
 *
 * @see setNumberOfPoints
 * @see setMode
 * @see setBunch1
 * @see setBunch2
 * @see setDate1
 * @see setDate2
 */
void TRealtimePlot::updateParams()
{
    time_t tstamp1 ;
    time_t tstamp2;
    QTime time1;
    QTime time2;
    switch(d_mode)
    {
      case 0:
        d_defaultArgin = QString("(0,%1)").arg(d_nPoints);
        break;
      case 1:
        d_defaultArgin = QString("(1,%1,%2)").arg(d_b1).arg(d_b2);
        break;
      case 2:
        tstamp1 = d_date1.toTime_t();
        tstamp2 = d_date2.toTime_t();
        time1 = d_date1.time();
        time2 = d_date2.time();
        d_defaultArgin = QString("(2,%1,%2,%3,%4)").arg(tstamp1).arg(time1.msec()).arg(tstamp2).arg(time2.msec());
        break;
      default:
        perr("TRealtimePlot \"%s\": invalid mode %d", qstoc(objectName()), d_mode);
    }
    pinfo("TRealtimePlot \"%s\" setMode(%d): new argin: \"%s\" - sources follow", qstoc(objectName()), d_mode, qstoc(d_defaultArgin));
    QStringList sources = sourcesList();
    qslisttoc(sources);
    clearArginsFromSourcesList(sources);
    /* set the period before setting the sources again */
    //unsetSources();
    setPeriod(period());
    /* call setSources() with the new sources without any argin and the fresh d_default argin extracted from the
     * properties. setSources() will encounter a list of sources without any argin, since we called
     * clearArginsFromSourcesList() and so it will only append the d_defaultArgin.
     */
    setSources(sources.join(";"));
}

QString TRealtimePlot::plotTitle()
{
  QString title = TPlotLightMarker::plotTitle();
  if(sourcesList().size())
  {
	/* append to the TPlotLightMarker title some information about the mode */
	switch(d_mode)
	{
	  case 0:
		title += QString("\nMode: last %1 samples").arg(d_nPoints);
		break;
	  case 1:
		title += QString("\nMode: from bunch %1 to %2").arg(d_b1).arg(d_b2);
		break;
	  case 2:
		title += QString("\nMode: timestamp from \n%1 to %2").arg(d_date1.toString("d MMM yy hh:mm:ss:zzz"))
		  .arg(d_date2.toString("d MMM yy hh:mm:ss:zzz"));
		break;
	}
	title += QString("\nPlot Refresh frequency: %1Hz").arg(1000.0/period());
  }
  return title;
}

