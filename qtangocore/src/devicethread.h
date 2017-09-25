/***************************************************************************
*   Copyright (C) 2008 by  Vincenzo Forchi`, Giacomo Strangolino	  *
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

#ifndef _DEVICETHREAD_H
#define _DEVICETHREAD_H

#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QEvent>
#include <QMultiMap>
#include <QStringList>
#include <tvariant.h>
#include <taction.h>
#include <action_factory.h>
#include <QQueue>

class DeviceThreadPrivate;

class DeviceThread : public QThread
{
Q_OBJECT
public:
	DeviceThread(const QString &);
	~DeviceThread();

	bool addPoint(PointEventData *);
	bool removePoint(PointEventData *);
	bool exitLoop(ExitLoopEvent e);
	
	void run();
	
    QTimer *timer() { return d_timer; }
	
    QString devName() { return d_devname; }

signals:
	void newAction(QString, TAction*);
	void modifiedPointList();
	void exitLoopThread();
	void configurationAvailable(QString s, TAction* a);

public slots:
	void process();

	protected slots:
		void lastProcessAndExit();
protected:
	
	QString		d_devname;
    DeviceProxy	*d_device;
	
	
	QList<PointEventData*>	  d_toBeAdded;
	QList<PointEventData*>    d_toBeRemoved;
	
	QMutex		d_pointEventMutex;
	QTimer		*d_timer, *d_exitTimer;
	

    DeviceThreadPrivate *d_ptr;
// 	QQueue <AddPointEvent> pendingAddEvents;
};

#endif
