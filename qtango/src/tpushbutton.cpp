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

/* $Id: tpushbutton.cpp,v 1.14 2012-02-03 11:11:13 giacomo Exp $ */

#include <QMessageBox>
#include <QEvent>
#include <QAction>
#include <QActionEvent>
#include <QStringList>
#include <elettracolors.h>
#include "tpushbutton.h"

#include <QtDebug>

TPushButton::TPushButton(const QString &text, QWidget * parent) : EPushButton(text, parent), QTangoComProxyWriter(this),
	QTangoWidgetCommon(this), m_confirmNeeded(false)
{
	commonConfiguration();
}

TPushButton::TPushButton(QWidget * parent) : EPushButton(parent), QTangoComProxyWriter(this),
  QTangoWidgetCommon(this),m_confirmNeeded(false)
{
	setText("OK");
	commonConfiguration();
}

TPushButton::TPushButton(const QIcon &icon, const QString &text, QWidget *parent) : 
	EPushButton(icon, text, parent), QTangoComProxyWriter(this),
  QTangoWidgetCommon(this), m_confirmNeeded(false)
{
	commonConfiguration();
}

TPushButton::~TPushButton()
{

}

void TPushButton::commonConfiguration()
{
	/* usually we do not need auto configuration for push buttons */
	setAutoConfiguration(false);
///	setHideEventEnabled(false);
	m_confirmNeeded = false;
	d_stepsCount = 0;
	connect(this, SIGNAL(clicked()), this, SLOT(confirmRequest()));
	connect(qtangoComHandle(), SIGNAL(progress(int, int)), this, 
		SLOT(changeStyleSheet(int, int)), Qt::QueuedConnection);
	/* when new data arrives, emit the signal containing the argout, if present */
	connect(qtangoComHandle(), SIGNAL(newData(const TVariant& )), this, 
		SIGNAL(newDataAvailable(const TVariant& )));

        /* Widget disabling on communication error is managed by the communication handle when the property
         * communicationStatusChangesWidgetEnable is set to true.
         */

        /* custom message box title, text, buttons text */
        setProperty("confirmDialogTitle", "Confirm Request");
        setProperty("confirmDialogText", "Do you really want to execute %1?");
}

void TPushButton::confirmRequest()
{
	bool launch = true;
	emit clicked(d_pbData);
	if (m_confirmNeeded)
	{
            int res = QMessageBox::question(this, property("confirmDialogTitle").toString(),
                          QString(property("confirmDialogText").toString()).arg(this->realTargets()),
			QMessageBox::Yes | QMessageBox::No);
		if (res == QMessageBox::No)
			launch = false;
	}
	if (launch)
	{
		QList<TVariant> v = execute();
	}
}

void TPushButton::changeStyleSheet(int steps, int total)
{
        qDebug() << "changing style to step " << steps << " over " << total;
	if (total == 1)
		return;
	else if (steps == total)
		setStyleSheet(oldStyle);
	else
	{
		if (steps == 1)
			oldStyle = styleSheet();
		double val = ((double)steps)/total;
		QString style;
	//	QColor c = palette().color(QPalette::Button);
		QColor c = EColor(Elettra::blue);
		QString color = QString("#%1%2%3").arg(c.red(),2,16,QChar('0')).arg(c.green(),2,16,QChar('0')).arg(c.blue(),2,16,QChar('0'));
//		QString color = "#0000ff";
		//style += QString("border-style: outset; border-width: 1px; margin: 0px; border-radius: 2px;");
		style += QString("border-style: outset; border-width: 1px; margin: 0px;");
		style += QString("border-color: black; padding: 4px;");
		style += QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, ");
//		style += QString("stop: 0 %1, stop: %2 %1, stop %3 white, stop: 1 white);").arg(color).arg(val-.1).arg(val+.1);
                style += QString("stop: %1 %2, stop: 1 white);").arg(val).arg(color);
                setStyleSheet(style);
	}
}

void TPushButton::dragLeaveEvent(QDragLeaveEvent *e)
{
  setDown(false);
  QPushButton::dragLeaveEvent(e);
}
