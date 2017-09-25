#ifndef ECOSTANTS_H
#define ECOSTANTS_H

#define MARGIN 0.1
#define MIN_MARGIN 1
#define MIN_SIZE 9
#define MIN_BUTTON_SIZE 54

/**
 * \mainpage QtControls: a set of %Qt widgets
 *
 * QtControls is a set of Qt-based widgets used to develop 
 * graphical user interfaces to set/display values. 
 * If you are going to use Tango don't use this library,
 * take a look at <a href="../../../qtango/doc/html/index.html">QTango</a> instead.
 *
 *
 * @author Giacomo Strangolino and Vincenzo Forchi` - 2006-2009
 * @version 5.x.y
 *
 * @see <a href="http://doc.trolltech.com/4.6/">Qt documentation</a>.
 * @see <a href="../../../qtangocore/doc/html/index.html">QTangoCore</a> documentation.
 * @see <a href="../../../qtango/doc/html/index.html">qtango</a> widgets documentation.
 * @see <a href="../../../qtangocore/doc/QTangoCorePresentation.pdf">QTangoCore presentation</a>, pdf document
 *
 * * **
 * <h1>Version 5.3.6</h1>
 * <p>
 *  The source is taken from what would have been the 4.3.6 release.
 *  Private pointers have been added to all relevant classes, so that they can be
 *  extended in the future by means of private classes without breaking the binary
 *  compatibility between releases.
 * </p>
 *
 * <h3>Features</h3>
 * <p>
 * This paragraph describes the main features of <em>qtcontrols</em>. Many of  them have been introduced since
 * version <em>4.x</em>.
 * <h4>Scalable fonts</h4>
 * <p>
 * Version <em>4.x</em> of the library introduced <strong>scalable fonts</strong> in many widgets used to display
 * tango values. Scalable font widgets intercept the <em>resize event</em> and calculate the best font size that 
 * fits the text displayed and the available space. <br/>
 * The class FontScalingWidget implements the necessary calculations aimed at font scaling. Classes inheriting from
 * FontScalingWidget automatically scale their fonts, as soon as they implement the pure virtual method
 * FontScalingWidget::calculateTextSpace(). <br/>
 * FontScalingWidget::calculateTextSpace() actually takes into account the <em>style</em> of a widget and the 
 * space needed by each element of that particular style in that particular widget.
 * </p>
 * <p>
 * Classes implementing font scaling widgets are:
 * <ul>
 * <li>EDoubleSpinBox</li><li>ESpinBox</li><li>ELineEdit</li><li>EPushButton</li><li>ESimpleLabel.</li>
 * </ul>
 * They directly inherit from the correspondig Qt widgets and FontScalingWidget, providing the implementation of 
 * FontScalingWidget::calculateTextSpace(). Should you need to create your own widget with fonts that scale with the
 * widget size, take a look at those classes.
 * A particular case is represented by ENumeric class. It is not a FontScalingWidget, but has FontScalingWidgets as 
 * labels for the displayed digits.
 * </p>
 * <h4>
 * Improved EPlotLight and EPlotLightMarker
 * </h4>
 * <p>
 * EPlotLight now has a configuration dialog which can be executed by right clicking on the plot and selecting the 
 * <em>Configure Plot</em> action.
 * Within the configuration dialog it is possible to
 * <ul><li>change axis lower and upper bounds</li>
 * <li>change the axis boundary adjustment</li>
 * <li>personalize the curves appearence and style</li>
 * </li></ul>
 * </p>
 *
 * <p>
 * EPlotLightBase integrates in the <em>right click menu</em> the possibility to save a snapshot of the plot
 * as an image or to copy it into the clipboard, to later paste the plot screenshot in any image viewer.
 * It is also possible to save data on a <em>comma separated values</em> text file.
 * </p>
 *
 * <p>
 * If used in conjunction with qtangocore, i.e. through <em>TPlotLightMarker</em>, it is possible to launch 
 * a <em>web browser to view the historical database</em> for the attribute displayed.
 * </p>
 *
 *<p>
 * The <em>title of the plot</em> can now be <em>painted</em> on the plot canvas, top left margin, by enabling the
 * Qt property <em>titleOnCanvasEnabled</em>.
 *</p>
 * 
 * <p>
 * The EPlot is made up of EPlotCurve objects. A new method has been introduced to retrieve the EPlotCurve by its name
 * and viceversa:<br/>
 * EPlotCurve* EPlotLight::curve(const QString& name); and<br/>
 * QString EPlotLight::curveName(EPlotCurve *curve),
 * </p>
 *
 * @see EPlotLightBase
 * @see EPlotLight
 * @see EPlotLightMarker
 * @see TPlotLightMarker
 * @see TRealTimePlot
 *
 * <h4>ELed</h4>
 * <p>
 * ELed has a new property called <em>scaleContents</em>, which scales the led to the widget's container width.
 * Gradient configuration has been improved and it is now more tunable.
 * @see ELed
 *</p>
 *
 * <h4>ELinearGauge and ECircularGauge</h4>
 * <p>
 * They now can be configured with a <em>logarithmic scale</em> alongside the linear one. Moreover, the label they
 * display in their center, used to represent the current value read by qtangocore, uses engineering notation if 
 * the <em>valueFormat</em> property is set according to the format recognized by EngString.
 * @see EngString
 * Read the next paragraph to learn more about <em>engineering notation</em> support in qtcontrols/QTango 4.x.
 * </p>
 *
 * <h4>Engineering notation</h4>
 * <p>
 * The new class EngString, defined in eng_notation.h file inside the <em>common</em> directory, residing on the top
 * level directory of qtango distribution (being shared between qtango and qtcontrols), implements an <em>engineering
 * representation</em> of the values it stores.
 * EngString is a QString that, given a particular format like "<em>%3eng</em>", stores the numeric value together with
 * the measurement unit prefix defined by the International System.
 * It is possible to specify the number of significant digits that are desired (like <em>%3</em> in the example above).
 * Using engineering notation, you will be able to read 10.0MW instead of 10000000W, with three significant digits.
 * Have a look at EngString class documentation for more details and usage.
 * </p>
 *
 * <h4>Easily inherit and integrate or personalize qtcontrols/qtango <em>right click menu</em> in your widgets!</h4>
 * <p>
 * Implementing a new widget with a right click menu is now easy with qtcontrols 4.x. Actually, it is very fast to 
 * create a new qtango or qtcontrols widget having a <em>right click menu integrated</em> with the default right click
 * menu provided by qtangocore and qtcontrols.
 * Have a look at the class RightClickMenuable.
 * The utility application named <em>Mango</em> makes intensive use of RightClickMenuable to personalize its right click
 * widgets' menu.
 * </p>
 *
 * <h4>ENumeric</h4>
 * <p>
 * ENumeric inherits now QFrame, so that it can be customized also as a QFrame, as qtango TReaderWriter does.
 * Enumeric and EApplyNumeric labels have now <em>scalable fonts</em>.
 * </p>
 *
 * <h4>EFlag</h4>
 * <p>
 * EFlag benefits from <em>scalable fonts</em>, since it uses ELabels with scalable fonts enabled.
 * </p>
 *
 *
 * <h4>EPlot</h4>
 * <p>
 * Since version <em>4.x</em>, you are discouraged to use EPlot and EPlot-derived widgets in your panels.
 * This class is now discontinued and no longer supported. Please migrate to EPlotLight and derived widgets
 * all your EPlot (TPlot) based plots. <br/>
 * EPlot instantiations will give you a warning.
 * </p>
 * 
 * <h3>Notes</h3>
 * <p>
 * In qtcontrols version <em>4.x</em>, scalable fonts are still experimental. Should you encounter any issue with them,
 * simply remove from the file <strong>qtcontrols.pro</strong> the directive<br/>
 * <strong>DEFINES += SCALABLE_FONTS_ENABLED_BY_DEFAULT</strong><br/>
 * and <strong>rebuild</strong> the whole library.<br/>
 * If the issue concerns a specific panel or a very limited range of widgets in a panel, then you can disable scalable
 * fonts by unsetting the property <strong>fontScaleEnabled</strong> on the widget itself. The fontScaleEnabled property
 * is a <em>Qt property</em> exported to the Qt designer.
 * </p>
 *
 * 
 * @see <a href="../../../qtangocore/doc/html/index.html">QTangoCore</a> documentation.
 * @see <a href="../../../qtango/doc/html/index.html">qtango</a> widgets documentation.
 *
 *
 *
 */
#endif
