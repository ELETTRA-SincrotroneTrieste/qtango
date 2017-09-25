#ifndef TSIMPLE_LABEL_H
#define TSIMPLE_LABEL_H

#include <com_proxy_reader.h>
#include <qtango_wcommon.h>
#include <esimplelabel.h>

class TSimpleLabelPrivate;

/** \brief a simple label that does nothing, except being right aligned and display the tango source 
           point in drop and paste events.
 *
 * Use your simple label when you want to have a simple QLabel right aligned. Since it supports 
 * qtango <em>drag and drop</em>, you can drag and drop from another qtango widget to have its
 * source displayed here.
 *
 * Since version 4.2.0, display unit is appended if configured in the tango database.
 *
 * <h3>Functionality</h3>
 *
 * <p>
 * If displayFullSource option is true, then the full tango source is displayed. This means
 * that if setSource is called with "test/tangotest/1/double_scalar", then
 * "test/tangotest/1/double_scalar" is displayed.
 * If displayFullSource is false, then only "double_scalar" would be shown, i.e. the
 * tango device name is removed.
 *
 * <h4>Notes</h4>
 *
 * <p> If the <strong>displayUnit</strong> is set on the tango database for the source
 * specified (if it is an attribute), then the display unit is <strong>appended</strong>
 * after the source name between square brackets.
 * </p>
 *
 * <p>
 * If the property <strong>label</strong> is configured into the tango database for the
 * source provided (in case it is an attribute), then that label is shown on the
 * TSimpleLabel, if <strong>displayFullSource</strong> is set to false.
 * If displayFullSource is true, then the <strong>label</strong> attribute property is
 * ignored.
 * </p>
 *
 * <h3>Resizable fonts </h3>
 * <p>
 * This widget supports scalable fonts. To enable/disable this feature, just call setFontScaleEnabled()
 * with a true or false parameter respectively.
 * <h4>Observation</h4>
 *<p>
 * Font scaling with <strong>frameShape</strong> different from <em>NoFrame</em> and 
 * text alignment different from <em>centered alignment</em> is not fully supported and
 * you might experience strange rescaling behaviour when your widget is in a layout.
 * </p>
 * </p>
 */
class TSimpleLabel : public ESimpleLabel, public QTangoComProxyReader, public QTangoWidgetCommon
{
Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE false)
Q_PROPERTY(bool displayFullSource READ displayFullSource WRITE setDisplayFullSource  DESIGNABLE true)
/* scalable fonts */
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)

  
  Q_OBJECT
  public:
	
	TSimpleLabel(QWidget *parent);
	
	/** \brief sets the text with the source name provided 
	 *
	 * @param source the tango source point that will be displayed by the simple label.
	 */
        void setSource(QString source);
	
	/** \brief returns the source displayed.
	 *
	 * @return the name of the source set with setSource
	 */
        QString source() { return QTangoComProxyReader::source(); }
	
	/** \brief configures the simple label to display either the full source name or just the command/attribute name.
	 *
	 * @param en true the full source is displayed (e.g. test/device/1/double_scalar ).
	 * @param en false only the attribute name is displayed (or the command name, e.g. double_scalar)
	 */
        void setDisplayFullSource(bool en);
	
	/** \brief returns the displayFullSource property
	 *
	 * @see setDisplayFullSource
	 */
        bool displayFullSource() { return d_displayFullSource; }

protected slots:
  /** \brief Provided for compatibility, does nothing
   *
   * This does not do anything.
   */
  void refresh(const TVariant &);
	
	
  private:
	bool d_displayFullSource;
	QString d_source;

        QString m_getShortSource(const QString& src);

        TSimpleLabelPrivate *d_ptr;
};

#endif
