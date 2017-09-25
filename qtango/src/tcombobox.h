/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino		   *
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

/* $Id: tcombobox.h,v 1.16 2013-04-30 07:34:03 giacomo Exp $ */

#ifndef TCOMBOBOX_H
#define TCOMBOBOX_H

#include <QComboBox>
#include <simple_dataproxy.h>
#include <com_proxy_writer.h>
#include <auto_configuration.h>
#include <qtango_wcommon.h>

class TComboBoxPrivate;

/** \brief A combo box derived from QComboBox and the QTango Proxy Writer.
 *
 * This widget can be used by a writer. The writer can read the current text
 * displayed in the combo box and use it to write to tango.
 * For instance, given a TPushButton *pb and a TComboBox *cb:
 * TPushButton *pb = new TPushButton(this);
 * pb->setText("Click to apply the value of the combo box");
 * TComboBox *cb = new TComboBox(this);
 * cb->setObjectName("specialcombo"); // important! Always provide the object name!
 * //Do you want the combo box to auto configure itself with the <em>values</em> attribute property, if set?
 * // test/device/1/string_scalar must have the <em>values</em> attribute property set on the tango database
 * cb->setSource("test/device/1/string_scalar");
 * // you might use setTargets() as well for the purpose above, setSource() is for compatibility: once this was a reader
 * cb->setTargets("test/device/1/string_scalar"); // this is <em>better</em>, although designer plugins export source property...
 * // connect the tango target providing the attribute (or command) and the object 
 * name from which to get the value (in this case the TComboBox name)
 *
 * pb->setTargets("test/device/1/string_scalar(&specialcombo)");
 *
 * that's all!
 *
 * <h3>Free properties</h3>
 * \list
 * \li <em>indexOffset</em> (integer) an offset to be added to the currentIndex in index mode
 * \li <em>indexMap</em> a QMap<QString, QVariant> that associates a string of the items in the
 *     combo box to an integer representing the index to return in index mode
 *     <em>Note</em>: if an item of the combo box is not mapped, the currentIndex() is returned
 *     instead.
 *
 * \endlist
 *
 * <h3>Index map in <strong>values</strong> attribute property</h3>
 * <p>It is possible to specify the integer to return when a specific item of the combobox is
 * shown by appending to the item string the <em>::</em> separator followed by the desired index.
 * </p>
 * <h4>Note</h4><p>If one of the elements of the list of strings in the <em>values</em> attribute
 * property does not contain the <em>::</em>, then all the mapping is discarded. This means that
 * if you want to specify the map inside the <strong>values</strong> attribute property, then all
 * the strings of the list must contain the separator followed by the associated integer index.
 * </p>
 *
 */

class TComboBox : public QComboBox, SimpleDataProxy, public QTangoComProxyWriter, public QTangoWidgetCommon
{
  Q_PROPERTY(bool indexMode READ indexMode WRITE setIndexMode)
  Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
  Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
  Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
  Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
  Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
  Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
  Q_PROPERTY(QStringList items READ items WRITE setItems)
  

  Q_OBJECT
  public:
    TComboBox(QWidget * = 0);
    
    /** @param index mode: if true getData() returns currentIndex() of comboBox,
     *  otherwise getData() returns the currentText() from the combo box
     */
    void setIndexMode(bool in) { d_indexMode = in; }
    bool indexMode() { return d_indexMode; }
    
    /** \brief sets the tango source point.
     * This is useful only for auto configuration: actually it does not read nor write from/to the tango point 
     */
    void setSource(QString s) { setTargets(s); } /* left for API compatibility */
    
    QString source() { return targets(); } /* left for API compatibility */
    
    void setTargets(QString);
	
	QStringList items() const;
	
	void setItems(const QStringList &);
    
  public slots:
    QString getData();
    void init(const TangoConfigurationParameters *cp);
    void refresh(const TVariant &) {};
	
	/** \brief provided for convenience, calls the writer execute() method passing data as argument.
	 *
	 * This slot can be useful to automatically execute the target with the data contained in the
	 * argument.
	 * A typical usage might be to connect the currentIndexChanged( const QString & text ) QComboBox
	 * signal to this slot, to execute the target when the index of the combobox changes, without needing
	 * an additional <em>Apply</em> button.
	 * This feature is available also in TSpinBox and TDoubleSpinBox
	 */
	void execute(const QString &data);
    
  private:
    bool d_indexMode;

    TComboBoxPrivate *d_ptr;
};

#endif

