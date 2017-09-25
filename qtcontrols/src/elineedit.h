#ifndef ELINEEDIT_H
#define ELINEEDIT_H

#include <QLineEdit>
#include "fontscalingwidget.h"

class ELineEditPrivate;

class ELineEdit : public QLineEdit, public FontScalingWidget
{
  Q_ENUMS(ScaleMode)
  Q_PROPERTY(ScaleMode fontScaleMode READ fontScaleMode WRITE setFontScaleMode DESIGNABLE true)
  Q_PROPERTY(double fontScaleFactor READ fontScaleFactor WRITE setFontScaleFactor DESIGNABLE false)
  Q_OBJECT
  public:
	
  /** \brief the font scaling modality
   *
   * Please refer to the ESimpleLabel::ScaleMode documentation.
   */
  enum ScaleMode { None, Height, WidthAndHeight, HeightManaged, WidthAndHeightManaged };
 
  ELineEdit(QWidget *parent);
  
  /** \brief rescales the font of the line edit while editing the text.
   */
  bool rescaleFontOnTextChanged() { return d_rescaleFontOnTextChanged; }
  
  /** \brief rescales the font of the line edit while editing the text.
   */
  void setRescaleFontOnTextChanged(bool en) { d_rescaleFontOnTextChanged = en; }
  
  virtual void setText(const QString &);
  
  QString text() const { return QLineEdit::text(); }
  
  void setFontScaleMode(ScaleMode m) { FontScalingWidget::setScaleMode((int) m); }
  ScaleMode fontScaleMode() { return (ScaleMode) d_scaleMode; }
  
  private slots:
	void rescaleFont(const QString& newText);
  
  protected:
	virtual bool event(QEvent *e);
	
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
	
	QSize calculateTextSpace();
	
  private:
    bool d_rescaleFontOnTextChanged;

    ELineEditPrivate *d_ptr;
};



#endif
