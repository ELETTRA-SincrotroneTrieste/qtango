#include "euniqueapplication_adapter.h"

/** This one inherits from QDBusAbstractAdaptor class.
 * TheQDBusAbstractAdaptor class is the starting point for all objects
 * intending to provide interfaces to the external world using D-Bus.
 * EUniqueApplicationAdapter has to be attached to a normal QObject, as it
 * happens for instance in EUniqueApplication class, and then registering that 
 * QObject (i.e. EUniqueApplication) with registerObject() (see euniqueaplication.cpp).
 */
EUniqueApplicationAdapter::EUniqueApplicationAdapter(QObject *parent) :
	QDBusAbstractAdaptor(parent)
{
	setAutoRelaySignals(true);
}

EUniqueApplicationAdapter::~EUniqueApplicationAdapter()
{

}






