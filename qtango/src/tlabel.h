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

/* $Id: tlabel.h,v 1.36 2013-05-07 08:21:44 giacomo Exp $ */

#ifndef _TLABEL_H
#define _TLABEL_H

#include <eflag.h>
#include <QPointer>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>
#include "view_trend.h"

class QMenu;
class EApplicationLauncher;
class TAction;
class TVariant;
class TLabelPrivate;

/**
 * \brief a viewer for Tango scalar types
 *
 * TLabel is a viewer for all Tango scalar types,
 * it inherits from ELabel
 *
 * \par Example
 * \verbatim
 * TLabel *tsv = new TLabel(this);
 * // connect to a tango device server
 * tsv->setSource("tangotest/1/1/short_scalar");
 * \endverbatim
 *
 * <h3>Note</h3><p>Hide and show events optimization is enabled by default.</p>
 *
 * Since QTango version <em>4.x</em>, QLabel implements <em>font scaling</em>, meaning that its
 * font are grown or shrinked according to the available space and to the text represented.
 * You might want to have a look at qtcontrols ESimpleLabel and FontScalingWidget documentation
 * to read more about this feature.
 * <br/>Remember that <em>left</em> or <em>right</em> alignment might need an increase of the lateralBorderWidth
 * property value to optimize font scaling. <br/>Font scaling <em>labels</em> are tailored for <em>center aligned</em>
 * text only.
 *
 * @see FontScalingWidget::setLateralBorderWidth
 * @see FontScalingWidget
 * @see ESimpleLabel
 * @see TSimpleLabel
 * @see ELabel
 *
 * <h3>backgroundColorChangeEnabled and excursionEnabled properties</h3>
 * <p>
 * Since <em>qtango version 4.x</em>, the excursionEnabled property has been added as an alternative
 * to the backgroundColorChangeEnabled. This feature draws a thin <em>coloured gauge</em> on the top
 * of the label, configured with minimum, maximum, alarm and warning values of the tango <em>attribute
 * </em> represented, if configured. A cross drawn over the gauge displays the value currently represented
 * with respect to the total excursion of the attribute itself.<br/>
 * The classical <em>backgroundColorChangeEnabled</em> property is still there and enabled by default,
 * while the <em>excursionEnabled</em> property must be explicitly enabled by the programmer.
 * </p>
 *
 * <h3>Notes on setEnumDisplay and setBooleanDisplay</h3>
 * <p>
 * If you use ELabel::setEnumDisplay or ELabel::setBooleanDisplay, remember to <em>remove</em> the
 * <em>display unit</em> from the tango attribute in the tango database.
 * </p>
 *
 * <h3>Automatic setEnumDisplay with attributes with <em>values</em> property set.</h3>
 * <h4>Since: QTango 4.3.4</h4>
 * <p>
 * If an attribute has the <em>values</em> attribute property set, then the TLabel can be
 * told to configure itself in order to display a string associated to the value of the
 * attribute (converted to integer).
 * </p>
 * <p>
 * The two free properties that control this behaviour are:
 * <ul><li>configureEnumFromValuesProperty, which must be set to true;</li>
 * <li>valuesPropertyOffset, 0 by default</li>
 * </ul>
 * </p>
 * <h3>The configureEnumFromValuesProperty free property</h3>
 * <p> If set to true, then the list of strings defined in the <em>values</em> attribute property
 * is used to call ELabel::setEnumDisplay when the attribute is configured. The <em>valuesPropertyOffset</em>
 * value is added to the first integer argument of the ELabel::setEnumDisplay.
 * </p>
 * <h3>Notes</h3>
 * <ul> <li>If no <em>values</em> attribute property is defined or it is empty, then the TLabel
 * will behave as usual, even if configureEnumFromValuesProperty is set to true;</li>
 * <li>the configureEnumFromValuesProperty cannot be changed after the source has been configured;</li>
 * <li>the Qt4 designer exports the configureEnumFromValuesProperty and valuesPropertyOffset properties
 * among the <em>Dynamic Properties</em> list.</li>
 * </ul>
 *
 *
 */
class TLabel : public ELabel, public QTangoComProxyReader, public QTangoWidgetCommon
{
Q_OBJECT
Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
Q_PROPERTY(int period READ period WRITE setPeriod)
Q_PROPERTY(bool autoConfiguration READ autoConfiguration WRITE setAutoConfiguration)
Q_PROPERTY(bool hideEventEnabled READ hideEventEnabled WRITE setHideEventEnabled)
Q_PROPERTY(bool viewTrendEnabled READ viewTrendEnabled WRITE setViewTrendEnabled)
Q_PROPERTY(bool trendColouredBackground READ trendColouredBackground WRITE setTrendColouredBackground)
Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
Q_PROPERTY(QString helperApplication READ helperApplication WRITE setHelperApplication)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)

Q_PROPERTY(bool backgroundColorChangeEnabled READ backgroundColorChangeEnabled WRITE setBackgroundColorChangeEnabled)
Q_PROPERTY(bool excursionEnabled READ excursionEnabled WRITE setExcursionEnabled);
Q_PROPERTY(int excursionDistFromMargin READ excursionDistFromMargin WRITE setExcursionDistFromMargin);
Q_PROPERTY(int excursionWidth READ excursionWidth WRITE setExcursionWidth);

Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(bool hdbActionEnabled READ hdbActionEnabled WRITE setHdbActionEnabled DESIGNABLE true)

Q_PROPERTY(QString pixmapName READ pixmapName WRITE setPixmapName DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)

Q_PROPERTY(double lowError READ lowError WRITE setLowError)
Q_PROPERTY(double lowWarning READ lowWarning WRITE setLowWarning)
Q_PROPERTY(double highWarning READ highWarning WRITE setHighWarning)
Q_PROPERTY(double highError READ highError WRITE setHighError)


public:
    TLabel(QWidget *parent, Qt::WindowFlags f = 0);

	virtual ~TLabel();

	bool excursionEnabled() { return d_excursionEnabled; }
	void setExcursionEnabled(bool en) { d_excursionEnabled = en; }

	/** \brief returns the width of the excursion line, if enabled.
	 *
	 * @see setExcursionWidth
	 * @see setExcursionEnabled
	 */
	int excursionWidth() { return d_excursionWidth; }

	/** \brief sets the width of the excursion line, if enabled.
	 *
	 * @see excursionWidth
	 * @see setExcursionEnabled
	 */
	void setExcursionWidth(int w) { d_excursionWidth = w; }

	/** \brief returns the distance from the top widget rect of the line drawn.
	 *
	 * @see setExcursionDistFromMargin
	 * @see setExcursionEnabled
	 * @see setExcursionWidth
	 */
	int excursionDistFromMargin() { return d_excursionDistFromMargin; }

	/** \brief sets the distance from the top widget rect of the line drawn.
	 *
	 * The excursion line is driven d pixels below the upper border of the widget.
	 * <strong>Note</strong>: you will appreciate changes on next refresh of the reader.
	 * @see setExcursionEnabled
	 * @see setExcursionWidth
	 */
	void setExcursionDistFromMargin(int d) { d_excursionDistFromMargin = d; }

	/** \brief if this property is set to true, then palette background color changes are enabled for the label.
	 *
	 * Remember to set it to true if you have an enum configured for the label, i.e. if you have
	 * associated special colors to determined values of the label.
	 * @see setBackgroundColorChangeEnabled
	 * <h3>Note</h3><p>If this property is enabled, then the palette of the TLabel might change
	 * depending on the meaning of the value represented (a particular tango state or a value inside an alarm or
	 * warning interval. If you need a custom palette for your TLabel, disable this property.</p>
	 */
	bool backgroundColorChangeEnabled() { return d_backgroundColorChangeEnabled; }

	/** \brief if this property is set to true, then palette background color changes are enabled for the label.
	 *
	 * Remember to set it to true if you have an enum configured for the label, i.e. if you have
	 * associated special colors to determined values of the label.
	 * @see backgroundColorChangeEnabled
	 */
	void setBackgroundColorChangeEnabled(bool en);

	/** \brief the name of the pixmap associated to the label
	 *
	 * If a pixmap was set with setPixmapName(), this method returns the name of
	 * the pixmap provided with setPixmapName().
	 * @see setPixmapName
	 */
	QString pixmapName() { return d_pixmapName; }

	/** \brief calls setPixmap on the label, creating the pixmap from the name provided.
	 *
	 * @param name the name of the pixmap. This might be a file name or a application's embedded resource,
	 *             if you want to set the pixmap using the Qt Resource System
	 *             (see http://doc.trolltech.com/4.4/resources.html)
	 *
	 * <em>Note</em>: if you specify a file name, be sure to store the file into the user-wide path
	 * used to store the icons. This path is returned by
	 * TUtil::iconPath() and modified by TUtil::setIconPath(). The value is stored into a QSettings object
	 * with Scope QSettings::UserScope: <em>
	 * the QSettings object ignores user-specific settings and provides access to system-wide settings</em>.
	 * <br/>see http://doc.trolltech.com/4.4/qsettings.html#QSettings-3<br/>
	 * This means that if TUtil::iconPath() returns /runtime/share/icons, then you will have to put there
	 * your icon or pixmap.
	 * <br/>This was introduced in qtango 4.x
	 */
	void setPixmapName(const QString& name);

        double lowError() const { return d_minE; };
        double lowWarning() const { return d_minW; };

        void setLowError(double le) { d_minE = le; };
        void setLowWarning(double lw) { d_minW = lw; };

        double highError() const { return d_maxE; };
        double highWarning() const { return d_maxW; }

        void setHighError(double he) { d_maxE = he; };
        void setHighWarning(double hw) { d_maxW = hw; };

        double maxValue() const { return d_maxValue; }

  public slots:
	/** \brief change the period of the readings. This is a slot.
	 *
	 * Simply changes the reading period, but since version 4.x this method is exported as a slot.
	 */
	void setPeriod(int p) { QTangoComProxyReader::setPeriod(p); }

protected slots:
	void refresh(const TVariant &);
	void configure(const TangoConfigurationParameters*);

protected:

	virtual void paintEvent(QPaintEvent *e);

private:
    /* calls ELabel::setPalette if d_backgroundColorChangeEnabled is enabled */
    void d_setPalette(const QPalette &);
	QVector<QColor> stateColors;
	Tango::AttrQuality  currentQuality;
	double d_maxValue, d_minValue, d_minW, d_maxW, d_minE, d_maxE;

	bool d_excursionEnabled, d_backgroundColorChangeEnabled;
	int d_excursionDistFromMargin, d_excursionWidth;

	QString d_pixmapName;

    TLabelPrivate *d_ptr;
};

#endif
