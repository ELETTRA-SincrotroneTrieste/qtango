
%Module qtcontrols 0

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip


class ELed : QWidget {
%TypeHeaderCode
#include "../../src/eled.h"
%End
public:
        ELed(QWidget*);
        ~ELed();

        void setColor(const QColor &, bool = true);
        QColor color();

        int angle();
        void setAngle(int);

        void setLedWidth(int);
        void setLedHeight(int);

        int ledWidth();
        int ledHeight();

        bool rectangular();
        void setRectangular(bool);

        void setAlphaChannel(int);
        int alphaChannel();

        bool gradientEnabled();
        void setGradientEnabled(bool);

        bool linearGradient();
        void setLinearGradient(bool);

        double gradientStop();
        void setGradientStop(double);

        double gradientStart();
        void setGradientStart(double);

        bool scaleContents();
        void setScaleContents(bool);

protected:
        virtual void paintEvent(QPaintEvent*);
        virtual QSize sizeHint() const;
        virtual QSize minimumSizeHint() const;
};

class FontScalingWidget {
%TypeHeaderCode
#include "../../src/fontscalingwidget.h"
%End
public:
    FontScalingWidget(QWidget *);
    virtual ~FontScalingWidget();
    void setLateralBorderWidth(double);
    double lateralBorderWidth();
    double fontScaleFactor();
    void setFontScaleFactor(double);
    bool fontScaleEnabled();
    void setBotTopBorderWidth(double);
    double botTopBorderWidth();
    void saveTextSpace(const QSize&);
    QSize savedTextSpace();
    QFont& savedFont();
    QWidget *widget();
    double calculateFontPointSizeF(const QString&, const QSize &);
    virtual QSize calculateTextSpace() = 0;
    virtual QString text() const = 0;
    int scaleMode();
    void setScaleMode(int);

protected:
    void rescaleFont(const QString&, const QSize &);
};

class ESimpleLabel : QLabel, FontScalingWidget {
%TypeHeaderCode
#include "../../src/esimplelabel.h"
%End
public:
    enum ScaleMode { None, Height, WidthAndHeight, HeightManaged, WidthAndHeightManaged };

    ESimpleLabel(QWidget *);
    ESimpleLabel(const QString&, QWidget *);
    virtual void setText(const QString&);
    QString text() const;
    QSize calculateTextSpace();
    void setFontScaleMode(ScaleMode);
    ScaleMode fontScaleMode();

};

class ELabel : ESimpleLabel {
%TypeHeaderCode
#include "../../src/elabel.h"
%End
public:
        ELabel(QWidget *parent);

        void setValue(QVariant, bool =true);
        QVariant value() const;

        void setBooleanDisplay(QString, QString, QColor, QColor);

        void setEnumDisplay(unsigned int, QString, QColor);

        void setTrueString(QString);
        QString trueString() const;

        void setFalseString(QString);
        QString falseString() const;

        void setTrueColor(QColor);
        QColor trueColor() const;

        void setFalseColor(QColor);
        QColor falseColor() const;

        bool enumDisplayConfigured();
        void clearEnumDisplay();

        void display();
};

class EAbstractGauge : QWidget {
%TypeHeaderCode
#include "../../src/egauge.h"
%End
public:
        enum ColorMode {
          GRADIENT,
          COLORBAR,
          SINGLECOLOR
        };

        EAbstractGauge(QWidget * = NULL);
        ~EAbstractGauge();

        bool logarithmicScale();
        void setLogarithmicScale(bool);

        void setLowWarning(double);
        double lowWarning();

        void setHighWarning(double);
        double highWarning();

        void setLowError(double);
        double lowError();

        void setHighError(double);
        double highError();

        void setMinValue(double);
        double minValue();

        void setMaxValue(double);
        double maxValue();

        double value();

        void setReference(double);
        double reference();

        void setScaleEnabled(bool);
        bool isScaleEnabled();

        void setReferenceEnabled(bool);
        bool isReferenceEnabled();

        void setColorMode(ColorMode);
        ColorMode colorMode();

        QColor normalColor();
        QColor warningColor();
        QColor errorColor();

        void setExternalScale(bool);
        bool externalScale();

        void setNumMajorTicks(int);
        int numMajorTicks();

        void setNumMinorTicks(int);
        int numMinorTicks();

        void setValueFormat(QString);
        QString valueFormat();

public slots:
        void setValue(double);

protected:
        virtual void configure();
};

class ELinearGauge : EAbstractGauge {
%TypeHeaderCode
#include "../../src/egauge.h"
%End
public:
        enum FillMode {
          ALL,
          FROM_MIN,
          FROM_ZERO,
          FROM_REF
        };

        ELinearGauge(QWidget * = NULL, Qt::Orientation =Qt::Vertical);
        ~ELinearGauge();

        void setOrientation(Qt::Orientation);
        Qt::Orientation orientation();

        void setFillMode(FillMode);
        FillMode fillMode();

protected:
        void paintEvent(QPaintEvent *);
        void drawColorBar(QPainter *);
        void drawScale(QPainter *);
        void drawMarker(QPainter *, bool);
        void drawLabels(QPainter *);
        virtual void configure();
};

class ECircularGauge : EAbstractGauge {
%TypeHeaderCode
#include "../../src/egauge.h"
%End
public:
        ECircularGauge(QWidget * =NULL);
        ~ECircularGauge();

        void setValueDisplayed(bool);
        bool isValueDisplayed();

        void setLabel(QString);
        QString label();

protected:
        void paintEvent(QPaintEvent *);
        void drawColorBar(QPainter *);
        void drawScale(QPainter *);
        void drawNeedle(QPainter *);
        void drawLabels(QPainter *);
        void drawValue(QPainter *);
        virtual void configure();
};

class EFlag : QWidget {
%TypeHeaderCode
#include "../../src/eflag.h"
%End
public:
        EFlag(QWidget *parent);

        void setValue(QVariant, bool =true);
        QList<QVariant> readValue() const;

        void setDisplayMask(QString);
        const QString getDisplayMask();

        void initDisplayMask();

        void setNumRows(int);
        int readNumRows() const;

        void setNumColumns(int);
        int readNumColumns() const;

        void setBooleanDisplay(int, QString, QString, QColor, QColor);

        void setEnumDisplay(int, unsigned int, QString, QColor);

        void setTrueStrings(QString);
        QString trueStrings();

        void setFalseStrings(QString);
        QString falseStrings();

        void setTrueColors(QString);
        QString trueColors();

        void setFalseColors(QString);
        QString falseColors();

        void setMouseTracking(bool);

        QList<ELabel*> cells;

protected:
        void arrangeCells();
        void configureCells();
};

class NumberDelegate {
%TypeHeaderCode
#include "../../src/number_delegate.h"
%End
public:
        NumberDelegate();
        virtual ~NumberDelegate();
        int integerDigits();
        int decimalDigits();
        void configureNumber(const double, const double, const QString&);
        bool configureNumber(const QString&);
};

class FloatDelegate : NumberDelegate {
%TypeHeaderCode
#include "../../src/float_delegate.h"
%End
public:
        FloatDelegate();
        virtual ~FloatDelegate();
        virtual void setValue(double) = 0;
        virtual void setMinimum(double) = 0;
        virtual void setMaximum(double) = 0;
        virtual void setIntDigits(int) = 0;
        virtual void setDecDigits(int) = 0;
};

class EApplyNumeric : QWidget, FloatDelegate {
%TypeHeaderCode
#include "../../src/eapplynumeric.h"
%End
public:
        EApplyNumeric(QWidget*, int =2, int =1, Qt::Orientation = Qt::Horizontal);
        ~EApplyNumeric();

        bool applyButtonActive();
        bool digitsFontScaleEnabled();
        void setDigitsFontScaleEnabled(bool);

        void setButtonPosition(Qt::Orientation);
        Qt::Orientation buttonPosition() const;

        void setValue(double);
        double value() const;

        void setMaximum(double);
        double maximum() const;

        void setMinimum(double);
        double minimum() const;

        void setIntDigits(int);
        int intDigits() const;

        void setDecDigits(int);
        int decDigits() const;

        void setButtonText(const QString &);
        QString buttonText() const;

        bool isModified();
        void setApplyButtonActive(bool);

        QFont numericFont() const;
        void setNumericFont(const QFont);

signals:
        void clicked(double);

public slots:
        virtual void applyValue();
        void clearModified();
        virtual void setFont(const QFont &);
        void disable();
};

class EArrow : QWidget {
%TypeHeaderCode
#include "../../src/earrow.h"
%End
public:
    EArrow( QWidget*);
    ~EArrow();

    enum ArrowMode {LEFT, RIGTH, DOUBLE, NONE};
    enum ArrowDirection {VERTICAL, HORIZONTAL, RIGTHBOTTOM, RIGTHTOP};

    void setLineSize(int);
    int getLineSize();

    void setLineColor(QColor);
    QColor getLineColor();

    void setBrushColor(QColor);
    QColor getBrushColor();

    void setArrowSize(int);
    int getArrowSize();

    void setArrowMode(ArrowMode);
    ArrowMode getArrowMode();

    void setArrowDirection(ArrowDirection);
    ArrowDirection getArrowDirection();

protected:
    virtual void paintEvent(QPaintEvent*);
};

class EContextHelp : QPushButton {
%TypeHeaderCode
#include "../../src/econtexthelp.h"
%End
public slots:
    void setWhatIsThis();

public:
    EContextHelp(QWidget*);
    ~EContextHelp();
};
