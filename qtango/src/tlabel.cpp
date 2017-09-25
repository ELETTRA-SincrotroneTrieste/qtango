/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino, Vincenzo Forchi`		   *
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

/* $Id: tlabel.cpp,v 1.56 2014-08-26 14:55:11 giacomo Exp $ */

#include "tlabel.h"
#include <taction.h>
#include <tutil.h>
#include <QMouseEvent>
#include <QMenu>
#include <EApplicationLauncher>
#include <elettracolors.h>
#include <QtDebug>
#include <QPainter>

TLabel::TLabel(QWidget *parent, Qt::WindowFlags) : ELabel(parent),
QTangoComProxyReader(this),
QTangoWidgetCommon(this)
{
    setText("No Link");
    setProperty("forceNoUnit", false);
    setAttribute(Qt::WA_AlwaysShowToolTips);

    setViewTrendEnabled(true);
    setHelperApplicationEnabled(false);
    setHideEventEnabled(true); /* generally enabled for readers */

    connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)),
            Qt::DirectConnection);
    connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), this, SLOT(configure(const TangoConfigurationParameters *)));
    setAutoConfiguration(true);
    currentQuality = ATTR_INVALID;
    d_maxValue = d_minValue = 0.0;
    d_excursionEnabled = false;
    d_backgroundColorChangeEnabled = true;
    setBackgroundColorChangeEnabled(true);
    d_minE = d_maxE = 0.0;
    d_minW = d_maxW = 0.0;
    d_excursionDistFromMargin = 5;
    d_excursionWidth = 2;

    /* auto configuration for values attribute property (from: 4.3.4) */
    setProperty("configureEnumFromValuesProperty", false);
    setProperty("valuesPropertyOffset", 0);
    this->setDesiredAttributePropertyList(QStringList() << "values");
}

TLabel::~TLabel()
{

}

void TLabel::refresh(const TVariant &v)
{
    QColor color;
    Config *conf = Config::instance();
    if(v.message() != toolTip())
        setReaderToolTip(v.message());

    /* enable/disable widget according to quality */
    setEnabled(v.quality() != ATTR_INVALID);

    if(v.quality() != currentQuality || currentQuality == ATTR_INVALID)
    {
        currentQuality = v.quality();
        QPalette pal = palette();
	
        if(currentQuality == ATTR_INVALID)
        {
            if (palette().color(QPalette::Background) != Qt::white)
            {
                if (palette().color(backgroundRole()) != Qt::white)
                {
                    QPalette pal = palette();
                    pal.setColor(backgroundRole(), Qt::white);
                    d_setPalette(pal);
                }
            }

            setReaderToolTip(v.message());
            ELabel::setValue(QVariant(QString("#####")));
            return;
        }

        /* alarm and error thresholds configured */
        if((d_minE != d_maxE || d_minW != d_maxW) && v.canConvertToDouble())
        {
             double val = v.toDouble();
             if((d_minE != d_maxE) && (val <= d_minE || val >= d_maxE))
                 color = conf->qualityColor(ATTR_ALARM);
             else if((d_minW != d_maxW) && (val <= d_minW || val >= d_maxW))
                 color = conf->qualityColor(ATTR_WARNING);
             else
                 color =  conf->qualityColor(ATTR_VALID);
        }
        else /* not a number or thresholds not configured */
        {
            switch (currentQuality)
            {
            case ATTR_VALID:
                color = conf->qualityColor(ATTR_VALID);
                break;
            case ATTR_ALARM:
                color = conf->qualityColor(ATTR_ALARM);
                break;
            case ATTR_WARNING:
                color = conf->qualityColor(ATTR_WARNING);
                break;
            case ATTR_CHANGING:
                color = conf->qualityColor(ATTR_CHANGING);
                break;
            default:
                break;
            }
        }
        if ((pal.color(backgroundRole()) != color) && !enumDisplayConfigured())
        {
            qDebug() << QThread::currentThread() << ":" << this << "- refresh(): setting palette";
            pal.setColor(backgroundRole(), color);
            d_setPalette(pal);
        }
    } /* attrQuality has changed */

    if (v.canConvertToBool())
    {
        ELabel::setValue(QVariant(v.toBool()));
    }
    else if (v.canConvertToState())
    {
        DevState stato = v.toState();
        ELabel::setValue(QVariant(v.toString()), false);
        if (this->palette().color(backgroundRole()) != conf->stateColor(stato))
        {
            QPalette p = this->palette();
            p.setColor(backgroundRole(), conf->stateColor(stato));
            d_setPalette(p);
        }
        ELabel::display();
    }
    /* QTango 4.3.4: configureEnumFromValuesProperty code branch */
    else if(property("configureEnumFromValuesProperty").toBool() == true
            && v.canConvertToInt() && enumDisplayConfigured())
    {
        ELabel::setValue(v.toInt(true));
    }
    else if (v.canConvertToString())
    {
        /* if display unit is set, then the full string is passed to ELabel, included a standard unit,
		 * if set. 
		 */
        ELabel::setValue(QVariant(v.toString(true, property("forceNoUnit").toBool())));
    }
    else if (v.canConvertToStringVector())
    {
        QVector<QString> strings = v.toStringVector();
        QString str;
        foreach (QString s, strings)
            str += s + ",";
        str.remove(str.size() - 1, 1);
        ELabel::setValue(QVariant(str));
    }
    else
    {
        setReaderToolTip("wrong data type");
        ELabel::setValue(QVariant(QString("#####")));
    }
}

void TLabel::configure (const TangoConfigurationParameters * cp) 
{
    if(!autoConfiguration())
	return;

    if (cp != NULL && cp->descriptionIsSet()) 
    {
        setWhatsThis(cp->description());
    }
    if(cp != NULL && cp->maxIsSet() && cp->minIsSet())
    {
        d_maxValue = cp->maxValue();
        d_minValue = cp->minValue();
    }
    else
        d_maxValue = d_minValue = 0.0;

    if(cp->mErrIsSet() && cp->MErrIsSet())
    {
        d_minE = cp->minError();
        d_maxE = cp->maxError();
    }
    else
        d_minE = d_maxE = 0.0;

    if(cp->mWarnIsSet() && cp->MWarnIsSet())
    {
        d_minW = cp->minWarning();
        d_maxW = cp->maxWarning();
    }
    else
        d_minW = d_maxW = 0.0;

    /* auto configuration for values attribute property (from: 4.3.4) */
    if(property("configureEnumFromValuesProperty").toBool() == true && cp->hasValues())
    {
        int offset = property("valuesPropertyOffset").toInt();
        for(int i = 0; i < cp->values().size(); i++)
            this->setEnumDisplay(i + offset, cp->values().at(i), Qt::white);
    }

    if((d_minE != d_maxE || d_minW != d_maxW) && cp->currentValue().canConvertToDouble())
        setAdditionalInfo(QString("<h4>Note</h4><p>Alarm and/or warning thresholds are defined for \"%1\" and data type is a number: will use thresholds to determine label background colour.</p>")
                          .arg(realSource()));
}

void TLabel::paintEvent(QPaintEvent *e)
{
    ELabel::paintEvent(e);
    if(d_excursionEnabled && d_maxValue != d_minValue)
    {
	TVariant v = currentValue();
	if(v.canConvertToDouble())
	{
            TVariant v = currentValue();
            int lineMargin = d_excursionDistFromMargin; /* 1 pixel margin */
            double value = v.toDouble();
            double interval = d_maxValue - d_minValue;
            QPoint end, endMinErr, endMinWarn, endMaxWarn, endMaxErr;
            QPainter p(this);
            QColor lineColor(KBLUE);
            lineColor.setAlpha(127);
            QPen pen(QBrush(lineColor), d_excursionWidth);
            p.setPen(pen);
            int w = e->rect().width() - 4 * lineMargin;
            double x = (value - d_minValue) * w / interval;
            // 	printf("width %d x %f - value %f\n", w, x, value);
            QPoint start = rect().topLeft();
            /* detach from the margin */
            start.setX(start.x() + 2 * lineMargin);
            start.setY(start.y() + lineMargin);
            end = start;
            endMinErr =  endMinWarn =  endMaxWarn =  endMaxErr = start; /* suppose err and warn not set */
            end.setX(end.x() + x);
            // 	printf("line driven %s/%s\n", qstoc(objectName()), qstoc(source()));
            QPoint limit(start.x() + (d_maxValue - d_minValue) * w / interval, start.y());

            QPoint startGreen = start;
            QPoint endGreen = limit;

            qDebug() << d_minE << d_maxE << d_minW << d_maxW;
            /* draw the line */
            if(d_minE != d_maxE)
            {
		endMinErr.setX(endMinErr.x() + (d_minE - d_minValue) * w / interval);
		endMaxErr.setX(endMaxErr.x()  + (d_maxE - d_minValue) * w / interval);
		pen.setColor(KDARKRED);
		p.setPen(pen);
		p.drawLine(endMaxErr, limit);
		p.drawLine(start, endMinErr);
		startGreen = endMinErr;
		endGreen = endMaxErr;
            }
            if(d_maxW != d_minW)
            {
		endMinWarn.setX(endMinWarn.x() + (d_minW - d_minValue) * w / interval);
		endMaxWarn.setX(endMaxWarn.x()  + (d_maxW - d_minValue) * w / interval);
		pen.setColor(KVERYDARKYELLOW);
		p.setPen(pen);
		if(d_minE != d_maxE)
		{
                    p.drawLine(endMinErr, endMinWarn);
                    p.drawLine(endMaxWarn, endMaxErr);
		}
		else
		{
                    p.drawLine(start, endMinWarn);
                    p.drawLine(endMaxWarn, limit);
		}
		startGreen = endMinWarn;
		endGreen = endMaxWarn;
            }

            pen.setColor(KDARKGREEN);
            p.setPen(pen);
            p.drawLine(startGreen, endGreen);

            /* now draw a cross corresponding to the value */
            QPoint x1(end.x() - 2, end.y() - 2);
            QPoint x2(end.x() + 2, end.y() + 2);
            QPoint x3(end.x() + 2, end.y() - 2);
            QPoint x4(end.x() - 3, end.y() + 3);
            pen.setWidth(1.4);
            pen.setColor(Qt::black);
            p.setPen(pen);
            p.drawLine(x1, x2);
            p.drawLine(x3, x4);

	}
    }
    //   else
    // 	printf("line not driven :%d %d %d %s/%s\n", v.canConvertToDouble(), d_excursionEnabled , d_maxValue != d_minValue, qstoc(objectName()), qstoc(source()));
}

void TLabel::d_setPalette(const QPalette &p)
{
    if(d_backgroundColorChangeEnabled)
	ELabel::setPalette(p);
}

void TLabel::setBackgroundColorChangeEnabled(bool en)
{
    d_backgroundColorChangeEnabled = en;
    if(!en)
    {
	QPalette p = this->palette();
	p.setColor(backgroundRole(), Qt::white);
	d_setPalette(p);
    }
}


void TLabel::setPixmapName(const QString& name)
{
    QPixmap pixmap;
    d_pixmapName = QString();
    if(name.contains(":") || name.contains('/'))
	pixmap = QPixmap(name);
    else if(!name.contains('/'))
	pixmap = QPixmap(TUtil::instance()->iconPath() + "/" + name);

    if(!pixmap.isNull())
    {
        /// 	setPixmap(pixmap);
	d_pixmapName = name;
    }
    else
    {
	setText("Error setting pixmap");
	perr("TLabel::setPixmapName(): error building the pixmap");
	perr("The file \"%s\", if not a qt resource system file, is searched into the directory", qstoc(name));
	perr("\"%s\"", qstoc(TUtil::instance()->iconPath()));
	perr("which is a user wide directory stored into a QSetting object.");
	perr("Check that the icon is there or change the system wide setting by calling TUtil::setIconPath()");
	perr("with the correct path where icons and pixmap are installed.");
    }
}


