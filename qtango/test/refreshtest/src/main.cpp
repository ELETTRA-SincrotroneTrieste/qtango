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


#include <TApplication>
#include "refreshtest.h"
#include <X11/Xlib.h>
#include <QX11Info>
#include <tutil.h>

/* uncomment to set application color */
#include <elettracolors.h>

int main( int argc, char ** argv ) {

    TApplication a( argc, argv );
    /* uncomment to set application color */
    a.setPalette(EPalette("fermi"));
    TUtil::instance()->setLoggingTarget(argv[0]);
    TPlotLight mw;
    mw.setWindowTitle("Plot Light Refresh Test");
    mw.show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) mw.winId();
    XSetCommand(disp, root_win, argv, argc);
	
	printf("Plot Light refresh test.\n\e[1;32mOptions\e[0m:\n");
	printf("a. \e[0;32mRandom vector generation mode\e[0m:\n");
	printf("\t%s NUMBER_OF_POINTS  REFRESH_INTERVAL\n", argv[0]);
	printf("\te.g. %s 1000 100 generates a random vector of 1000 points every 100 milliseconds\n", argv[0]);
	printf("b. \e[0;32mRealtime Plot mode\e[0m:\n");
	printf("\t%s \"test/realtime_device/1->GetData(...)\"\n", argv[0]);
	printf("\twhere GetData(...) is a realtime command, such as for example:\n");
	printf("\t%s \"inj/diagnostics/cm_inj.01->GetCharge(0,1000)\"\n\n", argv[0]);
	
    return a.exec();
}
