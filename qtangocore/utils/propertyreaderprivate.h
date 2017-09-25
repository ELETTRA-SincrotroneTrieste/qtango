#ifndef PROPERTYREADERPRIVATE_H
#define PROPERTYREADERPRIVATE_H

#include <QThread>
#include <QStringList>
#include <QHash>

class PropertyReaderThread : public QThread
{
    Q_OBJECT
public:

    virtual ~PropertyReaderThread();

    PropertyReaderThread(const QString &sourceName,
                         const QStringList &properties,
                         const QString& database,
                         QObject *parent);

    QHash<QString, QStringList> valuesHash;

    bool isDeviceProperty;

    QString sourceName;

    bool error;

    QString errorMessage;

protected:

    virtual void run();

private:
    QStringList mProperties;
    QString mSourceName;
    QString mDatabase;
};

class PropertyReaderPrivate
{
public:
    PropertyReaderPrivate();

    PropertyReaderThread* thread;

    bool isDeviceProperty;

    bool blocking;

    QString sourceName, propName, database;

};

#endif // PROPERTYREADERPRIVATE_H
