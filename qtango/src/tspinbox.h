#ifndef TSPINBOX_H
#define TSPINBOX_H

#include <espinboxes.h>
#include <com_proxy_writer.h>
#include <simple_dataproxy.h>
#include <int_delegate.h>
#include <qtango_wcommon.h>

class TSpinBoxPrivate;

/** \brief A spin box derived from QSpinBox.
 *
 * This widget can be used by a writer. The writer can read the current text
 * displayed in the line edit and use it to write to tango.
 * For instance, given a TPushButton *pb and a TSpinBox *cb:
 *
 * TPushButton *pb = new TPushButton(this);
 * pb->setText("Click to apply the value of the combo box");
 * TSpinBox *cb = new TSpinBox(this);
 * cb->setObjectName("specialcombo"); // important! Always provide the object name!
 * // connect the tango target providing the attribute (or command) and the object 
 * name from which to get the value (in this case the TSpinBox name)
 *
 * pb->setTarget("test/device/1/string_scalar(&specialcombo)");
 *
 * that's all!
 *
 * NOTE: The QTangoProxyWriter, although unused to effectively write the number
 * (although a valueChanged(int) signal might in the future be used to 
 * directly write when the user changes the spin box value), creates a 
 * connection to the device and provides the auto configuration functionality.
 */

class TSpinBox : public ESpinBox, public QTangoComProxyWriter, public SimpleDataProxy, public IntDelegate,
  public QTangoWidgetCommon
{
Q_PROPERTY (QString targets READ targets WRITE setTargets DESIGNABLE true)

Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
  
  Q_OBJECT
  public:
    TSpinBox(QWidget * = 0);
    virtual ~TSpinBox() {};
    /* implement from IntDelegate */
    void setMinimum(int m) { QSpinBox::setMinimum(m); }
    void setMaximum(int m) { QSpinBox::setMaximum(m); }
    void setValue(int  v) { QSpinBox::setValue(v); }
    
  public slots:
    /** must implement as slot the SimpleDataProxy pure virtual method QString getData() 
    */
    QString getData();

    
  public slots:
    void configure(const TangoConfigurationParameters * );
	
	/** \brief provided for convenience, calls the writer execute() method passing data as argument.
	 *
	 * This slot can be useful to automatically execute the target with the data contained in the
	 * argument.
	 * A typical usage might be to connect the valueChanged(double ) QDoubleSpinBox
	 * signal to this slot, to execute the target when the index of the spin box changes, without needing
	 * an additional <em>Apply</em> button.
	 * This feature is available also in TSpinBox and TComboBox
	 */
	void execute(int value);

private:
    TSpinBoxPrivate *d_ptr;
};

#endif

