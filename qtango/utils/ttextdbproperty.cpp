#include "ttextdbproperty.h"
#include <propertyreader.h>
#include <QObject>

class TTextPropertyPrivate
{
public:
    TTextPropertyPrivate();

    PropertyReader *propertyReader;
};

TTextPropertyPrivate::TTextPropertyPrivate()
{
    propertyReader = NULL;
}

/** \brief The constructor of the class that does all the necessary work.
  *
  * @param sourceName the name of the device or class.
  * @param propName the name of the property you want to read from the database
  * @param receiver a QObject that will receive the value of the property as a single QString
  *        through the slot provided in the next parameter
  * @param slot the Qt slot that will be invoked on the receiver when the class or device property
  *        has been obtained from the Tango database
  * @param isDeviceProperty true the sourceName is interpreted as a device
  * @param isDeviceProperty false the sourceName is interpreted as a class name
  * @param database the name of the tango database. If not specified, TANGO_HOST environment variable
  *        is used.
  *
  * \par Note
  * The property must have a single value. This means that a list of values is not supported.
  * If configured with jive, the property must not contain newline characters.
  *
  */
TTextDbProperty::TTextDbProperty(const QString &sourceName,
                                 const QString &propName,
                                 QObject *receiver,
                                 const char *slot,
                                 bool isDeviceProperty,
                                 const QString& database)
{
    d_ptr = new TTextPropertyPrivate();
    d_ptr->propertyReader = new PropertyReader(sourceName, receiver, database, isDeviceProperty);
    QObject::connect(d_ptr->propertyReader, SIGNAL(valueRead(QString)), receiver, slot);
    d_ptr->propertyReader->read(propName);
}

TTextDbProperty::~TTextDbProperty()
{
    if(d_ptr->propertyReader)
    {
        delete d_ptr->propertyReader;
    }
}


/** \brief Returns the reference to the PropertyReader used by this class
  *
  * @return a reference to the property reader used by this class.
  *
  * You may want to read the PropertyReader documentation in QTangoCore classes
  * in order to understand what you can do with this object.
  *
  * @see PropertyReader
  *
  */
PropertyReader *TTextDbProperty::propertyReader() const
{
    return d_ptr->propertyReader;
}
