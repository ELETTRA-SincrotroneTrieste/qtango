#include "qtwatcher.h"
#include <macros.h>

QTWatcher::QTWatcher(QObject *parent) :
    QTangoizer(parent),
    QTangoComProxyReader(this)
{
    setAutoConfiguration(true);
    connect(qtangoComHandle(), SIGNAL(connectionErrorMessage(const QString&)),
            this, SIGNAL(connectionErrorMessage(const QString&)));
    connect(qtangoComHandle(), SIGNAL(connectionOk(bool)),
            this, SIGNAL(connectionOk(bool)));
    connect(qtangoComHandle(), SIGNAL(connectionFailed()), this, SIGNAL(connectionFailed()));

    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters*)),
            this, SLOT(slotAttributeAutoConfigured(const TangoConfigurationParameters*)));

    connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)),
            this, SLOT(refresh(const TVariant&)));
}

QTWatcher::~QTWatcher()
{

}

 void QTWatcher::refresh(const TVariant &v)
 {
     updateValue(v);
     if(singleShot())
         unsetSource();
     if(this->autoDestroy())
     {
         pinfo("auto destroying qtwatcher for \"%s\"", qstoc(realSource()));
         QTimer::singleShot(2000, this, SLOT(deleteLater()));
     }
 }
