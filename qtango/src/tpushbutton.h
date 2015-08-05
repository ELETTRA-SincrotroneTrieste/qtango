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

/* $Id: tpushbutton.h,v 1.15 2013-04-30 07:34:03 giacomo Exp $ */

#ifndef TPUSHBUTTON_H
#define TPUSHBUTTON_H

#include <epushbutton.h>


#include <QList>
#include <QVariant>
#include <com_proxy_writer.h>
#include <taction.h>
#include <tutil.h>
#include <tvariant.h>
#include <qtango_wcommon.h>

class TPushButtonPrivate;

/**
   * \brief a button to execute a command
   *
   * This button is used to execute a command with no input parameters
   * \par Example
   * \verbatim
   * TPushButton *pb = new TPushButton("On", this, "mybutton");
   * pb->setTarget("d/f/m->On");
   * \endverbatim
   *
   * @see QTangoComProxyWriter
   * @see QTangoComProxyWriter::executionMode()
   *
   * <h3>The <em>newDataAvailable()</em> signal</h3>
   * <p>
   * The newDataAvailable() signal, introduced in qtango version <em>4.x</em> in all QTangoComProxyWriter
   * that perform an execute() on the writer itself, such as TPushButton, TApplyNumeric and TReaderWriter,
   * can be useful to retreive the <em>argout return value </em> of a <em>Tango command_inout</em> call.
   * A typical use of this signal is to <em>connect</em> it to some <em>refresh</em> slot on a 
   * receiver, to perform a <em>one shot</em> write/read back on a tango point.
   * For instance, a TPlotLightMarker might be refreshed on button click, as in this example:
   * \par Example
   * \code
   * TPushButton *button  = new TPushButton(this);
   * TRealTimePlot *plot = new TRealTimePlot(this);
   * button->setTargets("a/b/c->GetCh1Data(0,1000)");
   * button->setTargets("a/b/c->GetCh2Data(0,1000)");
   * connect(button, SIGNAL(newDataAvailable(const TVariant &)), plot, SLOT(updateCurve(const TVariant&)));
   * \endcode
   * Inside the TVariant object, the name of the tango point is saved and the plot knows to which curve it
   * pertains.
   *
   * <strong>Note</strong>: in the example above, consider setting the ExecutionMode of the TPushButton to
   * <em>Asynchronous</em>, so that the execution of the target takes place in another thread and the plot 
   * is updated when the data is ready.
   *
   * @see QTangoComProxyWriter::setExecutionMode()
   *
   * @see TPlotLightMarker::updateCurve()
   *
   * </p>
   * \par Message Box title and text properties
   * If the confirmNeeded property is set to true, then a confirm dialog asks the user whether
   * he wants to proceed or not. By means of the two string free properties
   * <ul>
   * <li>confirmDialogTitle</li>
   * <li>confirmDialogText</li>
   * </ul>
   * you are able to customize the title and the text of the Message Box.
   *
   
 */
class TPushButton : public EPushButton, public QTangoComProxyWriter, public QTangoWidgetCommon
{
Q_OBJECT
Q_PROPERTY(QString targets READ targets WRITE setTargets DESIGNABLE true)
Q_PROPERTY(bool confirmNeeded READ confirmNeeded WRITE setConfirmNeeded)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
Q_PROPERTY(QTangoCommunicationHandle::ExecutionMode executionMode READ executionMode WRITE setExecutionMode)

Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
/* scalable fonts */
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
Q_PROPERTY(QString data READ getData WRITE setData)

public:
	TPushButton(QWidget *);
	TPushButton(const QString &, QWidget *);
	TPushButton(const QIcon &, const QString &, QWidget *);
	
	virtual ~TPushButton();

	void setConfirmNeeded(bool b){ m_confirmNeeded = b; };
	bool confirmNeeded(){ return m_confirmNeeded; };
	
	/** \brief returns the data set with setData.
	 *
	 * If some data, in the format of string, was set with setData(), then
	 * you can retrieve its value with this method.
	 * @see setData
	 */
	QString getData() { return d_pbData; }
	
	/** \brief associates data in the form of a string to the push button.
	 *
	 * The data set with this method is used when the button is clicked, when the
	 * TPushButton emits the clicked(const QString&) signal.
	 * Such signal can be connected to the execute(const QString&) method to automatically write data
	 * on the target specified with setTargets.
	 */
	void setData(const QString &dataAsString) { d_pbData = dataAsString; }

signals:
        void stepsDone(int);
		
	/** \brief this signal is emitted after the execution of the target action.
	 *
	 * @param returnValue the TVariant representing the argout of the command
	 * If the target is a command, then it might have a return value. In that case, 
	 * its argout is available through the parameter returnValue.
	 * The returnValue contains also the name of the tango point, through 
	 * returnValue.tangoPoint(). 
	 * 
	 */
	void newDataAvailable(const TVariant &returnValue);
	
	/** \brief when the button is clicked, this signal is emitted 
	 * 
	 * The signal contains the data set with setData and is always emitted, even if confirmNeeded is 
	 * set to true.
	 */
	void clicked(const QString &data);

  public slots:
	
	void setTargets(const QString& tgt) { QTangoComProxyWriter::setTargets(tgt); }
	void clearTargets() { QTangoComProxyWriter::clearTargets(); }
	
protected:
	bool m_confirmNeeded;
	QString	oldStyle;
	
	/* avoids that the button stays pressed after a drag */
	virtual void dragLeaveEvent(QDragLeaveEvent *);

protected slots:
	void confirmRequest();
	void changeStyleSheet(int step, int total);
	void disable() { setDisabled(true); }
	
private:
	int			d_stepsCount;
	
	/* called by the two constructors */
	void commonConfiguration();
	QString d_pbData;

    TPushButtonPrivate *d_ptr;
};


#endif


