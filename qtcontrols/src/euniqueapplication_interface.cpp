#include "euniqueapplication_interface.h"

ItTriesteElettraXXXInterface::ItTriesteElettraXXXInterface(const QString &service,
	 const QString &path, const QDBusConnection &connection, QObject *parent)
		: QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{

}

ItTriesteElettraXXXInterface::~ItTriesteElettraXXXInterface()
{

}

