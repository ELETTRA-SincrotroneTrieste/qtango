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

/* $Id: auto_configuration.cpp,v 1.49 2013-07-03 13:54:42 giacomo Exp $ */


#include "auto_configuration.h"
#include "tutil.h"
#include "tvariant.h"
#include "configuration.h" 			/* for the getAttributeDefaultValues() method */
#include "tlog.h"
#include "communication_handle.h"
#include "action_factory.h"	/* for posting attribute configuration change event */
#include "action_configuration_changed_event.h" /* definition of AttributeConfigurationChangedEvent */
#include <device_factory.h>
#include <QtDebug>
#include <QCoreApplication>

TangoConfigurationParameters::TangoConfigurationParameters()
{ 
    d_maxIsSet = d_minIsSet = d_mErrIsSet = d_MErrIsSet = d_mWarnIsSet = d_MWarnIsSet = false;
}

void TangoConfigurationParameters::fill(
        double mxV, double mxE, double mxW,
        double mW, double mErr, double mV,
        QString  _desc, QString _label, QString stdunit,
        QString dunit, QString _format, const AttributeInfoEx &aiex)
{
    mxValue = mxV;
    mxError = mxE;
    mxWarning = mxW;
    mWarning = mW;
    mError = mErr;
    mValue = mV;
    d_desc = _desc;
    d_label = _label;
    d_stdUnit = stdunit;
    d_displayUnit = dunit;
    d_format = _format;
    d_writeType = READ;
    d_dataFormat = SCALAR;
    d_attributeInfoEx = aiex;
    d_maxIsSet = d_minIsSet = d_mErrIsSet = d_MErrIsSet = d_mWarnIsSet = d_MWarnIsSet = false;
}

QStringList TangoConfigurationParameters::attributePropertyValue(const QString& attributePropertyName) const
{
    QStringList ret;
    if(d_attributePropertiesMap.contains(attributePropertyName))
        ret = d_attributePropertiesMap.value(attributePropertyName);
    return ret;
}

QStringList TangoConfigurationParameters::values() const
{
    return attributePropertyValue(Config::instance()->valuesAttributePropertyName());
}

bool TangoConfigurationParameters::stdUnitIsSet() const 
{
    QRegExp re("no.*unit", Qt::CaseInsensitive);
    if(d_stdUnit.contains(re) || d_stdUnit == "None" || d_stdUnit == QString())
        return false;
    return true;
}

bool TangoConfigurationParameters::descriptionIsSet() const 
{
    QRegExp re("no.*description", Qt::CaseInsensitive);
    if(d_desc.contains(re) || d_desc == "None" || d_desc == QString())
        return false;
    return true;
}

bool TangoConfigurationParameters::unitIsSet() const
{
    QRegExp re("no.*unit", Qt::CaseInsensitive);
    QString sUnit = QString::fromStdString(d_attributeInfoEx.unit);
    if(sUnit.contains(re) || sUnit == "None" || sUnit == QString())
        return false;
    return true;
}

bool TangoConfigurationParameters::displayUnitIsSet() const 
{
    QRegExp re("no.*unit", Qt::CaseInsensitive);
    if(d_displayUnit.contains(re) || d_displayUnit == "None" || d_displayUnit == QString())
        return false;
    return true;
}

QTangoAutoConfiguration::~QTangoAutoConfiguration()
{
    unsubscribe();
    delete d_confParams;
}

QTangoAutoConfiguration::QTangoAutoConfiguration(QObject *parent,
                                                 Tango::DeviceProxy *device,
                                                 const QString& source,
                                                 const QStringList &desiredAttributeProperties,
                                                 int dbTimeoutMillis,
                                                 bool config_change_event_enabled)
    : QObject(parent)
{
    d_device = device;
    d_source = source;
    d_desiredAttributeProperties = desiredAttributeProperties;
    d_eventId = -1;
    d_confEventNo = 0;
    mDbTimeoutMillis = dbTimeoutMillis;
    d_maxIsSet = d_minIsSet = d_mErrIsSet = d_MErrIsSet = d_mWarnIsSet = d_MWarnIsSet = false;
    if(getenv("QTANGO_ATTR_CONF_CHANGE_DISABLED"))
        d_confChangeEnabled = !(bool) atoi(getenv("QTANGO_ATTR_CONF_CHANGE_DISABLED"));
    else
        d_confChangeEnabled = config_change_event_enabled;
    d_confParams = new TangoConfigurationParameters();
}

void QTangoAutoConfiguration::buildConfigParams(AttributeInfoEx *p_aiex)
{
    bool temp, ok=true;
    double maxValue, maxError, maxWarning, minWarning, minError, minValue;
    QString desc, label, stdUnit, displayUnit, format;

    maxValue = QString::fromStdString(p_aiex->max_value).toDouble(&temp);
    ok = ok & temp;
    minValue = QString::fromStdString(p_aiex->min_value).toDouble(&temp);
    ok = ok & temp;
    /* if minimum and maximum values are set, check their consistency and go on obtaining the warning
     * and alarm thresholds, verifying them
     */
    if ((ok) && (minValue < maxValue))
    {
        d_minIsSet = d_maxIsSet = true;
        /* min and max values are correct and the min < max: go on looking for alarms and warnings */
        maxError = QString::fromStdString(p_aiex->alarms.max_alarm).toDouble(&temp);
        ok = ok & temp;
        minError = QString::fromStdString(p_aiex->alarms.min_alarm).toDouble(&temp);
        ok = ok & temp;
        if ((ok) && (minError < maxError) && (minError >= minValue) && (maxError <= maxValue))
            d_mErrIsSet = d_MErrIsSet  = true;

        maxWarning = QString::fromStdString(p_aiex->alarms.max_warning).toDouble(&temp);
        ok = (ok | !d_mErrIsSet) & temp;
        minWarning = QString::fromStdString(p_aiex->alarms.min_warning).toDouble(&temp);
        ok = ok & temp;
        if(d_mErrIsSet && d_MErrIsSet) /* min err and max err are set */
        {
            if ((ok) && (minWarning < maxWarning) && (minWarning >= minError) && (maxWarning <= maxError))
                d_mWarnIsSet = d_MWarnIsSet = true;
        }
        else
        {
            if ((ok) && (minWarning < maxWarning) && (minWarning >= minValue) && (maxWarning <= maxValue))
                d_mWarnIsSet = d_MWarnIsSet = true;
        }
    }
    /* do not abort auto configuration if maximum and minimum aren't set: go on with the other interesting fields */
    desc = QString::fromStdString(p_aiex->description);
    label = QString::fromStdString(p_aiex->label);
    stdUnit = QString::fromStdString(p_aiex->standard_unit);
    displayUnit = QString::fromStdString(p_aiex->display_unit);
    format = QString::fromStdString(p_aiex->format);

    d_confParams->fill(maxValue, maxError, maxWarning,
                       minWarning, minError, minValue,
                       desc, label, stdUnit,displayUnit,
                       format, *p_aiex);

    d_confParams->setMinIsSet(d_minIsSet);
    d_confParams->setMaxIsSet(d_maxIsSet);
    d_confParams->setmErrIsSet(d_mErrIsSet);
    d_confParams->setMErrIsSet(d_MErrIsSet);
    d_confParams->setmWarnIsSet(d_mWarnIsSet);
    d_confParams->setMWarnIsSet(d_MWarnIsSet);

    d_confParams->setWriteType(p_aiex->writable);
    d_confParams->setDataFormat(p_aiex->data_format);
    d_confParams->setDataType(p_aiex->data_type);
}

void QTangoAutoConfiguration::get()
{
    vector<string> filters;
    QString attributeName  = d_source.split("/").last();
    attributeName.remove(QRegExp("\\(.*\\)"));
    AttributeInfoEx aiex;
    try
    {
        /* must get the current value to initialize the writers with the set point */
        qDebug() << QThread::currentThread() << ":" << this << "- getConfigurationParamsOnce()";
        aiex = getConfigurationParamsOnce();
        d_confParams->setAttributeInfoEx(aiex);
        /* subscribe to attribute change event only if not already subscribed */
        if(d_confChangeEnabled && d_eventId == -1)
        {
            try /* subscribe for attribute configuration change events */
            {
                d_eventId = d_device->subscribe_event(attributeName.toStdString(), ATTR_CONF_EVENT, this, filters);
                pok("\"%s/%s\" successfully subscribed to \e[1;37;42mattribute configuration change event\e[0m",
                    d_device->name().c_str(), qstoc(attributeName));
            }
            catch (DevFailed &e) /* cannot subscribe for attribute configuration change events */
            {
                pwarn("failed to subscribe to the \e[1;37;43mattr. configuration change event\e[m for \"%s\".", qstoc(source()));
                if(!ActionFactory::actionFactory()->loggingDisabled())
                {
                    TLog log(e);
                    TUtil::instance()->addLog(log.toTimeErrlistPair(), log.formatMessage(), true);
                }
            }
        } /*   if(d_confChangeEnabled) */
    }
    catch(DevFailed &e)
    {
        qDebug() << QThread::currentThread() << "catch(DevFailed &e) : " << this << "- getConfigurationParamsOnce()";
        perr("failed to complete the attribute configuration of \"%s\".",qstoc(source()));
        if(!ActionFactory::actionFactory()->loggingDisabled())
        {
            TLog log(e);
            TUtil::instance()->addLog(log.toTimeErrlistPair(), log.formatMessage(), true);
        }
        Tango::Except::re_throw_exception(e, "failed to get attribute configuration for \"" + d_device->name() +
                                          "\", attribute: \"" +  attributeName.toStdString() + "\".",
                                          "Failed to execute getConfigurationParamsOnce()",
                                          "QTangoAutoConfiguration::get()", Tango::ERR);

    }
    /* Now try to perform the first read_attribute() to initialize the currentValue field of the configuration
     * parameters.
     * This is not as critical as getConfigurationParamsOnce(). For example, the configuration parameters might
     * be available through get_attribute_config() (i.e. the device is alive), but the read_attribute() may
     * fail for some reason (for instance if the server does not allow reading the attribute in its current state.
     */
    try{
        DeviceAttribute devattr;
        devattr = d_device->read_attribute(qstoc(attributeName));
        TVariant v(devattr, aiex, d_source);
        v.setExecutionFailed(false);
        d_confParams->setCurrentValue(v);
    }
    catch(DevFailed &e)
    {
        /* build a valid device attribute to pass to TVariant constructor */
        DeviceAttribute devattr("Invalid", -1.0);
        devattr.quality = Tango::ATTR_INVALID;
        /* TVariant throws and exception if the DeviceAttribute passed is empty and now we do not
       * want it to happen.
       */
        TVariant v(devattr, aiex, d_source);
        /* then mark the TVariant as invalid */
        v.setExecutionFailed(true);
        v.setMessage(e.errors);
        v.setQuality(Tango::ATTR_INVALID);
        d_confParams->setCurrentValue(v);
        pwarn("auto configuration: failed to perform first read_attribute() on \"%s\"", qstoc(d_source));
    }
}

void QTangoAutoConfiguration::unsubscribe()
{
    QString attributeName = d_source;
    attributeName = attributeName.remove(QRegExp("\\(.*\\)")); /* remove arguments if present */
    if(attributeName.count('/') == 3)
    {
        attributeName = attributeName.split("/").last();
        qDebug() << "QTangoAutoConfiguration::unsubscribe() " << source() << QThread::currentThread() << ":" << this << "- unsubscribe_event() for attribute configuration change";
        if(eventId() == -1)
        {
            pok("attribute \"%s\" was not subscribed to attribute configuration events: not unsubscribing.",
                qstoc(attributeName));
        }
        else
        {
            try{
                d_device->unsubscribe_event(d_eventId);
                d_eventId = -1;
                pok("\e[1;37;42munsubscribed\e[0m \"%s\" from the \e[1;37;42mattribute configuration event\e[0m.",
                    qstoc(source()));
            }
            catch(DevFailed &e)
            {
                if(!ActionFactory::actionFactory()->loggingDisabled())
                {
                    TLog log(e);
                    TUtil::instance()->addLog(log.toTimeErrlistPair(), log.formatMessage());
                }
                perr("There was an error unsubscribing the \e[4mattribute configuration event\e[0m for the \"%s/%s\"",
                     d_device->name().c_str(), qstoc(attributeName));
            }
        }
    }
}

void QTangoAutoConfiguration::readDesiredAttributeProperties()
{
    Database db(d_device->get_db_host(), d_device->get_db_port_num());
    DbData db_data;
    QString attributeName = d_source.split("/").last();
    attributeName.remove(QRegExp("\\(.*\\)"));
    db_data.push_back(DbDatum(attributeName.toStdString().c_str()));
    QStringList ret;
    foreach(QString s, d_desiredAttributeProperties)
    {
        ret.clear();
        try
        {
            std::string attName = attributeName.toStdString();
            if(mDbTimeoutMillis > 0)
                db.set_timeout_millis(mDbTimeoutMillis);

            db.get_device_attribute_property(d_device->name(), db_data);
            vector<string> vs;

            for(size_t i = 0; i < db_data.size(); i++)
            {
                long nb_prop;
                string &attname = db_data[i].name;
                db_data[i] >> nb_prop;
                i++;
                for(int k = 0; k < nb_prop; k++)
                {
                    string &prop_name = db_data[i].name;
                    qprintf("attName %s attributeName %s prop_name %s s %s\n", attname.c_str(), qstoc(attributeName),
                            prop_name.c_str(), qstoc(s));
                    if(QString::fromStdString(attname).compare(attributeName, Qt::CaseInsensitive) == 0 &&
                            QString::fromStdString(prop_name).compare(s, Qt::CaseInsensitive) == 0)
                    {
                        if(!db_data[i].is_empty())
                        {
                            db_data[i] >> vs;
                        }
                    }
                    i++;
                }
            }
            for(size_t i = 0; i < vs.size(); i++)
                ret.push_back(QString::fromStdString(vs[i]));
            qprintf("\e[1;32m ATTRIBUTE PROPERTY \e[0;4m%s\e[0m: \n", qstoc(s));
            qslisttoc(ret);
            d_confParams->addProperty(s, ret);
        }
        catch (DevFailed &e)
        {
            TLog log(e);
            TUtil::instance()->addLog(log);
            perr("failed to read property: %s attribute %s", qstoc(s), qstoc(attributeName));
            perr("Error message: %s", qstoc(log.formatMessage()));
        }
    }
}

AttributeInfoEx QTangoAutoConfiguration::getConfigurationParamsOnce()
{
    AttributeInfoEx attrInfoEx;
    QStringList possibleValues;
    QString attributeName = d_source.split("/").last();
    attributeName.remove(QRegExp("\\(.*\\)"));
    try
    {
        attrInfoEx = d_device->get_attribute_config(attributeName.toStdString());
        buildConfigParams(&attrInfoEx);
        /* step II: the attribute might have the "values" attribute property */
        if(d_desiredAttributeProperties.size())
            readDesiredAttributeProperties();
        return attrInfoEx;
    }
    catch (DevFailed &e)
    {
        Tango::Except::re_throw_exception(e, "failed to get attribute configuration for \"" + d_device->name() +
                                          "\", attribute: \"" +  attributeName.toStdString() + "\".", "Failed to execute `get_attribute_config()'",
                                          "QTangoAutoConfiguration::getConfigurationParamsOnce()", Tango::ERR);
    }
    return attrInfoEx;
}

AttributeInfoEx QTangoAutoConfiguration::attributeInfoEx()
{
    AttributeInfoEx aie;
    if(d_confParams)
        aie = d_confParams->attributeInfoEx();
    return aie;
}

void QTangoAutoConfiguration::push_event(AttrConfEventData * e)
{
    if(e != NULL && e->err)
    {
        perr("\e[1;37;41mattribute auto configuration event error\e[0m [device: \"%s\" attribute: \"%s\"]!",
             e->device->name().c_str(), e->attr_name.c_str());
        if(!ActionFactory::actionFactory()->loggingDisabled())
        {
            TLog log;
            TUtil::instance()->addLog(log.toTimeErrlistPair(e->errors), log.formatMessage(e->errors));
        }
    }
    else
    {
        qDebug() << "QTangoAutoConfiguration::push_event() " << e->attr_name.c_str() << QThread::currentThread() << ":" << this << "- attribute configuration event";
        pinfo("attribute \e[1;37;42mconfiguration change event\e[0m for \"%s\"", qstoc(source()));
        if(d_confEventNo > 0) /* do not postEvent on first push_event(), since it is always invoked on subscribe_event() */
        {
            buildConfigParams(e->attr_conf);
            ActionConfigurationChangedEvent *acce = new ActionConfigurationChangedEvent(d_source, d_confParams);
            QCoreApplication::instance()->postEvent(ActionFactory::actionFactory(), acce);
            if(d_desiredAttributeProperties.size())
                readDesiredAttributeProperties();
        }
    }
    d_confEventNo++;
}





