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

/* $Id: com_proxy_reader.cpp,v 1.34 2013-07-03 13:54:42 giacomo Exp $ */


#include <QObject>
#include <QtDebug>
#include <QWidget>

#include "com_proxy_reader.h"

QTangoComProxyReader::QTangoComProxyReader(QObject *parent, RefreshMode mode, int period) :
	ProxyInterface()
{
	d_qtangoHandle = new QTangoCommunicationHandle(parent, mode, period);
	d_parent = parent;
	/* init some default values */
	d_qtangoHandle->setHideEventEnabled(true);
}

QTangoComProxyReader::~QTangoComProxyReader()
{
			 
}

TVariant QTangoComProxyReader::currentValue()
{
	TVariant v;
	if(d_qtangoHandle)
		v = d_qtangoHandle->lastValueRead();
	return v;
}

void QTangoComProxyReader::setSource(const QString& s)
{
	d_rawSource = s;
	if(d_qtangoHandle)
		d_qtangoHandle->setSource(s);
	/* the proxy reader stores the raw source provided by the code */
}

void QTangoComProxyReader::unsetSource(bool byHideEvent)
{
	if(d_qtangoHandle)
		d_qtangoHandle->unsetSource(byHideEvent);
}

QString QTangoComProxyReader::realSource() const
{
	QString ret = "";
	if(d_qtangoHandle)
		ret = d_qtangoHandle->source();
	return ret;	
}

/* returs the raw source: the one set in setSource (with wildcards, if were specified.
 * This is useful for the designer used in conjunction with environment variable 
 * QTANGO_DEVICES. (See TUtil::replaceWildcards() ).
 */
QString QTangoComProxyReader::source() const
{
	return d_rawSource;
}

void QTangoComProxyReader::read(QVariant v)
{
  d_qtangoHandle->executeAsynch(v);
}

void QTangoComProxyReader::hideEvent()
{
  /* in designer mode we do not want hide events */
  if(d_qtangoHandle && d_qtangoHandle->hideEventEnabled() && !d_qtangoHandle->designerMode())
	  d_qtangoHandle->stopPoller();
}
		
void QTangoComProxyReader::showEvent()
{
  /* in designer mode we do not want show events */
  if(d_qtangoHandle && d_qtangoHandle->hideEventEnabled() && !d_qtangoHandle->designerMode())
	  d_qtangoHandle->startPoller();
}

bool QTangoComProxyReader::getAutoConfiguration()
{
	if(d_qtangoHandle)
		return d_qtangoHandle->getAutoConfiguration();
	return false;
}
		
void QTangoComProxyReader::setAutoConfiguration(bool en,  bool confChangeEventEnabled)
{
	if(d_qtangoHandle)
		d_qtangoHandle->setAutoConfiguration(en, confChangeEventEnabled);
}

bool QTangoComProxyReader::autoConfiguration() const
{
  return d_qtangoHandle->autoConfiguration();
}

void QTangoComProxyReader::setWaitRefresh(bool enable)
{
  if(d_qtangoHandle)
	d_qtangoHandle->setActionWaitRefresh(enable);
}
		
bool QTangoComProxyReader::waitRefresh()
{
  if(d_qtangoHandle)
    return d_qtangoHandle->actionWaitRefresh();
  return false;
}

void QTangoComProxyReader::setDesignerMode(bool dm)
{
	d_qtangoHandle->setDesignerMode(dm);
}

bool QTangoComProxyReader::designerMode()
{
	return d_qtangoHandle->designerMode();
}

/* private */
void QTangoComProxyReader::enableHideEvents(bool e)
{
	d_qtangoHandle->setHideEventEnabled(e);
}

bool QTangoComProxyReader::hideEventsEnabled()
{
  return d_qtangoHandle->hideEventEnabled();
}

void QTangoComProxyReader::setTangoToolTipsEnabled(bool enable)
{
	d_qtangoHandle->setToolTipsEnabled(enable);
}

bool QTangoComProxyReader::tangoToolTipsEnabled()
{
	return d_qtangoHandle->toolTipsEnabled();
}

void QTangoComProxyReader::setReaderToolTip(const QString& message)
{
	QWidget *pw;
	if(tangoToolTipsEnabled())
	{
		pw = qobject_cast<QWidget *>(d_parent);
		if( pw != NULL)
			pw->setToolTip(message);
	}
}

int QTangoComProxyReader::period()
{
  if(d_qtangoHandle)
    return d_qtangoHandle->period();
  perr("QTangoComProxyReader::period(): qtango handle not initialized: bad and strange!");
  return 1000;
}

RefreshMode QTangoComProxyReader::refreshMode()
{
      return d_qtangoHandle->refreshMode();
}

bool QTangoComProxyReader::configured()
{
      return d_qtangoHandle->configured();
}

void QTangoComProxyReader::setPeriod(int p)
{
    d_qtangoHandle->setPeriod(p);
}

void QTangoComProxyReader::setDesiredAttributePropertyList(const QStringList &attProps)
{
  d_qtangoHandle->setDesiredAttributePropertyList(attProps);
}

void QTangoComProxyReader::setAttributePropertyDbTimeoutMillis(int dbTimeoutMillis)
{
    d_qtangoHandle->setAttributePropertyDbTimeoutMillis(dbTimeoutMillis);
}

QStringList QTangoComProxyReader::desiredAttributePropertyList()
{
  return d_qtangoHandle->desiredAttributePropertyList();
}

int QTangoComProxyReader::attributePropertyDbTimeoutMillis() const
{
   return  d_qtangoHandle->attributePropertyDbTimeoutMillis();
}

void QTangoComProxyReader::setRefreshMode(RefreshMode p)
{
    d_qtangoHandle->setRefreshMode(p);
}

QString QTangoComProxyReader::connectionErrorMessage()
{
	return d_qtangoHandle->connectionErrorMessage();
}

void QTangoComProxyReader::setSource(const QString &source, RefreshMode refreshMode, int period)
{
	this->setPeriod(period);
	this->setRefreshMode(refreshMode);
	this->setSource(source);
}

int QTangoComProxyReader::actionRefreshCount() const
{
  return d_qtangoHandle->readerActionRefreshCount();
}


