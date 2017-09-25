#include "tplotlight_marker.h"
#include <eplotcurve.h>
#include <elettracolors.h>
#include <QDateTime>
#include <QtDebug>
#include <QMenu>

TPlotLightMarker::TPlotLightMarker(QWidget*parent) : EPlotLightMarker(parent), TPlotRefreshConnector(this)
{	
    bg = new Background();
    bg->attach(this);
    QDateTime dt = QDateTime::currentDateTime();
    setHdbActionEnabled(true);
    d_autoConfiguredUpperBound = -1.0;
    d_autoConfiguredLowerBound = -1.0;
    d_boundsFromAutoConfiguration = true;
    d_yLowBoundAdjusted = d_yUpBoundAdjusted = false;
    /* autoscale enabled in EPlotLightBase */
    setAxisScale(QwtPlot::xBottom,  0, 1000);
    QMenu *menu = rightClickMenu();
    if(menu)
    {
        menu->addAction("Pause Acquisition", this, SLOT(pauseAcquisition()));
        menu->addAction("Restart Acquisition", this, SLOT(restartAcquisition()));
    }
    setProperty("m_setPointEnabled", false);
}

void TPlotLightMarker::updateCurve(const TVariant &v)
{
    double val;
    setToolTip(v.message());

        if(!isEnabled())
            setEnabled(communicationStatusChangesWidgetEnable());
        QString name = v.tangoPoint();
        name.remove(QRegExp("\\(.*\\)"));
        QString curveSetPointName = name + "[set]";

        EPlotCurve *eplc = NULL;
        EPlotCurve *crv = curve(name);

        //printf("\e[1;36mcurves found:\e[0m\n");
       // foreach(QwtPlotCurve *qc, curves())
      //  {
         //   EPlotCurve *c = static_cast<EPlotCurve *>(qc);
         //   printf("\e[1;33mTPlotLightMarker::updateCurve: curve %s: %p\e[0m\n", qstoc(c->title().text()), c);
         //   printf("\e[0;33m-TPlotLightMarker::updateCurve: dataSize %d from %s to %s\e[0m\n", c->dataSize(),
         //          qstoc(QDateTime::fromTime_t(c->x(0)).toString()),
         //          qstoc(QDateTime::fromTime_t(c->x(c->dataSize() - 1)).toString()));
       // }


        if(!crv)
        {
            printf("\e[1;31mNO CURVES FOUND WITH NAME \"%s\"\e[0m\n", qstoc(name));
            addCurve(name, crv = new EPlotCurve(this, name));
        }

        /* set quality (state) on the corresponding curve */
        if(v.quality() == ATTR_INVALID)
            crv->setState(EPlotCurve::Invalid);
        else if(v.quality() == Tango::ATTR_WARNING)
            crv->setState(EPlotCurve::Warning);
        else if(v.quality() == Tango::ATTR_ALARM)
            crv->setState(EPlotCurve::Alarm);
        else
            crv->setState(EPlotCurve::Normal);

        if(v.quality() != Tango::ATTR_INVALID && !v.isCommand() && v.getAttributeInfo().writable == Tango::READ_WRITE)
        {
            eplc = curve(curveSetPointName);
            if(!eplc)
                addCurve(curveSetPointName, eplc = new EPlotCurve(this, curveSetPointName));
            eplc->setVisible(property("m_setPointEnabled").toBool());
        }
        if(v.quality() != Tango::ATTR_INVALID && v.canConvertToDouble())
        {
            double xValue;
            if(!timeScaleDrawEnabled())
            {
                if(crv)
                {
                    if(!crv->size()) /* first x value */
                        xValue = crv->size();
                    else
                        xValue = crv->x(crv->size() - 1) + 1;
                }
            }
            else
                xValue = v.timeRead().tv_sec + v.timeRead().tv_usec * 0.000001;
            val = v.toDouble();
            /* do some optimizations: do not add to the curve data couples equal to the last value inserted. Some attributes
		   * might have a refresh interval faster than the data effective change.
		   */
            if(!crv->dataSize() || (crv->dataSize() > 0 && (crv->x(crv->dataSize() - 1) != xValue || crv->y(crv->dataSize() - 1) != val)))
                appendData(name, xValue, val);

            if(eplc) /* check this: eplc is NULL if attributeInfo.writable is not READ_WRITE */
            {
                double ySetPoint = v.toDouble(false);
                if(!eplc->dataSize() || (eplc->dataSize() && (eplc->x(eplc->dataSize() - 1) != xValue ||
                                                              eplc->y(eplc->dataSize() - 1) != ySetPoint)))
                    appendData(curveSetPointName, xValue, v.toDouble(false));
                // 		printf("\e[1;35mAppending x \"%s\" y %.2f\n", qstoc(QDateTime::fromTime_t(tangoTimestamp).toString()), val);
            }
        }
        else if(v.quality() != Tango::ATTR_INVALID && v.canConvertToDoubleVector())
        {
            QVector<double> yData = v.toDoubleVector(true);
            QVector<double> xData;
            for(int i = 0; i < yData.size(); i++)
                xData.push_back(i);
            setData(name, xData, yData);

            if(eplc) /* check this: eplc is NULL if attributeInfo.writable is not READ_WRITE */
                setData(curveSetPointName, xData, v.toDoubleVector(false));

            qDebug() << xData << yData;
        }
        /* update if the refresh timer is not active (sync mode is enabled) */
        if(!d_plotRefreshTimer->isActive())
        {
//             printf("\e[1;32m ----->> updateCurve() DIRECT REFRESH < ----- \e[0m\n");
            refresh();
        }
//        else
 //           printf("\e[1;35m ------>>  updateCurve() NO DIRECT REFRESH IN updateValue() << ------\e[0m\n");

}

bool TPlotLightMarker::syncRefresh() const
{
    return d_plotRefreshTimer->isActive();
}

void TPlotLightMarker::setSyncRefresh(bool en)
{
    if(!en)
        d_plotRefreshTimer->stop();
    else
        d_plotRefreshTimer->start();
}


void TPlotLightMarker::pauseAcquisition()
{
    TPlotRefreshConnector::pauseAcquisition();
}

void TPlotLightMarker::restartAcquisition()
{
    TPlotRefreshConnector::restartAcquisition();
}

void TPlotLightMarker::autoConfiguration(const TangoConfigurationParameters *cp)
{
    bg->configure(cp);
    if(cp->writeType() == Tango::READ_WRITE)
    {
	QString srcSetPoint = cp->currentValue().tangoPoint() + "[set]";
	EPlotCurve *plotCurve = new EPlotCurve(this, srcSetPoint);
	addCurve(srcSetPoint, plotCurve);
    }
}

void TPlotLightMarker::substitutePastedSource()
{
    setSources(pastedSource());
}

void TPlotLightMarker::addPastedSource()
{
    if(sources() != QString())
	_addSource(pastedSource());
    else
        setSources(pastedSource());
}

void TPlotLightMarker::extendYLowerBoundFromAutoConf(double l)
{
    if(d_boundsFromAutoConfiguration)
    {
	if(!d_yLowBoundAdjusted)
	{
            EPlotLightBase::setYLowerBound(l);
            d_yLowBoundAdjusted = true;
	}
	else
            EPlotLightBase::extendYLowerBound(l);
	d_autoConfiguredLowerBound = EPlotLightBase::yLowerBound();
	pinfo("auto configuration possible for \"%s\" source \"%s\": saving lower bound %.2f",
              qstoc(objectName()), qstoc(sources()), l);
    }
}

void TPlotLightMarker::extendYUpperBoundFromAutoConf(double u)
{
    if(d_boundsFromAutoConfiguration)
    {
	if(!d_yUpBoundAdjusted)
	{
            EPlotLightBase::setYUpperBound(u);
            d_yUpBoundAdjusted = true;
	}
	else
            EPlotLightBase::extendYUpperBound(u);
	d_autoConfiguredUpperBound = EPlotLightBase::yUpperBound();
	pinfo("auto configuration possible for \"%s\" source \"%s\": saving upper bound %.2f",
              qstoc(objectName()), qstoc(sources()), u);
    }
}

void TPlotLightMarker::restoreBoundsToAutoConfigurationValues()
{
    if(d_autoConfiguredLowerBound != d_autoConfiguredUpperBound)
    {
	EPlotLightBase::setYUpperBound(d_autoConfiguredUpperBound);
	EPlotLightBase::setYLowerBound(d_autoConfiguredLowerBound);
	pinfo("TPlotLightMarker::restoreBoundsToAutoConfigurationValues(): restoring min %.2f max %.2f", d_autoConfiguredLowerBound,
              d_autoConfiguredUpperBound);
    }
    else
	perr("TPlotLightMarker::restoreBoundstoAutoConfigurationValues(): the plot \"%s\", source(s) \"%s\" wasn't auto configured",
             qstoc(objectName()), qstoc(sources()));
}

QString TPlotLightMarker::plotTitle()
{
    QString ptitle;
    foreach(QwtPlotCurve *crv, curves())
        ptitle += crv->title().text() + "\n";
    if(ptitle.size() >= 1)
         ptitle.remove(ptitle.length() - 1);
    return ptitle;
}

void TPlotLightMarker::showEvent(QShowEvent *e)
{
    plotShowEvent();
    EPlotLightMarker::showEvent(e);
}

void TPlotLightMarker::hideEvent(QHideEvent *e)
{
    plotHideEvent();
    EPlotLightMarker::hideEvent(e);
}

EPlotCurve *TPlotLightMarker::curveBySource(const QString& source)
{
    QString cleanSource = source;
    cleanSource.remove(QRegExp("\\(.*\\)"));
    return curve(cleanSource);
}

QString TPlotLightMarker::connectionStatus()
{
    QString ret;
    foreach(PlotComProxyReader *r, m_readersMap.keys())
        ret += r->qtangoComHandle()->connectionStatus() + "\n";
    return ret;
}

bool TPlotLightMarker::configured()
{
    bool ok = true;
    foreach(PlotComProxyReader *r, m_readersMap.keys())
        ok = ok && r->qtangoComHandle()->configured();
    if(m_readersMap.size())
        return ok;
    else
        return false;
}

bool TPlotLightMarker::setPointEnabled() const
{
    return property("m_setPointEnabled").toBool();
}

void TPlotLightMarker::setSetPointEnabled(bool en)
{
    setProperty("m_setPointEnabled", en);
    foreach(QwtPlotCurve *c, curves())
    {
        /* contains, not endsWith because of [HDB] ending curves */
        if(c->title().text().contains("[set]"))
            c->setVisible(en);
    }
    replot();
}


