/***************************************************************************
     *   Copyright (C) 2008 by  	Giacomo Strangolino	  *
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

/* $Id: tlineedit.h,v 1.11 2013-04-30 07:34:03 giacomo Exp $ */


#ifndef TLINEEDIT_H
#define TLINEEDIT_H

#include <elineedit.h>
#include <simple_dataproxy.h>
#include <com_proxy_writer.h>
#include <string_delegate.h>
#include <tvariant.h>
#include <qtango_wcommon.h>

class TLineEditPrivate;

/** \brief A line edit derived from QLineEdit.
 *
 * This widget can be used by a writer. The writer can read the current text
 * displayed in the line edit and use it to write to tango.
 * For instance, given a TPushButton *pb and a TLineEdit *cb:
 *
 * TPushButton *pb = new TPushButton(this);
 * pb->setText("Click to apply the value of the combo box");
 * TLineEdit *cb = new TLineEdit(this);
 * cb->setObjectName("specialcombo"); // important! Always provide the object name!
 * // connect the tango target providing the attribute (or command) and the object 
 * name from which to get the value (in this case the TLineEdit name)
 *
 * pb->setTarget("test/device/1/string_scalar(&specialcombo)");
 *
 * If <em>auto configuration</em> is available for the specified target and the data type
 * represented is a number (integer or floating point), then the TLineEdit configures an
 * input validator not to allow the user to digit values out of range or unwanted characters.
 */

class TLineEdit : public ELineEdit, public QTangoComProxyWriter, public SimpleDataProxy, public StringDelegate,
  public QTangoWidgetCommon
{
  Q_PROPERTY (QString targets READ targets WRITE setTargets DESIGNABLE true)
  Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
  Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
  Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
  Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
  /* scalable fonts */
  Q_PROPERTY(bool rescaleFontOnTextChanged READ rescaleFontOnTextChanged WRITE setRescaleFontOnTextChanged)
  Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)

  Q_OBJECT
  public:
    TLineEdit(QWidget * = 0);
    
    void setTargets(QString);
    
    void setValue(const QString& s) { setText(s); }
    
  public slots:
    QString getData();
    void refresh(const TVariant &v);
    
    void configure(const TangoConfigurationParameters * cp);

private:
    TLineEditPrivate *d_ptr;
};

#endif

