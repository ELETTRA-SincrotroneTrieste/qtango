#ifndef EUNIQUEAPPLICATIONADAPTER_H
#define EUNIQUEAPPLICATIONADAPTER_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#define INTERFACE_NAME "it.trieste.elettra"

class EUniqueApplicationAdapter: public QDBusAbstractAdaptor
{
 Q_OBJECT
Q_CLASSINFO("D-Bus Interface", "it.trieste.elettra")
      	public:	
		EUniqueApplicationAdapter(QObject *parent);
		virtual ~EUniqueApplicationAdapter();

	/* Signals */
	Q_SIGNALS: 
                void rise_signal(const QString &identification);

};



#endif


