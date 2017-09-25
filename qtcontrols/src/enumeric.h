#ifndef ENUMERIC_H
#define ENUMERIC_H

#include <math.h>
#include <QFrame>
#include <QEvent>
#include "float_delegate.h"
#include "fontscalingwidget.h"

class QLabel;
class QButtonGroup;
class QLineEdit;
class QPushButton;
class QGridLayout;
class QAbstractButton;
class ENumericPrivate;

// class MouseMoveEventFilter : public QObject
// {
//   Q_OBJECT
// 
//   public:
// 	MouseMoveEventFilter(QObject *parent) : QObject(parent) {}
// 	
//   protected:
// 	bool eventFilter(QObject *o, QEvent *e);
// }

/**
 * \brief a widget to set a value
 *
 * this widget is used to set a value, whenever the displayed 
 * value changes the signal valueChanged(double) is emitted
 */
class ENumeric : public QFrame, public FloatDelegate
{
Q_OBJECT
Q_PROPERTY(int integerDigits READ intDigits WRITE setIntDigits)
Q_PROPERTY(int decimalDigits READ decDigits WRITE setDecDigits)
Q_PROPERTY(double value READ value WRITE setValue)
Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
Q_PROPERTY(double minimum READ minimum WRITE setMinimum)

/* scalable fonts */
Q_PROPERTY(bool digitsFontScaleEnabled READ digitsFontScaleEnabled WRITE setDigitsFontScaleEnabled)

public:
    /**
     * constructor
     * @param intDigits number of integer digits
     * @param decDigits number of decimal digits
     */
    ENumeric(QWidget *parent, int intDigits = 2, int decDigits = 1);
    ~ENumeric(){}
    
    /**
     * sets value displayed by the widget
     * @param v     the value to be displayed
     */
    void setValue(double v);

    /**
      * same as setValue(), but does not emit valueChanged() signal.
      * Used by QTango TNumeric widget when automatically configured, to
      * initialize a value on the apply numeric widget without causing a
      * signal to be emitted.
      */
    void silentSetValue(double v);

    /**
     * returns the value displayed by the widget
     * @return the value displayed
     */
    double value() const { return data*pow(10.0, -decDig); }

    /**
     * sets the maximum value of the widget
     * @param v     max val
     */
    void setMaximum(double v);
    /**
     * returns the maximum value of the widget
     * @return max val
     */
    double maximum() const { return d_maxAsDouble; }

    /**
     * sets the minimum value of the widget
     * @param v 	min val
     */
    void setMinimum(double v);
    /**
     * returns the minimum value of the widget
     * @return min val
     */
    double minimum() const { return d_minAsDouble; }

    /**
     * sets number of integer digits to be displayed
     * @param i	number of digits
     */
    void setIntDigits(int i);
    /**
     * returns number of integer digits displayed
     * @return i	number of digits
     */
    int intDigits() const { return intDig; }

    /**
     * sets number of deciam digits to be displayed
     * @param d	number of digits
     */
    void setDecDigits(int d);
   /**
     * returns number of decimal digits displayed
     * @return d	number of digits
     */
    int decDigits() const { return decDig; }
	
	bool digitsFontScaleEnabled() { return d_fontScaleEnabled; }
	
	void setDigitsFontScaleEnabled(bool en);


public slots:
    virtual void setEnabled(bool);
    virtual void setDisabled(bool);
    
signals:
    void valueChanged(double);

  protected:
	virtual void showEvent(QShowEvent *);
	
	virtual QSize sizeHint() const;
	
	virtual QSize minimumSizeHint() const;
	
private:
    int intDig;
    int decDig;
    int digits;
    long long data;
    long long minVal;
    long long maxVal;
    
    double d_minAsDouble, d_maxAsDouble;

    QButtonGroup *bup;
    QButtonGroup *bdown;
    QGridLayout *box;
    QLabel *signLabel, *pointLabel;
    QLineEdit *text;
    
    QVector<QLabel*> labels;

    void init();
    void clearContainers();
    void showData();
    //void resizeEvent(QResizeEvent*);
    //void draw();
    void formatDigit(QPushButton*, QLabel*, QPushButton*);
    
    void reconstructGeometry();
    
    int idUpVuoto, idDownVuoto;
    int idUpPoint, idDownPoint;

private slots:
    void dataInput();
    void upData(QAbstractButton*);
    void downData(QAbstractButton*);
    
    void valueUpdated();
	

private:
    void mouseDoubleClickEvent(QMouseEvent*);
	bool d_fontScaleEnabled;

protected slots:
   /* ENumeric uses ESimpleLabel::calculateFontPointSizeF method to calculate the point size of 
	* one of its numeric labels. Then applies the same point size to all the labels it is made up of.
	*/
    void resizeEvent(QResizeEvent *);
    
private:
    ENumericPrivate *d_ptr;
    
};
#endif // EDIGIT_H
