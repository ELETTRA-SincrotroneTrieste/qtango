#include "qtangoizer.h"
#include <macros.h>
#include "qtangoizerprivate.h"
#include "tvaluefilter.h"
#include <QWidget> /* to set tooltips on attached object, if it inherits QWidget */

QTangoizer::QTangoizer(QObject *parent) : QObject(parent)
        , d_ptr(new QTangoizerPrivate())
{
    Q_D(QTangoizer);
    d->type = Invalid;
    d->slot = QString();
    d->data = NULL;
}

QTangoizer::~QTangoizer()
{
    Q_D(QTangoizer);
    delete d;
}

void QTangoizer::setAutoConfSlot(AutoConfType act, const char* slot)
{
    Q_D(QTangoizer);
    d->autoConfSlotsHash.insert(act, QString(slot));
}

void QTangoizer::attach(QObject *refreshee,  const char *slot,
                        const char *setPointSlot,
     Qt::ConnectionType connType)
{
    Q_D(QTangoizer);
    QString method(slot);
    QString setPtSlot;
    char in_type[TYPELEN], in_setPt_type[TYPELEN];
    bool typeOfMethodOk, typeOfSetPointSlotOk = true;
    d->extractCode(method); /* removes slot code from the signature */

    if(setPointSlot && strlen(setPointSlot) > 0)
    {
        setPtSlot = QString(setPointSlot);
        /* remove signal slot code from the signature */
        extractCode(setPtSlot);
    }

    typeOfMethodOk = inTypeOfMethod(method, refreshee, in_type);
    if(setPointSlot != NULL)
        typeOfSetPointSlotOk = inTypeOfMethod(setPtSlot, refreshee, in_setPt_type);

    if(typeOfMethodOk && typeOfSetPointSlotOk && setPointSlot != NULL && strcmp(in_setPt_type, in_type) != 0)
    {
        perr("QTangoizer::attach: input types for method and set point method for object \"%s\"",
             qstoc(refreshee->objectName()));
        perr("                    method in type \"%s\" set point method type \"%s\"", in_type, in_setPt_type);
    }
    else if(typeOfMethodOk && typeOfSetPointSlotOk)
    {
        if(strlen(in_type) == 0)
            d->type = Void;
        else if(strcmp(in_type, "bool") == 0)
            d->type = Bool;
        else if(strcmp(in_type, "int") == 0)
            d->type = Int;
        else if(strcmp(in_type, "double") == 0)
            d->type = Double;
        /* must be tested before the String if below */
        else  if(strcmp(in_type, "QStringList") == 0)
            d->type = StringVector;
        else if(strstr(in_type, "String") != NULL)
            d->type = String;
        else if(strcmp(in_type, "short") == 0)
            d->type = Short;
        else if(strcmp(in_type, "unsigned int") == 0)
            d->type = UInt;
        else if(strstr(in_type, "State") != NULL)
            d->type = State;
        else  if(strcmp(in_type, "QVector<double>") == 0)
            d->type = DoubleVector;
        else  if(strcmp(in_type, "QVector<int>") == 0)
            d->type = IntVector;
        else  if(strcmp(in_type, "QVector<bool>") == 0)
            d->type = BoolVector;
        else
            perr("QTangoizer::attach() invalid or unrecognized type \"%s\"", in_type);

        d->data = refreshee;
        d->slot = method;
        /* save in method name only the name, without the parameter list */
        d->methodName = method.remove(QRegExp("\\(.*\\)"));
        d->connType = connType;
        qprintf("\e[1;32mattach() detected type %d on %s / %s (%s)\e[0m\n", d->type,
               qstoc(refreshee->objectName()), qstoc(method), in_type);
        if(!setPtSlot.isEmpty())
        {
            d->setPointSlot = setPtSlot;
            d->setPointMethodName = setPtSlot.remove(QRegExp("\\(.*\\)"));
        }
    }
    else
        perr("QTangoizer::attach: unsupported input type of method. Object \"%s\", slot \"%s\"",
             qstoc(refreshee->objectName()), qstoc(method));
}

bool QTangoizer::singleShot() const
{
    Q_D(const QTangoizer);
    return d->singleShot;
}

bool QTangoizer::autoDestroy() const
{
    Q_D(const QTangoizer);
    return d->autoDestroy;
}

void QTangoizer::setAutoDestroy(bool autoDestroy)
{
    Q_D(QTangoizer);
    d->singleShot = true;
    d->autoDestroy = autoDestroy;
}

void QTangoizer::setSingleShot(bool singleShot)
{
    Q_D(QTangoizer);
    d->singleShot = singleShot;
}

void QTangoizer::attach(short *s)
{
    Q_D(QTangoizer);
    d->data = s;
    d->type = Short;
}

void QTangoizer::attach(double *dou)
{
    Q_D(QTangoizer);
    d->data = dou;
    d->type = Double;
}

void QTangoizer::attach(int *i)
{
    Q_D(QTangoizer);
    d->data = i;
    d->type = Int;
}

void QTangoizer::attach(unsigned int *u)
{
    Q_D(QTangoizer);
    d->data = u;
    d->type = UInt;
}

void QTangoizer::attach(QString *qs)
{
    Q_D(QTangoizer);
    d->data = qs;
    d->type = String;
}

void QTangoizer::attach(bool *b)
{
    Q_D(QTangoizer);
    d->data = b;
    d->type = Bool;
}

void QTangoizer::attach( QStringList *sl)
{
    Q_D(QTangoizer);
    d->data = sl;
    d->type = StringVector;
}

void QTangoizer::attach( QVector<double> * v)
{
    Q_D(QTangoizer);
    d->data = v;
    d->type = DoubleVector;
}

void QTangoizer::attach( QVector<int> *vi)
{
    Q_D(QTangoizer);
    d->data = vi;
    d->type = IntVector;
}

void QTangoizer::attach( QVector<bool> *vb)
{
    Q_D(QTangoizer);
    d->data = vb;
    d->type = BoolVector;
}

void *QTangoizer::data()
{
    Q_D(const QTangoizer);
    return d->data;
}

QString QTangoizer::slot() const
{
    Q_D(const QTangoizer);
    return d->slot;
}

QString QTangoizer::methodName() const
{
    Q_D(const QTangoizer);
    return d->methodName;
}

QString QTangoizer::setPointSlot() const
{
    Q_D(const QTangoizer);
    return d->setPointSlot;
}

QString QTangoizer::setPointMethodName() const
{
    Q_D(const QTangoizer);
    return d->setPointMethodName;
}

QTangoizer::Type QTangoizer::type() const
{
    Q_D(const QTangoizer);
    return (QTangoizer::Type) d->type;
}

Qt::ConnectionType QTangoizer::connectionType() const
{
    Q_D(const QTangoizer);
    return d->connType;
}

void QTangoizer::removeRefreshFilter(TValueFilter *filter)
{
    Q_D(QTangoizer);
    Q_UNUSED(filter);
    d->refreshFilter = NULL;
}

void QTangoizer::installRefreshFilter(TValueFilter *filter)
{
    Q_D(QTangoizer);
    d->refreshFilter = filter;
}

TValueFilter *QTangoizer::refreshFilter() const
{
    Q_D(const QTangoizer);
    return d->refreshFilter;
}


void QTangoizer::setToolTipsDisabled(bool disable)
{
    Q_D(QTangoizer);
    d->toolTipsDisabled = disable;
}

bool QTangoizer::toolTipsDisabled() const
{
    Q_D(const QTangoizer);
    return d->toolTipsDisabled;
}

void QTangoizer::slotAttributeAutoConfigured(const TangoConfigurationParameters* cp)
{
    Q_D(QTangoizer);
    QObject *object = NULL;

    if(!d->slot.isEmpty())
        object = static_cast<QObject *>(d->data); /* QObject and slot were provided */

    emit attributeAutoConfigured(cp);

    if(object)
        d->autoConfigure(cp, object);

    if(object && !d->setPointMethodName.isEmpty())
    {
        qprintf("\e[1;33m QTangoizer::slotAttributeAutoConfigured setting current value on auto conf for obj %s method %s\e[0m\n",
              qstoc(object->objectName()), qstoc(d->setPointMethodName));
        /* invoke updateValue with a false parameter to extract the set point from the
         * current value.
         */
        updateValue(cp->currentValue(), false, qstoc(d->setPointMethodName), TValueFilter::AutoConfiguration);
    }

}

bool QTangoizer::inTypeOfMethod(const QString &method, QObject *obj, char* in_type)
{
    Q_D(QTangoizer);
    return d->inTypeOfMethod(method, obj, in_type);
}

int QTangoizer::extractCode(QString& method)
{
    Q_D(QTangoizer);
    return d->extractCode(method);
}

void QTangoizer::updateValue(const TVariant &v, bool read, const char* customMethod, TValueFilter::State updateState)
{
    Q_D(QTangoizer);
    QString methName;
    QString message = v.message();
    bool ok = false;

    if(customMethod != NULL)
        methName = QString(customMethod);
    else
        methName = methodName();

    if(v.quality() != Tango::ATTR_INVALID)
    {
        QObject *object = NULL;
        if(!slot().isEmpty())
            object = static_cast<QObject *>(data()); /* QObject and slot were provided */

        switch(type())
        {
        case QTangoizer::Int:
        case QTangoizer::Short:
            ok = v.canConvertToInt();
            if(ok)
            {
                int intVal = v.toInt(read);
                if(d->refreshFilter)
                    d->refreshFilter->filter(v, intVal, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(int, intVal));
                    if(!ok)
                        message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                            + "\". ]";
                }
                else
                    *static_cast<int *>(data()) = intVal;
                /* emit even if invokeMethod fails */
                emit newData(intVal);
            }
            else
                message += " [cannot convert to int] ";
            break;

        case QTangoizer::UInt:
            ok = v.canConvertToUInt();
            if(ok)
            {
                unsigned int uintVal = v.toUInt(read);
                if(d->refreshFilter)
                    d->refreshFilter->filter(v, uintVal, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(unsigned int, uintVal));
                    if(!ok)
                        message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                            + "\". ]";
                }
                else
                    *static_cast<int *>(data()) = uintVal;
                emit newData(uintVal);
            }
            else
                message += " [cannot convert to unsigned int] ";
            break;

        case QTangoizer::Double:
            ok = v.canConvertToDouble();
            if(ok)
            {
                double doubleVal = v.toDouble(read);
                if(d->refreshFilter)
                    d->refreshFilter->filter(v, doubleVal, read, updateState);
                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(double, doubleVal));
                    if(!ok)
                        message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                            + "\". ]";
                }
                else
                    *static_cast<double *>(data()) = doubleVal;
                emit newData(doubleVal);
            }
            else
                message += " [cannot convert to double] ";
            break;

        case QTangoizer::String:
            ok = v.canConvertToString();
            if(ok)
            {
                QString str = v.toString(read);
                if(d->refreshFilter)
                    d->refreshFilter->filter(v, str, read, updateState);

                if(object)
                {
                    ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QString, str));
                    if(!ok)
                        message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                            + "\". ]";
                }
                else
                    *static_cast<QString *>(data()) = str;

                emit newData(str);
            }
            else
                message += " [cannot convert to qt string] ";
            break;

            case QTangoizer::State:
                ok = v.canConvertToState();
                if(ok)
                {
                    Tango::DevState state = v.toState();
                    if(d->refreshFilter)
                        d->refreshFilter->filter(v, state, read, updateState);

                    if(object)
                    {
                        ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(Tango::DevState, state));
                        if(!ok)
                            message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                + "\". ]";
                    }
                    else
                        *static_cast<Tango::DevState *>(data()) = state;
                    emit newData(state);
                }
                else
                    message += " [cannot convert to tango DevState] ";
            break;

            case QTangoizer::Bool:
                ok = v.canConvertToBool();
                if(ok)
                {
                    bool abool = v.toBool(read);
                    if(d->refreshFilter)
                        d->refreshFilter->filter(v, abool, read, updateState);
                    if(object)
                    {
                        ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(bool, abool));
                        if(!ok)
                            message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                + "\". ]";
                    }
                    else
                        *static_cast<bool *>(data()) = abool;
                    emit newData(abool);
                }
                else
                    message += " [cannot convert to boolean] ";
            break;

            case QTangoizer::BoolVector:
                ok = v.canConvertToBoolVector();
                if(ok)
                {
                    QVector<bool> boolvect = v.toBoolVector(read);
                    if(d->refreshFilter)
                        d->refreshFilter->filter(v, boolvect, read, updateState);
                    if(object)
                    {
                        ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QVector<bool>, boolvect));
                        if(!ok)
                            message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                + "\". ]";
                    }
                    else
                        *static_cast<QVector<bool> *>(data()) = boolvect;
                    emit newData(boolvect);
                }
                else
                    message += " [cannot convert to boolean vector] ";
            break;

            case QTangoizer::DoubleVector:
                ok = v.canConvertToDoubleVector();
                if(ok)
                {
                    QVector<double> dblvect = v.toDoubleVector(read);
                    if(d->refreshFilter)
                        d->refreshFilter->filter(v, dblvect, read, updateState);
                    if(object)
                    {
                        ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QVector<double>, dblvect));
                        if(!ok)
                            message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                + "\". ]";
                    }
                    else
                        *static_cast<QVector<double> *>(data()) = dblvect;
                    emit newData(dblvect);
                }
                else
                    message += " [cannot convert to double vector] ";
            break;

            case QTangoizer::IntVector:
                ok = v.canConvertToIntVector();
                if(ok)
                {
                    QVector<int> intvect = v.toIntVector(read);
                    if(d->refreshFilter)
                        d->refreshFilter->filter(v, intvect, read, updateState);
                    if(object)
                    {
                        ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QVector<int>, intvect));
                        if(!ok)
                            message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                + "\". ]";
                    }
                    else
                        *static_cast<QVector<int> *>(data()) = intvect;
                    emit newData(intvect);
                }
                else
                    message += " [cannot convert to int vector] ";
            break;


            case QTangoizer::StringVector:
                ok = v.canConvertToStringVector();
                if(ok)
                {
                    QStringList strvect = v.toStringVector(read).toList();
                    if(d->refreshFilter)
                        d->refreshFilter->filter(v, strvect, read, updateState);
                    if(object)
                    {
                        ok = QMetaObject::invokeMethod(object, qstoc(methName), connectionType(), Q_ARG(QStringList, strvect));
                        if(!ok)
                            message += " [error invoking method \"" + slot() + "\" on \"" + object->objectName()
                                + "\". ]";
                    }
                    else
                        *static_cast<QStringList *>(data()) = strvect;
                    emit newData(strvect);
                }
                else
                    message += " [cannot convert to int vector] ";
            break;

         default:
            ok = false;
            message = QString(" [ invalid Type %1 specified ]").arg(type());
            break;

        }
        /* is it possible to set tooltip on a widget? */
        if(object && object->inherits("QWidget") && !d_ptr->toolTipsDisabled)
            static_cast<QWidget *>(object)->setToolTip(message);
        if(object && !ok)
            perr("QTWatcher::refresh: failed to invoke method \"%s\" on \"%s\": \"%s\"", qstoc(methName),
                 qstoc(object->objectName()), qstoc(message));
    }
    emit readOk(ok);
    emit refreshMessage(message);
}


