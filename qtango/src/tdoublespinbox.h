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

/* $Id: tdoublespinbox.h,v 1.12 2013-04-30 07:34:03 giacomo Exp $ */


#ifndef TDOUBLESPINBOX_H
#define TDOUBLESPINBOX_H

#include <espinboxes.h>
#include <com_proxy_writer.h>
#include <simple_dataproxy.h>
#include <float_delegate.h>
#include <qtango_wcommon.h>

class TDoubleSpinBoxPrivate;

/** \brief A spin box derived from QDoubleSpinBox.
 *
 * This widget can be used by a writer. The writer can read the current text
 * displayed in the line edit and use it to write to tango.
 * For instance, given a TPushButton *pb and a TDoubleSpinBox *cb:
 * \par Example
 * \code
 * TPushButton *pb = new TPushButton(this);
 * pb->setText("Click to apply the value of the combo box");
 * TDoubleSpinBox *cb = new TDoubleSpinBox(this);
 * cb->setObjectName("specialcombo"); // important! Always provide the object name!
 * // connect the tango target providing the attribute (or command) and the object 
 * name from which to get the value (in this case the TDoubleSpinBox name)
 *
 * pb->setTarget("test/device/1/string_scalar(&specialcombo)");
 * \endcode
 * that's all!
 *
 * NOTE: The QTangoProxyWriter, although unused to effectively write the number
 * (although a valueChanged(int) signal might in the future be used to 
 * directly write when the user changes the spin box value), creates a 
 * connection to the device and provides the auto configuration functionality.
 * @see QTangoAutoConfiguration
 *
 * The widget is derived from QDoubleSpinBox and allows to set a scalar floating point value
 * It does really nothing if used alone, but uses QTangoComProxyWriter just to auto
 * configure itself, with the maximum and minumum values, and with the format.
 * There is no way to write_attribute() with this object really, and you have to use it as in the example above.
 *
  * <h3>Widget auto configuration</h3>
  * <p>
   * The widget <em>auto configures itself </em> with <em>minimum and maximum</em> values, and
   * with the <em>format</em> specified in the tango database (jive settable).
   * The value displayed is initialized according to  the <em>attribute <strong>set value</strong></em>.
   * <p>
   * <strong>Note</strong>:<br/>
   * For the widget <em>auto configuration</em>, <strong>it is compulsory</strong> that the tango attribute
   * <em>is configured with its <strong>minimum and maximum values</strong></em>. If that is not the case,
   * then only the attribute set value is displayed, but <em>no minimum and maximum values, nor the format</em>
   * will be setup on the widget.
   * </p>
   * <p>
   *	In case of auto configuration possible, i.e. minimum and maximum values set on the tango database for the 
   * target attribute, the <em>minimum and maximum values have priority over the number of <strong>integer digits</strong>
   * with respect to the <strong>format</strong></em>. On the other hand, <em>the number of decimal digits is
   * derived from the string <em>format</em></em>.
   * </p>
   * </p>
   * @see DecIntFromFormat
   * @see TApplyNumeric
   * @see TNumeric
   * @see TPushButton
   * @see SimpleDataProxy
   * @see QTangoAutoConfiguration
   * 
   * <strong>Note</strong>:<br/>
   * <p>TNumeric is not a writer really: you need to use an additional TPushButton and set its target in the form of the example:
   *      <em>test/device/1/double_scalar(&applyNumericObjectName)</em>
   * <br/> TNumeric actually is a SimpleDataProxy, and is a QTangoComProxyWriter just to benefit from the auto configuration
   *  of the maximum and minimum values, and of the format, of course.
   * </p>
   * @see SimpleDataProxy
   * @see QTangoComProxyWriter
   * @see QTangoAutoConfiguration
 */

class TDoubleSpinBox : public EDoubleSpinBox, public QTangoComProxyWriter, public SimpleDataProxy, public FloatDelegate,
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
  Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
  
  Q_OBJECT
  public:
    TDoubleSpinBox(QWidget * = 0);
	
	virtual ~TDoubleSpinBox() {};
    
    /* methods to implement from FloatDelegate */
    void setMinimum(double m) { QDoubleSpinBox::setMinimum(m); }
    void setMaximum(double m) { QDoubleSpinBox::setMaximum(m); }
    void setValue(double v) { QDoubleSpinBox::setValue(v); }
    void setIntDigits(int) {};
    void setDecDigits(int dd)  {QDoubleSpinBox::setDecimals(dd); }
    
  public slots:
    /** must implement as slot the SimpleDataProxy pure virtual method QString getData() 
    */
    QString getData();
	
	/** \brief provided for convenience, calls the writer execute() method passing data as argument.
	 *
	 * This slot can be useful to automatically execute the target with the data contained in the
	 * argument.
	 * A typical usage might be to connect the valueChanged(double ) QDoubleSpinBox
	 * signal to this slot, to execute the target when the index of the spin box changes, without needing
	 * an additional <em>Apply</em> button.
	 * This feature is available also in TSpinBox and TComboBox
	 */
	void execute(double value);
    
  protected slots:
    /** auto configuration method. */
    void autoConfigure(const TangoConfigurationParameters * );	

private:
    TDoubleSpinBoxPrivate *d_ptr;
    
};

#endif

