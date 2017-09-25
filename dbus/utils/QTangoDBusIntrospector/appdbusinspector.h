#ifndef APPDBUSINSPECTOR_H
#define APPDBUSINSPECTOR_H

#include <QObject>
#include <QDBusInterface>
#include <QMap>

class TApplicationInterface;

class AppDbusInspector : public QObject
{
Q_OBJECT
public:
    AppDbusInspector(QObject *parent = 0);

    QStringList getWidgetList(const QString& serviceName);

    QStringList getMethodSignatureList(const QString& serviceName, const QString& objectName);

    QStringList getPropertyList(const QString& serviceName, const QString& objectName);

    QString lastError() const { return m_lastError; }

    bool execute(const QString& serviceName, const QString& widgetName, const QString &slot,
                 QVariant &returnValue,
                 const QVariant & arg1 = QVariant(),
                 const QVariant & arg2 = QVariant(), const QVariant & arg3 = QVariant(),
                 const QVariant & arg4 = QVariant(), const QVariant & arg5 = QVariant());

private:

    QString m_lastError;

    TApplicationInterface* m_getDBusInterface(const QString& service);
};

#endif // APPDBUSINSPECTOR_H
