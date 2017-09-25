#ifndef DEVICE_FACTORY_H
#define DEVICE_FACTORY_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QString>
#include <tango.h>

using namespace Tango;

/** \brief A Tango DeviceProxy factory, collecting all devices used in your application using qtangocore.
 *
 * This class, a <em>singleton</em> gathers the collection of Tango DeviceProxies. It provides methods
 * to retrieve already instantiated DeviceProxies and to create not existing ones.
 * DeviceProxy*	getDeviceProxy() returns a DeviceProxy if it is already allocated inside  the factory, or a
 * NULL pointer otherwise.
 * The create() method, returns an already existing device proxy with the name required if existing, otherwise
 * it <em>allocates</em> a new one and <em>adds it to the collection</em>.
 * <h3>Tango DeviceProxy deletion</h3>
 * <p>
 * If the Tango DeviceProxy creation is managed by means of the DeviceFactory, there is no need to
 * <em>delete</em> the device proxy provided by the factory itself. Indeed <em>the factory deletes
 * the device proxies by itself</em> at destruction time.<br/>
 * <strong>Not only should you not delete device proxies</strong>, but you are <strong>strongly
 * discouraged</strong> to do this, since the DeviceProxy you obtained from the factory <strong>might
 * be shared</strong> among other core elements using the same device.
 * </p>
 */
 
class DeviceFactory : public QObject
{
	Q_OBJECT
	public:
		/** \brief Obtain the factory instance to operate on it
		 *
		 * Call this one when you need to use the factory. You can call it without any argument.
		 */
	 	static DeviceFactory *instance(QObject *parent = NULL);

		/** \brief create a new DeviceProxy or obtain a handle to an existing one
		 *
		 * Call this method through the DeviceFactory instance to obtain a new DeviceProxy with the 
		 * given name.
		 * @param devname the device name whose Tango DeviceProxy you want to obtain.
		 * @return a DeviceProxy handle: it might be an already existing one - previously allocated and
		 *                stored into the factory - or a new one just created for you.
		 * Use getDeviceProxy if you are only interested in knowing wether a Device Proxy that suits you 
		 * already exists.
		 *
		 * @see getDeviceProxy
		 */
		DeviceProxy* create(const QString &devname);
		
		/** \brief returns a handle to a DeviceProxy already present in the factory, NULL otherwise
		 *
		 * Call this method to test if a DeviceProxy with the given name already exists.
		 * @param deviceName the name of the tango device proxy you want to obtain
		 * @return a handle to an existing device proxy or NULL if the factory does not 
		 *                contain a device proxy with the device name specified.
		 */
		DeviceProxy*	getDeviceProxy(const QString& deviceName);
		
	private:
		 DeviceFactory(QObject *parent);
	  	~DeviceFactory();
	  
	 	 static DeviceFactory *_devFactory;
		QMap< QString, DeviceProxy *> d_devMap;
		
		void cleanFactory();
		
		/* this does not hold any lock. Used by create */
		DeviceProxy *d_getDeviceProxy(const QString& deviceName);
		
		QMutex d_deviceMutex;
};

#endif
