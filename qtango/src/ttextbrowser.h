#ifndef TTEXTBROWSER_H
#define TTEXTBROWSER_H

#include <QTextBrowser>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>

/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	  						  *
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

/* $Id: ttextbrowser.h,v 1.4 2013-04-30 07:34:03 giacomo Exp $ */

class TTextBrowserPrivate;

/** \brief A text browser that can be used to display long strings, html formatted.
 *
 * This class supports Tango attributes whose values can be converted into string.
 * It can be useful either when such strings are too long to be clearly displayed in a TLabel
 * or when they are html formatted. The use is very simple and equivalent to the use of 
 * the TLabel.
 * Since this is used to represent strings, no auto configuration takes place in the setup phase.
 */
class TTextBrowser : public QTextBrowser, public QTangoComProxyReader, public QTangoWidgetCommon
{
  Q_OBJECT
  Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
  Q_PROPERTY(int period READ period WRITE setPeriod)
  Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)
  Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
  Q_PROPERTY(QString helperApplication READ helperApplication WRITE setHelperApplication)
  Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
  Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
  Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
  Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
  Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)
  Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
  Q_PROPERTY(bool plainText READ plainText WRITE setPlainText)
  
  public:
	TTextBrowser(QWidget *parent);
	
	QString source() { return QTangoComProxyReader::source(); }

	QUrl textBrowserSource() { return QTextBrowser::source(); }
	
	virtual ~TTextBrowser() {};
	
	bool plainText() const {return d_plainText; }
	
  public slots:
	
	virtual void setSource(const QString& source) { QTangoComProxyReader::setSource(source); }
	
	void setTextBrowserSource(const QUrl& s) { QTextBrowser::setSource(s); }
	
	void setPlainText(bool plain) { d_plainText = plain; }
	
  protected:
  
  protected slots:
	void refresh(const TVariant &);
	
  private:
	int d_hScrollPos, d_vScrollPos;
	bool d_plainText;

    TTextBrowserPrivate *d_ptr;
};

#endif
