#include "win_adapt.h"
#ifdef WIN32
#include <QDateTime>
#include <QTime>
#include <winsock.h>
 int  gettimeofday(timeval * tv, void *){
	QDateTime date = QDateTime::currentDateTime(); 
	QTime time = QTime::currentTime(); 
	tv->tv_sec = date.toTime_t(); 
	tv->tv_usec = 1000 * time.msec(); 
return 0; 
}

double exp10(double x){
	return pow(10,x); 
}
/*string ctime(const unsigned long * timer){
	QDateTime date = QDateTime::setTime_t(*timer); 
	return date.toString(); 

}*/


#endif
