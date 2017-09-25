#ifndef QTWATCHER_H
#define QTWATCHER_H

#include <QObject>
#include <com_proxy_reader.h>
#include "qtangoizer.h"

/** \brief Manage readings of various type of objects in a different thread.
  *
  * QTWatcher is a special class that reads a tango variable using QTangoCore.
  * Any QObject or base types (int, short, double, unsigned int, QString, bool) can be
  * attached to this reader and obtain the new data when the attribute or command is
  * refreshed.
  * If a QObject is attached, then a slot must be provided together with the object so that
  * the slot is invoked with a compatible input parameter in order to update the object itself on
  * new data arrival.
  * The data type that must be managed by the QTWatcher is guessed from the slot provided or
  * detected according to one of the other attach methods available.
  *
  * \par Single shot readings
  *
  * Call setSingleShot with a true parameter to make the QTWatcher perform a only single reading.
  *
  * <strong>warning</strong>: singleShot property is read inside the refresh method. Reimplementing
  * the refresh() method in your own subclass means taking care of correctly managing the
  * singleShot property.
  *
  * @see QTangoizer::setSingleShot
  * @see QTangoizer::singleShot
  *
  * In single shot mode, newData, attributeAutoConfigured, connectionOk and
  * connectionFailed signals are still emitted.
  *
  * \par Example
  * \code
  *
  *
    QProgressBar *progressBar = new QProgressBar(this);
    QTWatcher *progressBarWatcher = new QTWatcher(this);
    progressBarWatcher->attach(progressBar, SLOT(setValue(int)));
    // configure maximum and minimum values when available
    progressBarWatcher->setAutoConfSlot(QTWatcher::Min, SLOT(setMinimum(int)));
    progressBarWatcher->setAutoConfSlot(QTWatcher::Max, SLOT(setMaximum(int)));

    progressBarWatcher->setSource("$1/short_scalar_ro");
  *
  * \endcode
  *
  * That's it! The QProgressBar will be updated via the setValue(int) slot whenever new
  * data is available and even its maximum and minumum values are updated when available
  * through the QTango auto configuration process.
  *
  * \par Example 2
  *\code
    int intVar; // want to monitor a simple integer value
    QTWatcher *intWatcher = new QTWatcher(this);
    intWatcher->attach(&intVar); // attach the integer variable to intWatcher
    intWatcher->setSource("$1/long_scalar"); // read!
  * \endcode
  *
  * See the qtango/test/watcher example inside the library distribution for an example
  *
  * \chapter Supported data types (release 4.2.0)
  * \list
  * \li int
  * \li short
  * \li unsigned int
  * \li double
  * \li Tango::DevState
  * \li QString
  * \li bool
  * \li QStringList
  * \li QVector<double>
  * \li QVector<int>
  * \li QVector<bool>
  *
  * \endlist
  * \endchapter

  * <h3>News in release 4.3.0</h3>
  * <p> Since QTango release 4.3.0, it is possible to setAutoDestroy to true to have your reader automatically
  * destroyed after first read takes place. <strong>Beware that the object will be destroyed even if
  * the execution fails</strong>.
  * <br/> Secondly, you can call setSingleShot to have your reader perform only one reading from the
  * configured source.
  *
  * </p>
  *
  *
  */
class QTWatcher : public QTangoizer, public QTangoComProxyReader
{
Q_OBJECT
public:
    /** \brief Constructs a reader with parent parent.
     *
     * @see QTangoizer class documentation.
     */
    QTWatcher(QObject *parent);

    virtual ~QTWatcher();

public slots:

    /** \brief The refresh method.
      *
      * This can be reimplemented in a subclass to further customize the behaviour of the
      * data conversion method.
      *
      * @param v the TVariant from which data is extracted according to the data type
      *        it contains.
      *
      * @see TValueFilter
      *
      * Read TValueFilter before subclassing QTWatcher.
      */
    virtual void refresh(const TVariant &v);

private:

};

#endif // READER_H
