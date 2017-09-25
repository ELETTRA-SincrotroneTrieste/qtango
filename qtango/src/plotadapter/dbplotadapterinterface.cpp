#include "dbplotadapterinterface.h"
#include "dbplotbackground.h"

#include <EPlotLight>
#include <eplotcurve.h>
#include <QtDebug>
#include <macros.h>

/** \brief The constructor, taking an EPlotLight as parameter.
  *
  * @param plot an instance of EPlotLight
  *
  */
DbPlotAdapterInterface::DbPlotAdapterInterface(EPlotLight *plot) :
    QObject(plot)
{
    d_ptr = new DbPlotAdapterInterfacePrivate(this, plot);
    //setBackgroundEnabled(true);
}

/*! \brief sets the current data for the given source (curve name)
 *
 * @param source the name of the source (tango point in the form a/b/c/AttributeName)
 *        that data is referred to.
 *
 * @param constXData a vector of double representing data for the x axis (timestamp)
 * @param constYData a vector of double representing data for the y axis
 *
 * The method emits the dataUpdated signal after the data has been internally updated.
 *
 * @see setSetPointData, which is used to store and display the Tango set point for the
 *      source.
 *
 */
void DbPlotAdapterInterface::setData(const QString& source,
  const QVector<double >&constXData, const QVector<double> &constYData)
{
    d_ptr->setData(source, constXData, constYData);
    emit dataUpdated();
}

/*! \brief sets the current set point data for the given source (curve name)
 *
 * @param source the name of the source (tango point in the form a/b/c/AttributeName)
 *        that data is referred to.
 *
 * @param constXData a vector of double representing the data for the x axis (timestamp)
 * @param constYData a vector of double representing  <em>set point</em> data for the y axis
 *
 * @see setData, which is used to store and display the Tango read data for the
 *      source.
 *
 */
void DbPlotAdapterInterface::setSetPointData(const QString& source,
       const QVector<double >&xData, const QVector<double> &yData)
{
    d_ptr->setSetPointData(source, xData, yData);
}

/*! \brief Executes a configuration dialog, if reimplemented
  *
  * This method does nothing. Returns 0.
  */
int DbPlotAdapterInterface::configureDialog()
{
    return 0;
}

/*! \brief adds a background to the plot canvas between startDate and stopDate.
  *
  * @param startDate a unix timestamp, start of interval.
  * @param stopDate a unix timestamp, end of interval.
  *
  * @see setBackgroundEnabled
  *
  * This feature is not fully supported yet. Do not use it ;)
  */
void DbPlotAdapterInterface::addBackground(double startDate, double stopDate)
{
    d_ptr->addBackground(startDate, stopDate);
}

/*! \brief clears the data of each historical curve, and triggers a plot refresh in order
  *        to redraw the plot itself.
  *
  * This method only clears the data belonging to the historical curves.
  */
void DbPlotAdapterInterface::clearHistoricalData()
{
    d_ptr->clearHistoricalData();
}

/** \brief removes the historical curve identified by its name source
  *
  * @param source the name of the curve to remove.
  *
  * \note If there is an associated <em> set point curve </em>, whose title displays an
  * <em> [set] </em> suffix, then <em>the set point curve is removed too</em>.
  *
  */
void DbPlotAdapterInterface::deleteHistoricalCurve(const QString& source)
{
    d_ptr->deleteHistoricalCurve(source);
}

/*! \brief deletes <em>all</em> the historical curves
  *
  * @see deleteHistoricalCurve
  */
void DbPlotAdapterInterface::deleteHistoricalCurves()
{
    d_ptr->deleteHistoricalCurves();
}

/*! \brief for the specified curve with name source, this method clears all its data.
  *
  * This operation sets empty vectors of data for the x and y data of the selected curve
  *
  *@param source the name of the curve. Typically, the tango source point in the form
  *       <em>a/b/c/AttributeName</em>.
  *
  */
void DbPlotAdapterInterface::clearCurveHistory(const QString& source)
{
    d_ptr->clearCurveHistory(source);
}

void DbPlotAdapterInterface::installInterpolatorOnCurve(const QString& curveName, int period)
{
    d_ptr->interpolatorDataHash.insert(curveName, period);
}


