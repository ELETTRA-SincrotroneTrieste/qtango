#include "tuniqueapplication.h"
#include <dbus/TApplicationInterface.h>
#include <dbus/tapplicationspoolproxy.h>
#include <private/tapplication_p.h>

TUniqueApplication::TUniqueApplication(int &argc, char **argv) : TApplication(true, argc, argv)
{
    initialize(); /* calls init() on TApplication: SIGINT, SIGTERM and private pointer */
    QObject::setProperty("unique", true);
}

int TUniqueApplication::exec()
{
    TAPP_D(TApplication);
    TApplicationInterface *tAppIface = d->getRunningApplicationInterface(executable(), parameters());
    if(tAppIface)
    {
        tAppIface->onSecondInstanceDetected();
        tAppIface->raise();
        /* Since TApplicationPrivate in this version (VER_MAJ < 5) inherits QObject,
         * its destructor is invoked. In the destructor of TApplicationPrivate, finiDbus
         * is called.
         */
        return EXIT_SUCCESS;
    }
    /* initialize dbus registering the application */
    printf("initializing DBUS...\n");
    initializeDBus();
    return QCoreApplication::exec();
}
