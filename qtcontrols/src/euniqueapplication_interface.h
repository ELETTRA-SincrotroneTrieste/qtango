#ifndef EUNIQUEAPPLICATION_INTERFACE_H
#define EUNIQUEAPPLICATION_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/* Proxy class for interface it.trieste.elettra.browser */

class ItTriesteElettraXXXInterface : public QDBusAbstractInterface
{
	Q_OBJECT
		public: 
			static inline const char* staticInterfaceName()
				{ return "it.trieste.elettra"; }
	
	ItTriesteElettraXXXInterface(const QString &service, const QString &path, 
		const QDBusConnection &connection, QObject *parent = 0);

	~ItTriesteElettraXXXInterface();
	
	public Q_SLOTS: // METHODS

	Q_SIGNALS: // SIGNALS
    		void rise_signal(const QString &rise_mess);
};

namespace it
{
  namespace trieste 
 {
	namespace elettra
	{
	    	typedef ::ItTriesteElettraXXXInterface xxx_interface;
	}

 }
}

#endif
