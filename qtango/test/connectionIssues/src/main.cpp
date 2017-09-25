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


#include <TUniqueApplication>
#include "connectionIssues.h"
#include <tutil.h>
#include <QSplashScreen>

/* uncomment to set application color */
#include <elettracolors.h>

#define CVSVERSION "$Name  $"

int main( int argc, char ** argv ) {
	
	TUniqueApplication a( argc, argv );
    /* uncomment to set application color */
	a.setPalette(EPalette("booster"));
	a.setApplicationName("ConnectionIssuesApp");
	a.setApplicationVersion(CVSVERSION);
	TUtil::instance()->setLoggingTarget(argv[0]);
	
	printf("cmd line: \n");
	qslisttoc(a.arguments());

	ConnectionIssues mw;
	mw.show();
	
	return a.exec();
}
