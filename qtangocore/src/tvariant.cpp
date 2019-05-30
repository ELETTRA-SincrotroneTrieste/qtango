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

#include <tvariant.h>
#include <tutil.h>
#include <elettracolors.h>
#include <QThread>
#include <QtDebug>
#include <sys/time.h>
#include <typeinfo>
#include <eng_notation.h>
#include <QVariant>
#include <tlog.h>
#include <action_factory.h> /* for loggingDisabled() */
#include "timagedata_impl.h"

class TVariantPrivate
{
public:
    int dimx, dimy, dimx_w, dimy_w;
};

TVariant::TVariant()
{
    init();
}

TVariant::TVariant(const QString& tangoPoint)
{
    d_tangoPoint = tangoPoint;
    init();
}

TVariant::TVariant(const TVariant &o)
{
    init();
    d_type = o.d_type;
    d_quality = o.d_quality;
    isAttribute = o.isAttribute;
    d_executionFailed = o.d_executionFailed;

    dataReadStringVector = o.dataReadStringVector;
    dataReadDoubleVector			= o.dataReadDoubleVector;
    dataReadUIntVector	= o.dataReadUIntVector;
    dataReadIntVector			= o.dataReadIntVector;
    dataReadBoolVector		= o.dataReadBoolVector;
    dataReadStateVector     = o.dataReadStateVector;

    dataSetStringVector	= o.dataSetStringVector;
    dataSetDoubleVector	= o.dataSetDoubleVector;
    dataSetUIntVector	= o.dataSetUIntVector;
    dataSetIntVector	= o.dataSetIntVector;
    dataSetBoolVector	= o.dataSetBoolVector;

    d_message	= o.d_message;
    d_tangoPoint	= o.d_tangoPoint;

    d_attrInfo	= o.d_attrInfo;
    d_cmdInfo	= o.d_cmdInfo;

    d_timeRead  = o.d_timeRead;
    d_ptr->dimx = o.d_ptr->dimx;
    d_ptr->dimy = o.d_ptr->dimy;
    d_ptr->dimx_w = o.d_ptr->dimx_w;
    d_ptr->dimy_w = o.d_ptr->dimy_w;
}

TVariant::TVariant(DeviceAttribute &attr, const AttributeInfo& info, const QString& tangoPoint)
{
    qDebug() << QThread::currentThread() << ":" << this << "- ctr <<";
    init();
    setDeviceAttribute(attr, info);
    d_tangoPoint = tangoPoint;
    qDebug() << QThread::currentThread() << ":" << this << "- ctr >>";
}


TVariant::TVariant(DeviceData &data, const CommandInfo& info, const QString& tangoPoint)
{	
    qDebug() << QThread::currentThread() << ":" << this << "- ctr <<";
    init();
    d_tangoPoint = tangoPoint;
    setDeviceData(data, info);
    qDebug() << QThread::currentThread() << ":" << this << "- ctr >>";
}

TVariant &TVariant::operator=(const TVariant &o)
{
    delete d_ptr;
    init();
    d_type = o.d_type;
    d_quality = o.d_quality;
    isAttribute = o.isAttribute;
    d_executionFailed = o.d_executionFailed;

    dataReadStringVector = o.dataReadStringVector;
    dataReadDoubleVector = o.dataReadDoubleVector;
    dataReadUIntVector	 = o.dataReadUIntVector;
    dataReadIntVector			= o.dataReadIntVector;
    dataReadBoolVector		= o.dataReadBoolVector;
    dataReadStateVector     = o.dataReadStateVector;

    dataSetStringVector	= o.dataSetStringVector;
    dataSetDoubleVector	= o.dataSetDoubleVector;
    dataSetUIntVector	= o.dataSetUIntVector;
    dataSetIntVector	= o.dataSetIntVector;
    dataSetBoolVector	= o.dataSetBoolVector;

    d_message	= o.d_message;
    d_tangoPoint	= o.d_tangoPoint;

    d_attrInfo	= o.d_attrInfo;
    d_cmdInfo	= o.d_cmdInfo;

    d_timeRead  = o.d_timeRead;
    d_ptr->dimx = o.d_ptr->dimx;
    d_ptr->dimy = o.d_ptr->dimy;
    d_ptr->dimx_w = o.d_ptr->dimx_w;
    d_ptr->dimy_w = o.d_ptr->dimy_w;

    return *this;
}

void TVariant::setDeviceAttribute(DeviceAttribute &attr, const AttributeInfo& info, bool byEvent)
{
    isAttribute = true;
    d_attrInfo = info;
    TimeVal timev = attr.get_date();
    d_timeRead.tv_sec = timev.tv_sec;
    d_timeRead.tv_usec = timev.tv_usec;
    if (attr.is_empty())
    {
        qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - device attribute is empty";
        setMessage("device attribute is empty");
        d_quality = ATTR_INVALID;
        return;
    }
    /* attribute data type, if attribute is not empty */
    d_type = (CmdArgType) attr.get_type();

    if (attr.has_failed())
    {
        qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - read_attribute has failed";
        setMessage(attr.get_err_stack());
        d_quality = ATTR_INVALID;
        return;
    }
    d_quality = attr.quality;
    setMessage(attr, byEvent);
    try {
        unsigned int read = attr.get_nb_read();
        AttributeDimension adim_r = attr.get_r_dimension();
        AttributeDimension adim_w = attr.get_w_dimension();
        d_ptr->dimx = adim_r.dim_x;
        d_ptr->dimy = adim_r.dim_y;
        d_ptr->dimx_w = adim_w.dim_x;
        d_ptr->dimy_w = adim_w.dim_y;

        switch (d_type)
        {
        case DEV_STATE:
        {
            dataReadStateVector.clear();
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_STATE";
            // seems that tango is not able to exctact correctly devState >> vector<DevState>
            if (read == 1) {
                DevState temp;
                attr >> temp;
                dataReadStateVector.push_back(temp);
            } else {
                vector<DevState> temp;
                attr >> temp;
                for (unsigned i=0; i<read; i++) {
                    dataReadStateVector.push_back(temp[i]);
                }
            }
            break;
        }
        case DEV_BOOLEAN:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_BOOLEAN";
            vector<bool> temp;
            attr >> temp;
            dataReadBoolVector.clear();
            dataSetBoolVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadBoolVector.push_back(temp[i]);
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetBoolVector.push_back(temp[read+i]);
            break;
        }
        case DEV_SHORT:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_SHORT";
            vector<short> temp;
            attr >> temp;
            dataReadIntVector.clear();
            dataSetIntVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadIntVector.push_back((short) temp[i]);
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetIntVector.push_back((short) temp[read+i]);
            break;
        }
        case DEV_USHORT:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_USHORT";
            vector<unsigned short> temp;
            attr >> temp;
            dataReadUIntVector.clear();
            dataSetUIntVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadUIntVector.push_back((unsigned short) temp[i]);
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetUIntVector.push_back((unsigned short) temp[read+i]);
            break;
        }
        case DEV_LONG:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_LONG";
            vector<DevLong> temp;
            attr >> temp;
            dataReadIntVector.clear();
            dataSetIntVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadIntVector.push_back((int) temp[i]);
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetIntVector.push_back((int) temp[read+i]);
            break;
        }
        case DEV_ULONG:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_ULONG";
            vector<DevULong> temp;
            attr >> temp;
            dataReadUIntVector.clear();
            dataSetUIntVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadUIntVector.push_back((unsigned int) temp[i]);
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetUIntVector.push_back((unsigned int) temp[read+i]);
            break;
        }
        case DEV_FLOAT:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_FLOAT";
            vector<float> temp;
            attr >> temp;
            dataReadDoubleVector.clear();
            dataSetDoubleVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadDoubleVector.push_back((float) temp[i]);
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetDoubleVector.push_back((float) temp[read+i]);
            break;
        }
        case DEV_DOUBLE:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_DOUBLE";
            vector<double> temp;
            attr >> temp;
            dataReadDoubleVector.clear();
            dataSetDoubleVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadDoubleVector.push_back((double) temp[i]);
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetDoubleVector.push_back((double) temp[read+i]);
            break;
        }
        case DEV_STRING:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_STRING";
            vector<string> temp;
            attr >> temp;
            dataReadStringVector.clear();
            dataSetStringVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadStringVector.push_back(QString::fromStdString(temp[i]));
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetStringVector.push_back(QString::fromStdString(temp[read+i]));
            break;
        }
        case DEV_UCHAR:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceAttribute() - DEV_STRING";
            vector<unsigned char> temp;
            attr >> temp;
            dataReadUIntVector.clear();
            dataSetUIntVector.clear();
            for (unsigned int i = 0; i < read; i++)
                dataReadUIntVector.push_back(temp[i]);
            for (unsigned int i = 0; i < (temp.size()-read); i++)
                dataSetUIntVector.push_back(temp[read+i]);
            break;
        }
        default:
            d_message = "wrong data type";
            d_quality = ATTR_INVALID;
            break;
        }
    } catch (DevFailed &e) {
        setMessage(e.errors);
        d_quality = ATTR_INVALID;
    }
}

void TVariant::setDeviceData(DeviceData &data, const CommandInfo &info)
{	
    isAttribute = false;
    d_cmdInfo 	= info;
    /* Put out_type because TVariant is normally used to present the information
     * contained to the user, and it is not used inside command_inout.
     */
    d_type = (CmdArgType) info.out_type;
    /* in case of commands, set the execution time */
    gettimeofday(&d_timeRead, NULL);
    d_ptr->dimx =  d_ptr->dimy = d_ptr->dimx_w = d_ptr->dimy_w = 0;

    switch (d_type)
    {
    case DEV_SHORT:
    case DEV_USHORT:
    case DEV_LONG:
    case DEV_ULONG:
        /* format spectrum elements of commands argout consistently */
    case DEVVAR_SHORTARRAY:
    case DEVVAR_USHORTARRAY:
    case DEVVAR_LONGARRAY:
    case DEVVAR_ULONGARRAY:
        d_attrInfo.format = "%d";
        break;
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEVVAR_FLOATARRAY:
    case DEVVAR_DOUBLEARRAY:
        d_attrInfo.format = "%.2f";
        break;
    default:
        d_attrInfo.format = "";
        break;
    }

    if (data.is_empty())
    {
        setMessage("device data is empty");
        d_quality = ATTR_INVALID;
        return;
    }
    else
        d_quality = ATTR_VALID;
    setMessage();
    qDebug() << "data type del command in setDeviceData:" << d_type;
    try {
        switch (d_type)
        {
        case DEV_STATE:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_STATE";
            DevState temp;
            data >> temp;
            dataReadStateVector.clear();
            dataReadStateVector.push_back(temp);
            d_ptr->dimx = dataReadStateVector.size();
            break;
        }
        case DEV_BOOLEAN:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_BOOLEAN";
            DevBoolean booltmp;
            data >> booltmp;
            dataReadBoolVector.clear();
            dataSetBoolVector.clear();
            dataReadBoolVector.push_back((bool) booltmp);
            d_ptr->dimx = dataReadBoolVector.size();
            break;
        }
        case DEV_SHORT:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_SHORT";
            DevShort temp;
            data >> temp;
            dataReadIntVector.clear();
            dataSetIntVector.clear();
            dataReadIntVector.push_back(temp);
            d_ptr->dimx = dataReadIntVector.size();
            break;
        }
        case DEV_USHORT:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_USHORT";
            DevUShort temp;
            data >> temp;
            dataReadUIntVector.clear();
            dataSetUIntVector.clear();
            dataReadUIntVector.push_back((unsigned int) temp);
            d_ptr->dimx = dataReadUIntVector.size();
            break;
        }
        case DEV_LONG:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_LONG";
            DevLong temp;
            data >> temp;
            dataReadIntVector.clear();
            dataSetIntVector.clear();
            dataReadIntVector.push_back((int) temp);
            d_ptr->dimx = dataReadIntVector.size();
            break;
        }
        case DEV_ULONG:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_ULONG";
            DevULong temp;
            data >> temp;
            dataReadUIntVector.clear();
            dataSetUIntVector.clear();
            dataReadUIntVector.push_back((unsigned int) temp);
            d_ptr->dimx = dataReadUIntVector.size();
            break;
        }
        case DEV_FLOAT:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_FLOAT";
            DevFloat temp;
            data >> temp;
            dataReadDoubleVector.clear();
            dataSetDoubleVector.clear();
            dataReadDoubleVector.push_back((double) temp);
            d_ptr->dimx = dataReadDoubleVector.size();
            break;
        }
        case DEV_DOUBLE:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_DOUBLE";
            DevDouble temp;
            data >> temp;
            dataReadDoubleVector.clear();
            dataSetDoubleVector.clear();
            dataReadDoubleVector.push_back(temp);
            d_ptr->dimx = dataSetDoubleVector.size();
            break;
        }
        case DEV_STRING:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - DEV_STRING";
            string temp;
            data >> temp;
            dataReadStringVector.clear();
            dataSetStringVector.clear();
            dataReadStringVector.push_back(QString::fromStdString(temp));
            d_ptr->dimx = dataReadStringVector.size();
            break;
        }
        case DEVVAR_BOOLEANARRAY:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - vector DEVVAR_BOOLEANARRAY - NOT IMPLEMENTED";
            perr("TVariant::setDeviceData: boolean array is not implemented");
            break;
        }
        case DEVVAR_SHORTARRAY:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - vector DEVVAR_SHORTARRAY";
            vector<short> temp;
            data >> temp;
            vector<short>::iterator it;
            dataReadIntVector.clear();
            dataSetIntVector.clear();
            for (it = temp.begin(); it < temp.end(); it++)
                dataReadIntVector.push_back((int) *it);
            d_ptr->dimx = dataReadIntVector.size();
            break;
        }
        case DEVVAR_USHORTARRAY:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - vector DEVVAR_USHORTARRAY";
            vector<unsigned short> temp;
            data >> temp;
            vector<unsigned short>::iterator it;
            dataReadUIntVector.clear();
            dataSetUIntVector.clear();
            for (it = temp.begin(); it < temp.end(); it++)
                dataReadUIntVector.push_back((unsigned int) *it);
            d_ptr->dimx = dataReadUIntVector.size();
            break;
        }
        case DEVVAR_LONGARRAY:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - vector DEVVAR_LONGARRAY";
            vector<DevLong> temp;
            data >> temp;
            vector<DevLong>::iterator it;
            dataReadIntVector.clear();
            dataSetIntVector.clear();
            for (it = temp.begin(); it < temp.end(); it++)
                dataReadIntVector.push_back((int) *it);
            d_ptr->dimx = dataReadIntVector.size();
            break;
        }
        case DEVVAR_ULONGARRAY:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - vector DEVVAR_ULONGARRAY";
            vector<DevULong> temp;
            data >> temp;
            vector<DevULong>::iterator it;
            dataReadUIntVector.clear();
            dataSetUIntVector.clear();
            for (it = temp.begin(); it < temp.end(); it++)
                dataReadIntVector.push_back((unsigned int) *it);
            d_ptr->dimx = dataReadIntVector.size();
            break;
        }
        case DEVVAR_FLOATARRAY:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - vector DEVVAR_FLOATARRAY";
            vector<float> temp;
            data >> temp;
            vector<float>::iterator it;
            dataReadDoubleVector.clear();
            dataSetDoubleVector.clear();
            for (it = temp.begin(); it < temp.end(); it++)
                dataReadDoubleVector.push_back((double) *it);
            d_ptr->dimx = dataReadDoubleVector.size();
            break;
        }
        case DEVVAR_DOUBLEARRAY:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - vector DEVVAR_DOUBLEARRAY";
            vector<double> temp;
            data >> temp;
            dataSetDoubleVector.clear();
            dataReadDoubleVector = QVector<double>::fromStdVector(temp);
            d_ptr->dimx = dataReadDoubleVector.size();
            break;
        }
        case DEVVAR_STRINGARRAY:
        {
            qDebug() << QThread::currentThread() << ":" << this << "setDeviceData() - vector DEVVAR_STRINGARRAY";
            vector<string> temp;
            data >> temp;
            vector<string>::iterator it;
            dataReadStringVector.clear();
            dataSetStringVector.clear();
            for (it = temp.begin(); it < temp.end(); it++)
                dataReadStringVector.push_back(QString::fromStdString(*it));
            d_ptr->dimx = dataReadStringVector.size();
            break;
        }
        default:
            setMessage("wrong data type");
            d_quality = ATTR_INVALID;
            break;
        }
    } catch (DevFailed &e) {
        setMessage(e.errors);
        d_quality = ATTR_INVALID;
    }
}

int TVariant::dimX() const
{
    return d_ptr->dimx;
}

int TVariant::dimY() const
{
    return d_ptr->dimy;
}

int TVariant::w_dimX() const
{
    return d_ptr->dimx_w;
}

int TVariant::w_dimY() const
{
    return d_ptr->dimy_w;
}

void TVariant::init()
{
    d_ptr = new TVariantPrivate();
    d_executionFailed = true;
    d_quality = ATTR_INVALID;
    d_ptr->dimx = d_ptr->dimy = d_ptr->dimx_w = d_ptr->dimy_w = 0;
}

TVariant::~TVariant()
{
    delete d_ptr;
}

bool TVariant::canConvertToState() const
{
    switch (d_type)
    {
    case DEV_STATE:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToString() const
{
    if((isAttribute && (d_attrInfo.data_format != Tango::SCALAR)))
        return false;

    /* commands have special CmdArgType for arrays. Look for them */
    if(!isAttribute)
    {
        switch(d_type)
        {
        case DEVVAR_BOOLEANARRAY:
        case DEVVAR_SHORTARRAY:
        case DEVVAR_USHORTARRAY:
        case DEVVAR_LONGARRAY:
        case DEVVAR_ULONGARRAY:
        case DEVVAR_FLOATARRAY:
        case DEVVAR_DOUBLEARRAY:
        case DEVVAR_STRINGARRAY:
            return false;
        default:
            break;
        }
    }

    switch (d_type)
    {
    case DEV_STATE:
    case DEV_BOOLEAN:
    case DEV_SHORT:
    case DEV_USHORT:
    case DEV_LONG:
    case DEV_ULONG:
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEV_STRING:
    case DEV_UCHAR:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToDouble() const
{
    if(isAttribute && d_attrInfo.data_format != Tango::SCALAR)
        return false;
    switch (d_type)
    {
    case DEV_SHORT:
    case DEV_USHORT:
    case DEV_LONG:
    case DEV_ULONG:
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEV_UCHAR:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToUInt() const
{
    if(isAttribute && d_attrInfo.data_format != Tango::SCALAR)
        return false;
    switch (d_type)
    {
    case DEV_STATE:
    case DEV_USHORT:
    case DEV_ULONG:
    case DEV_SHORT:
    case DEV_LONG:
    case DEV_UCHAR:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToInt() const
{
    if(isAttribute && d_attrInfo.data_format != Tango::SCALAR)
        return false;
    switch (d_type)
    {
    case DEV_STATE:
    case DEV_SHORT:
    case DEV_USHORT:
    case DEV_LONG:
    case DEV_ULONG:
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEV_UCHAR:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToBool() const
{
    if(isAttribute && d_attrInfo.data_format != Tango::SCALAR)
        return false;
    switch (d_type)
    {
    case DEV_BOOLEAN:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToStringVector() const
{
    if(isAttribute && (d_attrInfo.data_format == Tango::SCALAR ||
                       d_attrInfo.data_format == Tango::IMAGE))
        return false;

    /* commands have special CmdArgType for arrays. Look for them */
    if(!isAttribute)
    {
        switch(d_type)
        {
        case DEVVAR_BOOLEANARRAY:
        case DEVVAR_SHORTARRAY:
        case DEVVAR_USHORTARRAY:
        case DEVVAR_LONGARRAY:
        case DEVVAR_ULONGARRAY:
        case DEVVAR_FLOATARRAY:
        case DEVVAR_DOUBLEARRAY:
        case DEVVAR_STRINGARRAY:
            return true;
        default:
            break;
        }
        return false;
    }

    /* attributes instead have data_format to distinguish between spectrum or scalar
     * argout. The check is performed above.
     */
    switch (d_type)
    {
    case DEV_STATE:
    case DEV_BOOLEAN:
    case DEV_SHORT:
    case DEV_USHORT:
    case DEV_LONG:
    case DEV_ULONG:
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEV_STRING:
    case DEV_UCHAR:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToDoubleVector() const
{
    if(isAttribute && (d_attrInfo.data_format == Tango::SCALAR ||
                       d_attrInfo.data_format == Tango::IMAGE))
        return false;

    switch (d_type)
    {
    case DEVVAR_DOUBLEARRAY:
    case DEVVAR_USHORTARRAY:
    case DEVVAR_ULONGARRAY:
    case DEVVAR_LONGARRAY:
    case DEVVAR_FLOATARRAY:
    case DEV_SHORT:
    case DEV_USHORT:
    case DEV_LONG:
    case DEV_ULONG:
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEV_UCHAR:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToUIntVector() const
{
    if(isAttribute && (d_attrInfo.data_format == Tango::SCALAR ||
                       d_attrInfo.data_format == Tango::IMAGE))
        return false;
    switch (d_type)
    {
    case DEVVAR_USHORTARRAY:
    case DEVVAR_ULONGARRAY:
    case DEVVAR_FLOATARRAY:
    case DEV_STATE:
    case DEV_USHORT:
    case DEV_ULONG:
    case DEV_SHORT:
    case DEV_LONG:
    case DEV_UCHAR:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToIntVector() const
{
    if(isAttribute && (d_attrInfo.data_format == Tango::SCALAR ||
                       d_attrInfo.data_format == Tango::IMAGE))
        return false;
    switch (d_type)
    {
    case DEVVAR_USHORTARRAY:
    case DEVVAR_ULONGARRAY:
    case DEVVAR_FLOATARRAY:
    case DEVVAR_DOUBLEARRAY:
    case DEVVAR_LONGARRAY:
    case DEV_STATE:
    case DEV_SHORT:
    case DEV_USHORT:
    case DEV_LONG:
    case DEV_ULONG:
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEV_UCHAR:
        return true;
    default:
        return false;
    }
}

bool TVariant::canConvertToBoolVector() const
{
    if(isAttribute && (d_attrInfo.data_format == Tango::SCALAR ||
                       d_attrInfo.data_format == Tango::IMAGE))
        return false;
    switch (d_type)
    {
    case DEV_BOOLEAN:
        return true;
    default:
        return false;
    }
}

/** \brief Returns true if TVariant can be converted to a double image
 *
 * @param strict if true, the data type must be double.
 *
 * \par Types of data that can be converted.
 * \li The source must be a DeviceAttribute
 *
 * If strict is false, the data type can be:
 * \li short
 * \li unsigned short
 * \li long
 * \li unsigned long
 * \li float
 * \li double
 * \li unsigned char
 *
 * If strict is true, then the data type must a double.
 *
 * @see toDoubleImage
 *
 */
bool TVariant::canConvertToDoubleImage(bool strict) const
{
    /* only attributes, IMAGE format */
    if(isAttribute && (d_attrInfo.data_format == Tango::IMAGE))
    {
        switch (d_type)
        {
        case DEV_DOUBLE:
            return true;
        case DEV_SHORT:
        case DEV_USHORT:
        case DEV_LONG:
        case DEV_ULONG:
        case DEV_FLOAT:
        case DEV_UCHAR:
            return !strict;
        default:
            break;
        }
    }
    return false;
}

/** \brief Returns true if the type of data can be converted to unsigned char
 *
 * @param strict true the method returns true only if the data type is unsigned char
 * @param strict false the method returns true for (u)short, (u)long, double, float.
 *
 * \li The source must be a DeviceAttribute
 *
 * The behaviour is the same as canConvertToDoubleImage, applied to unsigned char.
 *
 * @see toUCharImage
 *
 */
bool TVariant::canConvertToUCharImage(bool strict) const
{
    if(isAttribute && (d_attrInfo.data_format == Tango::IMAGE))
    {
        switch (d_type)
        {
        case DEV_UCHAR:
            return true;
        case DEV_DOUBLE:
        case DEV_SHORT:
        case DEV_USHORT:
        case DEV_LONG:
        case DEV_ULONG:
        case DEV_FLOAT:
            return !strict;
        default:
            break;
        }
    }
    return false;
}

/** \brief Returns true if TVariant can be converted to an image of unsigned integers
 *
 * @param strict true the method returns true only if the data type is unsigned long
 * @param strict false the method returns true for (u)short, unsigned char, double, float.
 *
 * \li The source must be a DeviceAttribute
 *
 * The behaviour is equivalent to canConvertToDoubleImage, applied to unsigned int.
 *
 * @see toUIntImage
 *
 */
bool TVariant::canConvertToUIntImage(bool strict) const
{
    if(isAttribute && (d_attrInfo.data_format == Tango::IMAGE))
    {
        switch (d_type)
        {
        case DEV_ULONG:
            return true;
        case DEV_UCHAR:
        case DEV_DOUBLE:
        case DEV_SHORT:
        case DEV_USHORT:
        case DEV_LONG:
        case DEV_FLOAT:
            return !strict;
        default:
            break;
        }
    }
    return false;
}

/** \brief Returns true if TVariant can be converted to an image of boolean values
 *
 * @param strict true the method returns true only if the data type is bool
 * @param strict false the method returns true for (u)short, unsigned char, double, float, (u)long,
 *        since toBoolImage converts every non zero value to true, zero to false.
 *
 * \li The source must be a DeviceAttribute
 *
 * @see toBoolImage
 *
 */
bool TVariant::canConvertToBoolImage(bool strict) const
{
    if(isAttribute && (d_attrInfo.data_format == Tango::IMAGE))
    {
        switch (d_type)
        {
        case DEV_BOOLEAN:
            return true;
        case DEV_UCHAR:
        case DEV_DOUBLE:
        case DEV_SHORT:
        case DEV_USHORT:
        case DEV_LONG:
        case DEV_ULONG:
        case DEV_FLOAT:
            return !strict;
        default:
            break;
        }
    }
    return false;
}

DevState TVariant::toState() const
{
    return dataReadStateVector.value(0);
}

QString TVariant::toString(bool read, bool forceNoUnit) const
{
    QString temp;
    Config* conf = Config::instance();
    std::string dataFormat = d_attrInfo.format;

    switch(d_type)
    {
    case DEV_SHORT:
    case DEV_LONG:
    case DEV_USHORT:
    case DEV_ULONG:
    case DEV_UCHAR:
        if(strstr(d_attrInfo.format.c_str(), "f"))
        {
            // 				pwarn("\"%s\" data type is integer, while format (\"%s\") contains 'f' conversion specifier.",
            // 				     d_attrInfo.name.c_str(), d_attrInfo.format.c_str());
            // 				pstep("\e[4mhint\e[0m: check the attribute string format into the tango database... setting \"%%d\" for you");
            dataFormat = string("%d");
        }
        break;
    case DEV_FLOAT:
    case DEV_DOUBLE:
        if(strstr(d_attrInfo.format.c_str(), "d"))
        {
            // 				pwarn("\"%s\" data type is floating point, while format (\"%s\") contains 'd' conversion specifier.",
            // 				      d_attrInfo.name.c_str(), d_attrInfo.format.c_str());
            // 				pstep("\e[4mhint\e[0m: check the attribute string format into the tango database... setting \"%%f\" for you");
            dataFormat = string("%f");
        }
        break;

    default:
        break;
    }

    switch (d_type)
    {
    case DEV_STATE:
        return conf->stateString(dataReadStateVector.value(0));
    case DEV_BOOLEAN:
        if (dataReadBoolVector.value(0))
            return "True";
        else
            return "False";
    case DEV_SHORT:
        if (read)
            temp = format<int>(dataFormat, dataReadIntVector.value(0));
        else
            temp = format<int>(dataFormat, dataSetIntVector.value(0));
        break;
    case DEV_LONG:
        if (read)
            temp = format<int>(dataFormat, dataReadIntVector.value(0));
        else
            temp = format<int>(dataFormat, dataSetIntVector.value(0));
        break;
    case DEV_USHORT:
    case DEV_ULONG:
    case DEV_UCHAR:
        if (read)
            temp = format<unsigned int>(dataFormat, dataReadUIntVector.value(0));
        else
            temp = format<unsigned int>(dataFormat, dataSetUIntVector.value(0));
        break;
    case DEV_FLOAT:
    case DEV_DOUBLE:
        if (read)
            temp = format<double>(dataFormat, dataReadDoubleVector.value(0));
        else
            temp = format<double>(dataFormat, dataSetDoubleVector.value(0));
        break;
    case DEV_STRING:
        if (read)
            temp = dataReadStringVector.value(0);
        else
            temp = dataSetStringVector.value(0);
        break;
    default:
        return QString("QString TVariant::toString() could not correctly interpret the type");
    }
    QString displayUnit = QString::fromStdString(d_attrInfo.display_unit);
    QRegExp re("no.*unit", Qt::CaseInsensitive);
    if (!displayUnit.contains(re) && (displayUnit != QString()) && (displayUnit != "None") && !forceNoUnit)
        temp += " [" + QString::fromStdString(d_attrInfo.display_unit) + "]";
    return temp;
}

int TVariant::toInt(bool read) const
{
    switch (d_type)
    {
    case DEV_STATE:
        return (int) dataReadStateVector.value(0);
    case DEV_USHORT:
    case DEV_ULONG:
    case DEV_UCHAR:
        if (read)
            return (int) dataReadUIntVector.value(0);
        else
            return (int) dataSetUIntVector.value(0);
    case DEV_SHORT:
    case DEV_LONG:
        if (read)
            return dataReadIntVector.value(0);
        else
            return dataSetIntVector.value(0);
    case DEV_FLOAT:
    case DEV_DOUBLE:
        if (read)
            return (int) dataReadDoubleVector.value(0);
        else
            return (int) dataSetDoubleVector.value(0);
    default:
        return 0;
    }
}

unsigned int TVariant::toUInt(bool read) const
{
    switch (d_type)
    {
    case DEV_STATE:
        return (unsigned int) dataReadStateVector.value(0);
    case DEV_USHORT:
    case DEV_ULONG:
    case DEV_UCHAR:
        if (read)
            return dataReadUIntVector.value(0);
        else
            return dataSetUIntVector.value(0);
    case DEV_SHORT:
    case DEV_LONG:
        if (read)
            return dataReadIntVector.value(0);
        else
            return dataSetIntVector.value(0);
    default:
        return 0;
    }
}

double TVariant::toDouble(bool read) const
{
    switch (d_type)
    {
    case DEV_SHORT:
    case DEV_LONG:
        if (read)
            return (double) dataReadIntVector.value(0);
        else
            return (double) dataSetIntVector.value(0);
    case DEV_ULONG:
    case DEV_USHORT:
    case DEV_UCHAR:
        if (read)
            return (double) dataReadUIntVector.value(0);
        else
            return (double) dataSetUIntVector.value(0);
    case DEV_FLOAT:
    case DEV_DOUBLE:
        if (read)
            return dataReadDoubleVector.value(0);
        else
            return dataSetDoubleVector.value(0);
    default:
        return 0;
    }
}

bool TVariant::toBool(bool read) const
{
    switch (d_type)
    {
    case DEV_BOOLEAN:
        if (read)
            return dataReadBoolVector.value(0);
        else
            return dataSetBoolVector.value(0);
    default:
        return false;
    }
}	

QVector<QString> TVariant::toStringVector(bool read) const
{
    switch (d_type)
    {
    case DEV_BOOLEAN:
    case DEVVAR_BOOLEANARRAY:
    {
        const QVector<bool> *vec = read ? &dataReadBoolVector : &dataSetBoolVector;
        QVector<QString> temp;
        foreach (bool d, *vec)
            if (d)
                temp.push_back("True");
            else
                temp.push_back("False");
        return temp;
    }
    case DEV_SHORT:
    case DEV_LONG:
    case DEVVAR_SHORTARRAY:
    case DEVVAR_LONGARRAY:
    {
        const QVector<int> *vec = read ? &dataReadIntVector : &dataSetIntVector;
        QVector<QString> temp;
        foreach (int d, *vec)
            temp.push_back(format<int>(d_attrInfo.format, d));
        return temp;
    }
    case DEV_USHORT:
    case DEV_ULONG:
    case DEVVAR_USHORTARRAY:
    case DEVVAR_ULONGARRAY:
    {
        const QVector<unsigned int> *vec = read ? &dataReadUIntVector : &dataSetUIntVector;
        QVector<QString> temp;
        foreach (unsigned int d, *vec)
            temp.push_back(format<unsigned int>(d_attrInfo.format, d));
        return temp;
    }
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEVVAR_FLOATARRAY:
    case DEVVAR_DOUBLEARRAY:
    {
        const QVector<double> *vec = read ? &dataReadDoubleVector : &dataSetDoubleVector;
        QVector<QString> temp;
        qDebug() << "vector before format " << *vec;
        foreach (double d, *vec)
            temp.push_back(format<double>(d_attrInfo.format, d));
        return temp;
    }
    case DEV_STRING:
    case DEVVAR_STRINGARRAY:
        if (read)
            return dataReadStringVector;
        else
            return dataSetStringVector;
    default:
        return QVector<QString>();
    }
}

QVector<int> TVariant::toIntVector(bool read) const
{
    switch (d_type)
    {
    case DEV_SHORT:
    case DEV_LONG:
    case DEVVAR_SHORTARRAY:
    case DEVVAR_LONGARRAY:
        if (read)
            return dataReadIntVector;
        else
            return dataSetIntVector;
    case DEV_USHORT:
    case DEV_ULONG:
    case DEVVAR_USHORTARRAY:
    case DEVVAR_ULONGARRAY:
    {
        const QVector<unsigned int> *vec = read ? &dataReadUIntVector : &dataSetUIntVector;
        QVector<int> temp;
        foreach (unsigned int d, *vec)
            temp.push_back((int) d);
        return temp;
    }
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEVVAR_FLOATARRAY:
    case DEVVAR_DOUBLEARRAY:
    {
        const QVector<double> *vec = read ? &dataReadDoubleVector : &dataSetDoubleVector;
        QVector<int> temp;
        foreach (double d, *vec)
            temp.push_back((int) d);
        return temp;
    }
    default:
        return QVector<int>();
    }
}

QVector<unsigned int> TVariant::toUIntVector(bool read) const
{
    switch (d_type)
    {
    case DEV_USHORT:
    case DEV_ULONG:
    case DEVVAR_USHORTARRAY:
    case DEVVAR_ULONGARRAY:
        if (read)
            return dataReadUIntVector;
        else
            return dataSetUIntVector;
    default:
        return QVector<unsigned int>();
    }
}

QVector<double> TVariant::toDoubleVector(bool read) const
{
    switch (d_type)
    {
    case DEV_SHORT:
    case DEV_LONG:
    case DEVVAR_SHORTARRAY:
    case DEVVAR_LONGARRAY:
    {
        const QVector<int> *vec = read ? &dataReadIntVector : &dataSetIntVector;
        QVector<double> temp;
        foreach (int d, *vec)
            temp.push_back((double) d);
        return temp;
    }
    case DEV_USHORT:
    case DEV_ULONG:
    case DEVVAR_USHORTARRAY:
    case DEVVAR_ULONGARRAY:
    {
        const QVector<unsigned int> *vec = read ? &dataReadUIntVector : &dataSetUIntVector;
        QVector<double> temp;
        foreach (unsigned int d, *vec)
            temp.push_back((double) d);
        return temp;
    }
    case DEV_FLOAT:
    case DEV_DOUBLE:
    case DEVVAR_FLOATARRAY:
    case DEVVAR_DOUBLEARRAY:
        if (read)
            return dataReadDoubleVector;
        else
            return dataSetDoubleVector;
    default:
        return QVector<double>();
    }
}

TImageData<double> TVariant::toDoubleImage(bool read) const
{
    switch (d_type)
    {
    case DEV_SHORT:
    case DEV_LONG:
        if(read)
            return TImageData<double>(convert<double>(dataReadIntVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<double>(convert<double>(dataSetIntVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_USHORT:
    case DEV_ULONG:
    case DEV_UCHAR:
        if(read)
            return TImageData<double>(convert<double>(dataReadUIntVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<double>(convert<double>(dataSetUIntVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_FLOAT:
    case DEV_DOUBLE:
        if (read)
            return TImageData<double>(dataReadDoubleVector, d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        else
            return TImageData<double>(dataSetDoubleVector, d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);
    default:
        break;
    }
    return TImageData<double>();
}

TImageData<unsigned char> TVariant::toUCharImage(bool read) const
{
    switch (d_type)
    {
    case DEV_UCHAR:
        if(read)
            return TImageData<unsigned char>(convert<unsigned char>(dataReadUIntVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<unsigned char>(convert<unsigned char>(dataSetUIntVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_SHORT:
    case DEV_LONG:
        if(read)
            return TImageData<unsigned char>(convert<unsigned char>(dataReadIntVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<unsigned char>(convert<unsigned char>(dataSetIntVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_USHORT:
    case DEV_ULONG:
        if(read)
        {
            TImageData<unsigned char> tid(convert<unsigned char>(dataReadUIntVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
            //TImageData<unsigned char> tid;
            return tid;
        }
        return TImageData<unsigned char>(convert<unsigned char>(dataSetUIntVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);
    case DEV_FLOAT: case DEV_DOUBLE:
        if (read)
            return TImageData<unsigned char>(convert<unsigned char>(dataReadDoubleVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        else
            return TImageData<unsigned char>(convert<unsigned char>(dataSetDoubleVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    default:
        break;
    }
    return TImageData<unsigned char>();
}

TImageData<unsigned int> TVariant::toUIntImage(bool read) const
{
    switch (d_type)
    {
    case DEV_UCHAR:
    case DEV_USHORT:
    case DEV_ULONG:
        if(read)
            return TImageData<unsigned int>(dataReadUIntVector, d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<unsigned int>(dataSetUIntVector, d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_SHORT:
    case DEV_LONG:
        if(read)
            return TImageData<unsigned int>(convert<unsigned int>(dataReadIntVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<unsigned int>(convert<unsigned int>(dataSetIntVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_FLOAT:
    case DEV_DOUBLE:
        if (read)
            return TImageData<unsigned int>(convert<unsigned int>(dataReadDoubleVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        else
            return TImageData<unsigned int>(convert<unsigned int>(dataSetDoubleVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    default:
        break;
    }
    return TImageData<unsigned int>();
}

TImageData<bool> TVariant::toUBoolImage(bool read) const
{
    switch (d_type)
    {
    case DEV_BOOLEAN:
        if(read)
            return TImageData<bool>(dataReadBoolVector, d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<bool>(dataSetBoolVector, d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_UCHAR:
    case DEV_USHORT:
    case DEV_ULONG:
        if(read)
            return TImageData<bool>(convert<bool>(dataReadUIntVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<bool>(convert<bool>(dataSetUIntVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_SHORT:
    case DEV_LONG:
        if(read)
            return TImageData<bool>(convert<bool>(dataReadIntVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        return TImageData<bool>(convert<bool>(dataSetIntVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    case DEV_FLOAT:
    case DEV_DOUBLE:
        if (read)
            return TImageData<bool>(convert<bool>(dataReadDoubleVector), d_ptr->dimx, d_ptr->dimy, d_quality, d_type);
        else
            return TImageData<bool>(convert<bool>(dataSetDoubleVector), d_ptr->dimx_w, d_ptr->dimy_w, d_quality, d_type);

    default:
        break;
    }
    return TImageData<bool>();
}

QVector<bool> TVariant::toBoolVector(bool read) const
{
    switch (d_type)
    {
    case DEV_BOOLEAN:
    case DEVVAR_BOOLEANARRAY:
        if (read)
            return dataReadBoolVector;
        else
            return dataSetBoolVector;
    default:
        return QVector<bool>();
    }
}

QVector<DevState> TVariant::toStateVector(bool read) const
{
    Q_UNUSED(read);
    switch (d_type) {
    case DEV_STATE:
        return dataReadStateVector;
    default:
        return QVector<DevState>();
    }
}

void TVariant::setMessage(DeviceAttribute &attr, bool byEvent)
{
    string mess;
    if(isAttribute)
        mess = "\"" + attr.get_name() + "\"";
    else
        mess = "\"" + d_cmdInfo.cmd_name + "\"";

    if(byEvent)
        mess += ": last read (E): ";
    else
        mess += ": last read (P): ";
    time_t time_read = attr.get_date().tv_sec;
    mess += ctime(&time_read);
    mess.erase(mess.size()-1, 1);
    d_message = QString::fromStdString(mess);
}

void TVariant::setMessage(QString s)
{
    d_message = s;
}

void TVariant::setMessage()
{
    string mess("Last read: ");
    time_t now = time(NULL);
    mess += ctime(&now);
    mess.erase(mess.size()-1, 1);
    d_message = QString::fromStdString(mess);
}

void TVariant::setMessage(const DevErrorList& el)
{
    TLog log;
    d_message = log.formatMessage(el);
    /* true: no console! */
    if(!ActionFactory::actionFactory()->loggingDisabled())
        TUtil::instance()->addLog(log.toTimeErrlistPair(el), d_message, true);
}

template<typename T> QString TVariant::format(string str, T attr) const
{
    QString s = QString::fromStdString(str);
    QString returnVal;
    QVariant v(attr);
    if (s.contains("%"))
    {
        EngString copy (QString().sprintf(s.toStdString().c_str(), attr), s, v);
        return copy;
        // 		return QString().sprintf(s.toStdString().c_str(), attr);
    }
    else /* rely upon QString::arg() for a conversion */
    {
        pwarn("format not recognized: \"%s\": relying on QString::arg() for \"%s\"", qstoc(s),
              qstoc(d_tangoPoint));
        returnVal = QString("%1").arg(attr);
        return returnVal;

        /* buggy code */
        //        QTextStream ostr;
        //        QString str = "";
        //        ostr.setString(&str, QIODevice::Text| QIODevice::ReadWrite);
        //        QStringList l = s.split(";", QString::SkipEmptyParts);
        //        foreach (QString s, l)
        //            manipulate(ostr, s);
        //         qDebug() << attr << "format " << s;
        //         ostr << attr;
        //         qDebug() << attr;
        //         qDebug() << "operatore >> verso ritorno isWritable" << ostr.device()->isWritable();
        //         ostr >> returnVal;
        //         qDebug() << "assegnato ritorno";
        //         return returnVal;
    }
}
void TVariant::manipulate(QTextStream &ostr, QString &manip) const
{
    qDebug() << QThread::currentThread() << ":" << this << "- manipulate << ";
    if (manip == "fixed")
        ostr.setRealNumberNotation(QTextStream::FixedNotation);
    //ostr.setf(ios::fixed,ios::floatfield);
    else if (manip == "scientific")
        ostr.setRealNumberNotation(QTextStream::ScientificNotation);
    //ostr.setf(ios::scientific,ios::floatfield);
    else if (manip == "uppercase")
        ostr.setNumberFlags(ostr.numberFlags() | QTextStream::UppercaseBase |  QTextStream::UppercaseDigits);
    //ostr.setf(ios::uppercase);
    else if (manip == "showpoint")
        ostr.setNumberFlags(ostr.numberFlags() | QTextStream::ForcePoint);
    //ostr.setf(ios::showpoint);
    else if (manip == "showpos")
        ; /* ??? */
    //ostr.setf(ios::showpos);
    else
    {
        QRegExp re("setprecision\\((\\d+)\\)");
        if (re.exactMatch(manip))
            ostr.setRealNumberPrecision(re.capturedTexts()[1].toUInt());
        else
        {
            QRegExp re("setw\\((\\d+)\\)");
            if (re.exactMatch(manip))
                ostr.setFieldWidth(re.capturedTexts()[1].toUInt());
        }
    }
    qDebug() << QThread::currentThread() << ":" << this << "- manipulate >> ";
}

template<typename Tout, typename Tin>
QVector<Tout> TVariant::convert(const QVector<Tin> &in) const
{
    QVector<Tout> v;
    foreach (Tin d, in)
        v.push_back(static_cast<Tout>(d));
    return v;
}
