#ifndef TNUMERIC_H
#define TNUMERIC_H

#include <enumeric.h>
#include <com_proxy_writer.h>
#include <simple_dataproxy.h>
#include <qtango_wcommon.h>

class TNumericPrivate;

/**
 * \brief a widget to set a scalar number 
 * 
 * a widget derived from ENumeric and  QTangoComProxyWriter that allows to set a scalar
 * data, which can be a:
 * DEV_FLOAT, DEV_DOUBLE, DEV_LONG, DEV_ULONG, DEV_SHORT, DEV_USHORT
 * It does really nothing if used alone, but uses QTangoComProxyWriter just to auto
 * configure itself, with the maximum and minumum values.
 * There is no way to write_attribute() with this object really.
 *
  * <h3>Widget auto configuration</h3>
  * <p>
   * The widget <em>auto configures itself </em> with <em>minimum and maximum</em> values, and
   * with the <em>format</em> specified in the tango database (jive settable).
   * The value displayed is initialized according to  the <em>attribute <strong>set value</strong></em>.
   * <p>
   * <strong>Note</strong>:<br/>
   * For the widget <em>auto configuration</em>, <strong>it is compulsory</strong> that the tango attribute
   * <em>is configured with its <strong>minimum and maximum values</strong></em>. If that is not the case,
   * then only the attribute set value is displayed, but <em>no minimum and maximum values, nor the format</em>
   * will be setup on the widget.
   * </p>
   * <p>
   *	In case of auto configuration possible, i.e. minimum and maximum values set on the tango database for the 
   * target attribute, the <em>minimum and maximum values have priority over the number of <strong>integer digits</strong>
   * with respect to the <strong>format</strong></em>. On the other hand, <em>the number of decimal digits is
   * derived from the string <em>format</em></em>.
   * </p>
   * </p>
   * @see DecIntFromFormat
   * @see TApplyNumeric
   * 
   * <strong>Note</strong>:<br/>
   * <p>TNumeric is not a writer really: you need to use an additional TPushButton and set its target in the form of the example:
   *      <em>test/device/1/double_scalar(&applyNumericObjectName)</em>
   * <br/> TNumeric actually is a SimpleDataProxy, and is a QTangoComProxyWriter just to benefit from the auto configuration
   *  of the maximum and minimum values, and of the format, of course.
   * </p>
   * @see SimpleDataProxy
   * @see QTangoComProxyWriter
   * @see QTangoAutoConfiguration
 */
class TNumeric : public ENumeric, public QTangoComProxyWriter, public SimpleDataProxy, public QTangoWidgetCommon
{
	Q_OBJECT
	
Q_PROPERTY (QString targets READ targets WRITE setTargets DESIGNABLE true)
Q_PROPERTY(bool autoConfiguration READ autoConfiguration WRITE setAutoConfiguration)

Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
Q_PROPERTY(bool applyOnValueChanged READ applyOnValueChanged WRITE setApplyOnValueChanged DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
	
	public:
		TNumeric(QWidget *parent = 0, int intDigits = 3, int decDigits = 2);;
		~TNumeric(){};	
	
		/** \brief enables/disables automatic write on value changed.
		 *
		 */
		void setApplyOnValueChanged(bool en) { d_applyOnValueChanged = en; }
	
	    /** \brief returns the value of the property applyOnValueChanged
		 *
		 * @return true when value changes, the action is immediately performed
		 * @return false when value changes, nothing happens (the default behaviour) 
		 *
		 * <p><em>Note</em>: introduced in QTango version 4.x.</p>
		 */
		bool applyOnValueChanged() { return d_applyOnValueChanged; }

	public slots:
		void configure(const TangoConfigurationParameters * );	
		/** implements the pure virtual QString getData of the SimpleDataProxy */
		QString getData();
		
  private slots:
	
	void slotValueChanged(double);
	
  private:
	bool d_applyOnValueChanged;

    TNumericPrivate *d_ptr;
		
};

#endif
