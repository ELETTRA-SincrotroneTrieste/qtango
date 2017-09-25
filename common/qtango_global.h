/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QTANGO_GLOBAL_H
#define QTANGO_GLOBAL_H

#include <qglobal.h>
#if QT_VERSION < 0x040000
#include <qmodules.h>
#endif



#if defined(Q_WS_WIN) || defined(Q_WS_S60)

	#if defined(_MSC_VER) /* MSVC Compiler */
	/* template-class specialization 'identifier' is already instantiated */
		#pragma warning(disable: 4660)
	#endif // _MSC_VER

	#ifdef QTANGO_DLL
		#if defined(QTANGO_MAKEDLL)     // create a Qwt DLL library 
			#define QTANGO_EXPORT  __declspec(dllexport)
			#define QTANGO_TEMPLATEDLL
		#else                        // use a Qwt DLL library
			#define QTANGO_EXPORT  __declspec(dllimport)
		#endif


		#if defined (QTCONTROLS_MAKEDLL)
			#define QTCONTROLS_EXPORT __declspec(dllexport)
		#else
			#define QTCONTROLS_EXPORT __declspec(dllimport)
		#endif

		#if defined (QTANGOCORE_MAKEDLL)
			#define QTANGOCORE_EXPORT __declspec(dllexport)
		#else
			#define QTANGOCORE_EXPORT __declspec(dllimport)
		#endif
	
		#ifndef QTANGO_EXPORT
			#define QTANGO_EXPORT __declspec(dllimport)
		#endif
		#ifndef QTCONTROLS_EXPORT
			#define QTCONTROLS_EXPORT __declspec(dllimport)
		#endif
		#ifndef QTANGOCORE_EXPORT
			#define QTANGOCORE_EXPORT __declspec(dllimport)
		#endif
	#endif // QTANGO_DLL

	
#endif // Q_WS_WIN || Q_WS_S60

#ifndef QTANGO_EXPORT
	#define QTANGO_EXPORT
#endif
#ifndef QTCONTROLS_EXPORT
	#define QTCONTROLS_EXPORT
#endif
#ifndef QTANGOCORE_EXPORT
	#define QTANGOCORE_EXPORT
#endif

// #define QTANGO_NO_COMPAT 1 // disable withdrawn functionality

// add support for timeval in windows
#ifdef WIN32
#include "win_adapt.h"
#else
#define TIMEVAL_ST struct timeval
#endif

#endif // QTANGO_GLOBAL_H
