/***************************************************************************
*   Copyright (C) 2008 by  Giacomo Strangolino   			   *
*   giacomo.strangolino@elettra.trieste.it   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef SIMPLE_READER_H
#define SIMPLE_READER_H
#include <QObject>
#include <QString>
#include <com_proxy_reader.h>

/* NOTE: see the comments on the simplewriter.h and simplewriter.cpp:
 * some of them are common to qtangocore system manipulation and are
 * not repeated here.
 */

/** A simple QTangoCore application which implements a reader.
 *  The real necessary things, as to QTangoCore, are the 
 *  constructor and the refresh() method, which the layer 
 *  requires compulsory.
 *
 *  SimpleReader must inherit from QObject (to have SIGNAL/SLOT 
 *  framework) and QTangoComProxyReader, to effectively read
 *  from the tango layer.
 */
class SimpleReaderMulti : public QObject, public QTangoComProxyReader
{
  Q_OBJECT
     public:
	  SimpleReaderMulti(QObject *parent);
	  
     protected slots:
	  void refresh(const TVariant &v);
	  
	  /* OPTIONAL: also for a writer it is possible to know the limits for a
	  * Tango device attribute.
	  */
	  void init(const TangoConfigurationParameters *);
	  
     private:
	  /* not a qtangocore related method */
	  void formatResult(const TVariant &v);
	  
	  /* what follows is completely optional and makes sense 
	  * if you make use of the SIGNAL `attributeAutoConfigured()'
	  * emitted by the qtangoComHandle. See the source in the 
	  * .cpp file to see how to use it.
	  */
	  double minWarn, maxWarn, minError, maxError;
	  double maxVal, minVal;
	  QString unit;
	  bool d_configured;
};


#endif
