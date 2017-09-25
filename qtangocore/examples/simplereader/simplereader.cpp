#include "simplereader.h"
#include <stdlib.h>
#include <QApplication>
#include <QString>
#include <signal.h>
#include <QtDebug>
#include <math.h>
#include <timagedata.h>
#include <timagedata_impl.h>

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

#include <string>

#define NSTEPS 100

void signal_handler(int signum);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString source;

    if(qApp->arguments().size() < 2)
    {
        printf("\e[1;31mUsage\e[0m: %s  "
               "test/device/member/attribute_name\n", qApp->arguments().at(0).toStdString().c_str());
        exit(EXIT_FAILURE);
    }
    source = qApp->arguments().at(1);

    /* here the QTangoCore interesting part */

    /* create reader, passing the qApp pointer as argument .
     * This is important if you want your writer deleted (and so
     * tango-disconnected at program shutdown
     */
    QVector<QObject *> readers;
    for(int i = 0; i < 1; i++)
    {
        QObject *parentO = new QObject(&a);
        SimpleReader *reader = new SimpleReader(parentO);
        /* default TUtil trigger is set to 1 second.
      * Should we refresh faster than that, it is advised to
      * set the globalRefreshInterval with a lower number.
      * The following two instructions are optional: if you
      * do not provide them, the periods are automatically set
      * to one second, which is a reasonable value for the most
      * cases.
      */
        TUtil::instance()->setGlobalRefreshInterval(380); /* optional */
        reader->setPeriod(1000); /* read refresh interval (optional) */
        reader->setAutoConfiguration(true, true);
        /* set the source point to read */
        reader->setSource(source);
        readers << parentO;
    }
    printf("deleting readers\n");
    // foreach(QObject *o, readers)
    //	delete o;
    // reader->unsetSource();
    /* end of qtangocore interesting part */

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    return a.exec();
}


SimpleReader::SimpleReader(QObject *parent) : QObject(parent),
    QTangoComProxyReader(parent)
{
    setAutoConfiguration(true); /* for init() */
    d_configured = false; /* needed by the formatResult */
    /* essential: to receive data updates */
    connect(qtangoComHandle(), SIGNAL(newData(const TVariant &)), this,
            SLOT(refresh(const TVariant&)));
    /* for init() - optional - */
    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(
                                          const TangoConfigurationParameters *)),
            this, SLOT(init(const TangoConfigurationParameters *)));
}

void SimpleReader::refresh(const TVariant &v)
{
    double readVal;
    if(v.canConvertToDoubleImage())
    {
        printf("read:\n");
        TImageData<double> img = v.toDoubleImage();
        img.print();
        printf("write:\n");
        img = v.toDoubleImage(false);
        img.print();
    }
    else if(v.canConvertToDouble() || v.canConvertToState())
    {
        readVal = v.toDouble();
        formatResult(v);
    }
    else
    {
        printf("\n  \e[1;4;31mcannot convert TVariant into double\e[0m!\n"
               "  make sure that the attribute provided is convertable into\n"
               "  a double value.\n\e[1;35m  Exiting in two seconds...\e[0m\n\n");
        sleep(1);
        printf("\e[1;35m  Exiting in one second...\e[0m\n\n");
        sleep(1);
        QCoreApplication::exit(EXIT_FAILURE);
    }

}

/* optional */
void SimpleReader::init(const TangoConfigurationParameters *cp)
{
    printf("\e[1;32m*\e[0m configuring the warn and error thresholds\n");
    minWarn = cp->minWarning();
    maxWarn = cp->maxWarning();
    maxError = cp->maxError();
    minError = cp->minError();
    unit = cp->stdUnit();
    maxVal = cp->maxValue();
    minVal = cp->minValue();
    double excursion = fabs(maxVal - minVal);
    int mW, MW, mE, ME, step, i;
    mW = NSTEPS * fabs(minVal - minWarn) /excursion;
    MW = NSTEPS * fabs(minVal - maxWarn)/excursion;
    mE = NSTEPS * fabs(minVal - minError)/excursion;
    ME = NSTEPS * fabs(minVal - maxError)/excursion;
    d_configured = cp->maxIsSet() && cp->minIsSet() && cp->mErrIsSet() && cp->MErrIsSet() &&
            cp->mWarnIsSet() && cp->MWarnIsSet();
    if(d_configured)
    {
        printf("\n");
        printf("%s: |", qApp->arguments().at(1).split('/').last().toStdString().c_str());
        for(int i = 0; i <= NSTEPS; i++)
        {
            if(i == mE)
                printf("\e[1;31m|"); /* red */
            else if(i == mW)
                printf("\e[1;33m|"); /* yellow */
            else if(i == MW)
                printf("\e[1;33m|");
            else if(i == ME)
                printf("\e[1;31m|");
            else
                printf("\e[0m ");
        }
        printf("| \t\t[Thresholds]\n");
    }
}

/* no qtangocore at all here! Just nice displaying */
void SimpleReader::formatResult(const TVariant &v)
{
    AttributeInfo ai;
    double d;
    bool v_outOfRange = false;
    if(v.canConvertToState())
    {
        Tango::DevState st = v.toState();
        QString state = Config::instance()->stateString(st);
        printf("\e[1;32m* \e[0mstate: %s\n", qstoc(state));
        return;
    }
    d = v.toDouble();
    std::string attName;
    attName = v.getAttributeInfo().name;

    if(d_configured)
    {
        double excursion = fabs(maxVal - minVal);
        int mW, MW, mE, ME, step, i;
        mW = NSTEPS * fabs(minVal - minWarn) /excursion;
        MW = NSTEPS * fabs(minVal - maxWarn)/excursion;
        mE = NSTEPS * fabs(minVal - minError)/excursion;
        ME = NSTEPS * fabs(minVal - maxError)/excursion;

        //      printf("min val: %.2f min warn: %.2f max val: %.2f mW : %d\n",
        // 	     minVal, minWarn, maxVal, mW);

        if(d <= maxVal && d >= minVal)
            step = NSTEPS * fabs((d - minVal)) / excursion;
        else
        {
            step = NSTEPS;
            v_outOfRange = true;
        }
        printf("\r");
        for(i = 0; i < NSTEPS + strlen(attName.c_str()) + 20; i++)
            printf(" ");
        printf("\r");
        fflush(stdout);
        printf("%s: |", attName.c_str());
        if(!v_outOfRange)
        {
            for(i = 0; i <= step; i++)
            {
                if(i <= mE)
                    printf("\e[1;31m="); /* red */
                else if(i <= mW && i >= mE)
                    printf("\e[1;33m="); /* yellow */
                else if(i > mW && i < MW)
                    printf("\e[1;32m="); /* green */
                else if(i >= MW && i < ME)
                    printf("\e[1;33m=");
                else if(i >= ME)
                    printf("\e[1;31m=");
            }
            printf("\e[0m"); /* reset color */
            for(i = step+1; i <= NSTEPS; i++)
                printf(" ");
        }
        else
            printf("Warning: value %.2f out of min and max range: [%.2f - %.2f] ", d, minVal, maxVal);

        printf("| %.2f\t[%s]", d, unit.toStdString().c_str());
    }
    else
    {
        printf("\r                                                          \r");
        printf("%s: %.2f\t[%s]", attName.c_str(), d, unit.toStdString().c_str());
    }
    fflush(stdout);
}

void signal_handler(int signum)
{
    printf("\n");
    if(signum == SIGINT || signum == SIGTERM)
        QCoreApplication::exit(0);
    else
        QCoreApplication::exit(EXIT_FAILURE);
}


