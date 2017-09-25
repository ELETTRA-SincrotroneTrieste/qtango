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

#include "simplewriter.h"
#include <stdlib.h>
#include <QApplication>
#include <QString>
#include <signal.h>
#include <math.h>
#include <string>
#include <QTimer>

#define NSTEPS 100

/* Manage program termination cleanly.
 * This is not necessary of course inside widget design
 */
void signal_handler(int signum); 

int main(int argc, char *argv[])
{
     QApplication a(argc, argv);
     QString target;
     SimpleWriter *writer = NULL;
     /* install the signal handler to correctly terminate the QTango communication */
     signal(SIGINT, signal_handler);
     signal(SIGTERM, signal_handler);
 
     /* just tell how it is expected to work */
     if(qApp->arguments().size() < 2)
     {
	       printf("\e[1;31mUsage\e[0m: %s"
		    "test/device/member/attribute_name\n");
	exit(EXIT_FAILURE);
     }
     
     pinfo("If you want to give a command, type the command line argument between \"\"");
     
     /* here the interesting part */
     target = qApp->arguments().at(1);
     /* create writer, passing the qApp pointer as argument .
      * This is important if you want your writer deleted (and so
      * tango-disconnected at program shutdown
      */
     writer = new SimpleWriter(&a);
     /* set targets */
     writer->setTargets(target);
     
     /* create a timer that each second makes a write on the device/command
      * or device/attribute specified.
      */
     QTimer *timer = new QTimer(&a);
     timer->setSingleShot(false);
     timer->setInterval(1000);
     /* execute the method write at every timeout */
     QObject::connect(timer, SIGNAL(timeout()), writer, SLOT(write()));
     timer->start();

     /* QApp main loop */
     return a.exec();
}


SimpleWriter::SimpleWriter(QObject *parent) : QObject(parent),
     QTangoComProxyWriter(parent)
{
     /* enable auto configuration (optional) */
     setAutoConfiguration(true);
     /* auto configure if possible, when ready */
     connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(
	  const TangoConfigurationParameters *)), 
	  this, SLOT(init(const TangoConfigurationParameters *))); 
	  connect(qtangoComHandle(), SIGNAL(newData(const TVariant &)), this,
		   SLOT(refresh(const TVariant&)));
    confSuccessful = false;      
}

void SimpleWriter::write()
{
     printf("writing on %s a random value between max and min allowed: %.2f and %.2f...",
	     qApp->arguments().at(1).toStdString().c_str(), maxVal, minVal);
     int r;
     double value, extension;
     if(confSuccessful) /* attribute info successfully configured the object */
	  extension = maxVal - minVal;
     else /* command or attribute not configured */
     {
	  minVal = 0.0;
	  extension = 255;
     }
     /* nothing qtangocore: just value generation */
     r = rand();
     value = (double) r / (double) RAND_MAX; /* between 0 and 1 */
     value = minVal + value * extension;
     printf(" %.2f...\n", value);
     /* this is all qtangocore you need */
     QVariant variant(value); 
     execute(variant);
     /* end qtango code */
}

void SimpleWriter::refresh(const TVariant &v)
{
     double readVal;
     if(v.canConvertToDouble())
     {
	  readVal = v.toDouble();
	  printf("\e[1;32mreadback\e[0m: %.2f\n", readVal);
     }
}


void SimpleWriter::init(const TangoConfigurationParameters *cp)
{
     double v;
     QString source = qApp->arguments().at(1);
     maxVal = cp->maxValue();
     minVal = cp->minValue();
     confSuccessful = true;
}

void signal_handler(int signum)
{
     /* it is important to invoke QCoreApplication::exit()
      * instead of standard exit() because QCoreApplication
      * takes care of the object destruction of the elements
      */
     if(signum == SIGINT || signum == SIGTERM)
	  QCoreApplication::exit(EXIT_SUCCESS);
     else
	  QCoreApplication::exit(EXIT_FAILURE);
}


