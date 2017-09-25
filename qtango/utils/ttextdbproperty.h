#ifndef TTEXTDBPROPERTY_H
#define TTEXTDBPROPERTY_H

class TTextPropertyPrivate;
class PropertyReader;

#include <QString>
#include <QObject>

/** \brief This class reads a single valued device or class property and sets the read value
  *        on a slot of a desired QObject having a single const QString reference as argument.
  *
  * This class reads a single valued device or class property and sets the read value
  *        on a slot of a desired QObject having a single const QString reference as argument.
  *
  * \par Description and usage
  * The usage of this class is very simple. Just allocate the object using the provided constructor
  * and the object will use a PropertyReader (QTangoCore/utils) to set the property value as a
  * QString invoking a slot on a QObject.
  *
  * \par Example
  * \code
  * // suppose you have a QLabel named label in your graphical application.
  * new TTextDbProperty("test/device/1",
  *                      "helperApplication",
  *                      label,
  *                      SLOT(setText(const QString&)));
  *
  * // now suppose you have a windowTitle property and you want to set the
  * // window title of your application according to its value
  * new TTextDbProperty("test/device/1"
  *                      "windowTitle",
  *                      this,
  *                      SLOT(setWindowTitle(QString)));
  * \endcode
  *
  * \par Example
  * You will find an example under the qtango/test directory, inside the tangoDbProperties
  * folder.
  *
  */
class TTextDbProperty
{
public:
    explicit TTextDbProperty(const QString &sourceName,
                             const QString& propName,
                             QObject *receiver,
                             const char *slot,
                             bool isDeviceProperty = true,
                             const QString& database = QString());

    virtual ~TTextDbProperty();

    PropertyReader *propertyReader() const;

private:
    TTextPropertyPrivate *d_ptr;
    
};

#endif // TTEXTDBPROPERTY_H
