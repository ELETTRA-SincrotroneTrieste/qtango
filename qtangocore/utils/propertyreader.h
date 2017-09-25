/***************************************************************************
*   Copyright (C) 2012 by  Giacomo Strangolino	  *
*     *
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

#ifndef PROPERTYREADER_H
#define PROPERTYREADER_H

#include <QObject>
#include <tango.h>

class PropertyReaderPrivate;

/** \brief This class is an utility to read <em>class</em> and <em> device</em>
  *        properties from the Tango database.
  *
  * \par Availability
  * This class has been introduced since QTango 4.3.3 and it makes part of the
  * QTangoCore/utils subsection.
  *
  *
  * \par Description
  * This class can be used to read class and device properties from the tango database.
  * To use the class instantiate it with the name of the source you want to read from,
  * i.e. the device name in case of device properties and the class name in case of
  * a class property. This is done in the class constructor through the sourceName
  * string parameter. <br/>
  * Then you have to call the read method passing the name of the property you want to
  * retrieve. <br/>
  * A thread is launched and, when done, a bunch of signals are emitted to notify the
  * results.
  * You can either be asynchronously notified when the results are ready through the signals
  * (without worrying about the underlying threads) or set the blocking parameter to true
  * and obtain the results by means of the property and propertyList methods.
  *
  * \par Example
  * \code
  *
  * // include file
  * #include <PropertyReader>
  * // Instantiate an object, passing a QObject as parent (<em>this</em> in this case)
  * // and a device name as string (to retrieve device properties).
  * //
  * PropertyReader *pr = new PropertyReader("test/device/1",  this);
  * pr->setBlocking(true); // to wait for read to be completed
  * //
  * // now perform a couple of readings from the database
  * pr->read("values");
  * pr->read("helperApplication");
  * //
  * // get the results
  * // It is possible to get the results now because setBlocking was called with a
  * // true parameter. Otherwise, you should have used signal/slot connections in order
  * // to be notified when data is available
  * //
  * qDebug() << "values" << pr->propertyList("values");
  * qDebug() << "helperApplication" << pr->property("helperApplication");
  * //
  * // Now using the same PropertyReader, get a class property:
  * pr->setDeviceProperty(false);
  * // change the source name, this time a class name:
  * pr->setSourceName("TangoTest");
  * // read the value of the class property "cvs_location":
  * pr->read("cvs_location");
  * // again, we are in blocking mode, so it is possible to read the results
  * // here
  * qDebug() << "cvs_location prop" << pr->property("cvs_location");
  *
  * \endcode
  *
  * \par Error handling.
  * Errors can be notified by connecting the error signal to a receiver slot.
  * There is no other way to obtain diagnostic error messages a posteriori.
  * You can test whether errors occurred with the errorsOccurred method call.
  * Anyway, it returns true if any of the one of the read calls failed.
  * The clear method removes all threads, so the call to clear also resets the
  * error state.
  *
  *
  */
class PropertyReader : public QObject
{
    Q_OBJECT
public:
    explicit PropertyReader(const QString& sourceName,
                            QObject *parent = 0,
                            const QString& database = QString(),
                            bool isDeviceProperty = true);

    virtual ~PropertyReader();

    QStringList propertyList(const QString& name);

    QString property(const QString &name);

    void setBlocking(bool b);

    bool isBlocking() const;

    void setIsDeviceProperty(bool dp);

    bool isDeviceProperty() const;

    QString sourceName() const;

    bool errorsOccurred() const;
    
signals:

    /** \brief notifies that the propertyValue associated to propertyName has been successfully read.
      *
      * @param propertyName the name of the device or class property just read
      * @param propertyValue the value of the property with propertyName name
      *
      * \par Note
      * This signal is emitted when the result is made up of a single value (not a list
      * of values).
      *
      * @see valueRead(const QString& propertyName, const QStringList& propertyValueList)
      * @see valueRead(const QString& value)
      * @see valueRead(const QStringList& propertyValueList)
      *
      * \par Note
      * If the property propertyName stores a list of values, then this signal is not emitted and
      * the valueRead(const QString& propertyName, const QStringList& propertyValueList) and
      * valueRead(const QStringList& propertyValueList) are emitted instead.
      */
    void valueRead(const QString& propertyName, const QString& propertyValue);

    /** \brief This signal has the same meaning of the
      *        valueRead(const QString& propertyName, const QString& propertyValue) and is
      *        provided for convenience.
      *
      * When you request a single property to the same PropertyReader, then you can use this
      * signal to get the property value.
      *
      * \par Note
      * This signal does not contain information about the property name, so it is not possible
      * to know to which property it is related if multiple read method calls have been done or
      * if a list of properties has been requested through the read call.
      *
      * @see valueRead(const QString& propertyName, const QString& propertyValue)
      */
    void valueRead(const QString& value);

    /** \brief notifies that the propertyValueList associated to propertyName has been successfully read.
      *
      * @param propertyName the name of the device or class property just read
      * @param propertyValueList the list of the property values with the propertyName name
      *
      * \par Note
      * This signal is emitted when the result is made up of a list of values (not a single value).
      * If the property contains a single value, then
      * valueRead(const QString& propertyName, const QString& propertyValue) is emitted.
      *
      *
      * @see valueRead(const QString& propertyName, const QString& propertyValue)
      * @see valueRead(const QString& value)
      * @see valueRead(const QStringList& propertyValueList)
      *
      */
    void valueRead(const QString& propertyName, const QStringList& propertyValueList);

    /** \brief This signal is analogous to the signal above, with the same considerations done
      *        for valueRead(const QString& value)
      *
      * @see valueRead(const QString& propertyName, const QStringList& propertyValueList)
      * @see valueRead(const QString& value)
      *
      */
    void valueRead(const QStringList& propertyValueList);

    /** \brief The reading thread is completed and it is possible to retrieve the data
      *        with property and propertyList.
      *
      * \par Note
      * It is not necessary to wait for this signal if the blocking property is set to true
      * or if you use the valueRead signals to obtain the results.
      */
    void readComplete();

    /** \brief notifies an exception including a message.
      *
      * Notifies a tango exception has occurred while reading the class or device property.
      *
      * @param message the tango exception
      *
      */
    void error(const QString& message);

public slots:

    void setSourceName(const QString& name);

    void read(const QString& propertyName);

    void read(const QStringList &properties);

    void clear();

private slots:
    void threadFinished();

private:
    PropertyReaderPrivate *d_ptr;

    Q_DECLARE_PRIVATE(PropertyReader)
    
};

#endif // PROPERTYREADER_H
