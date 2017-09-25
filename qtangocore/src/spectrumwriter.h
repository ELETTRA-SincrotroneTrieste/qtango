#ifndef SPECTRUM_WRITER
#define SPECTRUM_WRITER

#include <qtangocore.h>
#include <com_proxy_writer.h>
#include <simple_dataproxy.h>
#include "spectrumwriter.h"

class SpectrumWriter;
class ConfigHelper;
class SpectrumWriterPrivate;

/** \brief Given a tango <strong>writable spectrum</strong> attribute of N elements and M SimpleDataProxies, with M <= N this class 
  *            provides an interface to take the values from the M data proxies and write them to the tango attribute.
  *
  * Inherit from this class and call addElement() or setElements() providing as input arguments QTangoCore SimpleDataProxies.
  * SimpleDataProxies are, for example, TDoubleSpinBox, TSpinBox, TLineEdit... or any widget or object of your choice whic
  * <strong>is</strong> a SimpleDataProxy.
  *
  * @see SimpleDataProxy
   * @see TDoubleSpinBox
   * @see TSpinBox
   * @see TLineEdit
   * @see QTangoComProxyWriter
   * @see QTangoComProxyWriter::setTargets()
    * \par Example
    * Let's create a QPushButton that, when clicked, writes a spectrum attribute
    * <strong>Class definition:</strong>
    * \code
    *
     #ifndef GROUP_WRITER_BUTTON
#define GROUP_WRITER_BUTTON

#include <qtangocore.h>
#include <spectrumwriter.h>
#include <QPushButton>

class GroupWriterButton : public QPushButton, public SpectrumWriter
{
  Q_OBJECT
  public:
    GroupWriterButton(QWidget *parent);
    
  public slots:
    void writeValues(); // when clicked, this SLOT 'll be invoked

};

#endif
     *\endcode
     
     <strong>Implementation </strong>
     \code
     #include "groupWriterButton.h"

GroupWriterButton::GroupWriterButton(QWidget *parent) : QPushButton(parent), SpectrumWriter(this)
{
  // simply connect the clicked() signal to the writeValues SLOT
  connect(this, SIGNAL(clicked()), this, SLOT(writeValues()));
}

void GroupWriterButton::writeValues()
{
  // simply execute the execute() method of the SpectrumWriter class
  execute();
}

     \endcode
     
     <strong>Now use it!</strong>
     <p> In our application, let's see how to add SimpleDataProxies to the SpectrumWriter.</p>
     \code
     GroupWriterButton *wButton = new GroupWriterButton(this);
     wButton->setObjectName("Spectrum writer button");
     TDoubleSpinBox *tds1 = new TDoubleSpinBox(this);
     TDoubleSpinBox *tds2 = new TDoubleSpinBox(this);
     TDoubleSpinBox *tds3 = new TDoubleSpinBox(this);
     TDoubleSpinBox *tds4 = new TDoubleSpinBox(this);
     // for maximum and minimum values of the spinBox, if the attribute is configured
     tds1->setTargets("test/device/1/double_scalar");  // do the same if you want with tds2..tds4
     QList<SimpleDataProxy *>spinboxes;
     spinboxes << tds1 << tds2 << tds3 << tds4; // TDoubleSpinBox is a SimpleDataProxy
     wButton->setElements(spinboxes);
     wButton->setTargets("test/device/1/double_spectrum");
     \endcode
     
     <p> That's all. Now when you click on the wButton, the values set on the four spin boxes are written, in order,
      into the tango spectrum attribute set as target.
      </p>
      *
     * <h3>Writers auto configuration</h3> <p>
     * SpectrumWriter configures the writers with their initial values and with the format, if the spectrum attribute 
     * <em>is configured with the format</em>.
     * The writers, to have their initialization values automatically set, must inherit from one of the following 
     * interfaces:
      * <ul><li>IntDelegate, like <em>TSpinBox</em> or <em>TNumeric</em>;</li>
      * <li>FloatDelegate, like <em>TDoubleSpinBox</em> or <em>TNumeric</em>;</li>
      * <li>StringDelegate, like <em>TLineEdit</em>.</li>
      * <ul>
      * @see IntDelegate, FloatDelegate, StringDelegate, TSpinBox, TDoubleSpinBox, TLineEdit, TNumeric
      * </p>
      */
class SpectrumWriter :  public QTangoComProxyWriter
{
  public:
    /** \brief The constructor, remember to call it with the correct parent
     */
    SpectrumWriter(QObject *parent);
    
    ~SpectrumWriter();
    
    /** \brief Executes the action on the tango target.
     *
      * Taken the data from the SimpleDataProxy elements registered with addElement() or 
      * setElements(), it executes the action on the targets specified.
      * Data are taken from the SimpleDataProxies added with addElement() or set with
      * setElements, and are written on the tango spectrum attribute element per element, in order.
      *
      * @see QTangoComProxyWriter::setTargets()
      * @see TVariant
      */
    QList<TVariant> execute(QVariant v = QVariant());
    
    /** \brief adds a SimpleDataProxy object to the list of objects to be taken into account when writing 
      *             on the tango target.
     *
     * The first element added will provide the data for the <strong>first element</strong> of the tango spectrum attribute,
     * the second will provide the data for the <strong>second element</strong> of the tango spectrum attribute, and so on...
     * So remember to call addElement in the desired order.
     */
    virtual void addElement(QObject *proxy);
	
	/** \brief Removes the SimpleDataProxy proxy from the list of the proxies.
	 *
	 * Remember to call this when you delete an object which was added with addElement or when 
	 * you simply want to remove the proxy from the list.
	 */
	void removeElement(QObject *proxy);
    
    /** \brief Sets the DataProxy elements at once, from the passed list.
     *
     * @param list the list of SimpleDataProxy to set for the target execution. Remember to provide an ordered list so that
     *                 the value taken from the <strong>first element </strong>will be written to the 
     *                <strong>first element of the spectrum </strong> attribute,
     *                the value taken from the <strong>second element</strong> will be written to the <strong>second 
     *                element of the spectrum </strong> attribute, and so on, in order.
	 * Reimplemented in TSpectrumButton to make the SpectrumWriter safe when an element of the list is deleted.
     */
    virtual void setElements(const QList <QObject *>& list);
    
    /** \brief sets the tango targets
     *
     * Call this method <strong>after</strong> calling all addElement() methods or <strong>after</strong>setElements()
     */
    void setTargets(QString target);
	
	/** clears the configured targets, clearing the writers list. */
	void clearTargets();
    
  private:
    QList<QObject *> d_proxyList;
    ConfigHelper *d_cfgHelper;

    SpectrumWriterPrivate *d_ptr;
};


#endif
