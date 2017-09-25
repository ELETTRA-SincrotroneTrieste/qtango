#ifndef _QTANGO_H
#define _QTANGO_H

#include <tango.h>
using namespace Tango;

#include <qtangocore.h>
#include <tlabel.h>
#include <ttable.h>
#include <tlogbutton.h>
#include <tpushbutton.h>
#include <tled.h>
#include <tcheckbox.h>
#include <tpixmap.h>
#include <tdbbrowser.h>
#include <tnumeric.h>
#include <tspinbox.h>
#include <tapplynumeric.h>
#include <tdoublespinbox.h>
#include <tnumeric.h>
#include <tlineargauge.h>
#include <tcirculargauge.h>
#include <tcheckbox.h>
#include <tcombobox.h>
#include <tlineedit.h>
#include <tlogdialog.h>
#include <twidgetgroup.h>


/*#ifdef MYDEBUG
	#include <QtDebug>
	#define MYLOG(str) qDebug() << str
#else
	#define MYLOG(str)
#endif*/


/**
 * \mainpage QTango - Qt & Tango
 *
 * QTango is a framework built on top of <a href="../../../qtangocore/doc/html/index.html">QTangoCore</a> 
 * and <a href="../../../qtcontrols/doc/html/index.html">QtControls</a>. It consists of 
 * classes and widgets that interact with the <a href="http://www.tango-controls.org">Tango control system</a>,
 * while providing an easy <em>API</em> to the programmer and full integration with the <em>Qt4 designer</em>.
 *
 * @author Giacomo Strangolino and Vincenzo Forchi` - 2006/09/26 - 2013/05..
 * @version 5.x.y
 *
 * @see <a href="http://doc.trolltech.com/4.5/">Qt documentation</a>.
 * @see <a href="../../../qtcontrols/doc/html/index.html">qtcontrols</a> documentation.
 * @see <a href="../../../qtangocore/doc/html/index.html">QTangoCore</a> widgets documentation.
 * @see <a href="../../../qtangodbus/doc/html/index.html">QTangoDBus</a> QTango DBus module documentation.
 * @see <a href="../../../qtangocore/doc/QTangoCorePresentation.pdf">QTangoCore presentation</a>, pdf document
 * @see <a href="../../../mango/doc/index.html" alt="Mango online development tool">Mango online development tool</a> documentation.
 *
 * **
 * <h1>Version 5.3.6</h1>
 * <p>
 *  The source is taken from what would have been the 4.3.6 release.
 *  Private pointers have been added to all relevant classes, so that they can be
 *  extended in the future by means of private classes without breaking the binary
 *  compatibility between releases.
 * </p>
 *
 **
 * <h1>Version 4.3.1</h1>
 * <h2>Hdb adapter classes to display historical data into EPlotLight plot family</h2>
 * <p>
 * QTango version 4.3.1 introduces a bunch of new classes aimed at transforming the EPlotLight
 * plot family (included TPlotLightMarker, which is an EPlotLight) so that they can display
 * historical database data together with normal <em>live</em> curves.
 * The sources are located under the qtango/src/plotadapter directory.
 * The relevant classes are
 * \li DbPlotAdapterInterface
 * \li HdbPlotAdapter
 *
 * Take a look to their documentation to learn how to use them.
 * The plot adapter classes can be used with the THdb library. The latter is a separate library
 * that retrieves the data from the historical / temporary database and makes it available through
 * a simple SIGNAL/SLOT interface. You have to install that library to benefit from the HdbPlotAdapter
 * class in order to display archived data in the qtango/qtcontrols plots.
 *
 * @see DbPlotAdapterInterface
 * @see HdbPlotAdapter
 *
 * An example may be found in the THdb library <em>test/</em> directory. The test application makes use
 * of the HdbPlotAdapter class and  the THdb classes themselves.
 *
 * \note The DbPlotAdapterInterface and HdbPlotAdapter classes must be still considered in
 *       development state.
 *
 * </p>
 * <h2>Other news</h2>
 * <p>
 *  \li TComboBox indexMode has been made more flexible by means of the indexOffset property
 *      it is no more necessary that the index in the attribute property starts from zero.
 *
 * \li TReaderWriter: fixed behaviour for writer widgets that may overlap with other widgets.
 * \li TUtil::findHelperApplication takes into account a tango host different from the one in the
 *            environment.
 * \li TAction: it has two new methods to obtain tango database host and port.
 * \li QTangoHelperApplication in qtango_wcommon is now able to get the correct host and port, even
 *     if different from the one specified in the TANGO_HOST environment variable.
 * \li It is possible to customize the TPushButton confirm dialog title and text, through the
 *     confirmDialogTitle and confirmDialogText <em>free properties</em>
 *
 * </p>
 * <h2>Plugin news</h2>
 * <p>
 *
 * Drag and Drop of tango sources and targets in designer widgets pops up a Dialog where you
 * can quickly configure the source/target of the selected widget. You can easily configure
 * a source or a target inserting or removing wildcards.
 * Drag and drop is supported from the jive main panel and from any QTango application.
 * This new feature speeds up a lot the design phase.
 *
 *
 * </p>
 *
 * <h1>Projects related to the 4.3.1 release</h1>
 *
 * <p>The THdb library becomes a related project since the 4.3.1 release, due to the
 * introduction of the DbPlotAdapterInterface and HdbPlotAdapter classes described
 * above
 *
 * \note The DbPlotAdapterInterface and HdbPlotAdapter classes must be still considered in
 *       development state.
 * </p>
 *
 * <p>
 * Other fixes and improvements can be found in the CHANGELOG file
 * </p>
 **
 * <h1>Version 4.3.x</h1>
 * <p>
 * Library version 4.3 fixes some minor issues and adds some minor functionalities.
 * Among others, improvements and new functionalities have been added to QTWatcher and QTWriter
 * classes. It is now possible to benefit from the single shot option and have the objects auto
 * destroyed after the firs read (or write). <br/>
 * Overlapping curves' titles in EPlotLightMarker are now shown in the marker text and the property
 * vectorUpdateMode can be forced to true in order to let the plot interpret a curve as if its data
 * came from a vector (this can be useful in some cases). <br/>
 * Auto configuration is now correctly accomplished even when in the same client there are connections
 * towards devices pertaining to different tango hosts. In the previous versions, the tango database
 * connection was done taking the tango host from the environmental variable only.
 *
 * </p>
 *
 *
 * <h1>Version 4.2.x</h1>
 * <p>
 * Library version 4.2 brings some improvements to the library objects, strenghtens core stability and
 * adds a couple of utility classes.
 * </p>
 *
 * <h1>Core news</h1>
 * <p>
 * The <strong>DEFAULT_REFRESH_MODE</strong> environment variable can be set to make your application
 * adopt a default refresh mode when AUTO_REFRESH is chosen in setSource (the default) call. This
 * can be used to disable automatic subscribe_event calls when setSource is performed.
 *
 * @see ActionFactory::defaultRefreshMode
 *
 * </p>
 * <p>
 *  The <strong>QTANGO_ATTR_CONF_CHANGE_DISABLED</strong> environment variable can be tuned to disable
 *  configuration change events in your application (or in your environment). This avoids an extra call
 *  to subscribe_event at client connection. You may safely turn this option on if you are not interested
 *  being notified in your client application when some attribute configurations change into the tango
 *  database.
 *
 * @see QTangoAutoConfiguration::getConfigurationChangeEventEnabled
 *
 * </p>
 *
 * <h1><a href="../../../qtangodbus/doc/html/index.html">DBus support</a></h1>
 * <p>
 * QTango now supports the DBus session message bus.
 * The classes implementing this kind of support are located under the <strong>dbus/</strong> directory
 * of the main distribution folder. TApplication class uses dbus to register itself into the session
 * message bus. TUniqueApplication guarantees that your application is unique inside the DBus session.
 * TApplication exports its interface to clients through the TApplicationInterface class. TApplication
 * is exported to DBus via the adaptor design pattern through the TApplicationAdaptor class. The
 * TApplication dbus interface is described by the dbus/TApplication.xml file.
 * The pool of TApplications can be explored by means of the TApplicationsPoolProxy class, which is
 * aimed at retrieving the applications lists, finding an application by name and pid or by dbus
 * service name. TApplicationsPoolProxy returns TAppInfo objects that can be used to connect to the
 * QTango applications via the TApplicationInterface.
 *
 * @see TApplicationsPoolProxy
 * @see TApplication
 * @see TApplicationInterface
 * @see TUniqueApplication
 *
 * </p>
 *
 * <h1>Jive version >= 4.12 and drag and drop in Qt designer</h1>
 *
 * <p>
 * Since Jive version 4.12, it is possible to drag a tango command or attribute from the
 * device panel into any QTango widget of the Qt4 Designer. QTango library 4.2.x supports
 * this new feature.
 * </p>
 *
 * <p>
 * Of course, also <strong>copy and paste</strong> actions have been added to the Jive
 * version 4.12. With them it is possible to copy the tango attribute or command in plain
 * text and paste it anywhere.
 * </p>
 * <h3>Recommended readings</h1>
 * <p> QTangoWidgetCommon::enableDrops method documentation
 * </p>
 *
 *
 * <h1>QTWatcher class</h1>
 * <p>
 *  QTWatcher is a special class that reads a tango variable using QTangoCore.
 * Any QObject or base types (int, short, double, unsigned int, QString, bool) can be
 * attached to this reader and obtain the new data when the attribute or command is
 * refreshed.
 * This is one of the two QTangoizer classes, that use QTangoCore to update widgets
 * or other data which is QTango unaware.
 *
 * @see QTWatcher
 *
 * </p>
 *
 * <h1>QTWriter class</h1>
 * <p>
 *
 * QTWatcher class can be associated to a Qt widget that is capable of receiving input from the
 * user and represent it into a supported data type. Then a signal pertaining to that class is
 * connected to an internal execution slot that executes the target provided according to the
 * data type supported by the widget.
 *
 * @see QTWriter
 *
 * </p>
 *
 * <h1>TValueFilter class</h1>
 * <p>
 * QTWatcher can be used in conjunction with the TValueFilter class.
 * This class must be subclassed and one of its methods must be reimplemented. Thanks to
 * TValueFilter class you can intercept new data as it is refreshed, modify its value, and then
 * reinject it into QTWatcher to have its value displayed or updated.
 * Currently, this can be applied to QTWatcher only, but in a future revision of the QTango library,
 * it may be possible to attach a filter to any TVariant refresh related method.
 * </p>
 *
 * <h1>Version 4.x</h1>
 *
 *<p>
 * Version <em>4.x</em> of the <em>QTango</em> framework introduces some important features, which are described 
 * in continuation and affects a great many widgets. Font scaling widgets is the most relevant change in widgets,
 * but many other object specific improvements and optimizations have been added.</p>
 * <p>Globally, applications will result much more responsive at startup, since QTangoCore completely performs 
 * the initial configuration of the objects in the Device specific thread.
 * </p>
 *
 * <p>
 * Drag and drop among widgets has been introduced to open new perspectives in applications interoperability and 
 * in dynamic creation and configurtion of QTango GUI elements. The <a href="../../../mango/doc/index.html" 
 * alt="Mango online development tool">Mango</a> related project provides a synthesis of all these new features in
 * action.
 * It will now be possible to create a bunch of empty plots and drag and drop into them tango sources from other panels 
 * to start watching the curves of their values in time. 
 * </p>
 *
 * <h1>Projects related to the 4.x release</h1>
 *<p>
 * The <a href="../../../mango/doc/index.html" alt="Mango online development tool">Mango online development tool</a>.
 * An application taking advantage of the QTango <em>4.x</em> new features that allows very easy and fast development
 * of graphical user interfaces interacting with the <em>Tango</em> control system. With its profiles system, drag
 * and drop and <em>Qt properties</em> support, drawing simple control panels is now immediate.
 * Read the <a href="../../../mango/doc/index.html" alt="Mango online development tool">documentation</a> to learn
 * much more...
 * </p>
 *
 * <h1>Library main news</h1>
 *
 * <h1>Widgets with font scaling</h1>
 * <p>
 * QTango <em>4.x</em> widgets representing strings (readers and writers, except TComboBox) benefit from 
 * <a href="../../../qtcontrols/doc/html/index.html">qtcontrols</a> widgets with font scaling implementation.
 * If you build the library with the default options setup in the file <em>qtcontrols/qtcontrols.pro</em>, 
 * font scaling is enabled by default in all widgets supporting this feature in their constructor. 
 * If you experience any problem with them, rebuild the whole library commenting the line <br/><em>
 * DEFINES += SCALABLE_FONTS_ENABLED_BY_DEFAULT</em><br/> in <em>qtcontrols/qtcontrols.pro</em>.
 * <strong>TApplyNumeric</strong> also benefits from font scaling widgets, but <em>in 4.x beta version</em> 
 * the constructor <em>disables this feature</em>.
 * </p>
 * <p>
 * The <em>designable property</em> <strong>fontScaleMode</strong> controls the behaviour of the widgets
 * as to the font scaling feature. It is possible to disable the scaling feature, to scale fonts basing on
 * the widgets <em>height</em> or both <em>width and height</em>. The other two <em>managed</em> modes available
 * through the property are reserved for future use and are not currently supported, apart from an experimental
 * implementation made available by the qtcontrols EWidget class used by the 
 * <a href="../../../mango/doc/index.html" alt="Mango online development tool">Mango</a> development tool.
 * </p>
 *
 * <h1>Drag and drop</h1>
 * <p>
 * Tango source points can be dragged from a QTango widget into another. The receiver of the drop event will 
 * immediately configure itself with the same source and start reading.
 * </p> 
 *
 * <p>
 * Drop event is accepted according to each widget's <em>dropEnabled</em> Qt property enabled or not. Normally,
 * you can <em>drag from every widget</em> but <em>drop only if dropEnabled is true</em>. This choice is aimed 
 * at avoiding drag/drop actions performed by mistake.
 * </p>
 *
 * <h1>Easy development of new widgets</h1>
 * <p>
 * QTango widget development becomes easier and clearer now, by means of the QTangoWidgetCommon class that is 
 * inherited by all QTango aware widgets. Subclassing QTangoWidgetCommon means to make available to your 
 * widgets common functionalities such as:
 *
 * <ul>
 * <li><em>view trend</em> functionality;</li>
 * <li>source <em>copy and paste</em> capabilities;</li>
 * <li><em>show</em> and <em>hide events</em> interception and management;</li>
 * <li><em>tango point information</em> and <em>meta information</em>;</li>
 * <li><em>drag and drop</em> capabilities, to activate tango connection by dragging a source
 *    from a widget into another;</li>
 * <li><em>helper application</em> functionality;</li>
 * <li><em>E-Giga hdb integration</em> through desktop services.</li>
 * </ul>
 * 
 * @see QTangoWidgetCommon
 * 
 * Widgets that subclass QTangoWidgetCommon and ProxyInterface, besides QTangoComProxyReader or QTangoComProxyWriter
 * are QTango integrated and ready to be used through their common interface.
 *
 * </p>
 *
 * <h1>Changes in QTango widgets</h1>
 *
 * <h1>TLabel</h1>
 * <p>
 * The <em>backgroundColorChangeEnabled</em> property enables or disables the label background color that normally
 * indicates the quality of the Tango attribute represented. This feature might be disabled to benefit from a new
 * option introduced in TLabel: the <em>excursion indicator</em>.<br/>
 * The excursion indicator, which can be enabled by setting to true the property <em>setExcursionEnabled</em>, is
 * a very thin coloured bar drawn in the highest part of the label, whose colours represent the attribute quality.
 * The position of a little stylized <em>cross</em> (<strong>x</strong>) indicates the value displayed by the label
 * text proportionally to the minimum and maximum value for the attribute represented. For this to work, as usual,
 * the attribute must be configured with minumum and maximum values, and optionally with warning and alarm thresholds.
 * </p>
 *
 * <h1>TLinearGauge and TCircularGauge</h1>
 * <p>
 * Both gauges can now benefit from the <em>logarithmic scale</em> to represent data. They also benefit from
 * the <em>engineering notation</em> introduced in qtcontrols. The compatible format to display the engineering
 * notation has to be specified manually in the widgets <em>valueFormat</em> property.
 * </p>
 *
 * <h1>TReaderWriter</h1>
 * <p>
 * TReaderWriter has become a <em>reader</em> <strong>and</strong> a <em>writer</em>, just like the TCheckBox.
 * As a consequence, it is now possible to configure a <em>target different from the source</em>. Compatibility
 * needs suggest that if no target is specified, then it is assumed to be equal to the provided source. The 
 * writer widget can be obtained by the writer() method. It might be NULL if the TReaderWriter hasn't been 
 * configured yet. It is possible to set read only mode by calling setReadOnly.
 * </p>
 * <h5>Custom writer type</h5>
 * <p>
 * It is now possible to customly specify the desired writer type: TSpinBox, TDoubleSpinBox, TLineEdit, 
 * TComboBox and TNumeric, alongside automatic configuration (<em>Auto</em> mode).
 * </p>
 *
 * <h5>Custom writer position</h5>
 * <p>
 * It is also possible to customize the <em>WriterPosition</em> property, <em>AutoPosition, North, South, East, West</em>.
 * When a position different from AutoPosition is chosen, no calculations are performed by the TReaderWriter to 
 * see if the writer fits into the available space above or below the reader widget.
 * </p>
 * 
 * <h5>Zooming</h5>
 * <p>
 * When the widget <em>is not inserted into a layout</em>, it is possible to configure it to auto zoom when clicked.
 * This is not possible when the widget is part of a layout, since its resize would affect all the other widgets' size
 * sharing the layout. zoomPercentage and zoomEnabled properties are available to customize the zooming behaviour.
 * </p>
 *
 * <h5>Custom writer and apply button stylesheets</h5>
 * <p>
 * If you are unhappy with the default stylesheet of the writer widget and the apply button, two new methods are 
 * provided to change the widgets stylesheet: setWriterStyleSheet and setApplyButtonStyleSheet.
 * </p>
 *
 * <h1>TPixmap</h1>
 * <p>
 * The design possibilities of the TPixmap have been expanded by exporting some new Qt properties that allow fast 
 * configuration of the widget through the Qt4 designer. setStateList and setPixmapList new methods are aimed at
 * this purpose.
 * </p>
 *
 * <h1>TTable</h1>
 * <p>
 * TTable auto configures itself through some device attribute properties introduced to define its 
 * number of cells, its display mask and the true and false strings and colours. This means that it
 * is no more necessary to spend time configuring the table cells strings and colours in the panel
 * design phase. Moreover, changing the meaning or the bit mask of an array of boolean is done on
 * the server side and does not require the application rebuild. After all, the meaning of the elements
 * of an array of boolean is intrinsic to the device server. See TTable documentation for more details.
 * </p>
 *
 * <h1>TPlotLightMarker</h1>
 * <p>
 * The TPlotLightMarker class has been improved and now provides a right click menu action to configure the 
 * plot scales and some other properties such as curve colors and style. These new features have been inherited
 * from the new qtcontrols EPlotLight implementation. <em>Pause and restart acquisition</em> features have been
 * introduced, together with the possibility to <em>save a snapshot</em> of the plot on a file or through the 
 * desktop <em>clipboard</em> and to <em>save the plot data on a CSV file</em>. <em>Coloured background</em> 
 * property has been experimentally reintroduced, to visualize on the plot canvas the warning and alarm zones
 * of the Tango attribute being plot. colouredBackground property manages this feature. The default is disabled.
 * A virtual method plotTitle() has been introduced to provide a custom string to display as the plot title. The
 * default method simply returns the tango sources configured. It is possible to read <em>attribute history</em>
 * for the configured source(s) if the <em>fillFromHistoryEnabled</em> property is set to true.
 * </p>
 *
 * <p>
 * The <em>zoom action</em> has changed its shortcut in consequence of the introduction of drag and drop between
 * widgets: it is now necessary to <em>press Control + click</em> to activate zoom. A tooltip on the plot canvas
 * will suggest this action to the user anyway. This observation applies to all qtcontrols EPlotLight derived plots.
 * </p>
 * 
 * <h1>TrendPlot</h1>
 * <p>
 * Trend plot has been renewed and now inherits from TPlotLightMarker, thus providing the same aspect and interface.
 * At startup, it reads the <em>attribute history</em> from the device attribute configured as source by the widget
 * owning the plot. QTangoViewTrend enables coloured background on the plot.
 * </p>
 *
 * <h1>TRealtimePlot</h1>
 * <p>
 * The TRealtimePlot class is a special TPlotLightMarker tailored for <em>real time commands</em>. The source 
 * provided must be a tango <em>command</em>. Attributes are unsupported and so trying to specify an attribute
 * will cause an error. Use TPlotLightMarker instead.
 * TRealtimePlot::setMode method configures the reading method for the real time command. The integer passed as 
 * parameter assumes the same meaning as the first argument required by the real time Tango commands.
 * @see TRealtimePlot::setMode.
 * <p>
 * The configuration dialog available through the right click menu action gives access to the main configuration 
 * parameters of the TRealtimePlot.
 * </p>
 * </p>
 *
 * <h1>TPlot</h1>
 * <p>
 * TPlot, together with its super class EPlot from qtcontrols, has been now marked as <em>deprecated</em>.
 * A warning  (if QTANGO_PRINT environment variable is set on your console) at runtime will suggest you 
 * to migrate to EPlotLight set of plots. EPlot/TPlot is <em>discontinued</em> from now on.
 * </p>
 *
 * <h1>TSimpleLabel</h1>
 * <p>
 * The TSimpleLabel class was introduced to easily configure a QLabel with the name of a QTango source point
 * and with the Right alignment of the text, to easily couple a TSimpleLabels with a TLabel on its right.
 * Drag and drop support together with the TSimpleLabel::setSource method allow easy configuration of the 
 * displayed text with the name of the source you drop or manually set with the appropriate Qt property.
 * </p>
 *
 * <h1>TSpectrumButton</h1>
 * <p>
 * A push button that writes multiple attributes or gives multiple commands has been introduced.
 * It is enough to add the <em>objectNames</em> of the QTango widgets used to set the values 
 * (which must be SimpleDataProxy elements) to an objectName list of this special button.
 * Integration with the Qt4 designer is full as to the objectName list.
 * </p>
 *
 * <h1>TInfoTextBrowser</h1>
 * <p>
 * A <em>Text browser</em> that displays information about the tango source configured on the widgets that
 * populate a control panel. Moving the mouse over the widgets causes an update on the text.
 * By setting a defaultMessage, a custom text message is displayed whenever the mouse leaves each widget.
 * Each qtango widget has got Qt properties aimed at defining an additionalInfo string to show after the 
 * QTango messages specific to the tango source or target points. 
 * As an example, consider the TLabel and its <em>enterEventDelay, enterLeaveEventsEnabled and additionalInfo</em>
 * qt properties. They are available to all qtango widgets and are used to interact with TInfoTextBrowser.
 *
 * @see InfoWidget::setEnterEventDelay
 * @see InfoWidget::setEnterLeaveEventsEnabled
 * @see InfoWidget::setAdditionalInfo
 *
 * <h5>Availability</h5><p>
 * This class is exported as a Qt4 designer plugin and is ready to be included in any QTango application.
 * This is also invoked and instantiated inside a Window by <em>Ctrl+Click</em> action on a QTango widget.
 * </p>
 * </p>
 *
 * <h1>Testing the new widgets features</h1>
 * <p>
 * Alongside the <em>examples</em> and <em>test</em> directories inside the qtangocore and qtcontrols/qtango
 * folders, the <a href="../../../mango/doc/index.html" alt="Mango online development tool">Mango</a> related
 * project comes with a set of <em>examples</em> available through the 
 * <a href="../../../mango/doc/index.html" alt="Mango Profiles">Mango Profile Manager</a>, in the <em>Examples</em>
 * <em>context</em>. Just open Mango application and click on <em>File, Profile Manager...</em> menu action to 
 * watch a list of the available example profiles under the <em>Examples</em> context.
 *
 * </p>
 *
 */

#endif

