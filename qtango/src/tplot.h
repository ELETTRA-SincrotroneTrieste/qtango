#ifndef TPLOT_H
#define TPLOT_H

#include <eplot.h>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>

#define DEPRECATED(func) func __attribute__ ((deprecated))

/** \brief a plot with zooming capabilities. 
 *
 * This plot is a reader derived from qtcontrols EPlot. Please see qtcontrols' EPlot documentation.
 * This class is <strong>deprecated and no more maintained</strong>. Use TPlotLightMarker instead.
 *
 * <h3>Important note</h3>
 * <p>TPlot disables show/hide events on the reader by default. Please read QTangoCore documentation,
 * QTangoComProxyReader::setHideEventEnabled().</p>
 *
 */
class TPlot : public EPlot, public QTangoComProxyReader, public QTangoWidgetCommon
{
  Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
  Q_PROPERTY(int period READ period WRITE setPeriod)
  Q_PROPERTY(bool autoConfiguring READ getAutoConfiguration WRITE setAutoConfiguration)
  Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)
  Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
  Q_PROPERTY(QString helperApplication READ helperApplication WRITE setHelperApplication)
  Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
  Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
  Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
  Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
  
	Q_OBJECT
	public:
                DEPRECATED(TPlot(QWidget * =0, bool auto_conf= true));
		
	protected slots:
	  void refresh(const TVariant &);
	  void autoConfigure(const TangoConfigurationParameters *);
		
	private:
};

#endif

