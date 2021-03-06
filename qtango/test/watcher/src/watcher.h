/***************************************************************************
 *   Copyright (C) 2007 by Claudio Scafuri, Giacomo Strangolino   *
 *   claudio@hyo   *
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


#ifndef watcher_H
#define watcher_H

#include "ui_watcher.h"

class TangoConfigurationParameters;

class Watcher: public QWidget
{
    Q_OBJECT

public:
    Watcher(QWidget * =NULL);

private slots:
    void dialConfigured(const TangoConfigurationParameters *cp);
    void displayInternalVariables();

private:
    Ui::Watcher ui;

    int intVar;
    double doubleVar;
    short shortVar;
    QString stringVar;

};


#endif
