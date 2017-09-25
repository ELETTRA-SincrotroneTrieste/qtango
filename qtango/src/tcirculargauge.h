#ifndef  T_CIRCULAR_GAUGE_H
#define T_CIRCULAR_GAUGE_H

#include <egauge.h>
#include <qtangocore.h>
#include <egauge.h>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>

class TCircularGaugePrivate;

/**
 * \brief a thermo-like viewer
 
 * <h3>Note</h3><p>Hide and show events optimization is enabled by default. See QTancoCore QTangoWidgetCommon documentation.</p>
 * <p>
 * Since qtango version <em>3,1</em> the gauges support engineering notation through the format string.
 * The format string is a <em>designable</em> property named <strong>valueFormat</strong> and implemented
 * in EAbstractGauge (qtcontrols).
 * TLinearGauge and TCircularGauge <em>do not auto configure</em> the valueFormat property of the widget.
 * This is done for two reasons: if the data type is integer and the format specified into the tango database
 * is <em>%d</em>, then such format might not be compatible with the gauges scale division into ticks.
 * It would work fine only if all the widgets representing the tango entity were gauges. Actually, you might want
 * %d format in labels for instance, and %.1f in gauges, for the same integer attribute, to have correctly displayed
 * each value in the scale.
 * Secondly, as to floating point attributes, for the same attribute you might want to display its value with five 
 * significant digits on a label, and only two in the gauges.
 * Thus, auto configuration does not automatically set the valueFormat property: adjust its value from the designer
 * or by calling setValueFormat(), according to the type of attribute and the number of ticks of the graduated scale.
 * </p>
 *
 * @see TLinearGauge
 */
class TCircularGauge : public ECircularGauge, public QTangoComProxyReader, public QTangoWidgetCommon
{
Q_OBJECT

Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
Q_PROPERTY(int period READ period WRITE setPeriod)
Q_PROPERTY(bool autoConfiguration READ autoConfiguration WRITE setAutoConfiguration)
Q_PROPERTY(bool viewTrendEnabled READ viewTrendEnabled WRITE setViewTrendEnabled)
Q_PROPERTY(bool trendColouredBackground READ trendColouredBackground WRITE setTrendColouredBackground)
Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
Q_PROPERTY(QString helperApplication READ helperApplication WRITE setHelperApplication)
Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)

Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)

public:
	TCircularGauge(QWidget *, bool=true);
	~TCircularGauge();
	
public slots:
	/** \brief change the period of the readings. This is a slot.
	 *
	 * Simply changes the reading period, but since version 4.x this method is exported as a slot.
	 */
	void setPeriod(int p) { QTangoComProxyReader::setPeriod(p); }
	
protected slots:
	void init(const TangoConfigurationParameters *cp);
	void refresh(const TVariant& v);

private:
    TCircularGaugePrivate *d_ptr;

};

#endif
