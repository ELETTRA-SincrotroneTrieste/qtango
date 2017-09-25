#ifndef DBPLOTADAPTERINTERFACE_H
#define DBPLOTADAPTERINTERFACE_H

#include <QObject>
#include <QVector>
#include "dbplotadapterinterfaceprivate.h"

class QDateTime;
class EPlotLight;
class DbPlotBackground;

/** \brief Adapter to use EPlotLightMarker plot families with QTango hdb.
  *
  * You must inherit from this interface in order to get an adapter aimed at transforming
  * an EPlotLight or EPlotLightMarker into a plot capable of displaying historical data.
  * A bunch of methods need to be implemented
  * \list
  * \li historicalCurvelist
  * \li uniformizeCurveStyles
  * \li type
  * \endlist
  *
  * See the HdbPlotAdapter implementation for further details and an example.
  *
  * @see HdbPlotAdapter
  *
  */
class DbPlotAdapterInterface : public QObject
{
    Q_OBJECT
public:

    explicit DbPlotAdapterInterface(EPlotLight *parent);

    bool backgroundEnabled() const { return d_ptr->backgroundEnabled(); }

    EPlotLight *plot() { return d_ptr->plot(); }

    /** \brief returns the list of historical curves
      *
      * This method is pure virtual and must be reimplemented in a subclass.
      *
      * This method must implement a criterium to distinguish between historical and
      * non historical curves.
      * The HdbPlotAdapter subclass provides an example for the method implementation.
      *
      * @see HdbPlotAdapter::historicalCurvesList
      *
      */
    virtual QList<EPlotCurve *> historicalCurvesList() = 0;

    /** \brief If historical and <em>live</em> curves live in the same plot, they must have
      *        the same aspect (pen, brush, width)
      *
      * Historical and non historical curves referring to the same tango point must be individuated
      * and their pen and brush must be set equal.
      *
      * @see HdbPlotAdapter::historicalCurvesList
      */
    virtual void uniformizeCurveStyles() = 0;

    /** \brief must return a QString that identifies the type of curve.
      *
      * The implementation provided in HdbPlotAdapter::type returns the string ,em>Hdb</em>,
      * but another implementation may work with the temporary database for instance, and so
      * return the <em>Tdb</em> string.
      *
      * @see HdbPlotAdapter::type
      *
      */
    virtual QString type() const = 0;

signals:

    /** \brief a simple signal to directly request data given a source and a start/stop date time
     */
    void dataRequest(const QString& source, const QDateTime& startDate, const QDateTime& stopDate);

    /** \brief signal emitted by the setData method after setting the data on the plot.
      *
      * When the signal is emitted, the data has been set on the plot and the refresh() method
      * of the EPlotLight has been invoked.
      */
    void dataUpdated();


public slots:

    void installInterpolatorOnCurve(const QString& curveName, int period);

    virtual void setData(const QString& source, const QVector<double >&xData, const QVector<double> &yData);

    virtual void setSetPointData(const QString& source, const QVector<double >&xData, const QVector<double> &yData);

    /** \brief enables the background of the plot.
      *
      * @param en true background enabled
      * @param en false background disabled
      *
      * This feature is not fully implemented yet. Do not enable the background ;)
      *
      */
    void setBackgroundEnabled(bool en) { d_ptr->setBackgroundEnabled(en); }

    void addBackground(double start, double stop);

    /** \brief Executes a configuration dialog
      *
      */
    virtual int configureDialog();

    void clearHistoricalData();

    void deleteHistoricalCurve(const QString& source);

    void deleteHistoricalCurves();

    void clearCurveHistory(const QString &source);

    virtual EPlotCurve *getCurve(const QString& source) = 0;

    virtual QString curveName(const QString& source) = 0;

    /** \brief Returns a reference to the plot background.
      *
      * @return the DbPlotBackground attached to the plot
      * @return NULL if no DbPlotBackground was attached to the plot
      *
      * @see setBackgroundEnabled
      *
      * \note the plot background is not fully implemented yet.
      *
      */
    DbPlotBackground *dbPlotBackground() { return d_ptr->background(); }

private:
    DbPlotAdapterInterfacePrivate *d_ptr;

    Q_DECLARE_PRIVATE(DbPlotAdapterInterface)

};

#endif // DBPLOTADAPTERINTERFACE_H
