#ifndef _ELED_H
#define _ELED_H

#include <QWidget>

class ELedPrivate;

/**
 * \brief A color led
 *
 * ELed is a widget tailored to show true/false values.
 * It has a color, a width and height that might be different from the container widget's width
 * or height, depending on the <em>scaleContents</em> property being false or true respectively.
 * Moreover, a led can be rectangular or elliptical (circular), according to the <em>rectangular</em> property.
 * The color of the led can be shaded by means of a linear or rectangular gradient, and the start and
 * stop values of the gradient can be fine tuned via the <em>gradientStop</em> and <em>gradientStart</em> properties.
 * An integer between 0 and 255 can be specified to define the <em>alphaChannel</em> property, to add 
 * transparency to your led.
 *
 * scaleContents property and gradientStop and gradientStart properties were introduced in QTango <em>4.x</em>.
 */
class ELed : public QWidget
{
Q_OBJECT
	
public:
  Q_PROPERTY(bool rectangular READ rectangular WRITE setRectangular DESIGNABLE true)
  Q_PROPERTY(bool gradientEnabled READ gradientEnabled WRITE setGradientEnabled DESIGNABLE true)   
  Q_PROPERTY(int ledWidth READ ledWidth WRITE setLedWidth DESIGNABLE true)
  Q_PROPERTY(int ledHeight READ ledHeight WRITE setLedHeight DESIGNABLE true)
  Q_PROPERTY(int angle READ angle WRITE setAngle DESIGNABLE true)   
  Q_PROPERTY(int alphaChannel READ alphaChannel WRITE setAlphaChannel DESIGNABLE true) 
  Q_PROPERTY(bool linearGradient READ linearGradient WRITE setLinearGradient DESIGNABLE true)
  Q_PROPERTY(bool scaleContents READ scaleContents WRITE setScaleContents)
  Q_PROPERTY(double gradientStop READ gradientStop WRITE setGradientStop)
  Q_PROPERTY(double gradientStart READ gradientStart WRITE setGradientStart)
  
	ELed(QWidget*);
    ~ELed(){}

	void setColor(const QColor &, bool = true);
    QColor color(){ return ledColor; }
	
	int angle() { return d_angleDeg; }
	void setAngle(int angle);
	
	void setLedWidth(int);
	void setLedHeight(int);
	
	int ledWidth() { return d_width; }
	int ledHeight() { return d_height; }
	
	bool rectangular() { return d_rectangular; }
	void setRectangular(bool re);
	
	void setAlphaChannel(int a);
	int alphaChannel() { return d_alphaChannel; }
	
	bool gradientEnabled() { return d_gradientEnabled; }
	void setGradientEnabled(bool grad);
	
	bool linearGradient() { return d_linearGradient; }
	void setLinearGradient(bool l);
	
	/** \brief fine tunes the gradient end point.
	 *
	 * If the gradient is enabled, then it is possible to fine tune the gradient stop point.
	 * The accepted values are those in the range 0..1.
	 * Gradients have one start and one stop point.
	 *
	 * @see setGradientStop
	 */
	double gradientStop() { return d_gradientStop; }
	
	/** \brief changes the gradient stop point 
	 *
	 * \sa gradientStop
	 */
	void setGradientStop(double val);
	
	/** \brief fine tunes the gradient end point.
	 *
	 * If the gradient is enabled, then it is possible to fine tune the gradient stop point.
	 * The accepted values are those in the range 0..1.
	 * Gradients have one start and one stop point.
	 *
	 * @see setGradientStop
	 */
	double gradientStart() { return d_gradientStart; }
	
	/** \brief changes the gradient start point 
	 *
	 * \sa gradientStart
	 */
	void setGradientStart(double val);
	
	/** \brief returns the scaleContents property.
	 *
	 * If the scaleContents property is true, then the led will fit the widget width
	 * ignoring the properties ledWidth and  ledHeight.
	 *
	 * @see setScaleContents
	 */
        bool scaleContents() { return d_scaleContents; }
	
	/** \brief changes the property scaleContents
	 *
	 * @see scaleContents
	 */
	void setScaleContents(bool s);
	
protected:
        virtual void paintEvent(QPaintEvent*);
	
	virtual QSize sizeHint() const;
	
	virtual QSize minimumSizeHint() const;
	
private:
	QColor	ledColor;
	int d_width, d_height, d_angleDeg, d_alphaChannel;
	bool d_rectangular;
	bool d_gradientEnabled;
	bool d_linearGradient;
	bool d_scaleContents;
	double d_gradientStart;
	double d_gradientStop;

    ELedPrivate *d_ptr;
};

#endif
