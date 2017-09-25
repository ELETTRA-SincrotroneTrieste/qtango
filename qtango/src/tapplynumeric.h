#ifndef TAPPLYNUMERIC_H
#define TAPPLYNUMERIC_H

#include <QVariant>
#include <eapplynumeric.h>
#include <com_proxy_writer.h> 
#include <qtango_wcommon.h>



class TangoConfigurationParameters;
class  TApplyNumericPrivate;

/**
 * \brief a widget to set and write a value
  *
   * This widget is used to set a numeric value and apply it when clicking on the <em>Apply</em> button.
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
   *
   * <strong>Note</strong>:
   * <p>
   * In case of auto configuration possible, i.e. minimum and maximum values set on the tango database for the 
   * target attribute, the <em>minimum and maximum values have priority over the number of <strong>integer digits</strong>
   * with respect to the <strong>format</strong></em>. On the other hand, <em>the number of decimal digits is
   * derived from the string <em>format</em></em>.
   * </p>
   * </p>
   * @see DecIntFromFormat
   * @see TNumeric
   * @see QTangoAutoConfiguration
   *
   * @see QTangoComProxyWriter
   * @see QTangoComProxyWriter::executionMode()
   *
   * <h3>The <em>newDataAvailable()</em> signal</h3>
   * <p>See the TPushButton documentation for its usage
   * @see TPushButton::newDataAvailable()
   * </p>
   *
 */
class TApplyNumeric : public EApplyNumeric, public QTangoComProxyWriter, public QTangoWidgetCommon
{
Q_OBJECT
Q_PROPERTY (QString targets READ targets WRITE setTargets DESIGNABLE true)
Q_PROPERTY(bool autoConfiguration READ autoConfiguration WRITE setAutoConfiguration)
Q_PROPERTY(bool confirmNeeded READ confirmNeeded WRITE setConfirmNeeded)
Q_PROPERTY(QTangoCommunicationHandle::ExecutionMode executionMode READ executionMode WRITE setExecutionMode)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)

public:
	TApplyNumeric(QWidget * = 0, int = 3, int = 2, Qt::Orientation = Qt::Horizontal);
    ~TApplyNumeric(){}

// 	QVariant getData();
	
    void setConfirmNeeded(bool b){ m_confirmNeeded = b; }
    bool confirmNeeded(){ return m_confirmNeeded; }
	
	void setTarget(QString s);
	
	QString getData();

  signals:
	void applyClicked();
	
	/** \brief this signal is emitted after the execution of the target action.
	 *
	 * @param returnValue the TVariant representing the argout of the command
	 * If the target is a command, then it might have a return value. In that case, 
	 * its argout is available through the parameter returnValue.
	 * The returnValue contains also the name of the tango point, through 
	 * returnValue.tangoPoint(). 
	 * 
	 */
	void newDataAvailable(const TVariant &returnValue);
	
protected:
	bool m_confirmNeeded;

protected slots:
	void confirmRequest(double);
	void configure(const TangoConfigurationParameters * );
	void stepsDone(int, int);

private:
    TApplyNumericPrivate *d_ptr;
};









#endif
