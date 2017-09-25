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


#ifndef ANALYZER_H
#define ANALYZER_H

#include <QMainWindow>
#include "read_data.h"

class QAction;
class QMenu;
class QTextEdit;
class AnalyzerPlot;

class Analyzer:public QMainWindow
{
      Q_OBJECT

public:
      Analyzer();
      ~Analyzer();

private slots:
      void open();
      void about();
      void processData();
      void changeCurveStyle();

private:
      void createActions();
      void createMenus();
      void createToolBars();
      void createStatusBar();

      void loadFile(const QString &fileName);

      QString strippedName(const QString &fullFileName);

      QTextEdit *textEdit;
      QString curFile;

      QMenu *fileMenu;
      QMenu *editMenu;
      QMenu *helpMenu;
      QToolBar *fileToolBar;
      QToolBar *editToolBar;
      QAction *openAct;
      QAction *exitAct;
      QAction *aboutAct;
      QAction *aboutQtAct;
      
      QList <ReadData> d_readData;
      
      AnalyzerPlot *d_plot;
};

#endif
