#include "device_factory.h"
#include <macros.h>
#include <QtDebug>
#include <QThread> /* only for qDebug currentThread */

DeviceFactory * DeviceFactory::_devFactory = NULL;

DeviceFactory::DeviceFactory(QObject *parent) : QObject(NULL)
{
    Q_UNUSED(parent);
}

DeviceFactory::~DeviceFactory()
{
	qprintf("\e[0;31mDeviceFactory destruction\e[0m\n");
	cleanFactory();
	_devFactory = NULL;
}

void DeviceFactory::cleanFactory()
{
	d_deviceMutex.lock();
	QList<DeviceProxy *> devices = d_devMap.values();
	foreach(DeviceProxy *dev, devices)
	{
		try{
// 			pinfo("closing connection with tango device \"%s\"", dev->name().c_str());
			delete dev;
		}
		catch(Tango::DevFailed &e)
		{
			Tango::Except::print_exception(e);
		}
	}
	d_devMap.clear();
	d_deviceMutex.unlock();
}
			
DeviceFactory *DeviceFactory::instance(QObject *parent)
{
	if(parent != NULL && _devFactory == NULL)
		_devFactory = new DeviceFactory(parent);
	else if(parent == NULL && !_devFactory)
	{
		perr("DeviceFactory must have a parent NOT NULL when first instantiated.");
	  	_exit(EXIT_FAILURE);
	}
	return _devFactory;
}

/* returns a new device proxy if a DeviceProxy with the given name did not exist
 * and the proxy creation was successful. If the DeviceProxy was previously created,
 * returns the already existing instance.
 * If the DeviceProxy did not exist and the creation failed returns NULL.
 * Acquires the lock to guarantee thread safety.
 */
DeviceProxy* DeviceFactory::getDeviceProxy(const QString &devname)
{
	d_deviceMutex.lock();
	qDebug() << QThread::currentThread() << " DeviceFactory::getDeviceProxy() LOCK";
	DeviceProxy *proxy = NULL;
	proxy = d_getDeviceProxy(devname);
	qDebug() << QThread::currentThread() << " DeviceFactory::getDeviceProxy() UNLOCK";
	d_deviceMutex.unlock();
	return proxy;
}

/* does not hold the lock. For internal use */
DeviceProxy* DeviceFactory::d_getDeviceProxy(const QString &devname)
{
  DeviceProxy *proxy = NULL;
  if (d_devMap.contains(devname))
	proxy = d_devMap[devname];
  return proxy;
}

DeviceProxy* DeviceFactory::create(const QString &devname)
{
	qDebug() << QThread::currentThread() << " DeviceFactory::create() LOCK";
	d_deviceMutex.lock(); /* NOTE: lock */
	DeviceProxy *proxy = NULL;
	string dname = devname.toStdString();
	proxy = d_getDeviceProxy(devname); /* d_getDeviceProxy does not acquire locks */
	if(proxy == NULL)
	{
		try /* try to create a new DeviceProxy */
		{
			qDebug() << QThread::currentThread() << ":" << "DeviceFactory::create() - creo nuovo";
			proxy = new DeviceProxy(dname);
			d_devMap.insert(devname, proxy);
			pok("successfully created a new device [\"%s\"]", qstoc(devname));
		}
		catch(DevFailed &e) /* also catches connection failed */
		{
			perr("cannot create a new device proxy with the name \"%s\" [DeviceFactory::create()]", qstoc(devname));
                        delete proxy;
			proxy = NULL;
			qDebug() << QThread::currentThread() << " DeviceFactory::create() UNLOCK (DevFailed)";
			d_deviceMutex.unlock();  /* NOTE: unlock! */
			/* do not TUtil::log() here: since we re-throw, let the last catcher log all */
			Except::re_throw_exception(e, 
				"Error finding a device proxy already configured and creating a new one",
				"Could not create the device proxy with the name provided:\n\"" + dname + "\"",
				"DeviceFactory::create()", Tango::ERR);
		}
	}
	else /* a device with that name already existing: return that one */
	{
		pinfo("DeviceFactory: create: device proxy already existing with name \"%s\"", qstoc(devname));
	}
	d_deviceMutex.unlock();  /* NOTE: unlock! */
	qDebug() << QThread::currentThread() << " DeviceFactory::create() UNLOCK - exit -";
	return proxy;
}


