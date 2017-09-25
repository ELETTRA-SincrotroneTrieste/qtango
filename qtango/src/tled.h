/* $Id: tled.h,v 1.22 2013-04-30 07:34:03 giacomo Exp $ */
#ifndef _TLED_H
#define _TLED_H

#include <auto_configuration.h>
#include <com_proxy_reader.h>

#include <eled.h>
#include <elettracolors.h>
#include <QMenu>
#include <QMouseEvent>
#include <QPointer>
#include <qtango_wcommon.h>

class QMenu;
class EApplicationLauncher;
class TAction;
class TVariant;
class TLedPrivate;

/**
 * \brief a led-like widget
 *
 * TLed is suitable to display DevState and Devboolean variables
 * <h3>Note</h3><p>Hide and show events optimization is enabled by default. See QTancoCore QTangoComProxyReader::setHideEventEnabled().</p>
 *
 * <h3>Type conversion notes</h3>
 * <p>
 * The TLed can be associated to a Tango DevState, a boolean and an integer value.
 * The colour of the led can be set through the trueColor and falseColor properties when the object
 * is attached to a boolean type.
 * <br/>
 * The type conversion takes place in the following order:
 * <ul>
 * <li>DevState;</li>
 * <li>bool;</li>
 * <li>int;</li>
 * </ul>
 * </p>
 *
 * <h3>Notes on led colour</h3>
 * <p>
 * When the TLed is associated to a Tango::DevState, the colour is chosen among the available colors
 * globally defined in the Config class and returned by the Config::instance()->stateColor() method.
 * If one of the methods setStateColor or setStateColors is called, then the colour to associate
 * to the TLed is chosen from the provided vector (setStateColors) or association (setStateColor).
 * If the vector of QColor provided with setStateColors is not big enough to map all the int or
 * DevState converted value, then the TLed will not correctly change its colour.
 * On the other hand, if no colour corresponding to the current DevState/int is provided with
 * setStateColor, the TLed color will be wrong (the default constructed QColor).
 * To reset to the default behaviour (choice of the colours through Config class), just pass
 * an empty vector of QColor to setStateColors.
 * \par Example
 * \code
        // the following three calls to setStateColor setup a color for
        // the FAULT, ON and RUNNING states.
        // If the current state is not one of the three, the TLed will
        // be black (default constructed QColor.
        //
        ui.tLed->setStateColor(Tango::FAULT, Qt::red);
        ui.tLed->setStateColor(Tango::ON, Qt::green);
        ui.tLed->setStateColor(Tango::RUNNING, Qt::gray);

        // if the DevState or integer associated to the led is less than the size of
        // the vector of colours provided, then the led will display the corresponding QColor.
        // Otherwise, the colour will not change and an error message will be displayed on the console.
        ui.tLed_3->setStateColors(QVector<QColor>() << Qt::red << Qt::green << Qt::yellow << Qt::blue << Qt::cyan <<
                Qt::darkRed << Qt::darkGreen);
 *\endcode
 * </p>
*/

class TLed : public ELed, public QTangoComProxyReader, public QTangoWidgetCommon
{
Q_OBJECT
Q_PROPERTY(QString source                READ source                   WRITE setSource DESIGNABLE true)
Q_PROPERTY(QColor trueColor              READ trueColor                WRITE setTrueColor)
Q_PROPERTY(QColor falseColor             READ falseColor               WRITE setFalseColor)
Q_PROPERTY(bool viewTrendEnabled         READ viewTrendEnabled         WRITE setViewTrendEnabled)
Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
Q_PROPERTY(QString helperApplication   READ helperApplication        WRITE setHelperApplication)
Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)

Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)

public:
	TLed(QWidget *);
	~TLed();

	/**
	 * returns data displayed in bool format
	 * @return data
	 */
	bool getValue();

	/**
	 * sets background colors for DevState values, default is yellow
	 * Tango::ON is green, Tango::FAULT and Tango::ALARM are red
	 * @param vec a vector containing 14 colors, one for each state
	 */
	void setStateColors(QVector<QColor>&);

	/**
	 * sets background color for a given state
	 * @param state	the state you want to cofigure
	 * @param color the background color
	 */
	void setStateColor(DevState, QColor);

	QColor trueColor(){ return d_trueColor; }
	void setTrueColor(QColor c){ d_trueColor = c; }

	QColor falseColor(){ return d_falseColor; }
	void setFalseColor(QColor c){ d_falseColor = c; }
	
	void setViewTrendEnabled(bool e){ d_enableViewTrend = e; }
	bool viewTrendEnabled(){ return d_enableViewTrend; }

protected slots:
	void refresh(const TVariant &);
	void autoConfigure(const TangoConfigurationParameters *);

protected:
	
private:
	DevState 	state;
	QVector<QColor> stateColors;
	bool		bool_val;
	QColor		d_trueColor, d_falseColor;

	QPointer<QMenu> 	d_menu;
	bool 			d_enableViewTrend;

    TLedPrivate *d_ptr;

};

#endif
