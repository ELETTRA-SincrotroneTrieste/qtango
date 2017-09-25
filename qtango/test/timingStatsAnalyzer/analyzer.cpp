/***************************************************************************
 *   Copyright (C) 2009 by Giacomo S.   *
 *   delleceste@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QtGui>
#include <QtDebug>
#include "analyzer.h"
#include "data_loader.h"

#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <stdlib.h>
#include "analyzer_plot.h"
#include <qtangocore_read_stats.h>
#include <macros.h>
#include <QSplitter>
#include "text_browser.h"

Analyzer::Analyzer()
{
      createActions();
      createMenus();
      createStatusBar();
      QSplitter *splitter = new QSplitter(this);
      setCentralWidget(splitter);
      QWidget *mainWidget = new QWidget(this);
      d_plot = new AnalyzerPlot(QwtText("Analysis"), mainWidget);
      QGridLayout *lo = new QGridLayout(mainWidget);
      lo->addWidget(d_plot, 0, 0, 10, 10);
      
      TextBrowser *tb = new TextBrowser(this);
//       lo->addWidget(tb, 0, 7, 10, 3);
      connect(d_plot, SIGNAL(elementInfo(const QString&)), tb, SLOT(setHtml(const QString&)));
      connect(d_plot, SIGNAL(entered()), tb, SLOT(initHtml()));

      
      QComboBox *cbcurves = new QComboBox(mainWidget);
      cbcurves->setObjectName("curves");
      QComboBox *cbStyles = new QComboBox(mainWidget);
      cbStyles->setObjectName("styles");
      cbStyles->insertItems(0, QStringList() << "Dots" << "Line" << "Sticks");
      QDoubleSpinBox *sb = new QDoubleSpinBox(mainWidget);
      sb->setMinimum(0.2);
      sb->setMaximum(10);
      sb->setValue(2.0);
      sb->setObjectName("spinPenWidth");
      QComboBox *cbSymbol = new QComboBox(mainWidget);
      cbSymbol->insertItems(0, QStringList() << "No Symbol" << "Ellipse" << "Rect" << "Diamond" << "Triangle" << "Cross" << "Star");
      cbSymbol->setObjectName("symbols");
      QLabel *label = new QLabel(mainWidget);
      label->setText("Pen Width/Curve Style:");
      QPushButton *pbApplyStyle = new QPushButton(mainWidget);
      pbApplyStyle->setText("Apply");
      QCheckBox *cbEvent = new QCheckBox(mainWidget);
      cbEvent->setObjectName("cbEvent");
      cbEvent->setText("Show events");
      
      QCheckBox *cbSelectAll = new QCheckBox(mainWidget);
      cbSelectAll->setChecked(true);
      cbSelectAll->setText("Show/Hide All");
     
      
      QCheckBox *cbHideLegend = new QCheckBox(mainWidget);
      cbHideLegend->setChecked(true);
      cbHideLegend->setText("Legend Visible");
      connect(cbHideLegend, SIGNAL(toggled(bool)), d_plot, SLOT(showLegend(bool)));
      connect(cbHideLegend, SIGNAL(toggled(bool)), cbSelectAll, SLOT(setChecked(bool)));
      connect(cbSelectAll, SIGNAL(toggled(bool)), d_plot, SLOT(showAll(bool)));
      
      lo->addWidget(cbHideLegend, 10, 0, 1, 1);
      lo->addWidget(cbSelectAll, 10, 1, 1, 1);
      lo->addWidget(cbEvent, 10, 2, 1, 1);
      lo->addWidget(label, 10, 3, 1, 1);
      lo->addWidget(sb, 10, 4, 1, 1);
      lo->addWidget(cbcurves, 10, 5, 1, 2);
      lo->addWidget(cbStyles, 10, 7, 1, 1);
      lo->addWidget(cbSymbol, 10, 8, 1, 1);
      lo->addWidget(pbApplyStyle, 10, 9, 1, 1);
      
      /* populate splitter with the two widgets */
      splitter->addWidget(mainWidget);
      splitter->addWidget(tb);
      
      cbEvent->setChecked(false);
      
      connect(cbEvent, SIGNAL(toggled(bool)), d_plot, SLOT(showEvents(bool)));
      connect(pbApplyStyle, SIGNAL(clicked()), this, SLOT(changeCurveStyle()));
      if(qApp->arguments().size() > 1)
      {
	      pok("Processing specified command line file \"%s\"", qstoc(qApp->arguments().at(1)));
	      loadFile(qApp->arguments().at(1));
	      processData();
      }
      setAttribute(Qt::WA_DeleteOnClose);
}

void Analyzer::about()
{
      QMessageBox::about(this, tr("About Stats Analyzer"),
            tr("The <b>Stats Analyzer</b> opens a statistics file generated by any QTangoCore application launched"
	      "\nwith the option \"--dump-stats\""));
}

void Analyzer::createActions()
{
      openAct = new QAction(QIcon(":/fileopen.xpm"), tr("&Open..."), this);
      openAct->setShortcut(tr("Ctrl+O"));
      openAct->setStatusTip(tr("Open an existing file"));
      connect(openAct, SIGNAL(triggered()), this, SLOT(open()));


      exitAct = new QAction(tr("E&xit"), this);
      exitAct->setShortcut(tr("Ctrl+Q"));
      exitAct->setStatusTip(tr("Exit the application"));
      connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

      aboutAct = new QAction(tr("&About"), this);
      aboutAct->setStatusTip(tr("Show the application's About box"));
      connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

      aboutQtAct = new QAction(tr("About &Qt"), this);
      aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
      connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void Analyzer::open()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty())
	{
		loadFile(fileName);
		processData();
	}
}

void Analyzer::createMenus()
{
      fileMenu = menuBar()->addMenu(tr("&File"));
      fileMenu->addAction(openAct);
      fileMenu->addSeparator();
      fileMenu->addAction(exitAct);

      menuBar()->addSeparator();

      helpMenu = menuBar()->addMenu(tr("&Help"));
      helpMenu->addAction(aboutAct);
      helpMenu->addAction(aboutQtAct);
}

void Analyzer::createToolBars()
{
      fileToolBar = addToolBar(tr("File"));
      fileToolBar->addAction(openAct);
}

void Analyzer::createStatusBar()
{
      statusBar()->showMessage(tr("Ready"));
}

void Analyzer::loadFile(const QString &fileName)
{
      DataLoader loader(fileName);
//       loader.printResults();
      d_readData = loader.data();
      statusBar()->showMessage(tr("File loaded"), 2000);
      d_plot->setTitle(loader.header());
}

QString Analyzer::strippedName(const QString &fullFileName)
{
      return QFileInfo(fullFileName).fileName();
}

Analyzer::~Analyzer()
{
	
}

void Analyzer::processData()
{
	d_plot->removeCurves();
	d_plot->createCurves(d_readData);
	QCheckBox *cb = findChild<QCheckBox *>("cbEvent");
	if(cb)
		d_plot->showEvents(cb->isChecked());
	QComboBox *cbcurves = findChild<QComboBox *>("curves");
	cbcurves->clear();
	QList <QwtPlotCurve *>crvs = d_plot->curves();
	QStringList curveNames;
	foreach(QwtPlotCurve *c, crvs)
	      curveNames << c->title().text();
	cbcurves->insertItems(0, curveNames);
}
		
void Analyzer::changeCurveStyle()
{
	QwtPlotCurve::CurveStyle sty = QwtPlotCurve::Dots;
	QwtSymbol::Style ssty = QwtSymbol::NoSymbol;
	QComboBox *cbcrvs = findChild<QComboBox *>("curves");
	QComboBox *cbsty = findChild<QComboBox *>("styles");
	QComboBox *cbsym = findChild<QComboBox *>("symbols");
	QDoubleSpinBox *dsb = findChild<QDoubleSpinBox *>("spinPenWidth");
	if(cbcrvs && cbsty && dsb && cbsym)
	{
		switch(cbsty->currentIndex())
		{
			case 0:
				sty = QwtPlotCurve::Dots;
				break;
			case 1:
				sty = QwtPlotCurve::Lines;
				break;
			case 2:
				sty = QwtPlotCurve::Sticks;
				break;
		}
		
		switch(cbsym->currentIndex())
		{
			case 0:
				ssty = QwtSymbol::NoSymbol;
				break;
			case 1:
				ssty = QwtSymbol::Ellipse;
				break;
			case 2:
				ssty = QwtSymbol::Rect;
				break;
			case 3:
				ssty = QwtSymbol::Diamond;
				break;	
			case 4:
				ssty = QwtSymbol::Triangle;
				break;
			case 5:
				ssty = QwtSymbol::Cross;
				break;
			case 6:
				ssty = QwtSymbol::Star1;
				break;		
		}
		d_plot->setCurveStyle(cbcrvs->currentText(), sty, ssty, dsb->value());
		
	}
}
	
		
		
		
		
		
		

