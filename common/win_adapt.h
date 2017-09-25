#ifndef __WIN_ADAPT__
#define __WIN_ADAPT__



#ifdef WIN32
#include <QDateTime>
#include <QTime>
#include <QString>
#ifndef QTANGO_GLOBAL_H
#include "qtango_global.h" 
#include <math.h>
#endif

struct timeval; 

QTCONTROLS_EXPORT  int gettimeofday(timeval * tv, void *);

QTCONTROLS_EXPORT double exp10(double ); // 10^x


//QString ctime(const unsigned long * timer); 

#endif // win32

#endif
