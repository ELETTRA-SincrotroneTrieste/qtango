#include "propertyreaderprivate.h"
#include <tango.h>
#include <vector>
#include <macros.h>
#include <QtDebug>
#include <tlog.h>

PropertyReaderPrivate::PropertyReaderPrivate()
{
    isDeviceProperty = true;
}

PropertyReaderThread::~PropertyReaderThread()
{

}

PropertyReaderThread::PropertyReaderThread(const QString& sourceName,
                                           const QStringList &properties,
                                           const QString &database, QObject *parent)
    : QThread(parent)
{
    mProperties  = properties;
    mSourceName = sourceName;
    mDatabase = database;
}

void PropertyReaderThread::run()
{
    try
    {
        error = false;
        errorMessage = "";
        Tango::Database *db = NULL;
        string dbhost = mDatabase.toStdString();
        if(!mDatabase.isEmpty())
            db = new Tango::Database(dbhost);
        else
            db = new Tango::Database();

        vector<string> vs;
        Tango::DbData db_data;

        foreach(QString s, mProperties)
            db_data.push_back(DbDatum(qstoc(s)));

        /* get a list of properties */
        if(isDeviceProperty)
            db->get_device_property(qstoc(mSourceName), db_data);
        else
            db->get_class_property(qstoc(mSourceName), db_data);

        for(size_t i = 0; i < db_data.size(); i++)
        {
            if(!db_data[i].is_empty())
            {
                QStringList sl;
                db_data[i] >> vs;
                foreach(std::string s, vs)
                    sl << QString::fromStdString(s);
                valuesHash.insert(QString::fromStdString(db_data[i].name), sl);
            }
            else
            {
                error = true;
                errorMessage = QString("Data is Empty (does property for \"%1\" exist?)")
                        .arg(mSourceName);
            }
        }
        delete db;
    }
    catch(Tango::DevFailed& e)
    {
        error = true;
        TLog log(e);
        perr("PropertyReaderThread: error getting properties");
        errorMessage = log.formatMessage();
        perr("PropertyReaderThread: %s", qstoc(errorMessage));
    }
}


