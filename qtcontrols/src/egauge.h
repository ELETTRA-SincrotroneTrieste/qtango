#ifndef _EGAUGE_H
#define _EGAUGE_H

#include <math.h>
#include <QWidget>

#define LINEAR_MAJOR_TICKS 6
#define CIRCULAR_MAJOR_TICKS 11
#define NUM_MINOR_TICKS 5

#define DEPRECATED(func) func __attribute__ ((deprecated))

class QRect;
class QLineF;
class EAbstractGaugePrivate;
class ELinearGaugePrivate;
class ECircularGaugePrivate;

/**
 * \brief an abstract class used to implement widgets with a color scale
 *
 * you can configure the graphical behaviour by setting warnings, errors and colors
 */
class EAbstractGauge : public QWidget
{
Q_OBJECT

Q_ENUMS(ColorMode)

Q_PROPERTY(double minValue READ minValue WRITE setMinValue)
Q_PROPERTY(double lowError READ lowError WRITE setLowError)
Q_PROPERTY(double lowWarning READ lowWarning WRITE setLowWarning)
Q_PROPERTY(double highWarning READ highWarning WRITE setHighWarning)
Q_PROPERTY(double highError READ highError WRITE setHighError)
Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)
Q_PROPERTY(double value READ value WRITE setValue)
Q_PROPERTY(double reference READ reference WRITE setReference)
Q_PROPERTY(bool scaleEnabled READ isScaleEnabled WRITE setScaleEnabled)
Q_PROPERTY(bool referenceEnabled READ isReferenceEnabled WRITE setReferenceEnabled)
Q_PROPERTY(ColorMode colorMode READ colorMode WRITE setColorMode)
Q_PROPERTY(bool externalScale READ externalScale WRITE setExternalScale)
Q_PROPERTY(bool logarithmicScale READ logarithmicScale WRITE setLogarithmicScale)
Q_PROPERTY(QString valueFormat READ valueFormat WRITE setValueFormat)
Q_PROPERTY(int numMajorTicks READ numMajorTicks WRITE setNumMajorTicks)
Q_PROPERTY(int numMinorTicks READ numMinorTicks WRITE setNumMinorTicks)

public:

	/**
	 * Color Mode to be used to paint the widget
	 */
	enum ColorMode 
	{
		GRADIENT, /**< show a bar with a gradient of colors */
		COLORBAR, /**< show a bar with different colors */
		SINGLECOLOR /**< show a bar whose color represents the state */
	};

	EAbstractGauge(QWidget * = NULL);
	~EAbstractGauge(){};
	
	bool logarithmicScale() { return d_logarithmicScale; }
	void setLogarithmicScale(bool l);
	
	void setLowWarning(double w);
	double lowWarning(){ return d_naturalLowWarn; };

	void setHighWarning(double w);
	double highWarning(){ return d_naturalHighWarn; };

	void setLowError(double e);
	double lowError(){ return d_naturalLowErr; };

	void setHighError(double e);
	double highError(){ return d_naturalHighErr; };

	void setMinValue(double v);
	double minValue(){ return d_naturalMinV; };

	void setMaxValue(double v);
	double maxValue(){ return d_naturalMaxV; };
	
	double value(){ return d_naturalValue; };
	
	void setReference(double r);
	double reference(){ return d_naturalReference; };
	
	void setScaleEnabled(bool b);
	bool isScaleEnabled(){ return m_scaleEnabled; };
	
	void setReferenceEnabled(bool b);
	bool isReferenceEnabled(){ return m_referenceEnabled; };
	
	void setColorMode(ColorMode m);
	ColorMode colorMode(){ return m_colorMode; };

	DEPRECATED(void setNormalColor(QColor c));
	QColor normalColor(){ return m_normalColor; }
	
	DEPRECATED(void setWarningColor(QColor c));
	QColor warningColor(){ return m_warningColor; }
	
	DEPRECATED(void setErrorColor(QColor c));
	QColor errorColor(){ return m_errorColor; }

	void setExternalScale(bool b);
	bool externalScale(){ return m_externalScale; };

	void setNumMajorTicks(int t);
	int numMajorTicks(){ return m_numMajorTicks; }
	
	void setNumMinorTicks(int t);
	int numMinorTicks(){ return m_numMinorTicks; }
	
	void setValueFormat(QString s){ m_valueFormat = s; configure(); update(); };
	QString valueFormat(){ return m_valueFormat; };
	
public slots:
	void setValue(double);
	void setValue(int);

protected:
    /* these might store the values in natural or as logarithmic */
	double m_minValue, m_maxValue;
	double m_lowWarning, m_highWarning;
	double m_lowError, m_highError;
	double m_value,  m_reference;
	/* the following always store the natural values, never transformed into logarithmic */
	double d_naturalMinV, d_naturalMaxV, d_naturalLowWarn, d_naturalHighWarn,
	 d_naturalLowErr, d_naturalHighErr, d_naturalValue, d_naturalReference;
	
	ColorMode	m_colorMode;
	bool 		m_referenceEnabled;
	bool 		m_scaleEnabled;
	int		m_numMajorTicks, m_numMinorTicks;
	QStringList	labels;
	int		longestLabelIndex;
	bool		m_externalScale;
	
	QVector<qreal>	v_p;
	QVector<QColor>	v_c;
	QColor		m_normalColor;
	QColor		m_warningColor;
	QColor		m_errorColor;
	QString 	m_valueFormat;
	
	virtual void configure();
	
  private:
	bool d_logarithmicScale;
	double logarithm(double);

    EAbstractGaugePrivate *d_ptr;
};

/**
 * \brief a thermometer like widget
 */
class ELinearGauge : public EAbstractGauge
{
Q_OBJECT

Q_ENUMS(FillMode)
	
Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode)

public:
	/**
	 * How the value is displayed on the bar
	 */
	enum FillMode
	{
		ALL, /** fill the whole bar, moving marker */
		FROM_MIN, /** bar filled from min value to value */
		FROM_ZERO, /** bar filled from zero to value */
		FROM_REF /** bar filled from reference to value */
	};
	
	ELinearGauge(QWidget * = NULL, Qt::Orientation=Qt::Vertical);
	~ELinearGauge(){};

	void setOrientation(Qt::Orientation o){ m_orientation = o; configure(); update(); };
	Qt::Orientation orientation(){ return m_orientation; };

	void setFillMode(FillMode m){ m_fillMode = m; configure(); update(); };
	FillMode fillMode(){ return m_fillMode; };

protected:
	void paintEvent(QPaintEvent *);
	void drawColorBar(QPainter *);
	void drawScale(QPainter *);
	void drawMarker(QPainter *, bool);
	void drawLabels(QPainter *);
	virtual void configure();

private:
	Qt::Orientation	m_orientation;
	FillMode m_fillMode;
	int scalePos, labelsPos, totalSize;
	int barSize, scaleSize, labelsSize;

    ELinearGaugePrivate *d_ptr;
};

/**
 * \brief a tachometer like widget
 * 
 * A tachometer like widget.
 */
class ECircularGauge : public EAbstractGauge
{
Q_OBJECT

Q_PROPERTY(bool valueDisplayed READ isValueDisplayed WRITE setValueDisplayed)
Q_PROPERTY(QString label READ label WRITE setLabel)

public:
	ECircularGauge(QWidget * = NULL);
	~ECircularGauge(){};

	void setValueDisplayed(bool b){ m_valueDisplayed = b; update(); };
	bool isValueDisplayed(){ return m_valueDisplayed; };

	void setLabel(QString l){ m_label = l; update(); };
	QString label(){ return m_label; };
	
protected:
	void paintEvent(QPaintEvent *);
	void drawColorBar(QPainter *);
	void drawScale(QPainter *);
	void drawNeedle(QPainter *);
	void drawLabels(QPainter *);
	void drawValue(QPainter *);
	virtual void configure();
	
private:
	double m_startAngle, m_stopAngle, m_arcLength;
	int m_innerRadius, m_outerRadius, m_labelRadius;
	QString m_label;
	bool m_valueDisplayed;

    ECircularGaugePrivate *d_ptr;
};
#endif
