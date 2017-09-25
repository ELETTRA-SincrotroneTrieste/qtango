#ifndef QTANGOIZER_H
#define QTANGOIZER_H

#include <QObject>
#include <auto_configuration.h>
#include "tvaluefilter.h"

class QTangoizerPrivate;

/** \brief QTangoizes anything (not really anything, but...)
  *
  * This base class is reimplemented by QTWatcher and QTWriter to use QTangoCore
  * in order to update widgets that are not QTango aware.
  */
class QTangoizer : public QObject
{
    Q_OBJECT

public:
    /* Compatible parent object types supported */
    enum Type { Invalid = -1, Void, Int, Double, UInt, Short, String, State, Bool,
            BoolVector, DoubleVector, StringVector, IntVector };

    /** \brief Supported auto configuration parameters with which you can invoke slots on the
  * attached refreshee
  *
  * @see attach
  * @see setAutoConfSlot
  */
    enum AutoConfType { Min, Max, MinWarn, MaxWarn, MinErr, MaxErr,
                        StdUnit, Unit, DisplayUnit, Description, Label };

    QTangoizer(QObject *parent);

    virtual ~QTangoizer();


    /** \brief associate a QObject and one or two methods to be invoked by the class
      *        on new data arrival (QTWatcher) or on target execution (QTWriter).
      *
      * This method associates a QObject on which to invoke a slot when new data arrives
      * (for readers) or a QObject from which to wait for signals to execute a target
      * (for writers).
      *
      * @param object the QObject pointer
      * @param method the SLOT (readers) or SIGNAL (writers) methods to associate
      * @param setPointSlot if not NULL, this is the slot to be invoked in auto configuration
      *        to initialize the value read first.
      * @param connType the Qt::ConnectionType for signals/slots
      *
      * The type of input parameter is detected on the basis of the method specified.
      * For instance setValue(int) will detect an Int data type.
      * If you specify a setPointSlot, remember that its data type must be the same as
      * the data type of the method.
      *
      */
    virtual void attach(QObject *object, const char *method,
                        const char* setPointSlot = NULL,
                        Qt::ConnectionType connType = Qt::AutoConnection);

    void attach(short *s);

    void attach(double *d);

    void attach(int *i);

    void attach(unsigned int *u);

    void attach(QString *qs);

    void attach(bool *b);

    void attach( QStringList *sl);

    void attach( QVector<double> * v);

    void attach( QVector<int> *vi);

    void attach( QVector<bool> *vb);

    Type type() const;

    QString slot() const;

    /** \brief returns the method name, without the full signature.
      *
      * For instance if slot() returns setValue(int), this method will
      * return setValue
      */
    QString methodName() const;

    QString setPointSlot() const;

    QString setPointMethodName() const;

    void *data();

    Qt::ConnectionType connectionType() const;

    /** \brief when minimum and maximum values are available, these slots are invoked.
      *
      * These slots are invoked when the auto configuration is available.
      * The QTWatcher api provides shortcuts to set minimum, maximum, min err, min warn, units.
      * The supported data types for slots to be invoked are the ones enumerated in AutoConfType.
      * Slots must take exactly <strong>one argument</strong>, and must be of type<br/>
      * <strong>int, double, short, unsigned int, QString</strong>.<br/>
      * Nothing else is supported.
      *
      * @see AutoConfType
    */
    void setAutoConfSlot(AutoConfType act, const char* slot);

    /** \brief writes into the in_type parameter the type of the input argument of
      * the given method of the object obj.
      *
      * @param the method name, which has to have only one argument, for instance
      *        setMinimum(int), setValue(double), setText(QString)
      *
      * @param obj the QObject on which to look for the type of input argument.
      *
      * @param in_type a char pointing to previously allocated memory.
      *
      * @return true the method succeeded
      * @return false the method failed (method signature not found or input argument
      *         count greater than one.
      */
    bool inTypeOfMethod(const QString &method, QObject *obj, char* in_type);

    /** \brief extracts the signal/slot code at the beginning of the method name
     *
     *
     * methods are specified via SIGNAL and SLOT macros, so we have to remove
     * signals and slots codes from the method. See qt/src/corelib/kernel/qobject.h
     *  and qt/src/corelib/kernel/qobject.cpp.
     * Returns the extracted code and removes the code from the method parameter if
     * the code is QSIGNAL_CODE or QSLOT_CODE.
     */
    int extractCode(QString& method);

    /** \brief removes filter from the object.
      *
      * This method does not delete filter.
      */
    void removeRefreshFilter(TValueFilter *filter);

    /** \brief install the QTRefreshFilter filter into the QTangoizer.
      *
      * Whenever data is updated, your implementation of the filter methods
      * is invoked so that the read data can be modified before being displayed
      * on a widget or stored into a variable.
      * For each kind of data there is a method.
      *
      * @see QTRefreshFilter
      */
    void installRefreshFilter(TValueFilter *filter);

    /** \brief returns the currently installed refresh filter.
      *
      * @return the currently installed refresh filter.
      */
    TValueFilter *refreshFilter() const;

    /** \brief Call this one before setSource to ensure that only one read is performed.
      *
      * @param singleShot true a single read is performed, then unsetSource is automatically
      *        called and the reader disconnected.
      * @param singleShot false the object behaves as usual, with the specified RefreshMode.
      *
      * Call this method before setSource.
      * <strong>Warning</strong>: reimplementing QTWatcher::refresh() method bypasses
      * the singleShot property check and so setting singleShot to true will not
      * work as expected.
      */
    void setSingleShot(bool singleShot);

    /** \brief set single shot and then destroy the object right after the execution
      *
      * <h3>Important note</h3> <p>The object <strong>is destroyed even if the execution
      * is not successfully accomplished</strong>. This goes both for readers and for writers.
      * </p>
      * <h3> Note</h3> <p> Auto destruction for writers is possible only if the
      * QTangoComProxyWriter::ExecutionMode is set to Synchronous (the default)
      * </p>
      */
    void setAutoDestroy(bool autoDestroy);

    /** \brief returns true if the object auto destroys itself after first execution.
      *
      * @see setAutoDestroy
      */
    bool autoDestroy() const;

    /** \brief returns the singleShot property.
      *
      * @return true only one read is performed (or was performed)
      * @return false the reader behaves as readers usually behave
      */
    bool singleShot() const;

    void setToolTipsDisabled(bool disable);

    bool toolTipsDisabled() const;

signals:

    /** \brief Connection ok notification.
  *
  * Mapped from QTangoCommunicationHandle::connectionOk
  *
  * @see QTangoCommunicationHandle::connectionOk
  */
    void connectionOk(bool);

 /** \brief auto configuration signal.
  *
  * Mapped from QTangoCommunicationHandle::attributeAutoConfigured.
  *
  * @see QTangoCommunicationHandle::attributeAutoConfigured
  *
  * QTangoizer class can perform some simple auto configuration (see setAutoConfSlot method)
  * when the associated widgets have suitable slots to set minimum, maximum, text values.
  * In every case, this signal is <strong>emitted before any auto configuration is done</strong>
  * by QTangoizer derived classes.
  * This allows you to set minimum and maximum values (for example) without worrying to initialize
  * the first value read by yourself. Actually, if the setPointSlot parameter of the
  * void attach(QObject *refreshee, const char *slot, const char* setPointSlot = NULL, Qt::ConnectionType connType = Qt::AutoConnection);
  * method is not NULL, the first value read is set by means of that slot.
  */
  void attributeAutoConfigured(const TangoConfigurationParameters *);

    /** \brief this signal is emitted after the source or target configuration and reveals a connection failure
 *
 * After the handle configures via setSource(), emits a signal to reveal a connection failure
 * Mapped from QTangoCommunicationHandle::connectionFailed()
 */
    void connectionFailed();

    /** \brief a signal that contains the error message related to setSource - or setTargets - failures.
 *
 * Connect this signal to any slot accepting a QString as input parameter if you want to know the error message
 * related to the setSource() or setTargets() failure.
 * Mapped from QTangoCommunicationHandle::connectionErrorMessage()
 * @param msg the error message
 */
    void connectionErrorMessage(const QString& msg);

    /** \brief Emitted by the refresh method. The message is taken from the TVariant
  *
  * @param message the error message, i.e. the tango exception, or the
  *        read information (last read timestamp) if read was successful.
  */
    void refreshMessage(const QString& message);

    /** \brief this signal is emitted by the refresh() method and the success
  * parameter indicates whether the tango read was successful or not.
  *
  * @param success true if read was successful, false otherwise
  *
  * @see readFailed
  */
    void readOk(bool success);

    void newData(int);

    void newData(double);

    void newData(short);

    void newData(const QString&);

    void newData();

    void newData(unsigned int);

    void newData(unsigned short);

    void newData(Tango::DevState);

    void newData(bool);

    void newData(const QVector<double> &);

    void newData(const QVector<int> &);

    void newData(const QVector<bool> &);

    void newData(const QStringList &);

public slots:

protected:
    QTangoizerPrivate * const d_ptr;

protected slots:

    virtual void slotAttributeAutoConfigured(const TangoConfigurationParameters* cp);

    /* although refresh is used in readers (QTWatcher), it can be used in auto configuration
     * to invoke the auto configuration method.
     * read means extract the read value from v, not the set point. If you want to update the value
     * with set point value instead of read point, pass false as second parameter.
     */
    virtual void updateValue(const TVariant& v, bool read = true, const char* customMethod = NULL,
                             TValueFilter::State updateState = TValueFilter::Update);

private:
    Q_DECLARE_PRIVATE(QTangoizer)
};


#endif // QTANGOIZER_H
