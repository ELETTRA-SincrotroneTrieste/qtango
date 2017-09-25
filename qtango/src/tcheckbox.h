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

#ifndef _TCHECKBOX_H
#define _TCHECKBOX_H

#include <qtangocore.h>
#include <QCheckBox>

#include <com_proxy_reader.h>
#include <com_proxy_writer.h>
#include <qtango_wcommon.h>

class TCheckBoxPrivate;

/**
 * \brief a checkbutton-like widget
 *
 */
class TCheckBox : public QCheckBox, public QTangoComProxyReader, public QTangoComProxyWriter, public QTangoWidgetCommon
{
Q_OBJECT
Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
Q_PROPERTY(int period READ period WRITE setPeriod)
Q_PROPERTY (QString targets READ targets WRITE setTargets DESIGNABLE true)
Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)
Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)

public:
	TCheckBox(QWidget *);
	~TCheckBox();
	
	void setTarget(QString);
	QString getData();
	
	/* tcheckbox is a reader and a writer: must provide overriders for methods in ProxyInterface */
	QTangoCommunicationHandle* qtangoComHandle() const { return QTangoComProxyWriter::qtangoComHandle(); }
	virtual QString tangoPoint() const  { return QTangoComProxyWriter::tangoPoint(); }
	bool isReader() const { return true; }
	bool isWriter() const { return true; }
	virtual QList<QTangoCommunicationHandle*> handleList() const;
	
	
protected:
	void hideEvent(QHideEvent*);
	void showEvent(QShowEvent*);
	bool event(QEvent *);

protected slots:
	void refresh(const TVariant& v);
	void configure(const TangoConfigurationParameters *cp);
	void checkClicked();
	
private:
	bool bool_val;

    TCheckBoxPrivate *d_ptr;
	
};

#endif
