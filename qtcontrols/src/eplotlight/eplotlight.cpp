#include "eplotlight.h"
#include <QPaintEngine>
#include <macros.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_legend.h>
#include <QtDebug>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include "eplot_configuration_widget.h"
#include "ezoomer.h"
#include "snapshot.h"
#include "plotsaver.h"
#include "eplotcurve.h"
#include <qwt_plot_renderer.h>
#include <elettracolors.h>

EPlotLight::EPlotLight(QWidget *parent) : EPlotLightBase(parent)
{
 init();
}

EPlotLight::EPlotLight(const QwtText &title, QWidget *parent) : EPlotLightBase(title, parent)
{
  init();
}

void EPlotLight::init()
{
  /* scales are initialized to auto scale, both for x and for y. 
   * X scale is managed internally by EPlotLightBase, through the refresh() method.
   */
  d_bufSiz = -1;
  d_rightClickMenu = new QMenu("Plot", this);
  d_rightClickMenu->addAction("Print", this, SLOT(print()));
  d_rightClickMenu->addAction("Snapshot of the plot", this, SLOT(snapshot()));
  d_rightClickMenu->addAction("Copy plot image to clipboard", this, SLOT(copyImage()));
  d_rightClickMenu->addAction("Save Data", this, SLOT(saveData()));
  d_rightClickMenu->addSeparator();
  d_rightClickMenu->addAction("Configure Plot", this, SLOT(configurePlot()));
  QAction *showYValuesAct = new QAction("Show Y values on curves", this);
  showYValuesAct->setCheckable(true);
  showYValuesAct->setChecked(false);
  connect(showYValuesAct, SIGNAL(toggled(bool)), this, SLOT(setShowYValuesEnabled(bool)));
  d_rightClickMenu->addAction(showYValuesAct);
  d_rightClickMenu->addAction("Clear Plot", this, SLOT(clearPlot()));
  d_colors << KDARKGREEN << EBLUEGRAY << KDARKMAROON << KDARKVIOLET << KBLUE 
	<< KMAROON << KRED << KVERYDARKVIOLET << KYELLOW << KORANGE << KDARKCYAN;
  /* dynamic property to manage alpha channel for curves */
  setProperty("curvesAlphaChannel", 255);
}

void EPlotLight::appendData(const QString& curveName, double x, double y)
{
    appendData(curveName, &x, &y, 1);
}
    
void EPlotLight::appendData(const QString& curveName, double *x, double *y, int size)
{
	if(!d_curvesMap.contains(curveName))
	{
	  pinfo("EPlotLight \"%s\": appendData curve with name \"%s\" not existing: creating it", qstoc(objectName()), qstoc(curveName));
	  /* create the curve and add it into the curves map */
	  createAndAddCurve(curveName);
    }
	EPlotCurve* curve = d_curvesMap.value(curveName);
	/* pop front some data if buffer size is > 0 */
    while(d_bufSiz > 0 && timeScaleDrawEnabled() && curve->count() > 1 &&
          curve->data()->sample(curve->count() - 1).x() - curve->data()->sample(0).x() > d_bufSiz)
		curve->popFront();
	while(d_bufSiz > 0 && !timeScaleDrawEnabled() && curve->count() > d_bufSiz)
		curve->popFront();
	
    *x = *x * 1000;
    curve->appendData(x, y, size);
	curve->updateRawData();

    replot();
   // printf("\e[1;33mNOTE\e[0m: check if repainting!\n");
   // curve->drawSeries(curve->dataSize() - size, curve->dataSize() - 2);
}
    
void EPlotLight::setData(const QString& curveName, const QVector< double > &xData, const QVector< double > &yData)
{
  if(!d_curvesMap.contains(curveName))
  {
	  pinfo("EPlotLight \"%s\": setData curve with name \"%s\" not existing: creating it", qstoc(objectName()), qstoc(curveName));
	  /* create the curve and add it into the curves map */
	  createAndAddCurve(curveName);
  }
  EPlotCurve* curve = d_curvesMap.value(curveName);
  curve->setData(xData, yData);
}

void EPlotLight::createAndAddCurve(const QString &curveName)
{
  EPlotCurve *plotCurve = new EPlotCurve(this, curveName);
  /* and add it to the plot. This will select a color for the curve */
  addCurve(curveName, plotCurve);
}

void EPlotLight::addCurve(const QString& title, EPlotCurve *curve)
{
  if(!d_curvesMap.contains(title))
  {
      QSettings s;

      QString id = QString("%1_EPLOTLIGHT_CURVE_%2").arg(QCoreApplication::instance()->applicationName()).arg(title);

      /* choose a nice curve color or load it from QSettings, if defined for the application name and
       * curve name.
       */
      QColor curveColor = s.value(id + "_COLOR", d_colors.at(d_curvesMap.size() % d_colors.size())).value<QColor>();
      int alpha = s.value(id + "_COLOR_ALPHA", property("curvesAlphaChannel").toInt()).toInt();
      int curveStyle = s.value(id + "_STYLE", QwtPlotCurve::Lines).toInt();
      double penWidth = s.value(id + "_WIDTH", 0.0).toDouble();
      bool displayYValues = s.value(id + "_DISPLAY_YVALUES", false).toBool();
      curveColor.setAlpha(alpha);
      QPen curvePen(curveColor);
      curvePen.setWidth(penWidth);
      curve->setStyle((QwtPlotCurve::CurveStyle) curveStyle);
      curve->setPen(curvePen);
      curve->setProperty("showYValuesEnabled", displayYValues);
      d_curvesMap.insert(title, curve);
      curve->attach(this);
      emit curveAdded(curve);
    }
  else
  {
      pinfo("curve \"%s\" already present: \e[1;31mnot\e[0m substituting it", qstoc(title));
      delete curve;
  }
}

void EPlotLight::removeCurve(const QString &curveName)
{
  if(d_curvesMap.contains(curveName))
  {
        EPlotCurve* curve = d_curvesMap.value(curveName);
	d_curvesMap.remove(curveName);
	curve->detach();
	emit curveRemoved(curve);
        pinfo("EPlotLight: removed curve \"%s\" %d curves remaining", qstoc(curveName), d_curvesMap.size());
	delete curve;
  }
  else
	perr("curve \"%s\" is not present. Did you first add it with addCurve()?", qstoc(curveName));
}
   
void EPlotLight::clearPlot()
{
  foreach(EPlotCurve *c, d_curvesMap.values())
	c->clearData();
  replot();
}

void EPlotLight::removeData()
{

}
  
int EPlotLight::timeInterval() const
{
  return 0;
}

void EPlotLight::setTimeInterval(int )
{
  
}

void EPlotLight::setDataBufferSize(int s)
{
  d_bufSiz = s;
}

void EPlotLight::setCurveStyle(const QString &curveName, QwtPlotCurve::CurveStyle style)
{
  if(d_curvesMap.contains(curveName))
  {
    EPlotCurve *pCurve = d_curvesMap.value(curveName);
    pCurve->setStyle(style);
    replot();
  }
  else
	perr("EPlotLight::setCurveStyle(): no curve with name \"%s\"", qstoc(curveName));
}

EPlotCurve *EPlotLight::curve(const QString& name)
{
  EPlotCurve *crv = NULL;
  if(hasCurveIgnoreCase(name))
  {
    crv = getCurveIgnoreCase(name);
  }
  return crv;
}
	
QString EPlotLight::curveName(EPlotCurve *curve)
{
  QString name;
  if(d_curvesMap.values().contains(curve))
	name = d_curvesMap.key(curve);
  return name;
}

bool EPlotLight::hasCurveIgnoreCase(const QString &name) const
{
    foreach(QString cname, d_curvesMap.keys())
        if(cname.compare(name, Qt::CaseInsensitive) == 0)
            return true;
    return false;
}

EPlotCurve *EPlotLight::getCurveIgnoreCase(const QString &name)
{
    EPlotCurve *crv = NULL;
    foreach(QString cname, d_curvesMap.keys())
    {
        if(cname.compare(name, Qt::CaseInsensitive) == 0)
            crv = d_curvesMap.value(cname);
    }
    return crv;
}

void EPlotLight::snapshot()
{
  /* temporary enable the title on the canvas, saving its visibility 
   * state for subsequent restore.
   */
  bool saveTitleShown = titleOnCanvasEnabled();
  setTitleOnCanvasEnabled(true);
  PlotSnapshot pSnap;
  pSnap.grab(this);  
  setTitleOnCanvasEnabled(saveTitleShown);
}

void EPlotLight::copyImage()
{
  /* temporary enable the title on the canvas, saving its visibility 
   * state for subsequent restore.
   */
  bool saveTitleShown = titleOnCanvasEnabled();
  setTitleOnCanvasEnabled(true);
//   qApp->processEvents(); /* force the QApplication refresh of the widget before grabbing it */
  PlotSnapshot pSnap;
  pSnap.copy(this);
  setTitleOnCanvasEnabled(saveTitleShown);
}

void EPlotLight::saveData()
{
  PlotSaver plotSaver;
  if(!plotSaver.save(d_curvesMap.values(), timeScaleDrawEnabled()))
	QMessageBox::critical(this, "Error saving on file", QString("Error saving file \"%1\":\n%2").arg(plotSaver.fileName()).
		  arg(plotSaver.errorMessage()));
}

void EPlotLight::print()
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
        QwtPlotRenderer renderer;
        renderer.renderTo(this, printer);
	}
}

void EPlotLight::configurePlot()
{
  EPlotConfigurationDialog *cdialog = new EPlotConfigurationDialog(this);
  cdialog->show();
}


void EPlotLight::setShowYValuesEnabled(bool show)
{
    foreach(EPlotCurve *epc, d_curvesMap.values())
        epc->setShowYValuesEnabled(show);
}

bool EPlotLight::showYValuesEnabled() const
{
    foreach(EPlotCurve *epc, d_curvesMap.values())
        return epc->showYValuesEnabled();
    return false;
}


