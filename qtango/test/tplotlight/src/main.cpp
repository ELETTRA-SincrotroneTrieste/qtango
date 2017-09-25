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


#include <QApplication>
#include "tplotlight.h"
#include <X11/Xlib.h>
#include <QX11Info>
#include <tutil.h>

/* uncomment to set application color */
#include <elettracolors.h>

int main( int argc, char ** argv ) {

    QApplication a( argc, argv );
    /* uncomment to set application color */
    a.setPalette(EPalette("booster"));
    TUtil::instance()->setLoggingTarget(argv[0]);
    TPlotLight mw;
    if(argc > 1) 
	    mw.setWindowTitle(argv[1]);

    TUtil::replaceWildcards(mw.children(), a.arguments());
    mw.show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) mw.winId();
    XSetCommand(disp, root_win, QApplication::argv(), QApplication::argc());
	
    return a.exec();
}
