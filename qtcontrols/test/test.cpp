#include <QDialog>
#include <QApplication>
#include <QHBoxLayout>
#include <QDateTime>
#include <QVBoxLayout>
#include <enumeric.h>
#include <eapplynumeric.h>
#include <eapplybutton.h>
#include <eapplicationbutton.h>
#include <qpushbutton.h>
#include "eflag.h"
#include "elabel.h"
#include "eled.h"
#include "egauge.h"
#include "eplot.h"
#include <test.h>
#include <QVector>
#include <QDoubleSpinBox>
#include <QTimer>
#include <QColor>
#include <QtDebug>
#include <qwt_legend_item.h>
#include "elettracolors.h"

#include <iostream>
#include <sys/time.h>

using namespace std;

MyDialog::MyDialog(QWidget *parent, int arg) : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->setSpacing(0);
    setMinimumSize(QSize(400, 0) );
    appbutt = new EApplicationButton(this);
    appbutt->setText(QString("prova"));
    appbutt->setCmdLine(QString(""));
    vbox->addWidget(appbutt);
    refreshcnt = 0;     
    // ep = new EPlot(this, true); 
    // ep->setMinimumHeight(200);
    // ep->setYScaleFactor(0.01);
//    // ep->enableManualMode(true);
	// ep->setYScaleFactor(0.01);
	// ep->SetCurvePen(QPen(Qt::black), 0 );
	// ep->setMinimumSize(QSize(400, 50));
  //  vbox->addWidget(ep);
    ep2 = new EPlot(this);
    //ep->setFocusPolicy(Qt::StrongFocus);
    QwtLegend* leg= NULL; // = ep->GetLegend();
  //  leg->setMinimumSize(QSize(40,20) );
    if(leg)
    {
    	leg->setItemMode(QwtLegend::CheckableItem);
	leg->setMinimumSize(QSize(400,300) );
    }
//    connect(ep, SIGNAL(legendChecked(QwtPlotItem *, bool)), this, SLOT(showCurve(QwtPlotItem *, bool)));
    ep2->setMinimumHeight(200);
	ep2->setYScaleFactor(0.01);
//	ep2->enableManualMode(true);
    vbox->addWidget(ep2);
    
    // ep3 = new EPlot(this); 
    // ep3->setMinimumHeight(200);
  // vbox->addWidget( ep3);
  
    // ep4 = new EPlot(this);
    // ep4->setMinimumHeight(210);
    // vbox->addWidget( ep4);
    
	// ep->setScrollingEnabled(true);
	// ep->setTitle("Vettore, scrolling abilitato, senza specificare data e ora");
	ep2->setScrollingEnabled(false);
	ep2->setTitle("Vettore, scrolling disabilitato");
///    	ep2->enableManualMode(true);
	// ep3->setTitle("Vettore, scrolling abilitato, con dieci sec. in piu`.");
	// ep3->setScrollingEnabled(true);
	// ep4->setTitle("Valore singolo, senza data e ora.");

    an = new EApplyNumeric(this, 4, 2);
/*  n2 = new ENumeric(this, 4, 0);
    an = new EApplyNumeric(this, 4, 2);
    b = new EApplyButton(this);
    but = new QPushButton(this);*/
   f = new EFlag(this);
    l = new ELed(this);
/*
    c1 = new ECircularGauge(this);
    c1->setColorMode(EAbstractGauge::GRADIENT);
    c1->setExternalScale(false);
*/


/*  c2 = new ECircularGauge(this);
    c2->setColorMode(EAbstractGauge::COLORBAR);
    c2->setScaleEnabled(QVariant(false));
    
    c3 = new ECircularGauge(this);
    c3->setColorMode(EAbstractGauge::SINGLECOLOR);*/
    g1 = new ELinearGauge(this);
    g1->setOrientation(Qt::Horizontal);
    g1->setValue(0);
    g1->setColorMode(EAbstractGauge::GRADIENT);

    g2 = new ELinearGauge(this);
    g2->setOrientation(Qt::Horizontal);
    g2->setValue(0);
    g2->setColorMode(EAbstractGauge::COLORBAR);
//  g2->setDisabled(QVariant(true));

    g3 = new ELinearGauge(this);
    g3->setOrientation(Qt::Horizontal);
    g3->setValue(0);
    g3->setColorMode(EAbstractGauge::SINGLECOLOR);

/*  g4 = new ELinearGauge(this);
    g4->setOrientation(Qt::Horizontal);
    g4->setNumericValue(0);
    g4->setColorMode(EAbstractGauge::SINGLECOLOR);
    g4->setScaleEnabled(QVariant(false));*/

    sb = new QDoubleSpinBox(this);
    sb->setMinimum(-15);
    sb->setMaximum(15);

    /*connect(sb, SIGNAL(valueChanged(double)), c1, SLOT(setNumericValue(double)));
    connect(sb, SIGNAL(valueChanged(double)), c2, SLOT(setNumericValue(double)));
    connect(sb, SIGNAL(valueChanged(double)), c3, SLOT(setNumericValue(double)));*/
//  connect(sb, SIGNAL(valueChanged(double)), g4, SLOT(setNumericValue(double)));

/*  qDebug("setMinValue");
    g->setMinValue(-100);
    qDebug("setLowError");
    g->setLowError(-99.9);
    qDebug("setLowWarning");
    g->setLowWarning(-99.8);
    qDebug("setHighWarning");
    g->setHighWarning(10);
    qDebug("setHighError");
    g->setHighError(20);
    qDebug("setMaxValue");
    g->setMaxValue(50);*/

//  for (int i = 0; i < 10000; i++)
//	    n->setDecDigits(4);
//  an->setButtonText("PIPPO");
/////////////////////////////////////////////////////////    vbox->addWidget(an,1);
/*  vbox->addWidget(n2,1);
    vbox->addWidget(b,1);
    vbox->addWidget(an,1); */
    vbox->addWidget(f,1);
    vbox->addWidget(l,1);
  /*  vbox->addWidget(but,1);*/
//////////////////////////////////////////////////////////    vbox->addWidget(c1,1);
/////////////////////////////////////////////////////////    vbox->addWidget(g1,1);
/////////////////////////////////////////////////////////    vbox->addWidget(g2,1);
///////////////////////////////////////////////////////////    vbox->addWidget(g3,1);
    /*vbox->addWidget(g4,1);*/
///////////////////////////////////////////////////////////   vbox->addWidget(sb,1);
/*  vbox->addWidget(c1,1);
    vbox->addWidget(c2,1);
    vbox->addWidget(c3,1);*/

  l->setColor(Qt::red);

    f->setBooleanDisplay(1, "pippo", "ciccio", QColor("cyan"), QColor("yellow"));
    f->setBooleanDisplay(4, "pippo", "ciccio", QColor("cyan"), QColor("yellow"));
    
    for(int i = 0; i < 9; i++)
    {
    	f->setEnumDisplay(i, 0, "Valore zero", QColor("white") );
    
    	f->setEnumDisplay(i, 1, "Valore uno", QColor("red") );
    
   	f->setEnumDisplay(i, 2, "Valore due", QColor("cyan") );
    
   	f->setEnumDisplay(i, 3, "Valore tre", QColor("yellow") );

   	f->setEnumDisplay(i, 4, "Valore quattro", QColor("green") );

   	f->setEnumDisplay(i, 5, "Valore cinque", QColor("blue") );

   	f->setEnumDisplay(i, 6, "Valore sei", QColor("magenta") );

   	f->setEnumDisplay(i, 7, "Valore sette", QColor(240, 20, 22) );

	
    }
//////////////////////////////////////////////////////////////    vbox->addWidget(f);
    QList<unsigned int> m, m1, m3, m2, m4, m5, m6, m7, m8, m9;

    
    /* Example of usage with two nested lists */
    cout << "Setting list mask..." << endl;
    m1 << 15;
    m1 << 16;
    
    m << 3;
    m << 4;
    
    m2 << 2;
    m2 << 1;
    m2 << 3;
    
    m3 << 7;
    m3 << 8;
    m3 << 9;

    m4 << 0; m4 << 1;
    m5 << 0; m5 << 1; m5 << 3; m5 << 2;

    m6 << 0; m6 << 2;

    m7 << 31; m7 << 30;

    m8 << 5; m8 << 6;

    m9 << 7; m9 << 8;
    
    QList<QList <unsigned int> > ll;
    
    ll.push_back(m);
    ll.push_back(m1); 
    ll.push_back(m2);
    ll.push_back(m3);
    ll.push_back(m4);
    ll.push_back(m5); 
    ll.push_back(m6);
    ll.push_back(m7);
    ll.push_back(m8);
    ll.push_back(m9); 
    
	    //f->setDisplayMask(ll);

	    /* TEST vecchio stile con lista di unsigned */
	  /*
	    
	    QList<unsigned int >  uintlist;
	    uintlist << 2; uintlist << 0; uintlist << 1;
	    f->setDisplayMask(uintlist);
	    cout << "done" << endl;
	    */

	    /* Test vecchio stile con lista di QVariant */
	    QList<QVariant> d;
	    d << QVariant(true); 
	    d << QVariant(true); 
	    d << QVariant(true); 
	    d << QVariant(false);
	    d << QVariant(false);
	    d << QVariant(false);
	    d << QVariant(true); 
	    d << QVariant(true); 
	    d << QVariant(true); 
	    
	    f->setValue(d);
		
	   // f->setValue(arg);

	QList<QVariant> lista;
	for (int i = 0; i < 1; i++)
		lista << QVariant(QString("####"));
	//f->setValue(lista);

	    /* Test the EPlot */
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout() ), this, SLOT(updateEPlot() ) );
	//	timer->setSingleShot(true);
		timer->start(1000);
	}

	void MyDialog::showCurve(QwtPlotItem *item, bool on)
	{
		/////////
		return;
		item->setVisible(on);
		QWidget *w = ep->GetLegend()->find(item);
		if (w && w->inherits("QwtLegendItem"))
			((QwtLegendItem *)w)->setChecked(on);
		ep->replot();
	}

	void MyDialog::updateEPlot()
	{
		refreshcnt++;
		QVector<double> data;
		int i = 0, j = 0;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		
			data.clear();
			for(j = 0; j < 20; j++)
				data.push_back( (double) rand()/(float)RAND_MAX * 10 );
			// ep->insertData(data);
			tv.tv_sec += 10;	
			// ep->SetCurveName("Prima curva", 0);
			// ep->SetCurveName("Seconda curva", 1);
			// ep->SetCurveName("Terza curva", 2);
			// ep->SetCurveName("Quarta curva", 3);
			// ep->SetCurveName("Quinta curva", 4);
		ep2->insertData(data);
			// ep3->insertData(data, &tv);
			/* s ha solo un double */
			// ep4->insertData(data[0]);
			i++;		
		//	qApp->processEvents();
		

			if(refreshcnt %5 == 0)
			{
				qDebug() << "refreshcnt: " << refreshcnt;
				l->setColor(Qt::black);
				QList<QVariant> d;
				d << QVariant(true);
				d << QVariant(true);
				d << QVariant(true);
				d << QVariant(false);
				d << QVariant(false);
				d << QVariant(false);
				QVariant intero;
				QList<QVariant> interi1;
				interi1 << 1 << 2 << 3 << 5;
				intero.setValue(interi1);
			f->setValue(intero);

		}
		else
		{
			qDebug() << "ora cambio EFlag";
			qDebug() << "refreshcnt: " << refreshcnt;
			l->setColor(Qt::cyan);
			QList<QVariant> d;
	   	 d << QVariant(true);
	 	   d << QVariant(false);
	  	  d << QVariant(false);
		    d << QVariant(true);
		    d << QVariant(false);
		    d << QVariant(true);
		    d << QVariant(false);
		    d << QVariant(true);
		    d << QVariant(true);
			
			QVariant intero;
			QList<QVariant> interi1;
			interi1 << 5 << 4 << 7 << 2;
			intero.setValue(interi1);

		    f->setValue(intero);
		}
}

void MyDialog::changeColor()
{ 
	static int i = 0;
	if ((i++%2) == 1)
		l->setColor(Qt::red); 
	else
		l->setColor(Qt::green); 
};

int main(int argc, char **argv)
{
	int arg;

    QApplication a(argc, argv); 

    a.setPalette(EPalette("eltra"));

    if(argc == 2)
	    arg = atoi(argv[1]);
    else
	    arg = 0;
   cout << "calling with arg: " << arg << endl; 
    	MyDialog dialog(0, arg);
    		    
   
    //a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    
    dialog.resize(400,1000);
    dialog.show();

    return a.exec();  
}   

