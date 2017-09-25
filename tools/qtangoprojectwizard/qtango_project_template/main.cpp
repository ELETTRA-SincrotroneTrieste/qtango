#include "$HFILE$"
#include <TApplication>
#include <tutil.h>
#include <X11/Xlib.h>
#include <QX11Info>
#include <elettracolors.h>

#define CVSVERSION "$Name:  $"


int main(int argc, char *argv[])
{

    TApplication a( argc, argv );
    a.setOrganizationName("$ORGANIZATION_NAME$");
    a.setApplicationName("$APPLICATION_NAME$");
    QString version(CVSVERSION);
    a.setApplicationVersion(version);
    a.setProperty("author", "$AUTHOR$");
    a.setProperty("mail", "$AU_EMAIL$");
    a.setProperty("phone", "$AU_PHONE$");
    a.setProperty("office", "$AU_OFFICE$");
    a.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */
    
    TUtil::instance()->setLoggingTarget(argv[0]);

    $MAINCLASS$ w;
    if(w.property("epalette").isValid())
    	a.setPalette(EPalette(w.property("epalette").toString()));
    w.show();

    /* register to window manager */
    Display *disp = QX11Info::display();
    Window root_win = (Window) w.winId();
    XSetCommand(disp, root_win, argv, argc);

    return a.exec();
}
