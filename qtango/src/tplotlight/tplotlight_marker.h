#ifndef TPLOT_LIGHT_MARKER_H
#define TPLOT_LIGHT_MARKER_H

#include "plot_com_proxy_reader.h"
#include "tplot_refresh_connector.h"
#include "background.h"

#include <eplotlight_marker.h>

class EPlotCurve;
class TPlotLightMarkerPrivate;

/** \brief a plot with zooming capabilities supporting multiple sources, scalar or spectrum attributes or commands.
 *
 * This plot is a reader derived from qtcontrols EPlotLightMarker. Please see qtcontrols' EPlotLightMarker documentation.
 *
 * <h3>Important note</h3>
 * <p>TPlotLightMarker disables show/hide events on the reader by default. This avoids that hiding a plot 
 * loses data on the plot itself.</p>
 *
 * <h3>Features</h3>
 * <p>
 * The TPlotLightMarker is a plot that inherits from EPlotLightMarker and the TPlotRefreshConnector,
 * which is a class collecting a variable number of readers. Actually, this QTango plot supports multiple
 * sources, provided they are <em>consistent with each other</em>. This means: all scalar or spectrum attributes,
 * but not mixed together.
 * If the source is a scalar attribute, each new value obtained from QTangoCore is added to the y axis, with the
 * x corresponding to the <strong>tango timestamp</strong> stored in the tango device attribute (or command).
 * If the source is a spectrum, it is directly mapped with the values of the spectrum on the y axis and the x
 * axis going from 0 to N, where N is the number of elements of the spectrum.
 * If the source is an attribute and the attribute has been configured with maximum and minimum values, then
 * it is possible to extend the y upper and lower bound according to the greatest maximum and lowest minimum
 * values available from the attributes auto configuration.
 * It is possible to right click on the plot to change scales and curve aspects (see EPlotLight class).
 * Drop of a qtango source on the plot offers the possibility to add or replace the old source with the new one.
 * If enabled, it is possible to fill from the <em>device attribute history</em> the first N points of the plot, where N
 * is the depth of the device attribute history polling buffer. For this to work, source must be a <em>scalar
 * attribute</em>.
 * </p>
 * <p>
 * Clicking on the plot with the left button makes a marker appear on the plot canvas, showing the x and y values
 * of the closest point of the closest curve. In addition, the title of the curve is provided on the marker.
 * The behaviour of the marker can be changed by reimplementing EPlotLightMarker::markerText()
 * @see EPlotLightMarker()
 * @see EPlotLight()
 * @see EPlotLight()
 * </p>
 * <h3><em>One shot</em> refresh of the plot</h3> 
 * <p>
 * It is possible to connect a TPlotLightMarker to a TPushButton linked to a tango target to update the plot
 * after the TPushButton action is executed and the return value is available (this applies to commands with
 * an argout).
 * @see refresh()
 *
 * </p>
 * <h3> X axis scale</h3>
 * <p>
 * If the tango source is a scalar and is updated via the updateCurve() method, the time scale is automatically
 * enabled, since the mentioned method fills the x axis values with the tango timestamps.
 * By default, the property timeScaleDrawEnabled is disabled, thus representing spectrum attributes simply placing
 * values from 0 to N on the x axis, where N is the size of the tango spectrum attribute.
 * If you want a different behaviour, you must reimplement updateCurve() and call setTimeScaleDrawEnabled according
 * to your needs.
 * </p>
 *
 */
class TPlotLightMarker : public EPlotLightMarker, public TPlotRefreshConnector
{
Q_PROPERTY(QString source READ sources WRITE setSources DESIGNABLE true)
// Q_PROPERTY(QStringList sourcesList READ sourcesList WRITE setSourcesList DESIGNABLE true)
Q_PROPERTY(int period READ period WRITE setPeriod DESIGNABLE true)
Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
Q_PROPERTY(QString helperApplication READ helperApplication WRITE setHelperApplication)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)
 
Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)

/* TPlotLightMarker specific properties */
Q_PROPERTY(bool fillFromHistoryEnabled READ fillFromHistoryEnabled WRITE setFillFromHistoryEnabled DESIGNABLE true)
Q_PROPERTY(bool boundsFromAutoConfiguration READ boundsFromAutoConfiguration WRITE setBoundsFromAutoConfiguration DESIGNABLE true)
Q_PROPERTY(bool colouredBackground READ colouredBackground WRITE setColouredBackground DESIGNABLE true)
Q_PROPERTY(bool syncRefresh READ syncRefresh WRITE setSyncRefresh DESIGNABLE true)
Q_PROPERTY(bool setPointEnabled READ setPointEnabled WRITE setSetPointEnabled DESIGNABLE true)

/* this property, read only, is used by eplot configuration widget. EPlotConfigurationWidget is part
 * of qtcontrols. That widget does not know anything about qtango TPlotLightMarker, but simply uses
 * qt meta properties to retrieve its configuration and to change the properties.
 */
Q_PROPERTY(bool autoConfigured READ autoConfigured DESIGNABLE false)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)

  
	Q_OBJECT
	public:
	  TPlotLightMarker(QWidget *parent);
	
	  bool autoConfigured() { return d_autoConfiguredUpperBound != d_autoConfiguredLowerBound; }
			
	  double yUpperBoundFromAutoConf() { return d_autoConfiguredUpperBound; }
	  double yLowerBoundFromAutoConf()  { return d_autoConfiguredLowerBound; }

	  /** \brief enables coloured background, indicating warning and alarm bands with colours
	   *
	   * 
	   */
	  void setColouredBackground(bool en) { bg->setColoured(en); }
	  
	  /** \brief returns the value of the colouredBackground property
	   *
	   * @return true if the background draws coloured bands corresponding to warning and alarm
	   *         threshold, false otherwise.
	   *
	   * <p><em>Note</em> the sources must be attributes configured with alarm (and/or warning)
	   * thresholds into the tango database.</p>
	   * <h3>Observations</h3>
	   * <p>
	   * <ul>
	   *	<li>If you change the attribute configuration for a source and the events are available through tango,
	   *    the background might not correctly being updated
	   *    after the attribute configuration event;</li>
	   *    <li>multiple sources auto configuring <em>extend</em> the coloured bands so that they comprehend the 
	   *    the <em>union</em> of the alarm and warning intervals of the sources. Modifying the sources <em>live
	   *    </em> might not correctly clear the coloured bands, since there is no history of the subsequent auto
	   *    configurations for each source.</li>
	   *   
	   * </ul></p>
	   */
	  bool colouredBackground() { return bg->coloured(); }

	  /** \brief returns true if the property boundsFromAutoConfiguration is enabled.
	  *
	  * If true, the property indicates to the plot to configure its scales according to the 
	  * qtangocore auto configuration parameters read from the tango database.
	  * In case of multiple sources, the first configuration event sets the scales, while the
	  * following auto configuration events <em>extend the scale bounds</em> to correctly 
	  * visualize the curves of all configured sources.
	  * All the sources must be configured with maximum and minimum values into the tango
	  * database.
	  * <br/>This property applies to the <em>Y</em> axis only, and will change the 
	  * EPlotLightBase <em>yAxisAutoscale</em> property to false.<br/>
	  * <strong>Note</strong>: this property affects the plot only at the attribute configuration
	  * instant, changing the Y scale of the graph if the maximum and minimum values are set on the
	  * tango database. Changing this property <em>after<em> the auto configuration does not have
	  * any effect on the plot Y scale.
	  * @see setBoundsFromAutoConfiguration
	  * @see restoreBoundsToAutoConfigurationValues
	  */
	 bool boundsFromAutoConfiguration() { return d_boundsFromAutoConfiguration; }
	  
  public slots:
	
	  /** \brief the refresh method, which plots data if it is possible to convert value into 
	   *         a vector of double or a single double scalar value.
	   * 
	   * @param value the value to be inserted into the plot. The updated value will be pertinent to
	   *              the curve having the name of the <em>tango point</em> taken from the TVariant
	   *              value.
	   * <p>
	   * The method tries to convert value into a vector of double and calls EPLotLight
	   * setData( const QString& curveName , const QVector<double>&xData, const QVector <double>&yData)
	   * where curveName is equal to the <em>tango point</em> extracted from the TVariant value, 
	   * xData is a vector of x values filled from 0 to
	   * yData.size() and yData is the vector of y values.
	   * If the TVariant value cannot be converted into a vector of double, the method tries to convert it
	   * into a single double scalar, and then calls 
	   * EPlotLight::appendData( const QString&curveName, const double xValue, const double yValue), where
	   * curveName is the name parameter, xValue is the timestamp obtained from the tango timestamp memorized into
	   * the TVariant value and yValue is the value of the y point of the curve with name <em>name</em>.
	   * </p>
	   * <p><em>Notes</em>: this method was introduced in QTango 4.x. It is possible to
	   * connect newDataAvailable(const QString&, const TVariant &) signal coming from some QTango widgets
	   * such as TPushButton or TReaderWriter to this slot in order to produce an update of the data on the
	   * plot on demand. For instance, it is possible to create a TPushButton which performs a <em>getData</em>
	   * command, returning an argout in the form of a scalar or spectrum value and updates the plot.
	   * \par Example
	   * \code 
	   * TPushButton *pb = new TPushButton(this);
	   * pb->setTargets("test/device/1->GetData"); // suppose this is a command that returns an argout
	   * TPlotLightMarker *plot = new TPlotLightMarker(this);
	   * // now the magic: on button newDataAvailable signal we update the plot
	   * connect(pb, SIGNAL(newDataAvailable(const QString&, const TVariant &)), plot, SLOT(refresh(const
	   *   QString&, const TVariant &)));
           *
           * To achieve this, updateCurve creates curves on the fly where needed. Their name (title) is created
           * with the same semantics as setSources: equal to the name of the source, without source arguments.
	   * 
	   *\endcode
	   * 
	   * <br/>Observe that newDataAvailable signal contains the source name from which the new data is obtained and
	   * that is not necessary to create a new curve on the plot when using this refresh method. Actually, this method
	   * calls EPlotLight appendData() or setData() methods which, since QTango 4.x, create new curves with the name
	   * provided if they do not exist yet.
	   * 
	   * @see EPlotLight::setData()
	   * @see EPlotLight::appendData()
	   *
	   * This might be useful for commands, when you want a single read and plot update.
	   *
	   * @see TRealTimePlot
	   *
	   * </p>
	   */
	  virtual void updateCurve(const TVariant &value);
	
	  /** \brief extends the lower bound of the Y axis and marks the changes as derived from
	  *          an auto configuration event.
      *
      * Disables Y axis autoscale on the plot.
	  * Call this method instead of EPlotLightBase::extendYLowerBound() when you want to save the y lower bound
	  * of the y scale as an auto configured value.
	  * You can get the saved value with yLowerBoundFromAutoConf() method.
      */
     void extendYLowerBoundFromAutoConf(double l);
     
     /** \brief extends the upper bound of the Y axis marks the changes as derived from
	  *          an auto configuration event.
      *
      * Disables Y axis autoscale on the plot.
	  * Call this method instead of EPlotLightBase::extendYUpperBound() when you want to save the y upper bound
	  * of the y scale as an auto configured value.
	  * You can get the saved value with yUpperBoundFromAutoConf() method.
      */
     void extendYUpperBoundFromAutoConf(double u);
	 
	 /** \brief if the plot Y scale was auto configured, this method restore those values
	  *
	  * If auto configuration was possible and the boundsFromAutoConfiguration property was set to
	  * true at the moment of the plot configuration, then the upper and lower bounds of the Y scale
	  * were saved and can be restored.
	  * This <em>disables auto scale</em> on  the plot.
	  */
	 void restoreBoundsToAutoConfigurationValues();
	 
	 /** \brief enables or disables the boundsFromAutoConfiguration property on the plot.
	  *
	  * Call this before setSources() to modify the behaviour of the plot auto configuration
	  * according to what explained in boundsFromAutoConfiguration().
	  *
	  * @see boundsFromAutoConfiguration
	  * @see restoreBoundsToAutoConfigurationValues
	  */
	 void setBoundsFromAutoConfiguration(bool en) { d_boundsFromAutoConfiguration = en; }
	
	/** \brief substitutes the source returned by pastedSource()
	 */
	virtual void substitutePastedSource();
	
	/** \brief adds the source provided by pastedSource()
	 * 
	 * pastedSource() is implemented in TPlotRefreshConnector  
	 */
	virtual void addPastedSource();
	
	/** \brief pauses the acquisition of the plot.
	 *
	 */
	virtual void pauseAcquisition();
	
	/** \brief restarts the acquisition of the plot.
	 *
	 */
	virtual void restartAcquisition();
	
	void setPeriod(int p) { TPlotRefreshConnector::setPeriod(p); }

        /** \brief this property causes plot refresh each time new data arrives on each curve
          *
          * @return true if refresh is synchronous to a synchronization timer which timeout is set to 0.9 * period()
          * @return false if the plot is refreshed every time a curve is updated with new data.
          *
          * Normally, when new data is read from the tango source point(s), the plot is refreshed.
          * The effect is that each curve is refreshed as soon as new data is available.
          * To speed up the plot, enable this feature, which is disabled by default.
          * This might be very helpful to improve efficiency
          * in plots with many curves refreshing very fast, such as in TRealtimePlot.
          *
          * @see setSyncRefresh
          */
        bool syncRefresh() const;

        /** \brief enables or disabled synchronous refresh of the plot with a trigger.
          *
          * @see syncRefresh
          */
        void setSyncRefresh(bool en);

        void setSetPointEnabled(bool en);

    public:
        /**  \brief finds and returns an EPlotCurve that represents the source specified.
          *
          * @return the EPlotCurve assigned to the source specified with setSource or NULL if no
          *         curve with that name was found.
          *
          * With respect to the already existing EPlotLight::curve(),
          * curveBySource removes trailing arguments of commands, if present, and look for the curve with the name specified.
          * For instance curveBySource("test/device/1->getData(0,1000)") will look for "test/device/1->getData" curve name.
          * Actually, curve titles are now created without source arguments. This means that
          * setSource("test/device/1->getData(0,1000)"); will add a curve with title "test/device/1->getData".
          * In that case, calling
          * \code EPlotLight::curve("test/device/1->getData"); \endcode
          * is equivalent to calling
          * \code curveBySource("test/device/1->getData(0,1000)"); \endcode
          */
        EPlotCurve *curveBySource(const QString& source);

        /** \brief returns the connection status of the plot.
          *
          * @return a string containing the list of the statuses of the single sources.
          */
        QString connectionStatus();

        /** \brief returns the configuration state of the sources.
          *
          * @return true if all the sources are configured, i.e. each
          *         QTangoCommunicationHandle::configured() method returns true.
          */
        bool configured();

        bool setPointEnabled() const;
  
    signals:
        void connectionOk(bool);
        void connectionErrorMessage(const QString&);

  protected:
	/** \brief returns the sources configured.
	 *
	 * Reimplemented from EPlotLightBase::plotTitle()
	 */
	virtual QString plotTitle();
	
		
	virtual void showEvent(QShowEvent *);
	
	virtual void hideEvent(QHideEvent *);
	
   protected slots:
	 
	virtual void autoConfiguration(const TangoConfigurationParameters *);
	
  private:
	double d_autoConfiguredUpperBound, d_autoConfiguredLowerBound;
	bool d_boundsFromAutoConfiguration, d_yLowBoundAdjusted, d_yUpBoundAdjusted;
	Background *bg;

    TPlotLightMarkerPrivate *d_ptr;
};


#endif
