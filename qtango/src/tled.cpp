/***************************************************************************
*   Copyright (C) 2008 by  Vincenzo Forchi`, Giacomo Strangolino	  *
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

/* $Id: tled.cpp,v 1.34 2011-03-07 10:40:10 giacomo Exp $ */
#include "tled.h"
#include <QtDebug>

TLed::TLed(QWidget *parent) : ELed(parent), QTangoComProxyReader(this), QTangoWidgetCommon(this),
    d_menu(NULL)
{
    d_trueColor  = EColor(Elettra::green);
    d_falseColor = EColor(Elettra::red);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setCommunicationStatusChangesWidgetEnable(false);
    setAutoConfiguration(true); /* one might need the signal in his code... */
    setHideEventEnabled(true); /* generally enabled for readers */
    setColor(Config::instance()->stateColor(Tango::UNKNOWN));
    connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)));
    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)),
            this, SLOT(autoConfigure(const TangoConfigurationParameters *)));
}

TLed::~TLed()
{

}

void TLed::setStateColors(QVector<QColor> &v)
{
    stateColors = v;
}

void TLed::setStateColor(DevState s, QColor c)
{
    /* if s is greater than the size of the vector, then resize it
     */
    if(stateColors.size() < (int) s + 1)
        stateColors.resize((int) s + 1);
    stateColors[s] = c;
}

void TLed::refresh(const TVariant &v)
{
    qDebug() << QThread::currentThread() << ":" << this << "- refresh() BEGIN";
    setReaderToolTip(v.message());
    if (v.quality() == ATTR_INVALID)
    {
        if(isEnabled()) /* disable widget if attribute is invalid */
            setDisabled(communicationStatusChangesWidgetEnable());
        if(stateColors.size() > Tango::UNKNOWN)
            setColor(stateColors.at(Tango::UNKNOWN));
        else
            setColor(Config::instance()->stateColor(Tango::UNKNOWN));
    }
    else
    {
        if(!isEnabled()) /* previously disabled because attribute invalid */
            setDisabled(communicationStatusChangesWidgetEnable());
        if (v.canConvertToState())
        {
            Tango::DevState sta = v.toState();
            if(stateColors.size() > sta)
                setColor(stateColors.at(sta));
            else if(sta < Config::instance()->numStates())
                setColor(Config::instance()->stateColor(sta));
            else
                perr("TLed::refresh() (state conversion): state %d not existing", sta);
        }
        else if(v.canConvertToBool())
        {
            if (v.toBool())
                setColor(d_trueColor);
            else
                setColor(d_falseColor);
        }
        else if(v.canConvertToInt())
        {
            int val = v.toInt();
            if(stateColors.size() > val)
                setColor(stateColors.at(val));
            else
                perr("TLed::refresh(): (int conversion): stateColors size less than %d: not changing color", val);
        }
        else
        {
            if(isEnabled()) /* disable widget if attribute is invalid */
                setDisabled(communicationStatusChangesWidgetEnable());
            setReaderToolTip("wrong data type");
            setColor(Config::instance()->stateColor(Tango::UNKNOWN));
            setDisabled(communicationStatusChangesWidgetEnable());
        }
    }
    qDebug() << QThread::currentThread() << ":" << this << "- refresh()  END";
}

void  TLed::autoConfigure(const TangoConfigurationParameters *cp)
{
    if (cp != NULL && cp->descriptionIsSet()) {
        setWhatsThis(cp->description());
    }
}




