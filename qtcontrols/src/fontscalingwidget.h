#ifndef FONTSCALING_WIDGET_H
#define FONTSCALING_WIDGET_H

#include <QWidget>

class FontScalingWidget
{ 
  public:
	
	FontScalingWidget(QWidget *parent);
	
	virtual ~FontScalingWidget() {};
	
	/** \brief the number of pixels of the width to be taken into account to define a border
	 *         on the left and on the right of the widget.
	 *
	 * @param pixValue number of pixels of the lateral border width (left + right) 
	 *        Setting the value to 2 means 1 pixel on the right and one on the left.
	 * The default value is 4: a right border of 2 pixels and the left border of 2 pixels too.
	 *
	 * @see setBotTopBorderWidth, which has the same semantic.
	 */
	void setLateralBorderWidth(double pixValue);
	
	/** \brief returns the width of the borders to take into account when scaling a font 
	 *         according to the width
	 *
	 * @see setBotTopBorderWidth
	 * @see setLateralBorderWidth
	 */
	double lateralBorderWidth() { return d_lateralBorderWidth; }
	
	/** \brief in Managed font scale modality, this returns the font scale factor for the widget.
	 *
	 * This applies to font scaling widgets that have the scaleMode property set to 
	 * <em>HeightManaged</em> or <em>HeightAndWidthManaged</em>, in conjunction with an
	 * EWidget as container. This is currently <em>unsupported</em> in the QTango designer
	 * plugins, but external applications can make use of this property, i.e. the <em>Mango</em>
	 * graphical design tool.<br/>
	 * <em>Note</em>: although this factor might be calculated on the basis of the current point size of the
	 * scaling widget and the font point size of the QApplication font, this scale factor, if
	 * defined like this, will be independent of the point size continuously changed by the
	 * font scaling widget when it rescales its fonts.
	 *
	 * @see setFontScaleFactor
	 */
	double fontScaleFactor() { return d_fontScaleFactor; }
	
	/** \brief in Managed font scale modality, sets the font scale factor for the widget.
	 *
	 * This applies to font scaling widgets that have the scaleMode property set to 
	 * <em>HeightManaged</em> or <em>HeightAndWidthManaged</em>, in conjunction with an
	 * EWidget as container. This is currently <em>unsupported</em> in the QTango designer
	 * plugins, but external applications can make use of this property, i.e. the <em>Mango</em>
	 * graphical design tool.
	 *
	 * @param factor a double number representing how the widget scales its fonts with respect to the
	 *        other scaling widgets. A widget with a scale factor of 2.0 will have its font with a 
	 *        point size twice the value of another widget with 1.0 font scale factor.
	 *
	 * @see fontScaleFactor
	 *
	 */
	void setFontScaleFactor(double factor) { d_fontScaleFactor = factor; }
	
	bool fontScaleEnabled() const { return d_scaleMode != 0; }
	
	void setBotTopBorderWidth(double pixValue);
	double botTopBorderWidth() { return d_botTopBorderWidth; }
	
	void saveTextSpace(const QSize& size) { d_savedTextSpace = size; }
	QSize savedTextSpace() { return d_savedTextSpace; }
	
	QFont& savedFont() { return d_savedFont; }

	QWidget *widget() { return d_widget; }
	
	/** \brief calculates the point size returning a floating point value 
	 *
	 * @param text the string that must fit the size provided.
	 * @param size the size inside which the text having the point size returned fits.
	 *
	 * This is used internally by rescaleFont(). In that case, the size is normally provided
	 * by the subclass implementation of calculateTextSpace(). For an example see ESimpleLabel
	 * or ESpinBoxes. Anyway, this method is public and can be used by any FontScalingWidget or
	 * any widget having FontScalingWidgets inside (such as ENumeric) just to calculate the 
	 * point size that the font of the widget <em>should</em> have to fit text and size.
	 * ENumeric uses this method to calculate the point size of one of its numeric labels inside
	 * the resizeEvent(). Then applies the same point size to all the labels it is made up of.
	 * @see ENumeric::resizeEvent()
	 */
	double calculateFontPointSizeF(const QString& text, const QSize & size);
	
	/** \brief calculate the space reserved for the displayed string.
	 * 
	 * Depending to style options, the space reserved for text labels in QLineEdits, QPushButtons and so
	 * on is not always equal to the whole widget size.
	 * So it is necessary in subclasses to implement this method that calculates the size, according to
	 * QStyleOption and depending on the style() of a widget.
	 * Examples can be found in EPushButton, ESpinBox and EDoubleSpinBox, ELineEdit.
	 */
	virtual QSize calculateTextSpace() = 0;
	
	virtual QString text() const = 0;
	
	int scaleMode() { return d_scaleMode; }
	/* passed by subclasses as enum from 0 (None) to 4 */
	void setScaleMode(int m);
	
  protected:
	
	void rescaleFont(const QString& text, const QSize &);
	
	/* let this accessible by subclasses to improve performance */
	QSize d_savedTextSpace;
	
	int d_scaleMode;
	
  private:
	double d_lateralBorderWidth;
	double d_botTopBorderWidth;
	double d_fontScaleFactor;
	
	QWidget *d_widget;
	
	/* the font saved before setFontScaleEnabled() was called with a true parameter,
	 * to be able to restore it when setFontScaleEnabled() is called again with false.
	 */
	QFont d_savedFont;
	
	static bool longerThan(const QString& s1, const QString& s2) { return s1.length() > s2.length(); }
};

#endif
