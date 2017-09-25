/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	   *
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

/* $Id: helper_application.h,v 1.4 2013-04-30 07:34:03 giacomo Exp $ */


#ifndef QTANGO_HELPER_APPLICATION
#define QTANGO_HELPER_APPLICATION

#include <QString>
#include <proxy_interface.h>

class EApplicationLauncher;
class QTangoHelperApplicationPrivate;

class QTangoHelperApplication
{
	public:
		QTangoHelperApplication(QObject *parent);
		
		void launchApplication();
		
		/** \brief enables/disables helper application
		 *
		 * @param en if true, enables the helper application.
		 * <strong>Note</strong><p> remember to call setHelperApplication() with the 
		 * application name.</p>
		 * @see setHelperApplication()
		 */
                void setHelperApplicationEnabled(bool e);
		
		/** \brief helper application enabled for the object
		 *
		 * @return true if the helper application is enabled, false otherwise
		 */
		bool helperApplicationEnabled(){ return d_enableHelperApplication; }
	
	
		/** \brief the helper application executable name (+ path)
		 *
		 * @return the path of the helper application
		 */
		QString helperApplication();
		
		/** \brief enable/disable helper application
		 *
		 * @param s the executable name of the helper application you want to associate to the
		 *          QTango object.
		 */
		void setHelperApplication(QString a){ d_helperApplication = a; }
			
	protected:
		bool d_enableHelperApplication;
		EApplicationLauncher *	d_app;
		QString 		d_helperApplication;
		QObject *d_parentObject;

            private:
                QString m_getHelperApplication();

      private:
              QTangoHelperApplicationPrivate* d_ptr;
};


#endif

