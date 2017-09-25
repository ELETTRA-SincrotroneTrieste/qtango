/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino, Vincenzo Forchi`		  *
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

#include <tutil.h>
#include "tcheckbox.h"

#include <EApplicationLauncher>
#include <QApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QMessageBox>
#include <QtDebug>

TCheckBox::TCheckBox(QWidget *parent) : 
	QCheckBox(parent), 
	QTangoComProxyReader(this),
	QTangoComProxyWriter(this), 
	QTangoWidgetCommon(this)
{
	/* instead of the previous `quality = ATTR_INVALID;' , set partially 
	 * checked directly when initiating.
	*/
	setCheckState(Qt::PartiallyChecked);
	setHideEventEnabled(false);
	update();
	bool_val = false;
	setText("No Link");
	setAttribute(Qt::WA_AlwaysShowToolTips);
	setTristate(true);
	QTangoComProxyReader::setAutoConfiguration(true);
	QTangoComProxyWriter::setAutoConfiguration(false);
// 	setStyleSheet("TCheckBox::indicator { width: 18px; height: 18px; }");
	
	connect(QTangoComProxyReader::qtangoComHandle(), 
		SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)));
	connect(QTangoComProxyReader::qtangoComHandle(), SIGNAL
		(attributeAutoConfigured(const TangoConfigurationParameters *)), this,
		  SLOT(configure(const TangoConfigurationParameters *)));
	connect(this, SIGNAL(clicked()), this,
		 SLOT(checkClicked()));
}

TCheckBox::~TCheckBox()
{
	
}

void TCheckBox::configure(const TangoConfigurationParameters *cp)
{
	if(!cp || text() != "No Link")
		return;
	
	QString name;
        if (cp->label() != "")
                name = cp->label();
        else
        { 
                QString s = realSource();
                if (s.contains("->"))
                        name = s.split("->")[0].split("/").last();
                else
                {
                        QStringList l = s.split("/");
                        if (l.size() > 1)
                                name = l[l.size() - 2];
                }
        }
        setText(name);
    if (cp->descriptionIsSet()) {
        setWhatsThis(cp->description());
    }
}

QString TCheckBox::getData()
{
	if (isChecked())
		return QString("true");
	else
		return QString("false");
}

void TCheckBox::checkClicked()
{
	/* call execute() with the last read value */
    printf("calling execute on proxy writer %d\n", bool_val);
	QTangoComProxyWriter::execute(QVariant(!bool_val));
}

void TCheckBox::refresh(const TVariant& v)
{	
	setToolTip(v.message());
	if (v.quality() == ATTR_INVALID)
		setCheckState(Qt::PartiallyChecked);
	else
    {
		if (v.canConvertToBool())
        {
			bool_val = v.toBool();
			if (v.toBool())
				setChecked(Qt::Checked);
			else
				setChecked(Qt::Unchecked);
		}
        else
            setChecked(Qt::PartiallyChecked);
	}
	update();
}


QList<QTangoCommunicationHandle*> TCheckBox::handleList() const
{
  QList<QTangoCommunicationHandle*> hlist;
  hlist << QTangoComProxyReader::qtangoComHandle() << QTangoComProxyWriter::qtangoComHandle();
  return hlist;
}

void TCheckBox::hideEvent(QHideEvent *)
{
	QTangoComProxyReader::hideEvent();
}

void TCheckBox::showEvent(QShowEvent *)
{
	QTangoComProxyReader::showEvent();
}

bool TCheckBox::event(QEvent *ev)
{
	if (ev->type() == QEvent::EnabledChange)
	{
		if (!isEnabled())
			setCheckState(Qt::PartiallyChecked);
// 		return true;
	}

		return QCheckBox::event(ev);
}

void TCheckBox::setTarget(QString s)
{
	if (s.contains("("))
	{
		s = s.remove(")");
		s += ",&" + objectName() + ")";
	}
	else
		s += "(&" + objectName() + ")";
	
	QTangoComProxyWriter::setTargets(s);
}

