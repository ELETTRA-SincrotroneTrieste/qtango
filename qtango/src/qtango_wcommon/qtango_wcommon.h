#ifndef QTANGO_WIDGET_COMMON_H
#define QTANGO_WIDGET_COMMON_H

#include "source_metadata.h"
#include "copypaste.h"
#include "view_trend.h"
#include "showhide.h"
#include "infowidget.h"
#include "whatsthis.h"
#include "helper_application.h"
#include "qtangohdb.h"

class QDragEnterEvent;
class DropEvent;
class QWidget;
class QMenu;
class RightClickEvent;
class QTangoWidgetCommonPrivate;

/** \brief a class which is not a widget but that can be used as a common base 
 *         qtango widgets that have to share common behaviour
 *
 * This class provides a common interface for all qtango widgets that need to integrate
 * <ul>
 * <li><em>view trend</em> functionality (in the right click menu);</li>
 * <li>source <em>copy and paste</em> capabilities;</li>
 * <li><em>show</em> and <em>hide events</em> interception and management;</li>
 * <li><em>tango point information</em> and <em>meta information</em>;</li>
 * <li><em>drag and drop</em> capabilities, to activate tango connection by dragging a source
 *    from a widget into another;</li>
 * <li><em>helper application</em> functionality (in the right click menu);</li>
 <li><em>E-Giga hdb integration</em> (in the right click menu).</li>
 * </ul>
 * 
 * <h3>Some important notes</h3>
 * <p>
 * QTango widget <em>common functionalities</em> implement, together with other features, a <em>right click
 * menu</em>. To provide such feature, the QTangoWidgetCommon <em>captures the right click event</em>.
 * Widgets that need to provide <em>right click menu actions</em> must inherit from <strong>qtcontrols</strong>
 * RightClickMenuable class and implement the pure virtual RightClickMenuable::rightClickMenu() method.
 * QTango <em>TPlotLightMarker</em> represents an example of a QTangoWidgetCommon that also has a custom 
 * right click menu, inherited from qtcontrols' EPlot.<br/>
 * Right click event is <em>eaten</em> by QTangoWidgetCommon and no more available for widgets that have
 * inherited QTangoWidgetCommon.
 * </p>
 */
class QTangoWidgetCommon : public QTangoHelperApplication, 
  public QTangoViewTrend, 
  public CopyPaste,
  public ShowHide,
  public InfoWidget,
  public QTangoWhatsThis,
  public QTangoHdb
{
  public:
	QTangoWidgetCommon(QWidget *parent);

    virtual ~QTangoWidgetCommon();

	/** \brief given the source string, it sets source on the tango handle.
	 *
	 * @param source the tango source, in the form test/device/name/attribute or test/device/name->command.
	 *
	 * It can be reimplemented to provide personalized tango point changes. The default implementation simply
	 * recognizes whether the QTangoWidgetCommon is a reader or a writer and calls setSource() or setTarget()
	 * on it.
	 */
	virtual void changeTangoPoint(const QString& source);
	
	/** \brief given the dropped/pasted string, it decodes it updating source metadata and returning the 
	 *         tango point ready to use.
	 *
	 * @param rawStringData the text to paste into the qtango widget.
	 *
	 * You may reimplement this in a derived class to personalize the method behaviour. You'll still
	 * have to provide a valid QString as return value to be used with the changeTangoPoint() method.
	 *
	 * @see changeTangoPoint()
	 */
	virtual QString decodePasteData(const QString& rawStringData);
	
        /** \brief Forces the widget's <em>enabled</em> property to be independent from the Tango connection status.
	 * 
	 * When a qtango widget configures successfully, it is enabled by the qtango framework. On the other hand, if
	 * configuration fails, it is disabled, to alert the user and at the same time avoid actions on a not configured
         * target. This method allows overriding this behaviour: if the property is set to false, then the connection status
         * does not affect the widget <em>enabled</em> property. Set this property to false if you like to keep your widget
         * always enabled or disabled regardless the connection status of the associated reader or writer.
	 * 
         * @param d true Connection success/failure enables or disables the widget, acting on the <em>enabled</em> property.
         * @param d false the widget is neither enabled nor disabled in consequence to a success/failure of the tango connection.
	 *
	 * @see communicationStatusChangesWidgetEnable
	 */
        void setCommunicationStatusChangesWidgetEnable(bool d) { d_wForceDisabled = d; }
	
	/** \brief returns true if the widget will be forced to stay disabled after a successful tango connection
	 *
	 * @see setCommunicationStatusChangesWidgetEnable
	 */
        bool communicationStatusChangesWidgetEnable() { return d_wForceDisabled; }
	
	/** \brief reveals if drop is enabled
	 *
	 * @return true if drop is enabled, false otherwise.
	 * 
	 * The default behaviour is drop disabled and drag enabled for all qtango widgets
	 */
	bool dropEnabled() { return d_dropEnabled; }
	
	/** \brief enables or disables drop functionality
	 *
	 * @param en true: enables drop from other qtango widgets drag capable or from acceptable 
	 *           strings that represent a qtango point, in the form a/b/c/d or h/i/l->m
	 *
	 * Drop is disabled by default for newly created qtango widgets.
	 */
	void setDropEnabled(bool en);

        /** \brief returns true if QTango drag is enabled, false otherwise.
          *
          * @return true QTango drag event filter is enabled
          * @return false QTango drag event filter is disabled.
          *
          * <h3>Note</h3><p>By default the QTango drag event filter is installed.
          * For widgets that do not want this feature enabled (TLineEdit is a case)
          * it is necessary to call setDragEnabled with a false parameter after the
          * QTango widget constructor has been invoked.
          *
          * @see setDragEnabled
          */
        bool dragEnabled() const;

        /** \brief Enables or disables QTango drag event filter
          *
          * @param en true QTango drag event filter is installed.
          * @param false  QTango drag event filter is removed.
          *
          * By default, QTango widgets inheriting from this class have drag event
          * filter installed.
          */
        void setDragEnabled(bool en);
	
	SourceMetadata* sourceMetadata() { return d_sourceMetadata; }
	
	/** \brief formats the string to be copied into the clipboard or to be drag and dropped
	 *
	 * @return a string that must be understood by the method decodePasteData().
	 *
	 * May be reimplemented in a derived class to provide a special format for the data dragged or copied.
	 *
	 * @see decodePasteData
	 * @see changeTangoPoint
	 */
        virtual QString formatCopyText();

        /** \brief calls setEnabled(true) on the widget. Provided for compatibility
          *
          */
        void enableWidget();

        /** \brief calls setDisabled(true) on the widget. Provided for compatibility
          *
          */
        void disableWidget();
	
	QWidget *widget() { return d_parentWidget; }

        /** \brief Installs the DropEvent event filter without affecting the property dropEnabled.
          *
          * This can be used to silently enable drops for the QTango widget. In particular, it is
          * used by the Qt Designer plugins to enable drops and allow fast configuration of tango
          * sources/targets, even from the jive device panel (Jive version >= 4.12).
          *
          * <h4>Note</h4>
          * <p>
          * Beware that when <strong>dropping a tango attribute or command into a Qt designer
          * widget</strong>, although
          * setProperty (with source or target) is invoked, the <strong>designer property editor</strong>
          * might not update the value <strong>on the fly</strong>. This means you have to click outside
          * the widget being edited and click it again to see the updated source or target property.
          * As a consequence, the designer window title and the <strong>save</strong> button might not
          * be in the state indicating that the form must be saved.
          * On the other hand, <strong>saving the widget right after a drag and drop operation behaves
          * correctly</strong>.
          *
          * </p>
          * <p>
          *
          * Of course, alongside drag and drop, <strong>copy and paste is possible</strong> from the jive
          * main panel.
          * </p>
          */
        void enableDrops();
private:
	void installCopyFilters();
	
	bool d_dropEnabled;
        bool d_wForceDisabled;
	QWidget *d_parentWidget;
	SourceMetadata *d_sourceMetadata;
	DropEvent *d_dropEvent;
	RightClickEvent *rClickEvent;
	QMenu *d_menu;

    QTangoWidgetCommonPrivate *d_ptr;
};

#endif
