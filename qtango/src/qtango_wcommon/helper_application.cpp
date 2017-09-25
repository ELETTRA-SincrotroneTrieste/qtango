/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino, Vincenzo Forchi`	   *
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

/* $Id: helper_application.cpp,v 1.5 2012-01-31 15:05:40 giacomo Exp $ */

#include <action_factory.h>
#include <macros.h>
#include <tutil.h>
#include <communication_handle.h>
#include "helper_application.h"
#include <QWidget>
#include <com_proxy_reader.h>
#include <EApplicationLauncher>

QTangoHelperApplication::QTangoHelperApplication(QObject *parent)
{
    Q_ASSERT(parent != NULL);
    d_parentObject = parent;
    d_enableHelperApplication = false;
    d_app = NULL;
    d_helperApplication = QString();
}

void QTangoHelperApplication::launchApplication()
{
    QString helperApp;
    if(d_enableHelperApplication)
        helperApp =  helperApplication();
    if(helperApp.isEmpty() && d_enableHelperApplication)
	pwarn("You enabled the helper application functionality but perhaps you did not provide the application name");
    else if(!d_enableHelperApplication)
        pwarn("helper application disabled: should you need it, enable the functionality and provide the application name");
    else if(!helperApp.isEmpty() && d_enableHelperApplication)
    {
        if (!d_app)
            d_app = new EApplicationLauncher(helperApplication());
        d_app->Rise();
    }
}

void QTangoHelperApplication::setHelperApplicationEnabled(bool e)
{
    d_enableHelperApplication = e;
}

QString QTangoHelperApplication::helperApplication()
{
    QString helperApplication;
    if(d_helperApplication.isEmpty() && d_enableHelperApplication)
        helperApplication = m_getHelperApplication();
    else if(d_enableHelperApplication)
        helperApplication = d_helperApplication;
    return helperApplication;
}

QString QTangoHelperApplication::m_getHelperApplication()
{
    QString app;
    TAction *action = NULL;
    QTangoCommunicationHandle *handle;
    ProxyInterface *proxyI = dynamic_cast<ProxyInterface *>(d_parentObject);
    if (proxyI && proxyI->qtangoComHandle() && helperApplicationEnabled())
    {
        handle = proxyI->qtangoComHandle();
        if(ActionFactory::actionFactory()->actionAlive(handle->d_action))
            action = handle->d_action;
        if(action)
        {
            app = TUtil::instance()->findHelperApplication(action->deviceName(), action->databaseHost(),
                                                           action->databasePort());
            if (app != "")
            {
                QStringList args = app.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                /* only the executable name: for compatibility reasons, add the device name
             * as first argument.
             */
                if(args.size() == 1)
                    app = app + " " + action->deviceName();
            }
        }
        else
            pwarn("action not alive for object \"%s\", cannot obtain helper application", qstoc(d_parentObject->objectName()));
    }
    return app;
}
