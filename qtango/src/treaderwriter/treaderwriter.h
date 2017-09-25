#ifndef TREADERWRITER_H
#define TREADERWRITER_H

#include <QWidget>
#include <TLabel>
#include <com_proxy_writer.h>
#include <TSpinBox>
#include <QGridLayout>
#include <QDialog>

class TReaderPushButton;
class TWriterWidget;
class TDoubleSpinBox;
class TSpinBox;
class TComboBox;
class TLineEdit;
class QPushButton;
class QTimer;
class TNumeric;
class TReaderWriterPrivate;

/** \brief A class providing a label with read functionalities. When mouse is over, a configuration button appears and a writer is available
 *
 *  This class represents a Tango reader with a classic TLabel.
  * But when the mouse is over the widget, a push button appears on the right and lets you write a value on the same 
  * tango attribute configured as source.
  * Just create a TReaderWriter and call setSource() on it. The attribute must be configured with minimum and maximum values,
  * so that the TReaderWriter  can use auto configuration to create the writer part and set the limits on it.
  * The writer part can be a spin box, a double spin box or a line edit, depending on the  type of source you configured.
  * 
  * This class <em>is</em> a TLabel.
  *
  * \par Example
  * \code
  * #include <QDialog>
  * #include <TReaderWriter> 
  * QDialog *dlg = new QDialog(0);
  * QHBoxLayout *hlo = new QHBoxLayout(dlg);
  * TReaderWriter *rw = new TReaderWriter(dlg);
  * hlo->addWidget(rw);
  * hlo->setSource("test/device/1/double_scalar");
  * 
  * \endcode
  *
  *	Configuration events are intercepted and connected to an initialization slot that sets up the writer part.
  *	Since hide events call proxy reader's unsetSource, on the next show event setSource() is called again:
  *	<em>at every hide/show event the writer part is reconfigured</em>.
  *
  * If the TReaderWriter is not managed by a QLayout, then its zooom property can be enabled and the widget
  * is zoomed when the user clicks on it. All overlapping widgets are temporarily hidden and shown back again
  * when unzooming takes place.
  *
  * The TReaderWriter provides a default style for each kind of writer and for the apply button. You can
  * personalize the stylesheet by means of the properties <em>writerStyleSheet</em> and <em>applyButtonStyleSheet</em>.
  * 
  */
class TReaderWriter : public TLabel, public QTangoComProxyWriter
{
Q_OBJECT
Q_PROPERTY (QString targets READ targets WRITE setTargets DESIGNABLE true)
Q_PROPERTY(QTangoCommunicationHandle::ExecutionMode executionMode READ executionMode WRITE setExecutionMode)
Q_PROPERTY(int configButtonHideTimeout READ configButtonHideTimeout WRITE setConfigButtonHideTimeout)
Q_PROPERTY(double writerSingleStep READ writerSingleStep WRITE setWriterSingleStep)
Q_PROPERTY(double zoomPercentage READ zoomPercentage WRITE setZoomPercentage)
Q_PROPERTY(bool zoomEnabled READ zoomEnabled WRITE setZoomEnabled)
Q_PROPERTY(WriterType writerType READ writerType WRITE setWriterType)
Q_PROPERTY(WriterPosition writerPosition READ writerPosition WRITE setWriterPosition)
Q_PROPERTY(QString writerStyleSheet READ writerStyleSheet WRITE setWriterStyleSheet);
Q_PROPERTY(QString applyButtonStyleSheet READ applyButtonStyleSheet WRITE setApplyButtonStyleSheet)
/* hdb action enabled property inherited from TLabel */

Q_ENUMS(WriterType)
Q_ENUMS(WriterPosition)
// Q_PROPERTY(bool zoomOnHoverEnabled READ zoomOnHoverEnabled WRITE setZoomOnHoverEnabled)
/* additionalInfoProperty inherited by TLabel */

  public:
	
	/** \brief configuration directives for the TReaderWriter
	 *
	 * <ul>
	 * <li><em>Auto</em> automatic configuration is chosen depending on the type of data to represent
	 * (spin box, double spinbox, line edit, combo box..);</li>
	 * <li><em>SpinBox</em>, <em>DoubleSpinBox</em>, <em>ComboBox</em>, <em>LineEdit</em>: force one of
	 * the widgets;</li>
	 * <li>Numeric: use the TNumeric as interface for changing the scalar value.</li>
	 * </ul>
	 * <h3>Note</h3>
	 * <p>If <strong>Auto</strong> mode is chosen then the <em>Numeric</em> writer
	 * is chosen when the data type is not a string or the attribute has not the <cite>values</cite> property. 
	 * If the TReaderWriter is inserted into a layout, then then the zoom is disabled. In that case, if the 
	 * Numeric writer is chosen, it might not fit the available space. Specify another writer type if needed.</p>
	 * <p>If <strong>Auto</strong> mode is chosen and the <em>format</em> of the Tango attribute contains
	 * <em>eng</em> specifier, then a Line edit is chosen.</p>
	 */
	enum WriterType { Auto, SpinBox, DoubleSpinBox, ComboBox, LineEdit,  Numeric};
	
	/** \brief Available choices for the position of the writer part.
	 *
	 * <ul><li><em>AutoPosition</em>: tries to position the writer above or below the reader, depending on the
	 * available size around  the reader itself. Will never place the writer at the left or right of the
	 * reader.</li>
	 * <li><em>North, South, East, West</em>: places the writer part above, below, at the right or at 
	 * the left of the reader, respectively. The positioning is made without checking whether the space
	 * needed is available or not.</li>
	 * </ul>
	 * 
	 * @see setWriterPosition
	 */
	enum WriterPosition { AutoPosition, North, South, East, West };
	
	/** The constructor: pass a not null parent please.
	 * @param parent a not null parent for the widget.
	 */
    TReaderWriter(QWidget *parent);
    
	virtual ~TReaderWriter() {};

	/** @return the configuration button hide timeout
     */
    int configButtonHideTimeout();
    
    /** \brief set the configuration button hide timeout
     * @param to after to milliseconds the configuration button disappears (after the mouse leaves the widget, of course )
     */
    void setConfigButtonHideTimeout(int to);
    
    void setWriterSingleStep(double s);
    double writerSingleStep() { return d_writerSingleStep; }
	
	WriterType writerType() { return d_writerType; }
	void setWriterType(WriterType t);
	
	/** \brief returns the writer position with respect to the reader.
	 *
	 * @return the position of the writer when the user clicks on the 
	 * <em>edit button</em> of the TReaderWriter, with respect to the
	 * reader.
	 *
	 * @see WriterPosition
	 */
	WriterPosition writerPosition() { return d_writerPosition; }
	
	/** \brief change the writer position with respect to the reader.
	 *
	 * Changes the position of the writer when the user clicks on the 
	 * <em>edit button</em> of the TReaderWriter.
	 *
	 * @see WriterPosition
	 */
	void setWriterPosition(WriterPosition wp) { d_writerPosition = wp; }
	
	/** \brief returns the writer widget, if intialized, otherwise returns NULL
	 *
	 * If setSource was called on the TReaderWriter, then a writer widget is available
	 * and returned by this method.
	 * The objectName() of the writer is composed by the fixed prefix <em>TRW_WRITER</em>,
	 * remembering that this is the writer widget of a TReaderWriter followed by the 
	 * TReaderWriter objectName() and a random number generated for internal use.
	 * The composed string is without white spaces, unless the objectName() contains them
	 * (no character substitution is done on the objectName()).
	 *
	 */
	QWidget *writer() { return d_writerWidget; }

	/** \brief returns the readOnly property set with setReadOnly.
	 *
	 * @return true the widget is in read only mode
	 * @return false the widget is in its default state: on mouse enter event a push button 
	 *         appears and a writer becomes available on the push button click.
	 */
	bool readOnly() { return d_readOnly; }
	
	/** \brief sets the zoom percentage to be applied when zoom is enabled.
	 *
	 * @see setZoomEnabled
	 * @see zoomEnabled
	 */
	double zoomPercentage() { return d_zoomPercentage; }
	
	/** \brief returns true if the zoom is enabled, false otherwise.
	 *
	 * Note that it is not enough to set to true this property.
	 * In addition, the TReaderWriter must not be in a layout.
	 */
	bool zoomEnabled() { return d_zoomEnabled; }
	
	/** \brief not implemented
	 *
	 */
	bool  zoomOnHoverEnabled() { return d_zoomOnHoverEnabled; }
	
	/* TReaderWriter is a reader and a writer: must provide overriders for methods in ProxyInterface */
	QTangoCommunicationHandle* qtangoComHandle() const { return QTangoComProxyWriter::qtangoComHandle(); }
	virtual QString tangoPoint() const  { return QTangoComProxyReader::tangoPoint(); }
	bool isReader() const { return true; }
	bool isWriter() const { return true; }
	virtual QList<QTangoCommunicationHandle*> handleList() const;
	
	/** \brief Changes the writer widget's stylesheet.
	 *
	 * @param style a valid CSS stylesheet for the widget.
	 * See qt documentation to customize widgets with stylesheets
	 * http://doc.trolltech.com/4.6/stylesheet.html
	 */
	void setWriterStyleSheet(const QString &style);
	
	/** \brief returns the writer stylesheet
	 */
	QString writerStyleSheet() { return d_writerStyleSheet; }
	
	/** \brief Changes the apply button style
	 *
	 * @see setWriterStyleSheet
	 */
	void setApplyButtonStyleSheet(const QString &style);
	
	/** \brief returns the apply button stylesheet
	 *
	 * @see setWriterStyleSheet
	 * @see setApplyButtonStyleSheet
	 */
	QString applyButtonStyleSheet() { return d_applyButtonStyleSheet; }
	
  signals:
	
	/** \brief this signal is emitted after the execution of the target action.
	 *
	 * @param returnValue the TVariant representing the argout of the command
	 * If the target is a command, then it might have a return value. In that case, 
	 * its argout is available through the parameter returnValue.
	 * @param tangoPoint the target on which the command was executed.
	 * 
	 */
	void newDataAvailable(const QString& tangoPoint, const TVariant &returnValue);

        /** \brief returns the writer when it is created.
          *
          * @param writer the widget created
          * @param type the type of widget created.
          *
          * The type can be used to cast the writer to the correct type.
          * Once you cast the writer widget to the correct type you are able to
          * manupulate it, such as changing its properties.
          */
        void writerCreated(QWidget* writer, WriterType type);

        /** \brief signal indicating that the edit button of the TReaderWriter has been clicked.
          *
          * @param toggled: if true, the user requested the writer part of the TReaderWriter.
          *
          * <strong>Note</strong>: this signal is emitted whenever the edit button is toggled, i.e.
          * whenever the writer visibility changes. In particular, it is emitted programmatically by
          * the TReaderWriter every time it shows/hides the writer part.
          * If you want to intercept user clicks, then look for the editButtonClicked signal.
          *
          * @see editButtonClicked
          */
        void editButtonToggled(bool toggled);

        /** \brief The signal indicates that the Apply button has been clicked on the writer widget.
          *
          * This signal is emitted <strong>after</strong> the execute() method has been called on the
          * QTangoComProxyWriter.
          */
        void applyClicked();

        /** \brief This signal is emitted when the user clicks on the edit button.
          *
          */
        void editButtonClicked();


  public slots:
	/** \brief enables read only mode for the reader writer.
	 *
	 * @param ro true when mouse enters the widget, nothing happens: the writer is disabled.
	 * @param ro false (default) the writer is enabled and the writer widget appears if you
	 *           click on the push button.
	 * 
	 * <br/>Note that in both cases the writer part is not destroyed/(re)created and remains
	 * retrievable via the writer() method.
	 */
	void setReadOnly(bool ro) { d_readOnly = ro; }
	
	/** \brief enables zoom on click when the TReaderWriter is not managed by the layout system.
	 *
	 * Enables zooming on the TReaderWriter when the user clicks on it to edit the write value.
	 * For this feature to work, the widget must not be managed by a layout.
	 * On the click event, the widget is resized of a percentage given by zoomPercentage(),
	 * and overlapping widgets, if any, are hidden. The designer must take care that
	 * the zoomed widget fits the window size, acting on the zoom factor, with setZoomPercentage()
	 * or moving the reader writer around its container or increasing the window size.
	 * If the reader writer is already configured, setZoomEnabled() calls unsetSource and then
	 * setSource() again to permit a new configuration of the widget, if WriterType is set to
	 * <em>Auto</em>.
	 * When the widget is zoomed, all widgets intersecting the reader and the writer parts 
	 * are temporarily hidden, and then show again when treaderWriter is unzoomed.
	 *
	 * @see setZoomPercentage()
	 * @see zoomPercentage()
	 * @see zoomEnabled()
	 */
	void setZoomEnabled(bool en);
	
	/** \brief sets the zoom percentage of the reader writer when the user clicks on the reader.
	 * 
	 * The default value is 130. Change it to fit the zoomed reader writer into your window.
	 */
	void setZoomPercentage(double p) { d_zoomPercentage = p; }
	
// 	void setZoomOnHoverEnabled(bool en) { d_zoomOnHoverEnabled = en; }
	
	/** \brief zooms the widget. Stacks itself over overlapping sibling widgets.
	 *
	 * @param z true zooms 
	 * @param z false unzooms.
	 */
	void zoom(bool z);
	
	/** \brief reimplemented from QTangoComProxyWriter to set the writer part target.
	 *
	 * To guarantee a default behaviour, if setTargets is not called, the target is set equal to the source.
	 * <h3>Observations</h3>
	 * <p>
	 * setSource() configures the writer part with the type of widget suitable for the data type of the source
	 * if writerType is set to Auto. setTargets does not modify the type of widget created by the source configuration.
	 * So, if you want the writer part to write to a tango entity different from the source, specify a 
	 * specific writerType, keeping in mind that having a reader connected to a tango point different from the one of
	 * the writer is misleading.
	 * </p>
	 * <p>
	 * setTargets sets or changes the target on the writer, producing the <em>reconfiguration</em> of the writer 
	 * itself, if necessary, but, as said above, <em>not changing the writer widget type</em>.
	 * If you first call setSource(), the source is set to the reader and the same point is assigned to the target.
	 * A subsequent call to setTargets takes the writer and changes the targets.
	 * If you first call setTargets, the target is memorized and when setSource() is called, it triggers the 
	 * auto configuration of the TReaderWriter and configures the target when configuration parameters are available,
	 * through the slot configure().
	 * </p>
	 *
	 * <h3>Wildcard processing</h3>
	 * <p>
	 * If you want to refer to the writer widget as the source for the input value to write to the target, just 
	 * put into the target the wildcard <em>"%writer%"</em>. It will be replaced with the writer object name, 
	 * thus obtaining data from the writer part of the TReaderWriter. <br/>
	 * For instance, <em>setTargets("a/b/c->GetData(0,%writer%)")</em>, <em>setTargets("a/b/c/attribute(%writer%)")</em>.
	 * In the first case, the command <em>GetData</em> will receive 0 as first argin and the value taken from
	 * the writer as second argin.
	 * </p>
	 */
	virtual void setTargets(QString target);
	
	/** \brief Returns the TReaderWriter's writer targets, if initialized with setTargets.
	 *
	 * Returns the TReaderWriter's writer targets, if initialized with setTargets.
	 * If targets are automatically configured via the TLabel::setSource method, then the 
	 * returned string is empty.
	 * This was introduced to correctly manage custom targets in Qt designer properties.
	 */
        QString targets();
	
	/** \brief unsets the targets specified with setTargets.
	 *
	 * Clears the targets previously set with setTargets.
	 * If setSource was called right after clearTargets, then the writer would have as targets 
	 * the point specified with setSource, unless setTargets is called again.
	 */
	void clearTargets();
	
	void setDesignerMode(bool en) { QTangoComProxyReader::setDesignerMode(en); QTangoComProxyWriter::setDesignerMode(en); }
	
  protected slots:
	/** \brief executes the target of the TReaderWriter
	 *
	 * This method is invoked on the click of the apply button.
	 */
	virtual void execute();
	
  protected:
	virtual void enterEvent(QEvent *e);
	virtual void leaveEvent(QEvent *e);
	virtual void hideEvent(QHideEvent *e);
	virtual bool event(QEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *);
  
  private:
	TReaderPushButton *d_pbEdit;
	QString d_objName;
	QString d_writerTargets;
	QTimer 	*d_writerAutoHideTimer;
	double 	d_writerSingleStep;
	double d_zoomPercentage;

	TDoubleSpinBox 	*d_dsb;
	TSpinBox 		*d_sb;
	TLineEdit		*d_le;
	TComboBox		*d_comboBox;
	QWidget 		*d_writerWidget;
	QPushButton 	*d_pbApply;
	TNumeric 		*d_tNumeric;
	QSize d_savedSize;
	
	WriterType d_writerType;
	WriterPosition d_writerPosition;
	
	QString d_applyButtonStyleSheet, d_writerStyleSheet;
	
	bool d_readOnly;
	bool d_zoomEnabled;
    bool d_zoomOnHoverEnabled;
	bool d_inZoom;
	int  d_randomId;
	QList <QWidget *> d_widgetsHiddenWhenZooming;
	
    private slots: 
	void showWriter();
	void configure(const TangoConfigurationParameters *);
	void editToggled(bool = false);
	
	/* shows edit button, only if not in read only mode */
	void showEditButton();
	void repositionAll();
	void disable() { setDisabled(true); }
	
	QString manipTarget(const QString& target);
	
	/* takes the objectName(), prepends TRW_WRITER and appends d_randomId, which is 
	 * generated in the constructor and is a random number aimed at guaranteeing the
	 * unicity of the writer object name.
	 */
	QString writerObjectName();
	
	/* sets the target on the communication handle. The setTargets public method above,
	 * just sets d_writerTargets and immediately calls this method. This is introduced 
	 * to guarantee the expected behaviour independently of the order in which 
	 * setSource() and setTargets() are called.
	 */
	void d_setTargets(QString);

private:
    TReaderWriterPrivate *d_ptr;
};

#endif



