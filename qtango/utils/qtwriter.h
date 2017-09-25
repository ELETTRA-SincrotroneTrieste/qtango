#ifndef QTWRITER_H
#define QTWRITER_H

#include <com_proxy_writer.h>
#include "qtangoizer.h"

/** \brief A class that can be used to write tango values from (almost) any kind of widget
  *        or object.
  *
  * This class can be associated to a Qt widget that is capable of receiving input from the
  * user and represent it into a supported data type.
  * The usage is quite simple and is described in this example:
  * \par Example
  * \code
    // use the dial to change  short scalar
    QTWriter *dualWriter = new QTWriter(this);
    //  adding SLOT(setValue(int)) would not be useful because the QSlider has not
    //  got the slots to set minimum and maximum values. So
     //
    dualWriter->attach(ui.writeDial, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));

    //  connect auto configuration signal to dialConfigured slot to set max and min
    //  on the dial.
    //  Current initialization values are then set by the auto configuration performed by
    //  QTWriter.
    //
    connect(dualWriter, SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters*)),
            this, SLOT(dialConfigured(const TangoConfigurationParameters*)));
    dualWriter->setTargets(labelWatcher->source());
  *
  * \endcode
  *
  * \chapter Example
  * See the code inside qtango/test/watcher example
  * \endchapter
  *
  * \note
  *
  * Suppose you want to create on the fly
  * a writer to immediately trigger a Reset command on the device a/b/c.
  * Suppose you are inside a reset() slot invoked by the clicked() signal
  * emitted by a <cite>Reset</cite> button.
  * The piece of code that follows does not work.
  *
  * \code
  * MyPanel::MyPanel(QWidget *parent) : QWidget(parent)
  * {
  * // ...
  *  connect(pushButtonReset, SIGNAL(clicked()), this, SLOT(slotResetClicked()));
  * //
  * // ...
  * }
  *
  * void MyPanel::slotResetClicked()
  * {
  *     // do additional stuff before resetting
  *     QTWriter *resetWriter = new QTWriter(this);
  *     resetWriter->setTargets("a/b/c->Reset");
  *     resetWriter->execute();
  * }
  * \endcode
  *
  * The piece of code above will not work because <cite>execute()</cite> is invoked
  * immediately, before that the configuration of the target takes place in the Device
  * thread.
  * The correct way to the implementation above is the following.
  *
  * \code
  *
  * MyPanel::MyPanel(QWidget *parent) : QWidget(parent)
  * {
  * // ...
  *  QTWriter *resetWriter = new QTWriter(this);
  *  resetWriter->setObjectName("resetter");
  *  resetWriter->setTargets("a/b/c->Reset");
  *  connect(pushButtonReset, SIGNAL(clicked()), this, SLOT(slotResetClicked()));
  * //
  * // ...
  * }
  *
  * void MyPanel::slotResetClicked()
  * {
  *  // do additional stuff before resetting
  *     QTWriter *resetWriter = findChild<QTWriter *>("resetter");
  *     resetWriter->execute();
  * }
  * \endcode
  *
  * If no additional work has to be performed before calling execute, it is more
  * straightforward to connect the clicked() signal to the resetWriter's execute()
  * slot.
  *
  * <h3>News in release 4.3.0</h3>
  * <p> Since QTango release 4.3.0, it is possible to call execute() right after set targets.
  * On target configuration, execute() is called for you with your desired argument.
  * <br/>Moreover, it is possible to setAutoDestroy to true to have your writer automatically
  * destroyed after first execution. <strong>Beware that the object will be destroyed even if
  * the execution fails</strong> and that the auto destruction is possible only for
  * QTangoComProxyWriter::executionMode() set to Synchronous.
  * </p>
  */
class QTWriter : public QTangoizer, public QTangoComProxyWriter
{
    Q_OBJECT

public:
    /** \brief the class constructor
      *
      * @param parent the QObject which will be the parent for the QTWriter.
      */
    QTWriter(QObject *parent);

    virtual ~QTWriter();

    /** \brief Signal signal of the executor is connected to the execute method of this class.
      *
      * The available slots are listed below.
      * @param executor the QObject to which we connect to receive an execution signal
      * @param signal the signal name, declared with the SIGNAL macro which we are listening for
      * @param setPointSlot provide a custom slot to initialize the set point value inside the
      *        auto configuration method.
      * @param connType the Qt connection type.
      *
      * \par Example
      * \code
      * QTWriter *dualWriter = new QTWriter(this);
      * dualWriter->attach(ui.writeDial, SIGNAL(sliderMoved(int)), SLOT(setValue(int)));
      * dualWriter->setTargets("test/device/1/double_scalar");
      *
      * \endcode
      *
      * Every time the slider is moved, it emits sliderMoved(int) signal. The QTWriter <strong>
      * internally connects</strong> sliderMoved(int) signal to the <strong> execute(int)</strong>
      * slot, recognizing the signal input parameter type.
      * In the example above, when auto configuration  of "test/device/1/double_scalar" is available,
      * the current set point of double_scalar is set on the widget via the setValue(int) SLOT.
     */
    virtual void attach(QObject *executor, const char *signal, const char *setPointSlot = NULL,
                        Qt::ConnectionType connType = Qt::AutoConnection);

    /** \brief executes some code after QTangoComProxyWriter::execute is called.
      *
      * Normally it tests whether the user wants the object to be destroyed after execution
      */
    void postExecute();

public slots:

    /** \brief Reimplemented from QTangoizer::slotAttributeAutoConfigured.
      *
      * If execute() was called before the action configuration, then the target
      * is executed here.
      */
    void slotAttributeAutoConfigured(const TangoConfigurationParameters*);

    /** \brief the execute method tailored for integer values
      *
      * The execute method can be reimplemented in subclasses to specialize its behaviour.
      * By default, the signal provided inside attach method is connected to this slot, when
      * the signal has an integer input type.
      * For instance, when the value of a QSpinBox changes, this slot can be invoked.
      */
    virtual void execute(int);

    /** \bref the execute method tailored for short integer values
      *
      * @see void execute(int)
      */
    virtual void execute(short);

    /** \bref the execute method tailored for string values
      *
      * @see void execute(int)
      */
    virtual void execute(const QString&);

    /** \bref the execute method tailored for double values
      *
      * @see void execute(int)
      */
    virtual void execute(double);

    /** \bref the execute method tailored for unsigned integer values
      *
      * @see void execute(int)
      */
    virtual void execute(unsigned int);

    /** \bref the execute method tailored for a void input value
      *
      * @see void execute(int)
      */
    virtual void execute();

    /** \bref the execute method tailored for boolean values
      *
      * @see void execute(int)
      */
    virtual void execute(bool);

    /** \brief The execute method tailored for a list of strings
      *
      *
      */
    virtual void execute(const QStringList&);

    /** \brief The execute method tailored for a vector of integers
      *
      */
    virtual void execute(const QVector<int>&);

    /** \brief The execute method tailored for a vector of double
      *
      *
      */
    virtual void execute(const QVector<double>&);

    /** \brief The execute method tailored for a vector of booleans
      *
      *
      */
    virtual void execute(const QVector<bool>&);

    /* tests the success of the execution */
    virtual void refresh(const TVariant& v);


};

#endif // QTWRITER_H
