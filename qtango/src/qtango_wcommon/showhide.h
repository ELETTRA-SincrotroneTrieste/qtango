#ifndef SHOW_HIDE_H
#define SHOW_HIDE_H

class QWidget;
class ShowHidePrivate;

class ShowHide
{
  public:
	ShowHide(QWidget *parent);
	
	/** \brief enables or disables hide/show event optimization for widgets
	 *
	 * @param  en if true hide events are enabled: when a widget is hidden, the tango poller is stopped, and when
	 * 	   the widget is shown again, the poller is restarted.<br/>
	 * 	   if en is false, hide and show events are ignored, and the poller is never stopped nor started.
	 *
	 * <strong>Note</strong> this option affects only <em>polled</em> attributes.
	 *
	 * @see hideEventEnabled
	 *
	 * <h3>Observation</h3>
	 * <p>
	 * 	Keep in mind that readers connected to the same source are shared together.
	 *	Imagine you have a <em>QTabWidget</em> with two tabs and in each of them
	 *	a reader (for instance a QTango TLabel) connected to the same source, for instance
	 *	<em>"test/device/1/double_scalar"</em>.<br/>
	 *	When your application starts up, setSource() takes place and the tango source is 
	 * 	connected to the tango device. After a while, your hidden TLabel, which has hide/show events
	 *	<em>enabled</em>, realizes it is hidden and will tell the poller to stop polling the tango
	 *	attribute. The visible TLabel, located in the visible <em>tab</em> will also stop refreshing,
	 *	because it shares the reader with its hidden sister.
	 *	This is a tipical scenario in which you want the TLabels to call setHideEventEnable with 
	 *	<em>false</em> as parameter.
	 * </p>
	 * <h3> Observation II </h3>
	 * <p>
	 * Imagine you have two readers connected to the same source, be it <em>test/device/1/double_scalar</em>.
	 * One reader is a qtango <em>TPlot</em> displaying the value of the source as time passes. 
	 * The other reader is a simple qtango <em>TLabel</em> providing a visual representation of the value plotted in the graph.
	 * By default, <em>TPlot show hide events are disabled</em>, while <em>TLabel hide/show events are enabled</em>.
	 * In this scenario, the TLabel intercepts hide and show events. When you hide your GUI, TLabel's reader <em>would</em>
	 * stop the poller for <em>test/device/1/double_scalar</em>. This might not be what you want, since also the 
	 * plot will stop refreshing. To avoid this behaviour, since QTango <em>version 4.x</em>, <em>if two or more readers  
	 * share the same source, then <em>hide/show events are disabled</em> for that source</em>. Moreover, if you connect
	 * two readers to the same source and then delete or disconnect one of them, show hide events <em>stay disabled</em> 
	 * even if just one reader still remains linked to only one source. See <em>Observation III</em>.
	 * An example will clarify this behaviour.
	 * 
	 * </p>
	 * <h4>An example</h4>
	 * <p>
	 * \code
	 * TLabel *l1 = new TLabel(this);
	 * l1->setSource("a/b/c/attr"); // will receive show/hide events and stop polling when hidden 
	 * TPlotLightMarker *plot = new TPlotLightMarker(this);
	 * // set the same source in the plot!
	 * plot->setSource("a/b/c/attr"); // l1 and plot won't receive hide events any more
	 * // this behaviour prevents the reading to be stopped if one of the two widgets is hidden
	 * // ...
	 * delete plot;
	 * // hide events remain disabled!
	 * 
	 * \endcode
	 * </p>
	 *
	 * <h3>Writing Qt Designer plugins implementing QTangoComProxyReader</h3>
	 * <p>
	 * 	Inside a Qt designer plugin, show/hide events must be disabled. Actually, suppose you are designing
	 *	your GUI and at a certain moment you watch a <em>preview</em> of your widget in the Designer workspace.
	 *	Readers of the designer editor are <em>shared</em> with the readers of the <em>Form Preview</em>.
	 *	When you close the <em>Form Preview</em>, the reader will receive a <em>hide event</em>, being itself
	 * 	a <em>QWidget</em>. Well, being readers with the same source <em>shared</em>, also the readers inside
	 *	the editor widget would stop refreshing if the show/event system wasn't implemented as described in the 
	 * <em>Observation II</em> above (see also <em>Observation III</em>).
	 * </p>
	 * <h3>Observation III</h3>
	 * <p>
	 * The observations made above suggest that qtangocore hideEventEnabled property is a <em>hint</em> that the 
	 * programmer suggests to the library, and the library then decides when this property is suitable or not.
	 * </p>
	 *
	 * @see QTangoComProxyReader::setDesignerMode
	 *
	 * </p>
	 *
	 */
	void setHideEventEnabled(bool en);
	
  /** \brief reveals if hide/show events optimization is enabled or disabled
   *
   * @return true hide events are enabled: when a widget is hidden, the tango poller is stopped, and when
   * 	   the widget is shown again, the poller is restarted.
   * @return false hide and show events are ignored, and the poller is never stopped nor started.
   *
   * <strong>Note</strong> this option affects only <em>polled</em> attributes.
   * @see setHideEventEnabled
   *
   * <h3>Observation</h3>
   * <p>
   * 	Keep in mind that readers connected to the same source are shared together.
   *	Imagine you have a <em>QTabWidget</em> with two tabs and in each of them
   *	a reader (for instance a QTango TLabel) connected to the same source, for instance
   *	<em>"test/device/1/double_scalar"</em>.<br/>
   *	When your application starts up, setSource() takes place and the tango source is 
   * 	connected to the tango device. After a while, your hidden TLabel, which has hide/show events
   *	<em>enabled</em>, realizes it is hidden and will tell the poller to stop polling the tango
   *	attribute. The visible TLabel, located in the visible <em>tab</em> will also stop refreshing,
   *	because it shares the reader with its hidden sister.
   *	This is a tipical scenario in which you want the TLabels to call setHideEventEnable with 
   *	<em>false</em> as parameter.
   * </p>
   */
	bool hideEventEnabled() { return d_hideEventEnabled; }
	
	
  private:
	bool d_hideEventEnabled;
	QWidget *d_parentWidget;

    ShowHidePrivate *d_ptr;
};



#endif
