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

/* $Id: com_proxy_writer.cpp,v 1.22 2013-07-03 13:54:42 giacomo Exp $ */


#include "com_proxy_writer.h"
#include <QtDebug>
#include <QObject>

QTangoComProxyWriter::QTangoComProxyWriter(QObject *parent) : ProxyInterface()
{
	d_qtangoHandle = new QTangoCommunicationHandle(parent, TARGET);
}

QTangoComProxyWriter::~QTangoComProxyWriter()
{
  
}

void QTangoComProxyWriter::setTargets(QString s)
{
	QStringList sl = s.split(';', QString::SkipEmptyParts);
	d_qtangoHandle->setSources(sl);
	d_rawTargets = s;
}

void QTangoComProxyWriter::clearTargets()
{
	d_qtangoHandle->unsetSources();
	d_rawTargets.clear();
}

QString QTangoComProxyWriter::realTargets() const
{
	QString ret = "";
	if(d_qtangoHandle)
		ret = d_qtangoHandle->sourcesAsSingleString();
	return ret;
}

QString QTangoComProxyWriter::tangoPoint() const
{
  return realTargets();
}

QString QTangoComProxyWriter::targets() const
{
	return d_rawTargets;
}

void QTangoComProxyWriter::hideEvent()
{
  pok("writer hide event (source:%s) {%s}", qstoc(d_qtangoHandle->source()), objinfo(d_qtangoHandle->parent()));
	clearTargets();
}
		
void QTangoComProxyWriter::showEvent()
{
  
  pok("writer show event (source:%s) {%s}", qstoc(d_qtangoHandle->source()), objinfo(d_qtangoHandle->parent()));
	if (!targets().isEmpty())
		setTargets(targets());
}

int QTangoComProxyWriter::stepsCount()
{
	return d_qtangoHandle->actionsCount();
}

QTangoCommunicationHandle::ExecutionMode QTangoComProxyWriter::executionMode()
{
	return d_qtangoHandle->executionMode();
}

void QTangoComProxyWriter::setExecutionMode(QTangoCommunicationHandle::ExecutionMode mode)
{
	d_qtangoHandle->setExecutionMode(mode);
}

QList<TVariant> QTangoComProxyWriter::execute(QVariant v)
{
	QList<TVariant> tlist;
	if(d_qtangoHandle)
                tlist = d_qtangoHandle->executeAll(v);
	return tlist;
}

bool QTangoComProxyWriter::getAutoConfiguration()
{
	return d_qtangoHandle->getAutoConfiguration();
}
		
void QTangoComProxyWriter::setAutoConfiguration(bool en, bool confChangeEventEnabled)
{
	if(d_qtangoHandle)
		d_qtangoHandle->setAutoConfiguration(en, confChangeEventEnabled);
}

bool QTangoComProxyWriter::autoConfiguration() const
{
  return d_qtangoHandle->autoConfiguration();
}

void QTangoComProxyWriter::setDesiredAttributePropertyList(const QStringList &attProps)
{
  d_qtangoHandle->setDesiredAttributePropertyList(attProps);
}

QStringList QTangoComProxyWriter::desiredAttributePropertyList()
{
  return d_qtangoHandle->desiredAttributePropertyList();
}

void QTangoComProxyWriter::setAttributePropertyDbTimeoutMillis(int dbTimeoutMillis)
{
    d_qtangoHandle->setAttributePropertyDbTimeoutMillis(dbTimeoutMillis);
}

int QTangoComProxyWriter::attributePropertyDbTimeoutMillis() const
{
   return  d_qtangoHandle->attributePropertyDbTimeoutMillis();
}

QString QTangoComProxyWriter::connectionErrorMessage()
{
	return d_qtangoHandle->connectionErrorMessage();
}

void QTangoComProxyWriter::setDesignerMode(bool dm)
{
      d_qtangoHandle->setDesignerMode(dm);
}

bool QTangoComProxyWriter::designerMode()
{
      return d_qtangoHandle->designerMode();
}
