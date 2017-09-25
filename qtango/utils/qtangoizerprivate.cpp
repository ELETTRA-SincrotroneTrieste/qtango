#include "qtangoizerprivate.h"
#include <QMetaMethod>
#include <auto_configuration.h>
#include <macros.h>

#define TYPELEN 64

QTangoizerPrivate::QTangoizerPrivate()
{
    refreshFilter = NULL;
    singleShot = false;
    autoDestroy = false;
    m_executeOnConnection = false;
    toolTipsDisabled = false;
}

QTangoizerPrivate::~QTangoizerPrivate()
{

}

void QTangoizerPrivate::setExecuteOnConnection(bool execute, const QVariant &argument)
{
    m_executeOnConnection = execute;
    m_executeArgument = argument;
}

bool QTangoizerPrivate::inTypeOfMethod(const QString &method, QObject *obj, char* in_type)
{
    memset(in_type, 0, TYPELEN);
    int idx = obj->metaObject()->indexOfMethod(QMetaObject::normalizedSignature(qstoc(method)));
    if(idx > -1)
    {
        QMetaMethod metaMethod = obj->metaObject()->method(idx);
        QList<QByteArray> parTypes = metaMethod.parameterTypes();
        if(parTypes.size() == 1)
        {
            strncpy(in_type, parTypes.first().constData(), TYPELEN - 1);
            return true;
        }
        else if(parTypes.size() == 0) /* void */
            return true;
    }
    else
        printf("\e[1;31mmethod \"%s\" on obj \"%s\" not found!\e[0m\n", qstoc(method), qstoc(obj->objectName()));
    return false;
}

bool QTangoizerPrivate::autoConfigure(const TangoConfigurationParameters *cp, QObject *object)
{
    Q_Q(QTangoizer);
    bool ret = true;
    char in_type[TYPELEN];
    double value;
    void *dataptr;
    QString u, methodName;
    /* minimum */
    if(cp->minIsSet() && autoConfSlotsHash.contains(q->Min))
    {
        methodName = autoConfSlotsHash.value(q->Min);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, cp->minValue());
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object,
                    qstoc(methodName.remove(QRegExp("\\(.*\\)"))),
                    connType, QGenericArgument(in_type, dataptr));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);

            printf("min value set to %f %d in_type \"%s\"\n", value, ret, in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
        else
            printf("inTypeOf method returned false\n");
    }
 ///   printf("cp->minIsSet %d autoconf contains min %d \n", cp->minIsSet(), autoConfSlotsHash.contains(q->Min));
    /* maximum */
    if(cp->maxIsSet() && autoConfSlotsHash.contains(q->Max))
    {
        methodName = autoConfSlotsHash.value(q->Max);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, cp->maxValue());
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object,
                   qstoc(methodName.remove(QRegExp("\\(.*\\)"))), connType,
                   QGenericArgument(in_type, dataptr));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);

            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);

            printf("max value set to %f %d in_type \"%s\"\n", value, ret, in_type);
        }
    }
    /* min warning */
    if(cp->mWarnIsSet() && autoConfSlotsHash.contains(q->MinWarn))
    {
        methodName = autoConfSlotsHash.value(q->MinWarn);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, cp->minWarning());
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->MinWarn)), connType,
                                             QGenericArgument(in_type, &value));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    /* max warning */
    if(cp->MWarnIsSet() && autoConfSlotsHash.contains(q->MaxWarn))
    {
        methodName = autoConfSlotsHash.value(q->MaxWarn);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, cp->maxWarning());
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->MaxWarn)), connType,
                         QGenericArgument(in_type, &value));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    /* min error */
    if(cp->mErrIsSet() && autoConfSlotsHash.contains(q->MinErr))
    {
        methodName = autoConfSlotsHash.value(q->MinErr);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            dataptr = m_getDataPtr(in_type, cp->minError());
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->MinErr)), connType,
                                             QGenericArgument(in_type, &value));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    /* max error */
    if(cp->MErrIsSet() && autoConfSlotsHash.contains(q->MaxErr))
    {
        methodName = autoConfSlotsHash.value(q->MaxErr);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {

            dataptr = m_getDataPtr(in_type, cp->maxError());
            ret &= dataptr != NULL;
            if(dataptr)
                ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->MaxErr)), connType,
                                             QGenericArgument(in_type, &value));
            else
                perr("QTangoizerPrivate::autoConfigure: unsupported data type \"%s\"", in_type);
            if(dataptr)
                m_DeleteDataPtr(in_type, dataptr);
        }
    }
    if(cp->stdUnitIsSet() && autoConfSlotsHash.contains(q->StdUnit))
    {
        methodName = autoConfSlotsHash.value(q->StdUnit);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            u = cp->stdUnit();
            ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->StdUnit)), connType,
                                             QGenericArgument(in_type, &u));
        }
    }
    if(cp->unitIsSet() && autoConfSlotsHash.contains(q->Unit))
    {
        methodName = autoConfSlotsHash.value(q->Unit);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            u = cp->unit();
            ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->Unit)), connType,
                                             QGenericArgument(in_type, &u));
        }
    }
    if(cp->displayUnitIsSet() && autoConfSlotsHash.contains(q->DisplayUnit))
    {
        methodName = autoConfSlotsHash.value(q->DisplayUnit);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            u = cp->displayUnit();
            ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->DisplayUnit)), connType,
                                             QGenericArgument(in_type, &u));
        }
    }
    if(autoConfSlotsHash.contains(q->Label))
    {
        methodName = autoConfSlotsHash.value(q->Label);
        extractCode(methodName);
        if(inTypeOfMethod(methodName, object, in_type))
        {
            u = cp->label();
            ret &= QMetaObject::invokeMethod(object, qstoc(autoConfSlotsHash.value(q->Label)), connType,
                                             QGenericArgument(in_type, &u));
        }
    }

    if(!ret)
        perr("QTWatcher::attributeAutoConfigured: one or more invokeMethod() failed");
    return ret;
}

void *QTangoizerPrivate::m_getDataPtr(const char *type, double val)
{
    void *dataptr = NULL;
    if(strcmp(type, "int") == 0)
    {
        int *i = new int[1];
        *i = (int) val;
        dataptr = i;
    }
    else if(strcmp(type, "double") == 0)
    {
        double *d = new double[1];
        *d = val;
        dataptr = d;
    }
    else if(strcmp(type, "unsigned int") == 0)
    {
        unsigned int *ui = new unsigned int[1];
        *ui = (unsigned int) val;
        dataptr = ui;
    }
    else if(strcmp(type, "float") == 0)
    {
        float *f = new float[1];
        *f = val;
        dataptr = f;
    }
    else if(strcmp(type, "short") == 0)
    {
        short *s = new short[1];
        *s = val;
        dataptr = s;
    }
    else if(strcmp(type, "unsigned short") == 0)
    {
        unsigned short *us = new unsigned short[1];
        *us = (unsigned short) val;
        dataptr = us;
    }
    return dataptr;
}

void QTangoizerPrivate::m_DeleteDataPtr(const char *type, void *dataptr)
{
    if(strcmp(type, "int") == 0)
        delete (int *) dataptr;
    else if(strcmp(type, "double") == 0)
        delete (double *) dataptr;
    else if(strcmp(type, "unsigned int") == 0)
         delete (unsigned int *) dataptr;
    else if(strcmp(type, "float") == 0)
        delete (float *) dataptr;
    else if(strcmp(type, "short") == 0)
        delete (short *) dataptr;
    else if(strcmp(type, "unsigned short") == 0)
        delete (unsigned short *) dataptr;
}

int QTangoizerPrivate::extractCode(QString &method)
{
    if(method.size() > 1)
    {
      int code = QString(method.at(0)).toInt();
      if(code > QMETHOD_CODE && code <= QSIGNAL_CODE)
          method.remove(0, 1);
    }
    return -1;
}

