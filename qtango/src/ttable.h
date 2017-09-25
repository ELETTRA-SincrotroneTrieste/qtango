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

/* $Id: ttable.h,v 1.22 2013-04-30 07:34:03 giacomo Exp $ */

#ifndef _TSPECTRUM_H
#define _TSPECTRUM_H

#include <eflag.h>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>

class QString;
class QColor;
class TTablePrivate;

/**
 * \brief a viewer for Tango spectrum types
 *
 * TTable is a viewer for all Tango spectrum types,
 * it inherits from EFlag and TDataViewer
 *
 * \par Example
 * \verbatim
TTable *tsv = new TTable(this);
// configure aspect
tsv->setNumCols(16);
tsv->setNumRows(16);
// connect to a tango device server
tsv->setSource("tangotest/1/1/short_spectrum");
\endverbatim
if you want to display boolean values
\verbatim
TTable *tsv = new TTable(this);
// configure aspect
tsv->setNumCols(16);
tsv->setNumRows(16);
// configure text and colors
for (int i = 0; i < 256; i++)
	tsv->setBooleanDisplay(i, "False", "True", Qt::red, Qt::green);
// connect to a tango device server
tsv->setSource("d/f/m/bool_val");
\endverbatim
 * <h3>Note</h3><p>Hide and show events optimization is enabled by default. See QTancoCore QTangoComProxyReader::setHideEventEnabled().</p>
 *
 * <h3>Auto configuration</h3>
 * <p>
 * Since QTango version 4.x it is possible to auto configure the TTable by means of the following device
 * attribute properties:
 * <ul>
 * <li><em>numRows</em>: an integer representing the number of rows that the table must have;</li>
 * <li><em>numColumns</em>: an integer representing the number of columns that the table must have;</li>
 * <li><em>displayMask</em>: a list of integers that will set the display mask on the table. The list 
 *     is <em>newline</em> separated on the <em>jive</em> displayMask attribute property, as usual;</li>
 * <li><em>trueColours</em> (note: Colo<em>u</em>rs): a list of colours chosen among the ones defined in
 * elettracolors.cpp in the colorStrings array (see Elettra::colors).</li>
 * <li><em>falseColours</em>: a list of colours chosen among the ones defined in
 * elettracolors.cpp in the colorStrings array (see EColor).</li>
 * <li><em>trueStrings</em>: the ordered list of true strings to apply to the cells;</li>
 * <li><em>falseStrings</em>: the ordered list of true strings to apply to the cells;</li>
 * </ul>
 * </p>
 * <h4>Notes about device attribute properties and auto configuration</h4>
 * <p>
 * The number of elements defined in trueStrings and falseStrings must be the same, otherwise no true nor
 * false strings are set on the table cells. This also goes for the falseColours and trueColours device
 * attribute properties.
 * </p>
 * <p>
 * The list of colours is a list of strings where each element can be chosen from the Elettra::colors enumeration
 * (translated into the corresponding string), for instance:
 * <p>
 * <ul><li><em>trueColours</em> orange<br/>darkRed<br/>darkWater<br/>camel<br/></li></ul>
 * <ul><li><em>falseColours</em> white<br/>red<br/>green<br/>dirtyYellow<br/></li></ul>
 * </p>
 * You might be interested in knowing which colours among the Elettra::colors enumeration have been chosen
 * to represent the tango states in the QTango widgets. This mapping is done into configuration.cpp file,
 * in the method Config::initStateColorsAndStrings(). See Config class introduction.
 * 
 * @see Config
 * </p>
 */
class TTable : public EFlag, public QTangoComProxyReader, public QTangoWidgetCommon
{
Q_OBJECT

Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
Q_PROPERTY(int period READ period WRITE setPeriod)
Q_PROPERTY(bool autoConfiguration READ autoConfiguration WRITE setAutoConfiguration)
Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
Q_PROPERTY(QString helperApplication READ helperApplication WRITE setHelperApplication)
Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
	
Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)

public:
	TTable(QWidget *parent = NULL);
	~TTable();

protected slots:
	void refresh(const TVariant &v);
	void configure(const TangoConfigurationParameters*);

protected:
  virtual void paintEvent(QPaintEvent *e) { EFlag::paintEvent(e); }

private:
	void disabled();
	void enabled();

    TTablePrivate *d_ptr;
};

#endif
