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

/* $Id: ttable.cpp,v 1.31 2012-08-10 13:00:24 giacomo Exp $ */
#include "ttable.h"
#include <eflag.h>

#include <QtPlugin>
#include <QVector>
#include <elettracolors.h>
#include <QtDebug>

TTable::TTable(QWidget *parent) : EFlag(parent), QTangoComProxyReader(this), QTangoWidgetCommon(this)
{
    /* a TTable user might need auto configuration information, so enable it */
    setAutoConfiguration(true);
    setHideEventEnabled(true); /* generally enabled for readers */
    setViewTrendEnabled(false);

    foreach(ELabel *l, cells)
        l->setValue(QString("No Link"));
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setDesiredAttributePropertyList(QStringList() << "numRows" << "numColumns" <<
                                    "displayMask" << "trueStrings" << "trueColours" << "falseColours"
                                    << "falseStrings");
    connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)),
            Qt::DirectConnection);
    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), this, SLOT(configure(const TangoConfigurationParameters *)));
}

TTable::~TTable()
{

}

void TTable::disabled()
{
    setDisabled(true);
    QPalette palette;
    palette.setColor(backgroundRole(), EDARK_GRAY);
    for (int i = 0; i < cells.size(); i++)
    {
        if (cells[i]->palette().color(backgroundRole()) != EDARK_GRAY)
        {
            cells[i]->setPalette(palette);
            cells[i]->display();
        }
    }
    foreach (ELabel *it, cells)
        it->setValue(QString("######"));
}

void TTable::enabled()
{
    if(!isEnabled())
        setDisabled(false);
}

void TTable::refresh(const TVariant &v )
{
    qDebug() << "TTable::refresh <<";
    setReaderToolTip(v.message());
    if (v.quality() == ATTR_INVALID)
    {
        disabled();
        return;
    }

    if (v.canConvertToUInt())
    {
        enabled();
        qDebug() << "TTable: converting to UInt" << v.toUInt();
        EFlag::setValue(QVariant(v.toUInt()));
    }

    else if (v.canConvertToBoolVector())
    {
        enabled();
        QVector<bool> bv = v.toBoolVector();
        qDebug() << bv;
        QVector<bool>::const_iterator it;
        QList<QVariant> l;
        for (it = bv.begin(); it < bv.end(); it++)
            l << QVariant(*it);
        qDebug() << l;
        EFlag::setValue(QVariant(l));
    }
    else if (v.canConvertToStringVector())
    {
        enabled();
        QColor col;
        QPalette palette;
        switch (v.quality())
        {
        case ATTR_VALID:
            col = Qt::white;
            break;
        case ATTR_ALARM:
            col = EColor(Elettra::red);
            break;
        case ATTR_WARNING:
        case ATTR_CHANGING:
            col = EColor(Elettra::darkYellow);
            break;
        case ATTR_INVALID:
            break;
        }
        palette.setColor(backgroundRole(), col);

        QVector<QString> data = v.toStringVector();
        qDebug() << "TTable: got data";
        qDebug() << "cells: " << cells.size() << " data: " << data.size();
        /*		vector<string>::const_iterator itString;
        QList<ELabel*>::const_iterator itCells;
        int i = 0;
        for (itString = data.begin(), itCells = cells.begin(); itString < data.end(), itCells < cells.end(); itString++, itCells++)
        {
            MYLOG("i: " << i << endl);i++;
            MYLOG("TTable: " << *itString << endl);
            (*itCells)->setPalette(palette);
            (*itCells)->setValue(QVariant(QString::fromStdString(*itString)));
            MYLOG("ok" << endl);
        }*/
        int size = qMin(cells.size(),(int)data.size());
        for (int i = 0; i < size; i++)
        {
            cells[i]->setValue(QVariant(data[i]), false);
            if (cells[i]->palette().color(backgroundRole()) != col)
            {
                qDebug() << "cambio palette";
                cells[i]->setPalette(palette);
            }
            cells[i]->display();
        }
        qDebug() << "exit";
    }
    else
    {
        setReaderToolTip("wrong data type");
        disabled();
    }

}

void TTable::configure (const TangoConfigurationParameters * cp) 
{
    if(!autoConfiguration())
        return;
    if(cp->descriptionIsSet())
    {
        setWhatsThis(cp->description());
    }
    if(cp->propertyFound("numRows") && cp->attributePropertyValue("numRows").size() == 1)
    {
        bool ok;
        int numRows = cp->attributePropertyValue("numRows").first().toUInt(&ok);
        if(ok && numRows > 0)
        {
            pinfo("TTable: setting number of rows %d", numRows);
            EFlag::setNumRows(numRows);
        }
        else
            perr("TTable: wrong value \"%s\" for attribute property \"numRows\"",
                 qstoc(cp->attributePropertyValue("numRows").first()));
    }
    if(cp->propertyFound("numColumns") && cp->attributePropertyValue("numColumns").size() == 1)
    {
        bool ok;
        int numColumns = cp->attributePropertyValue("numColumns").first().toUInt(&ok);
        if(ok && numColumns > 0)
        {
            pinfo("TTable: setting number of columns %d", numColumns);
            EFlag::setNumColumns(numColumns);
        }
        else
            perr("TTable: wrong value \"%s\" for attribute property \"numColumns\"",
                 qstoc(cp->attributePropertyValue("numColumns").first()));
    }
    if(cp->propertyFound("displayMask") && cp->attributePropertyValue("displayMask").size())
    {
        EFlag::setDisplayMask(cp->attributePropertyValue("displayMask").join(",")); /* EFlag display mask wants `,' */
        /* need to refresh immediately */
        refresh(cp->currentValue());
    }
    if(cp->propertyFound("trueStrings") && cp->propertyFound("falseStrings"))
    {
        int trueStringsNo = cp->attributePropertyValue("trueStrings").size();
        int falseStringsNo = cp->attributePropertyValue("falseStrings").size();
        if((trueStringsNo == falseStringsNo) && trueStringsNo > 0)
        {
            EFlag::setTrueStrings(cp->attributePropertyValue("trueStrings").join(";"));
            EFlag::setFalseStrings(cp->attributePropertyValue("falseStrings").join(";"));
            pinfo("TTable: setting true strings %s", qstoc(cp->attributePropertyValue("trueStrings").join(";")));
            pinfo("TTable: setting false strings %s", qstoc(cp->attributePropertyValue("falseStrings").join(";")));
        }
        else if(trueStringsNo != 0 || falseStringsNo != 0)
        {
            perr("TTable::configure: table \"%s\" connected to \"%s\" has true strings and false strings different in size: %d/%d",
                 qstoc(objectName()), qstoc(source()), trueStringsNo, falseStringsNo);
            perr("TTable: trueStrings and falseStrings attribute properties must have the same number of elements");
        }
    }
    if(cp->propertyFound("trueColours") && cp->propertyFound("falseColours"))
    {
        int trueColoursNo = cp->attributePropertyValue("trueColours").size();
        int falseColoursNo = cp->attributePropertyValue("falseColours").size();
        if((trueColoursNo == falseColoursNo) && trueColoursNo > 0)
        {
            QList<QColor> trueColors;
            QList <QColor> falseColors;
            foreach(QString s, cp->attributePropertyValue("trueColours"))
                trueColors << EColor(s);
            foreach(QString s, cp->attributePropertyValue("falseColours"))
                falseColors << EColor(s);
            EFlag::setTrueColorList(trueColors);
            EFlag::setFalseColorList(falseColors);
            pinfo("TTable: setting true colours %s", qstoc(cp->attributePropertyValue("trueColours").join(";")));
            pinfo("TTable: setting false colours %s", qstoc(cp->attributePropertyValue("falseColours").join(";")));
        }
        else if(trueColoursNo != 0 && trueColoursNo != 0)
        {
            perr("TTable::configure: table \"%s\" connected to \"%s\" has true colours and false colours different in size: %d/%d",
                 qstoc(objectName()), qstoc(source()), trueColoursNo, falseColoursNo);
            perr("TTable: trueColours and falseColours attribute properties must have the same number of elements");
        }
    }


    /* additional info displaying mask (since: 4.3.4) */
    /* first remove old information about display mask, if present */
    QString addInfo = this->additionalInfo();
    addInfo.remove(QRegExp("<!-- displayMask info -->.*<!-- end displayMask info -->"));
    setAdditionalInfo(addInfo);
    /* then proceed */
    QList< QList<unsigned int> >  dMask = displayMask();
    if(dMask.size())
    {
        QString displayMask;
        displayMask += "<!-- displayMask info -->";
        displayMask += "<h3>Display Mask</h3><ul>\n";
        for(int i = 0; i < dMask.size(); i++)
        {
            QList<unsigned int> nested = dMask.at(i);
            displayMask += "<li>";
            for(int j = 0; j < nested.size(); j++)
                displayMask += QString("%1").arg(nested.at(j)) + " ";
            displayMask += "</li>";
        }
        displayMask += "</ul><!-- end displayMask info -->";
        this->setAdditionalInfo(QString("<p>%1</p><p>%2</p>").arg(additionalInfo()).
                                arg(displayMask));
    }
}



