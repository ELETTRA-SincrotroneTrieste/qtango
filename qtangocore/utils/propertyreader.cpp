#include "propertyreader.h"
#include "propertyreaderprivate.h"
#include <QVariant>
#include <macros.h>
#include <QtDebug>

PropertyReader::~PropertyReader()
{
    pinfo("PropertyReader: deleting threads.");
    QList<PropertyReaderThread *> threads = this->findChildren<PropertyReaderThread *>();
    foreach(PropertyReaderThread * thread, threads)
    {
        thread->wait();
        delete thread;
    }
}

/** \brief The constructor of a PropertyReader
  *
  * Use this method to instantiate a PropertyReader with the desired source
  *
  * @param sourceName the device name (if a device property is desired) or
  *                   the class name, if you want to read a class property.
  *                   In the second case, the isDeviceProperty parameter must
  *                   be set to false, or setDeviceProperty(false) must be called
  *                   before read.
  * @param parent the parent QObject
  * @param database the tango database to connect to. By default, it is taken from
  *                 the TANGO_HOST environment variable
  * @param isDeviceProperty true: get a device property from the sourceName device
  *                         false: get a class property from the class specified in sourceName
  *
  */
PropertyReader::PropertyReader(const QString &sourceName,
                               QObject *parent,
                               const QString &database,
                               bool isDeviceProperty) :
    QObject(parent)
{
    d_ptr = new PropertyReaderPrivate();
    d_ptr->isDeviceProperty = isDeviceProperty;
    d_ptr->sourceName = sourceName;
    d_ptr->database = database;
    d_ptr->blocking = false;
}


/** \brief returns the source name
  *
  * @return the source name, i.e. the device name or the class name
  *         used to retrieve the property value.
  *
  * @see setSourceName
  * @see setDeviceProperty
  * @see isDeviceProperty
  */
QString PropertyReader::sourceName() const
{
    return d_ptr->sourceName;
}

/** \brief Change the source name
  *
  * @param name the name of the source, which can be a device name or a class
  *        name, according to what you want to read.
  *
  * @see sourceName
  * @see setDeviceProperty
  * @see isDeviceProperty
  */
void PropertyReader::setSourceName(const QString& name)
{
    d_ptr->sourceName = name;
}

/** \brief Sets or unsets the blocking mode
  *
  * @param b true the call to read blocks until the properties are read.
  * @param b false the call to read does not block and the reading is
  *          performed in another thread. In this case, the results are
  *          asynchronously notified by the two valueRead signals.
  *
  * \par Note
  * If the PropertyReader operates in blocking mode, the caller can immediately
  * access to the results through the property and propertyList methods, right
  * after the read call
  *
  * @see isBlocking
  *
  */
void PropertyReader::setBlocking(bool b)
{
    d_ptr->blocking = b;
}

/** \brief returns the blocking operation mode of the PropertyReader
  *
  * @return true the read is blocking, i.e. the caller waits until the
  *          property is read from the database
  * @return false the read is asynchronous
  *
  *@see setBlocking
  *
  */
bool PropertyReader::isBlocking() const
{
    return d_ptr->blocking;
}

/** \brief changes the isDeviceProperty flag
  *
  * @param dp true get a device property
  * @param dp false get a class property
  *
  * @see setSourceName
  * @see sourceName
  * @see isDeviceProperty
  */
void PropertyReader::setIsDeviceProperty(bool dp)
{
    d_ptr->isDeviceProperty = dp;
}

/** \brief returns the value of the isDeviceProperty property
  *
  *
  * @return true will get (got) a device property
  * @return false will get (or just read) a class property
  *
  * @see setIsDeviceProperty
  * @see setSourceName
  * @see sourceName
  *
  */
bool PropertyReader::isDeviceProperty() const
{
    return d_ptr->isDeviceProperty;
}

/** \brief read a device or class property with the given name
  *
  * @param propertyName the name of the device or attribute property to read.
  *
  * This call creates and starts a thread to perform the read operation on the tango
  * database. The read is performed in a different thread. Whether the caller is blocked
  * or not while waiting for the results is determined by the isBlocking property.
  *
  * \par Note
  * Every time a read is invoked a thread is created. A thread terminates as soon as the
  * property is obtained, but its memory is kept until clear is called, so that the results
  * remain available to the PropertyReader even after subsequent calls to read.
  * Delete the PropertyReader or call clear on it in order to ensure that the allocated memory
  * for the threads is freed.
  *
  * \par Behaviour
  * This method reads a property at a time. If you want to read more than one property at once,
  * please consider using the other read method below.
  * Either you use this or the other read method, the way you are notified is the same. In case of
  * a list of properties to be read, a number of valueRead signals will be emitted.
  *
  * @see  read(const QStringList &properties)
  *
  * @see setBlocking
  * @see setIsDeviceProperty
  *
  */
void PropertyReader::read(const QString &propertyName)
{
    PropertyReaderThread *prThread = new PropertyReaderThread(d_ptr->sourceName, QStringList() << propertyName,
                                             d_ptr->database, this);
    prThread->isDeviceProperty = d_ptr->isDeviceProperty;
    connect(prThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    prThread->start();
    if(d_ptr->blocking)
        prThread->wait();
}

/** \brief read a certain number of device properties
  *
  * Reads a list of properties from the tango database.
  *
  * @param properties a list of properties
  *
  * @see read(const QString &propertyName)
  *
  * You will receive a number of valueRead signals according to the number of properties
  * requested (supposing they are all successful).
  *
  * Refer the the read(const QString &propertyName) method above for more details and notes.
  *
  */
void PropertyReader::read(const QStringList &properties)
{
    PropertyReaderThread *prThread = new PropertyReaderThread(d_ptr->sourceName, properties, d_ptr->database, this);
    prThread->isDeviceProperty = d_ptr->isDeviceProperty;
    connect(prThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    prThread->start();
    if(d_ptr->blocking)
        prThread->wait();
}

/** \brief returns true if at least one of the read calls failed
  *
  * @return true one of the calls to read failed.
  * @return false no errors occurred in any of the read calls.
  *
  * \par Note
  * The call to clear discards all the previous error conditions.
  *
  */
bool PropertyReader::errorsOccurred() const
{
    QList<PropertyReaderThread *> threads = this->findChildren<PropertyReaderThread *>();
    foreach(PropertyReaderThread * thread, threads)
    {
        if(thread->error)
            return true;
    }
    return false;
}

void PropertyReader::threadFinished()
{
    PropertyReaderThread *thread = qobject_cast<PropertyReaderThread *>(sender());
    /* extract properties and notify */
    foreach(QString s, thread->valuesHash.keys())
    {
        QStringList v = thread->valuesHash.value(s);

        if(v.size() == 1)
        {
            emit valueRead(v.first());
            emit valueRead(s, v.first());
        }
        else if(v.size() > 1)
        {
            emit valueRead(v);
            emit valueRead(s, v);
        }
    }

    if(thread->error)
        emit error(thread->errorMessage);

    emit readComplete();
}

/** \brief Get the value of a property in a list of strings
  *
  * Normally, the get_device_property and get_class_property return values
  * are saved into a list of strings.
  *
  * \par Behaviour.
  * If you are sure that your device or class property contains only one string
  * you can directly call the property(const QString& name) method below.
  * Actually, the property(const QString& name) below simply extracts the first value
  * of the list of strings returned by propertyList.
  * In many cases, a device or class property is a simple string.
  * In the case of a device or class property that has a list of values, you are
  * compelled to use this method.
  *
  * @see property(const QString& name)
  *
  * \par Note
  * You can call propertyList or property methods when you are sure that the
  * reading thread is finished, i.e. after the readComplete signal has been received.
  * Otherwise, you must use setBlocking and set the blocking property to true in order
  * to be able to access the property values right after the read call.
  *
  * @see isBlocking
  * @see setBlocking
  *
  *
  */
QStringList PropertyReader::propertyList(const QString& name)
{
    QStringList l;
    QList<PropertyReaderThread *> threads = this->findChildren<PropertyReaderThread *>();
    if(!threads.size())
    {
        perr("PropertyReader::propertyList: no alive threads found.");
        perr("Did you call read()?");
        perr("Have you called clear() and thus removed all results?");
    }
    else
    {
        foreach(PropertyReaderThread * thread, threads)
        {
            if(thread->valuesHash.contains(name))
                l += thread->valuesHash.value(name);
        }
    }
    return l;
}

/** \brief obtain the value of the property identified by name
  *
  * You can use this method when you are sure that the seeked property
  * has a single value stored into the database, i.e. there is not a list
  * of values associated to the property.
  *
  * \par Note
  * Refer to the propertyList method documentation for further details
  *
  */
QString PropertyReader::property(const QString &name)
{
    QStringList l = this->propertyList(name);
    if(l.size() > 0)
        return l.first();
    return QString();
}

/** \brief clears all data associated with the threads used up to now.
  *
  * Normally, the data read from the tango database remains available event through
  * subsequent calls to read on the same PropertyReader.
  * If you want to release data and memory related to old reads, call the clear method.
  *
  * \par Note
  * Threads must be finished in order to be deleted. If not finished, the clear method
  * does not delete them.
  *
  */
void PropertyReader::clear()
{
    QList<PropertyReaderThread *> threads = this->findChildren<PropertyReaderThread *>();
    foreach(PropertyReaderThread * thread, threads)
    {
        if(thread->isFinished())
            delete thread;
        else
            perr("PropertyReader::clear: cannot call clear() while threads still running. Please wait.");
    }
}



