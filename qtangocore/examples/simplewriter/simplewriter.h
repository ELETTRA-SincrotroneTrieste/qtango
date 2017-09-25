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
#include <com_proxy_writer.h>

/** A simple QTangoCore application which implements a writer.
*  The real necessary things, as to QTangoCore, are the 
*  constructor and the getData() method, which the layer 
*  requires compulsory. Actually, the getData() here is 
*  really implemented because known unnecessary.
*
*  SimpleWriter must inherit from QObject (to have SIGNAL/SLOT 
*  framework) and QTangoComProxyWriter, to effectively read
*  from the tango layer.
*
*  It is possible to create both a reader and a writer by 
*  inheriting both from QTangoComProxyWriter and QTangoComProxyReader,
*  and implemented the pure virtual methods getData() and refresh()
*  respectively.
*  Have a look at QTango TCheckBox for an example of a reader/writer.
*
*/

class SimpleWriter : public QObject, public QTangoComProxyWriter
{
  Q_OBJECT /* QObject because we normally need SIGNAL/SLOT framework */
     public:
	  /* The constructor */
	  SimpleWriter(QObject *parent);
	  
	  /* NOTE: You do not need the destructor to unset source/targets if you call
	   * QCoreApplication::exit() at program shutdown.
	   */
	  
     protected slots:
	  /* in case of commands, the immediate read back is possible.
	   * in case of attributes, this is not.
	   */
	  void refresh(const TVariant &v);
	  
	  /* OPTIONAL: also for a writer it is possible to know the limits for a
	   * Tango device attribute.
	   * For commands this is not available through tango 
	   */
	  void init(const TangoConfigurationParameters *);
	  
	  /* The QTango framework usually needs an execution loop and so we chose for this
	   * simple example a method invoked every timer timeout.
	   * This simply executes the write, be it attribute or command.
	   * NOTE: if we had chosen for instance a while(1) { scanf; execute() }
	   * this infinite cycle would have prevented the QApplication main loop 
	   * to deliver SIGNALS and this would have caused misconfiguration of the 
	   * QTango objects.
	   * You must keep in mind this only when developing non Graphical objects.
	   * You needn't worry when designing GUIs, which are QApplication main loop
	   * managed.
	   */
	  void write();
	  
	  /* necessary for a writer: leave empty if we need not this */
	  QString getData() { return QString("Unused"); }
	  
     private:
	  /* this stores the value to write. It is not strictly necessary in this 
	  * simple example, but since the ProxyWriter is widget-oriented, it must
	  * implement the getData method to provide a string representation of the
	  * value to write. 
	  */
	  double d_value;
	  
	  /* what follows is completely optional and makes sense 
	  * if you make use of the SIGNAL `attributeAutoConfigured()'
	  * emitted by the qtangoComHandle. See the source in the 
	  * .cpp file to see how to use it.
	  * In this example, it suggests the user the range of values to
	  * insert.
	  */
	  double maxVal, minVal;
	  
	  /* in case we are treating attributes, they can be auto configured.
	   * In case of commands, this is not the case 
	   */
	  bool confSuccessful;
};


#endif
