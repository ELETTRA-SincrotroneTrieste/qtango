#ifndef QTANGOIZERPRIVATE_H
#define QTANGOIZERPRIVATE_H

#include <QString>
#include <QHash>
#include "qtangoizer.h"

class QObject;
class TValueFilter;
class TangoConfigurationParameters;

#define TYPELEN 64

class QTangoizerPrivate
{
    Q_DECLARE_PUBLIC(QTangoizer);
public:

    QTangoizerPrivate();

    virtual ~QTangoizerPrivate();

    /* type of object, for rtti */
    int type;

    QString slot, methodName;

    QString setPointSlot, setPointMethodName;

    void *data;

    Qt::ConnectionType connType;

    QHash<int, QString> autoConfSlotsHash;

    bool inTypeOfMethod(const QString& method, QObject *obj, char* in_type);

    bool autoConfigure(const TangoConfigurationParameters *cp, QObject *obj);

    bool singleShot, autoDestroy;

    bool toolTipsDisabled;

    void setExecuteOnConnection(bool execute, const QVariant &argument);

    QVariant executeArgument() const { return m_executeArgument; }

    bool executeOnConnection() const { return m_executeOnConnection; }

    /* methods are specified via SIGNAL and SLOT macros, so we have to remove
     * signals and slots codes from the method. See qt/src/corelib/kernel/qobject.h
     *  and qt/src/corelib/kernel/qobject.cpp.
     * Returns the extracted code and removes the code from the method parameter if
     * the code is QSIGNAL_CODE or QSLOT_CODE.
     */
    int extractCode(QString& method);

    TValueFilter *refreshFilter;

private:
    QTangoizer *q_ptr;

    QVariant m_executeArgument;

    bool m_executeOnConnection;

    void *m_getDataPtr(const char *type, double val);
    void m_DeleteDataPtr(const char *type, void *dataptr);
};


#endif // QTANGOIZERPRIVATE_H
