#ifndef QTANGO_MACROS_H
#define QTANGO_MACROS_H

#include <stdio.h>

#define QTANGOLIB_GET_VERSION()  (VER_MAJ * 10 + VER_MIN)

#define QTANGOLIB_VERSION(maj, min) (maj * 10 + min)

#include <stdlib.h>

	#ifdef QTANGO_PRINTINFO

#define pwarn(x, args...) do { if(getenv("QTANGO_PRINT")) {printf("\e[1;33m* \e[0;4mwarning\e[0m: "), printf(x, ##args), printf("\n"); } }while(0)

#define perr(x, args...) do {  if(!getenv("QTANGO_NOPRINT_ERRORS")) {fprintf(stderr, "\e[1;31m* \e[0;4merror\e[0m: "); printf(x, ##args); printf("\n");} }while(0)

#define pinfo(x, args...) do {  if(getenv("QTANGO_PRINT")) {printf("\e[1;35m* \e[0;4minfo\e[0m: "), printf(x, ##args), printf("\n"); } }while(0)

/* like pinfo but without newline */
#define pinfononl(x, args...) do {  if(getenv("QTANGO_PRINT")) {printf("\e[1;35m* \e[0;4minfo\e[0m: "), printf(x, ##args); } }while(0)

#define pok(x, args...) do {  if(getenv("QTANGO_PRINT")) {printf("\033[1;32m* \033[0m"), printf(x, ##args), printf("\n"); } }while(0)
		
#define padd(x, args...) do { if(getenv("QTANGO_PRINT")) {printf("\033[1;32m+ \033[0m"), printf(x, ##args), printf("\n"); } }while(0)
		
#define pwrn(x, args...) do { if(getenv("QTANGO_PRINT")) {printf("\033[1;33m* \033[0m"), printf(x, ##args), printf("\n"); } }while(0)

#define pdelete(x, args...) do {   if(getenv("QTANGO_PRINT")) {printf("\e[1;32mX \e[0m"), printf(x, ##args), printf("\n"); } }while(0)

#define pnodelete(x, args...) do { if(getenv("QTANGO_PRINT")) { printf("\e[1;31mX \e[0m"), printf(x, ##args), printf("\n"); } }while(0)
		
#define pstep(x, args...) do { if(getenv("QTANGO_PRINT")) { printf("   \e[0;37m-\e[0m "), printf(x, ##args), printf("\n"); } }while(0)
		
#define pstepl2(x, args ...) do {  if(getenv("QTANGO_PRINT")) { printf("    \e[0;37m-\e[0m "), printf(x, ##args), printf("\n"); } }while(0)

#define qprintf(x, args ...) do  {  if(getenv("QTANGO_QPRINTF")) {printf(x, ##args); } }while(0)
		
#define qstoc(x) 			 	x.toStdString().c_str() 
		
#define qslisttoc(x)	do { if(getenv("QTANGO_PRINT")) { printf("  { "); for(int i = 0; i < x.size(); i++) printf("\"%s\", ", qstoc(x[i])); printf("\b\b }\n"); } }while(0)
		
#define qobjinfo(o)			o->metaObject()->className()
		
		#define qobjname(o)			o->objectName().toStdString().c_str()
		
		#define objinfo(o) \
		      QString("%1 \"%2\"").arg(((o != NULL && o->metaObject() != NULL && \
			o->metaObject()->className() != NULL) ?  o->metaObject()->className() \
			: QString("object or meta object is null\n   - Please create a proxy reader or writer passing a QObject not null to obtain class name and object name here.\n  "))) \
			.arg(((o != NULL) ?  (o->objectName()) :  \
			 QString("object is null"))).toStdString().c_str()
		

	#else

		#define pwarn(x, args...) 		do {}while(0)

		#define perr(x, args...) 			do {}while(0)

		#define pinfo(x, args...) 			do {}while(0)
		  
		#define pinfononl(x, args...)    do {}while(0)

		#define pok(x, args...) 			do {}while(0)
		
		#define pwrn(x, args...) 			do {}while(0)

		#define pdelete(x, args...) 		do {}while(0)
		
		#define padd(x, args...) 	do {}while(0)

		#define pnodelete(x, args...) 		do {}while(0)
		
		#define qstoc(x) 				""
		
		#define qobjinfo(o)			""
		
		#define qobjname(o)			""
		
		#define objinfo(o)			""
		
		#define pstep(x, args...) 		do {}while(0)
		
		#define pstepl2(x, args...) 		do {}while(0)
		
		#define qslisttoc(x) 			do {}while(0)
		  
		#define qprintf(x, args...)		do {}while(0)
		

	#endif

#endif

