/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	   *
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

/* $Id: tcombobox.cpp,v 1.20 2012-01-31 15:06:52 giacomo Exp $ */

#include "tcombobox.h"

TComboBox::TComboBox(QWidget *parent) : QComboBox(parent), SimpleDataProxy(), QTangoComProxyWriter(this),
    QTangoWidgetCommon(this)
{
    d_indexMode = false;
    setAutoConfiguration(true);
    setProperty("indexOffset", 0);
    setDesiredAttributePropertyList(QStringList() << Config::instance()->valuesAttributePropertyName());
    /// 	setHideEventEnabled(false); /* disabled for writers */
    connect(qtangoComHandle(),
            SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)),
            this, SLOT(init(const TangoConfigurationParameters *)));
}

QString TComboBox::getData()
{
    if(d_indexMode)
    {
        int index;
        /* first test indexMap, THEN indexOffset.
         * This allows to always setProperty("indexOffset", 0) in order to create a
         * free property which is editable by the designer.
         * So, even if the Qt designer sets up the indexOffset property, the indexMap one
         * has higher priority.
         */
        if(!this->property("indexMap").isNull())
        {
            QMap<QString, QVariant> map = this->property("indexMap").toMap();
            if(map.isEmpty() || !map.contains(this->currentText()))
            {
                perr("TComboBox \"%s\": map empty or \"%s\" not mapped: returning index %d",
                     qstoc(objectName()), qstoc(this->currentText()), currentIndex());
                index = currentIndex();
            }
            else
                index = map.value(this->currentText()).toInt();
        }
        else if(!this->property("indexOffset").isNull())
            index = currentIndex() + this->property("indexOffset").toInt();
        else
            index = currentIndex();

        return QString("%1").arg(index);
    }
    else
        return currentText();
}

void TComboBox::setTargets(QString s)
{
    if(s != targets())
    {
        clear();
        QTangoComProxyWriter::setTargets(s);
    }
}

void TComboBox::init(const TangoConfigurationParameters *cp)
{
    QMap<QString, QVariant> map;

    if(cp->hasValues())
    {
        /* save the items to be mapped. Use a separate list to avoid keys()
         * sorting in map
         */
        QStringList items;
        clear();
        foreach(QString s, cp->values())
        {
            if(s.count("::") == 1)
            {
                QStringList parts = s.split("::", QString::SkipEmptyParts);
                if(parts.size() == 2)
                {
                    map.insert(parts.first(), parts.last().toInt());
                    items << parts.first();
                }
            }
            else /* all elements must have :: separator */
            {
                map.clear(); /* otherwise clear the whole map */
                break; /* and leave the map empty */
            }
        }
        if(!map.isEmpty())
        {
            setProperty("indexMap", map);
            addItems(items);
        }
        else
            addItems(cp->values());
    }

    /* initialize combo with current value, if pertinent */
    TVariant value = cp->currentValue();
    if(value.canConvertToInt())
    {
        int currentVal = value.toInt(false); /* set point */
        if(!map.isEmpty())
        {
            foreach(QVariant v, map.values())
            {
                if(v.toInt() == currentVal)
                {
                    this->setCurrentIndex(this->findText(map.key(v)));
                    break;
                }
            }
        }
        else
        {
            int offset = 0;
            if(property("indexOffset").isValid())
                offset = property("indexOffset").toInt();
            int setPointIndex = currentVal - offset;
            if(setPointIndex > -1 && setPointIndex < this->count())
                setCurrentIndex(setPointIndex);
            else
                setCurrentIndex(0);
        }
    }

    QString desc = "";
    if ((cp != NULL && cp->descriptionIsSet())) {
        desc.append(cp->description());
    }
    if(cp != NULL && cp->maxIsSet() && cp->minIsSet()) {
        desc.append("\n(min: "+ QString("%1").arg(cp->minValue()) + " max: "+ QString("%1").arg(cp->maxValue()) + ")");
    }
    setWhatsThis(desc);
}

void TComboBox::execute(const QString &data)
{
    QTangoComProxyWriter::execute(data);
}

QStringList TComboBox::items() const
{
    QStringList ret;
    for(int i = 0; i < count(); i++)
        ret << itemText(i);
    return ret;
}

void TComboBox::setItems(const QStringList &items)
{
    QComboBox::clear();
    QComboBox::insertItems(0, items);
}


