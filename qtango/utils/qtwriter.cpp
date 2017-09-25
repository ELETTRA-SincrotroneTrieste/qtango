#include "qtwriter.h"
#include "qtangoizerprivate.h" /* for TYPELEN */
#include <QWidget> /* for static cast and tooltip */
#include <QDateTime>
#include <macros.h>

QTWriter::QTWriter(QObject *parent) : QTangoizer(parent), QTangoComProxyWriter(this)
{
    setAutoConfiguration(true);
    connect(qtangoComHandle(), SIGNAL(connectionErrorMessage(const QString&)),
            this, SIGNAL(connectionErrorMessage(const QString&)));
    connect(qtangoComHandle(), SIGNAL(connectionOk(bool)),
            this, SIGNAL(connectionOk(bool)));
    connect(qtangoComHandle(), SIGNAL(connectionFailed()), this, SIGNAL(connectionFailed()));

    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters*)),
            this, SLOT(slotAttributeAutoConfigured(const TangoConfigurationParameters*)));

    /* to test the success of the execution */
    connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)),
            this, SLOT(refresh(const TVariant&)));
}

QTWriter::~QTWriter()
{

}

void QTWriter::attach(QObject *executor,  const char *signal,
                      const char *setPointSlot, Qt::ConnectionType connType)
{
    char in_type[TYPELEN];
    QTangoizer::attach(executor, signal, setPointSlot, connType);
    QString method = QString(signal);
    /* remove the signal code at the beginning of the method */
    extractCode(method);
    if(inTypeOfMethod(qstoc(method), executor, in_type))
    {
        printf("\e[1;32mQTWriter::attach: detected in_type \"%s\" method \"%s\" obj \"%s\"\e[0m\n",
               in_type, qstoc(method), qstoc(executor->objectName()));
        bool connectOk = false;

        if(strlen(in_type) == 0)
            connectOk = connect(executor, signal, this, SLOT(execute()));
        else  if(strcmp(in_type, "QStringList") == 0)
            connectOk = connect(executor, signal, this, SLOT(execute(const QStringList&)));
        else  if(strcmp(in_type, "QVector<double>") == 0)
            connectOk = connect(executor, signal, this, SLOT(execute(const QVector<double>&)));
        else  if(strcmp(in_type, "QVector<int>") == 0)
            connectOk = connect(executor, signal, this, SLOT(execute(const QVector<int>&)));
        else  if(strcmp(in_type, "QVector<bool>") == 0)
            connectOk = connect(executor, signal, this, SLOT(execute(const QVector<bool>&)));
        else if(!strcmp(in_type, "int"))
            connectOk = connect(executor, signal, this, SLOT(execute(int)));
        else if(!strcmp(in_type, "bool"))
            connectOk = connect(executor, signal, this, SLOT(execute(bool)));
        else if(!strcmp(in_type, "double"))
            connectOk = connect(executor, signal, this, SLOT(execute(double)));
        else if(!strcmp(in_type, "short"))
            connectOk = connect(executor, signal, this, SLOT(execute(short)));
        else if(!strcmp(in_type, "unsigned int"))
            connectOk = connect(executor, signal, this, SLOT(execute(unsigned int)));
        else if(strstr(in_type, "String") != NULL)
            connectOk =  connect(executor, signal, this, SLOT(execute(const QString&)));


        else
        {
            perr("QTWriter::attach: signal \"%s\" of object \"%s\" is not compatible with supported types",
                 qstoc(method), qstoc(executor->objectName()));
            perr("                  supported types: \"int\", \"unsigned int\", \"short\", \"double\", \"const QString&\"");
        }

        if(!connectOk)
        {
            perr("TWriter::attach: failed to connect signal \"%s\" of object \"%s\" to the ", 
                 qstoc(method), qstoc(executor->objectName()));
            perr("                 corresponding execute() slot");
        }
    }
    else
        printf("\e[1;31mQTWriter::attach: inTypeOfMethod failed, in_type \"%s\" method \"%s\" obj \"%s\"\e[0m\n",
               in_type, qstoc(method), qstoc(executor->objectName()));

}

void QTWriter::slotAttributeAutoConfigured(const TangoConfigurationParameters* cp)
{
    QTangoizer::slotAttributeAutoConfigured(cp);
    if(d_ptr->executeOnConnection())
    {
        printf("\e[1;32mexecuting scheduled execution for \"%s\"\e[0m\n", qstoc(realTargets()));
        QVariant arg = d_ptr->executeArgument();
        if(arg.isValid())
            QTangoComProxyWriter::execute(arg);
        else
            QTangoComProxyWriter::execute();

        postExecute();
    }
}

void QTWriter::postExecute()
{
    if(QTangoComProxyWriter::executionMode() == QTangoCommunicationHandle::Synchronous &&
            d_ptr->autoDestroy)
    {
        pinfo("auto destroying qtwriter for \"%s\"", qstoc(realTargets()));
        QTimer::singleShot(2000, this, SLOT(deleteLater()));
    }
}

void QTWriter::execute(int i)
{
    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, i);
    else
    {
        QTangoComProxyWriter::execute(i);
        postExecute();
    }

}

void QTWriter::execute(short s)
{
    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, s);
    else
    {
        QTangoComProxyWriter::execute(s);
        postExecute();
    }
}

void QTWriter::execute(const QString& s)
{
    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, s);
    else
    {
        QTangoComProxyWriter::execute(s);
        postExecute();
    }
}

void QTWriter::execute(double d)
{
    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, d);
    else
    {
        QTangoComProxyWriter::execute(d);
        postExecute();
    }
}

void QTWriter::execute(unsigned int i)
{
    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, i);
    else
    {
        QTangoComProxyWriter::execute(i);
        postExecute();
    }
}

void QTWriter::execute(bool bo)
{
    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, bo);
    else
    {
        QTangoComProxyWriter::execute(bo);
        postExecute();
    }
}

void QTWriter::execute()
{
    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, QVariant());
    else
    {
        QTangoComProxyWriter::execute();
        postExecute();
    }
}

void QTWriter::execute(const QStringList& sl)
{
    QVariant v(sl);
    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, v);
    else
    {
        QTangoComProxyWriter::execute(v);
        postExecute();
    }
}

void QTWriter::execute(const QVector<int>& iv)
{
    QList<QVariant> vl;
    foreach(int i, iv)
        vl.push_back(i);

    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, vl);
    else
    {
        QTangoComProxyWriter::execute(vl);
        postExecute();
    }
}

void QTWriter::execute(const QVector<double>& dv)
{
    QList<QVariant> vl;
    foreach(double d, dv)
        vl.push_back(d);

    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, vl);
    else
    {
        QTangoComProxyWriter::execute(vl);
        postExecute();
    }
}

void QTWriter::execute(const QVector<bool>& bv)
{
    QList<QVariant> vl;
    foreach(bool b, bv)
        vl.push_back(b);

    if(!this->qtangoComHandle()->configured())
        d_ptr->setExecuteOnConnection(true, vl);
    else
    {
        QTangoComProxyWriter::execute(vl);
        postExecute();
    }
}



void QTWriter::refresh(const TVariant& v)
{
    if(data() != NULL && !methodName().isEmpty())
    {
        QObject *obj = static_cast<QObject *>(data());
        if(obj->inherits("QWidget"))
        {
            QString msg;
            if(v.executionFailed())
                msg += "<p><b>" + v.tangoPoint() + " execution failed</b></p>\n";
            else
                msg += "<p><b>" + v.tangoPoint() + " correctly executed</b></p>\n";
            msg += "<p>" + QDateTime::currentDateTime().toString() + "</p>\n";
            msg += v.message();
            static_cast<QWidget *>(obj)->setToolTip(msg);
        }
    }
}
