#include <QDialog>

class ENumeric;
class EApplyNumeric;
class EApplyButton;
class EFlag;
class ELabel;
class ELed;
class ELinearGauge;
class ECircularGauge;
class QPushButton;
class QDoubleSpinBox;
class EApplicationButton;
class EPlot;
class QTimer;
class QwtPlotItem;

class MyDialog : public QWidget
{
Q_OBJECT
public:
	MyDialog(QWidget *parent ,int arg);
	void EPlotTest(EPlot* p1, EPlot* p2, EPlot* p3, EPlot* p4);
	EApplyNumeric *an;
	ENumeric *n, *n2;
	EApplyButton *b;
	EFlag	*f;
	ELed	*l;
	QPushButton *but;
	ELinearGauge *g1, *g2, *g3, *g4;
	ECircularGauge *c1, *c2, *c3;
	QDoubleSpinBox *sb;
	EApplicationButton *appbutt;
	QTimer *timer;
	EPlot* ep, *ep2, *ep3, *ep4;
	int refreshcnt;

public slots:
	void changeColor();
	void updateEPlot();
	void showCurve(QwtPlotItem *item, bool on);

};
