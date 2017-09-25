/***************************************************************************
*   Copyright (C) 2008 by  Giacomo Strangolino   			   *
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

#ifndef QTANGO_DEVICE_THREAD_FACTORY_H
#define QTANGO_DEVICE_THREAD_FACTORY_H

#include <stdlib.h>
#include <QObject>
#include <QMap>
#include <QString>
#include <devicethread.h>

class DeviceThreadFactory : public QObject
{
     Q_OBJECT

     Q_PROPERTY(QStringList threads READ threads)
     Q_PROPERTY(int threadCount READ threadCount)

     public:
	  static DeviceThreadFactory *instance();
	  
	  DeviceThread *getDeviceThread(const QString &devName);
	    	
	/* adds a thread to the device thread map */
	void registerThread(QString s, DeviceThread *th);
	
	/* removes a thread from the device thread map. 
	 * Remember to call this when the device is disconnected and the thread deleted.
	 * This is used in the DeviceThread destructor to remove the DeviceThread pointer
	 * from the device  thread map.
	 */
	void deregisterThread(DeviceThread *);
	
	QList<DeviceThread *>threadList() { return d_threads.values(); }

        QStringList threads() const { return d_threads.keys(); }

        int threadCount() const { return d_threads.size(); }

        /** \brief sets the stack size for the threads that will be created.
          *
          * If a stackSize is specified and greater than zero, then the new threads
          * created by the factory will have the specified stack size.
          * Note that the thread will fail to start if the stack size is outside the
          * limits imposed by the operating system.
          * If zero is passed, then the stack size is decided by the operating system.
          *
          * @see Qt QThread::setStackSize method.
          *
          * @see stackSize
          *
          */
        void setStackSize(unsigned int stackSize) { m_stackSize = stackSize; }

        /** \brief returns the maximum stack size for the thread (if set with setStackSize());
         *         otherwise returns zero.
         *
         * @see setStackSize
         */
        unsigned int stackSize() const { return m_stackSize; }
	  
     private:
	  DeviceThreadFactory();
	  ~DeviceThreadFactory();
	  
	  static DeviceThreadFactory *_threadFactory;
	
	  
	  /* this contains the name of the device proxy as first element, and the pointer to 
	 *  the associated DeviceThread as second element. The map is filled by 
	 * DeviceThread* getDeviceThread(const QString &devName)', that creates a 
	 * new DeviceThread if not existing for the devName specified as argument.
	 * subscribe() and unsubscribe() normally call findDeviceThread(), thus populating and 
	 * de-populating the map.
	 */
	QMap<QString, DeviceThread*>	d_threads;

        unsigned int m_stackSize;
};

#endif
