#ifndef HDBPLOTADAPTER_H
#define HDBPLOTADAPTER_H

#include "dbplotadapterinterface.h"

/** \brief An implementation of the DbPlotAdapterInterface aimed at providing and
  *        adapter to transform an EPlotLight into a plot capable of displaying and
  *        managing historical data together with <em>live data</em> read from the field.
  *
  * @see DbPlotAdapterInterface
  *
  * Construct an EPlotLight or an EPlotLightMarker and then pass it as parameter to the
  * constructor of HdbPlotAdapter in order to adapt your EPlotLight so that it can display
  * historical data from the historical database.
  * <br/>
  *
  * This class reimplements a bunch of methods that are pure virtual methods in the
  * DbPlotAdapterInterface.
  *
  * @see THdb documentation
  *
  * \par Implementing a plot that reads historical data from the Hdb database.
  * \code
  * // Instantiate a THdb object, a threaded data retriever. It will query the sql database
  * // to get archived data.
  * //
  * THdb *thdb = new THdb(this);
  * // This tells thdb that the database is the historical one and provides default
  * // connection parameters, which are read from the HDB_SERVER environment variable
  * // in the form host:portNumber
  *  //
    thdb->setType(THdbBase::Historical, HdbConnectionParams());
    //
    // suppose we want to retrieve data archived during the last day
    //
    QDateTime now = QDateTime::currentDateTime();
    QDateTime start = now.addDays(-1);

    // now instantiate the plot adapter
    //
    HdbPlotAdapter *hdbPlotAdapter = new HdbPlotAdapter(ui->plot);

    // SIGNAL/SLOT connections to display data in the plot
    connect(thdb,
            SIGNAL(dataAvailable(const QString&, const QVector<double >&, const QVector<double> &)),
            hdbPlotAdapter,
            SLOT(setData(const QString&, const QVector<double >&, const QVector<double> &)));

    connect(thdb,
            SIGNAL(dataSetPointAvailable(const QString&, const QVector<double >&, const QVector<double> &)),
            hdbPlotAdapter,
            SLOT(setSetPointData(const QString&, const QVector<double >&, const QVector<double> &)));

    // get data! The query is executed in another thread. When data is ready, your setData and setSetPoint
    // data will be invoked thanks to the connections made above.
    // You need not worry about threads anyway!
    //
    thdb->getData("test/device/1/double_scalar", start, now);

    // ui->plot is your EPlotLight or EPlotLightMarker. If you use a TPlotLightMarker object for the plot,
    // then you even can call setSourcesList or setSources to enable the <em>live</em> mode, where
    // the archived data is shown together with the data read from the field.
  *
  * \endcode
  *
  * \note Please read carefully the THdb library documentation in order to understand how things work
  *       as to database data retrieval. The data fetch is performed in another thread, but the THdb
  *       library is structured in a way that threads are transaparent to you. You receive the data when
  *       it is ready, and that's all. Special care must be taken when subsequent queries must be done
  *       and the query order is relevant. But this is a THdb related issue...
  *
  * THdb library sources comes with an example under the <em>test/</em> directory.
  */
class HdbPlotAdapter : public DbPlotAdapterInterface
{
public:
    HdbPlotAdapter(EPlotLight *parent);

    virtual EPlotCurve *getCurve(const QString& source);

    virtual QString curveName(const QString& source);

    virtual QList<EPlotCurve *> historicalCurvesList();

    virtual void uniformizeCurveStyles();

    /** \brief returns the identification string <em>Hdb</em>
      *
      * @return the string <em>Hdb</em>.
      */
    virtual QString type() const { return "Hdb"; }
};

#endif // HDBPLOTADAPTER_H
