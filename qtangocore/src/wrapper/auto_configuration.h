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

/* $Id: auto_configuration.h,v 1.25 2013-07-03 13:54:42 giacomo Exp $ */


#ifndef QTANGO_AUTO_CONFIGURATION_H
#define QTANGO_AUTO_CONFIGURATION_H

#include <tango.h>
#include <QString>
#include <tvariant.h>
#include <QObject>
#include <QEvent>
#include <qtangocore_common.h>

class TangoConfigurationParametersPrivate;

class TangoConfigurationParameters 
{
	public:
	
	TangoConfigurationParameters();
	
	void fill(double MV, double ME, double MW,
		double mW, double mE, double mV, 
   		QString d, QString l, QString stdu, QString du, QString f,
		const AttributeInfoEx& aiex);
	
	double maxValue() const { return mxValue; }
	double minValue()  const { return mValue; }
	double maxWarning()  const { return mxWarning; }
	double maxError()  const { return mxError; }
	double minError()  const { return mError; }
	double minWarning()  const { return mWarning; }
	
	/* test against this if you want to know if the 
	 * values were really found into the db and set
	 */
	bool maxIsSet() const { return d_maxIsSet; }
	bool minIsSet() const { return d_minIsSet; }
	bool mErrIsSet() const { return d_mErrIsSet; }
	bool MErrIsSet() const { return d_MErrIsSet; }
	bool mWarnIsSet() const { return d_mWarnIsSet; }
	bool MWarnIsSet() const { return d_MWarnIsSet; }
	bool stdUnitIsSet()const  ;
	bool unitIsSet() const;
	bool descriptionIsSet() const;

	/** \brief the measurement unit displayed by the readers
	 *
	 * @return true if the display measurement unit is set, i.e. it is available 
	 *         from the tango database
	 * @return false the tango database has not set the property <em>displayUnit</em>
	 * @see displayUnit
	 */
	bool displayUnitIsSet() const ;
	
	void setMaxIsSet(bool b) { d_maxIsSet = b; }
	void setMinIsSet(bool b) { d_minIsSet = b; }
	void setMErrIsSet(bool b) { d_MErrIsSet = b; }
	void setmErrIsSet(bool b) { d_mErrIsSet = b; }
	void setmWarnIsSet(bool b) { d_mWarnIsSet = b; }
	void setMWarnIsSet(bool b) { d_MWarnIsSet = b; }
	
	QString description() const { return d_desc; }
	QString label() const { return d_label; }
	QString stdUnit() const { return d_stdUnit; }
	QString unit() const { return QString::fromStdString(d_attributeInfoEx.unit); }
	
	/** \brief the measurement unit to display in the widgets
	 *
	 * According to the tango reference manual, <a href=""></a>, this is the string 
	 * representing the measurement unit to display in the graphical applications.
	 */
	QString displayUnit() const  { return d_displayUnit; }
	
	QString format() const { return d_format; }
	
	void setWriteType(AttrWriteType type) { d_writeType = type; }
	AttrWriteType writeType() const { return d_writeType; }
	
	void setDataFormat(AttrDataFormat fmt) { d_dataFormat = fmt; }
	AttrDataFormat dataFormat() const  { return d_dataFormat; }
	
	int dataType() const  { return d_dataType; }
	void setDataType(int datat) { d_dataType = datat; }
	
	TVariant currentValue() const { return t_current; }
	void setCurrentValue(const TVariant &v) { t_current = v; }
	
	QStringList values() const;
	
	/** \brief backward compatibility shorcut for propertyFound
	 *
	 * @see propertyFound
	 */
	bool hasValues() const { return d_attributePropertiesMap.contains("values"); }
	
	/** \brief tells if a desired attribute property was found on the database.
	 *
	 * @see attributePropertyValue()
	 */
	bool propertyFound(const QString& attrPropName) const { return d_attributePropertiesMap.contains(attrPropName); }
	
	AttributeInfoEx attributeInfoEx() const { return d_attributeInfoEx; }
	
	void setAttributeInfoEx(const AttributeInfoEx& aie) { d_attributeInfoEx = aie; }
	
	/** \brief returns the value of the property named attributePropertyName or an empty string list if
	 *         the attribute property was not found.
	 *
	 * @see propertyFound
	 * You can check propertyFound before calling this if you want to know if the property was found.
	 */
	QStringList attributePropertyValue(const QString& attributePropertyName) const;
	
	void addProperty(const QString& s, const QStringList& values) { d_attributePropertiesMap.insert(s, values); }
		
	private:
		double mxValue, mxError, mxWarning, mWarning,
  			mValue, mError;
		QString d_desc, d_label, d_stdUnit, d_displayUnit, d_format;
		TVariant t_current;
		
		bool d_maxIsSet, d_minIsSet, d_mErrIsSet, d_MErrIsSet, d_mWarnIsSet, d_MWarnIsSet;
		AttrWriteType d_writeType;
		AttrDataFormat d_dataFormat;
		int d_dataType; /* tango data type */

		/* store also the whole attribute info ex, if someone needs it */
		AttributeInfoEx d_attributeInfoEx;
		
		QMap <QString, QStringList> d_attributePropertiesMap;

        TangoConfigurationParametersPrivate *d_ptr;
};

class AttributeConfigurationEvent : public QEvent
{
	public:
		AttributeConfigurationEvent(const TangoConfigurationParameters *cp) : 
			QEvent(ATTRIBUTE_CONFIGURATION_QEVENT), d_confParams(cp) {};
		
		const TangoConfigurationParameters* configurationParameters()  { return d_confParams; }
		
	private:
		const TangoConfigurationParameters* d_confParams;
};

class QTangoAutoConfiguration : public QObject, public Tango::CallBack
{
  Q_OBJECT
	public:
		/** \brief QTangoAutoConfiguration constructor. Used by the library. Avoid it
		 *
		 * Tries to subsribe to attribute configuration change event.
		 */
		QTangoAutoConfiguration(QObject *parent, Tango::DeviceProxy *device, 
			const QString& source, 
			const QStringList &desiredAttributeProperties,
            int dbTimeoutMillis = -1,
			bool config_change_event_enabled = true);
			
		~QTangoAutoConfiguration();
		
		void setConfigurationChangeEventEnabled(bool en) { d_confChangeEnabled = en; }
		bool getConfigurationChangeEventEnabled() { return d_confChangeEnabled; }
		
		TangoConfigurationParameters* autoConfigurationData() { return d_confParams; }
		
		/** Tries to subscribe to the attribute configuration event through
		 * tango 6 subscribe_event().
		 * If it fails, it will configure once calling getConfigurationParams.
		 * If it succeeds, auto configuration of the widgets will be made by
		 * tango events.
		 */
		void get();
		
		void unsubscribe();
		
		int eventId() { return d_eventId; }
		
		AttributeInfoEx attributeInfoEx();
		
		QString source() { return d_source; }
		
	protected:
		void push_event(AttrConfEventData * e);
	
  private:
		void buildConfigParams(AttributeInfoEx *p_aiex);
		
		AttributeInfoEx getConfigurationParamsOnce();
		
	    void readDesiredAttributeProperties();
		
		bool d_confChangeEnabled;
		bool d_maxIsSet, d_minIsSet, d_mErrIsSet, d_MErrIsSet, d_mWarnIsSet, d_MWarnIsSet;
		int d_eventId;
		
		TangoConfigurationParameters *d_confParams;
		/* the list of attribute properties to get from the tango database */
		QStringList d_desiredAttributeProperties;
		
		QString  d_source;
		Tango::DeviceProxy *d_device;
		int d_confEventNo;
        int mDbTimeoutMillis;
};


#endif

