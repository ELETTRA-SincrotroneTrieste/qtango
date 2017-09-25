/***************************************************************************
 *   Copyright (C) 2007 by Giacomo Strangolino   *
 *   giacomo.strangolino@elettra.trieste.it   *
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


#include <QApplication>
#include "commander.h"
#include <tutil.h>
#include <X11/Xlib.h>
#include <QX11Info>
#include <QSplashScreen>

/* uncomment to set application color */
#include <elettracolors.h>

int main( int argc, char ** argv ) {
	
	QApplication a( argc, argv );
	
	Commander mw;
	QString title(a.arguments().first());
	title += " A test/device/N command tester";
	mw.setWindowTitle(title);
	mw.show();
	return a.exec();
}

