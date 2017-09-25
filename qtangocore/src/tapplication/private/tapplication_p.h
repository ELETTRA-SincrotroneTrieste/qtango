#ifndef TAPPLICATIONPRIVATE_H
#define TAPPLICATIONPRIVATE_H

class TApplication;
class TApplicationInterface;

#include <QString>
#include <QVariant>
#include <QStringList>

class SecondInstanceHandler;

/* Inherits QObject in order to be accessible from TApplication by means of findChild().
 * This is not strictly necessary and should be removed in the next major release.
 */
class TApplicationPrivate : public /* to remove in the next major release */ QObject
{
    Q_OBJECT

public:
    TApplicationPrivate(TApplication *parent);

    virtual ~TApplicationPrivate();

    void initDbus();
    void finiDbus();

    QString composeDbusObjectName();

    QStringList methods(const QString& widgetName);

    QStringList properties(const QString& widgetName);

    QString executable() const { return m_exename; }

    QStringList parameters() const;

    QStringList widgetList();

    QVariant invokeMethod( const QString& widgetName, const QString& memberSignature,
                       const QVariant& val0 = QVariant(),
                       const QVariant& val1 = QVariant(), const QVariant& val2 = QVariant(),
                       const QVariant& val3 = QVariant(), const QVariant& val4 = QVariant());

    QVariant readProperty(const QString& widgetName, const QString& propertyName);

    void writeProperty(const QString& widgetName, const QString& propertyName, const QVariant& val);

    TApplicationInterface *getRunningApplicationInterface(const QString& exename, const QStringList &args);

    void onSecondInstanceDetected();

    void installSecondInstanceHandler(SecondInstanceHandler *sif) { d_secondInstanceHandler = sif; }

    SecondInstanceHandler *secondInstanceHandler() { return d_secondInstanceHandler; }

    void removeSecondInstanceHandler();

    QString dbusServiceName() const { return m_dbusServiceName; }

    QString dbusPath() const { return m_dbusPath; }

private:
    Q_DECLARE_PUBLIC(TApplication)
    TApplication * const q_ptr;

    /* store arguments and exename, because call to qApp->arguments() is slow, as suggested by
     * Qt doc.
     */
    QStringList m_parameters;
    QString m_exename;
    QString m_dbusServiceName;
    QString m_dbusPath;

    SecondInstanceHandler *d_secondInstanceHandler;
};

#endif // TAPPLICATIONPRIVATE_H
