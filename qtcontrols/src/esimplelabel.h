#ifndef ESIMPLELABEL_H
#define ESIMPLELABEL_H

#include <QLabel>
#include <QStyleOptionFrame>
#include <QStyle>
#include <QtDebug>

#include "fontscalingwidget.h"

class ESimpleLabelPrivate;

/** \brief A QLabel that provides the font scaling feature.
 *
 * This class is a QLabel that scales its fonts when resized.
 * This is a base font scaling class for all the objects that need to implement the 
 * font scaling features, such as ELabel, QTango TLabel and QTango TSimpleLabel.
 * You might have a look at those classes.
 * If your label is <em>left</em> or <em>right</em> aligned, then consider increasing the
 * lateralBorderWidth if you experience some problems of font scaling: an ESimpleLabel with
 * a thick <em>boxed frame</em> might have its string not perfectly fit the space in some circumstances.
 * <br/>Font scaling <em>labels</em> are tailored for <em>center aligned</em>
 * text only. The ESimpleLabel is constructed with a horizontal centered alignment if scalingMode
 * property is different from ESimpleLabel::None, and a right alignment otherwise.
 * 
 * @see TSimpleLabel
 * @see ELabel
 * @see EFlag
 * @see TLabel
 * @see TTable
 * @see FontScalingWidget
 *
 */
class ESimpleLabel : public QLabel, public FontScalingWidget
{
  /* scalable fonts */
  Q_PROPERTY(bool fontScaleEnabled READ fontScaleEnabled DESIGNABLE false)
  Q_PROPERTY(double botTopBorderWidth READ botTopBorderWidth WRITE setBotTopBorderWidth DESIGNABLE fontScaleEnabled)
  Q_PROPERTY(double lateralBorderWidth READ lateralBorderWidth WRITE setLateralBorderWidth DESIGNABLE fontScaleEnabled)
  Q_PROPERTY(double fontScaleFactor READ fontScaleFactor WRITE setFontScaleFactor  DESIGNABLE false)
  Q_PROPERTY(ScaleMode fontScaleMode READ fontScaleMode WRITE setFontScaleMode)
  
  Q_ENUMS(ScaleMode)
  Q_OBJECT
  public:
	
	/** \brief the font scaling modality
	 *
	 * <ul>
	 * <li><em>None</em>: no font scaling is applied on show and resize events. This was the behaviour before version 4.x;</li>
	 * <li><em>Height</em>: font scaling takes into account the height of the font of the widget. The font size fits the 
	 *     height of the widget, but during the design phase take into account the maximum length of the string represented
	 *     in the label: the font will not be adjusted if the length does not fit. If you do not know a priori the length 
	 *     of the string, consider using the <em>WidthAndHeight</em> mode, which is less precise in the calculation of the
	 *     font point size (to limit the overhead brought by the calculations) but guarantees that the string fits into the
	 *     available space.</li>
	 * <li><em>WidthAndHeight</em>: calculates the point size taking into account the width and the height of the string to
	 *     fit it into the available space.</li>
	 * <li><em>HeightManaged</em> and <em>WidthAndHeightManaged</em>: these are currently unsupported.</li>
     *</ul>
     */	 
	enum ScaleMode { None, Height, WidthAndHeight, HeightManaged, WidthAndHeightManaged };
	
	ESimpleLabel(QWidget *parent);
	
	ESimpleLabel(const QString& text, QWidget *parent);
	
	virtual void setText(const QString&);
	
	QString text() const { return QLabel::text(); }
	
	QSize calculateTextSpace();
	
	void setFontScaleMode(ScaleMode m) { FontScalingWidget::setScaleMode((int) m); }
	ScaleMode fontScaleMode() { return (ScaleMode) FontScalingWidget::scaleMode(); }
	
  protected:
	virtual bool event(QEvent *);
	
	/** \brief provides sizeHint for the widgets inserted into a layout
	 *
	 * The size hint is used by the layout system to resize the widgets according to the 
	 * resize policy. The implementation of size hint returns a size whose width is wide
	 * enough to contain the current text() with the current font with size imposed to 10.
	 * If font scaling is disabled, returns the super class sizeHint.
	 */
	virtual QSize sizeHint() const;
	
	/** \brief returns the minimumSizeHint.
	 *
	 * The implementation returns sizeHint() if font scaling is enabled, super class 
	 * minimumSizeHint() if disabled.
	 */
	virtual QSize minimumSizeHint() const;

private:
    ESimpleLabelPrivate *d_ptr;
	
};


#endif
