#include "device_thread_factory.h"
#include <dbus/DeviceThreadFactoryAdaptor.h>
#include <macros.h>
#include <QtDebug>
#include <tapplication.h>

DeviceThreadFactory * DeviceThreadFactory::_threadFactory = NULL;

DeviceThreadFactory::DeviceThreadFactory() : QObject()
{
        m_stackSize = 256000; /* should be good for DeviceThreads */
	d_threads.clear();
        TApplication* tApp = qobject_cast<TApplication *>(qApp);
        if(tApp)
        {
            DeviceThreadFactoryAdaptor *deviceThreadFactoryAdaptor = new DeviceThreadFactoryAdaptor(this);
            deviceThreadFactoryAdaptor->setObjectName("DeviceThreadFactoryAdaptor");
            QDBusConnection connection = QDBusConnection::sessionBus();
            QString dbusPath = "/DeviceThreadFactory";
            if(!connection.registerObject(dbusPath, this))
            {
                perr("DeviceThreadFactory: failed to register object with path \"%s\":", qstoc(dbusPath));
                perr("DeviceThreadFactory: %s: %s", qstoc(connection.lastError().name()), qstoc(connection.lastError().message()));
            }
        }
}

DeviceThreadFactory::~DeviceThreadFactory()
{
	if(d_threads.size() != 0)
		pwarn("device thread factory destroyed with threads still registered");
	_threadFactory = NULL;
}

DeviceThreadFactory *DeviceThreadFactory::instance()
{
	if(_threadFactory == NULL)
		_threadFactory = new DeviceThreadFactory();
	return _threadFactory;
}

DeviceThread *DeviceThreadFactory::getDeviceThread(const QString &devName)
{
	if (!d_threads.contains(devName))
	{
		return NULL;
	}
	return d_threads[devName];
}

void DeviceThreadFactory::deregisterThread(DeviceThread *th)
{
	if(d_threads.values().contains(th))
		d_threads.remove(d_threads.key(th));
}

void DeviceThreadFactory::registerThread(QString devName, DeviceThread *th)
{
	d_threads.insert(devName, th);
}

