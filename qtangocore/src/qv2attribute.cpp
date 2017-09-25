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

/* $Id: qv2attribute.cpp,v 1.8 2012-05-31 08:46:31 giacomo Exp $ */


#include "qv2attribute.h"
#include "qtangocore_macros.h"
#include <QtDebug>

using namespace std;
using namespace Tango;

QVariantConverter::QVariantConverter(const QString& attname, const AttributeInfoEx& inf)
{
	name = attname.toStdString();
	info = inf;
}

QVariantConverter::QVariantConverter(const QString& attname, const CommandInfo& cinf)
{
  name = attname.toStdString();
  cmdinfo = cinf;
}

DeviceAttribute QVariantConverter::convertToAttribute(QVariant &v)
{
	int i;
	QVariant typeDiscriminator;
	QList<QVariant> varlist;
	
	std::vector<double> dv;
	std::vector<std::string> sv;
	std::vector<short> shv;
	std::vector<unsigned short> ushv;
    std::vector<DevULong> ulv;
    std::vector<DevLong> lv;
	std::vector<bool> bv;
        std::vector<float> fv;
	
	if(info.data_format == SCALAR)
	{
		std::string stds;
		DevLong dlong;
		DevULong dulong;
		DevShort ds;
		DevUShort us;
		switch(info.data_type)
		{
			case DEV_DOUBLE:
			  if(v.canConvert(QVariant::Double))
					return DeviceAttribute(name, v.toDouble());
			case DEV_FLOAT:
                                if(v.canConvert(QVariant::Double))
				{
                                  float f = v.toFloat();
					return DeviceAttribute(name, f);
				}
			case DEV_STRING:
				if(v.canConvert(QVariant::String))
				{
					stds = v.toString().toStdString();
					return DeviceAttribute(name, stds);
				}
			case DEV_SHORT:
				if(v.canConvert(QVariant::Int))
				{
					ds = (DevShort) v.toInt();
					return DeviceAttribute(name, ds);
				}
			case DEV_USHORT:
				if(v.canConvert(QVariant::UInt))
				{
				  us = (DevUShort) v.toUInt();
					return DeviceAttribute(name, us);
				}
			case DEV_ULONG:
			  if(v.canConvert(QVariant::UInt))
			  {
			    dulong = (DevULong) v.toUInt();
			    return DeviceAttribute(name, dulong);
			  }	
			case DEV_LONG:
				if(v.canConvert(QVariant::Int))
				{
					dlong = (DevLong) v.toInt();
					return DeviceAttribute(name, dlong);
				}
			case DEV_BOOLEAN:
				if(v.canConvert(QVariant::Bool))
				{
					return DeviceAttribute(name, v.toBool());
				}
		}
	}
	else if(info.data_format == SPECTRUM)
	{  
	  
	  if(!v.canConvert(QVariant::List))
	  {
	    perr("cannot convert into a list the data for the attribute named \"%s\"!", name.c_str()) ;
	    return DeviceAttribute();
	  }
	  
	  pok("data array detected: trying to build the attribute \"%s\" now", name.c_str());
	  varlist = v.toList();
	  
	  if(varlist.size() == 0)
	  {
	    pwarn("the size of the data vector is zero.");
	    return DeviceAttribute();
	  }
	  else /* use a QVariant to discriminate the data type in the switch/case below */
	    typeDiscriminator = varlist[0]; 
	
	  switch(info.data_type)
	  {
          case DEV_FLOAT:
              if(typeDiscriminator.canConvert(QVariant::Double))
              {
                for(i = 0; i < varlist.size(); i++)
                  fv.push_back(varlist[i].toFloat());
                return DeviceAttribute(name, fv);
              }
            case DEV_DOUBLE:
	      if(typeDiscriminator.canConvert(QVariant::Double))
	      {
		for(i = 0; i < varlist.size(); i++)
		  dv.push_back(varlist[i].toDouble());
		return DeviceAttribute(name, dv);
	      }
	    case DEV_STRING:
	      if(typeDiscriminator.canConvert(QVariant::String))
	      {
		for(i = 0; i < varlist.size(); i++)
		  sv.push_back(varlist[i].toString().toStdString());
		return DeviceAttribute(name, sv);
	      }
	    case DEV_SHORT:
	      if(typeDiscriminator.canConvert(QVariant::Int))
	      {
		for(i = 0; i < varlist.size(); i++)
		  shv.push_back(varlist[i].toInt());
		return DeviceAttribute(name, shv);
	      }
	    case DEV_USHORT:
	      if(typeDiscriminator.canConvert(QVariant::UInt))
	      {
		for(i = 0; i < varlist.size(); i++)
		  ushv.push_back(varlist[i].toInt());
		return DeviceAttribute(name, ushv);
	      }
	    case DEV_LONG:
	      if(typeDiscriminator.canConvert(QVariant::Int))
	      {
		for(i = 0; i < varlist.size(); i++)
		  lv.push_back(varlist[i].toInt());
		return DeviceAttribute(name, lv);
	      }
	    case DEV_ULONG:
	      if(typeDiscriminator.canConvert(QVariant::UInt))
	      {
		for(i = 0; i < varlist.size(); i++)
		  ulv.push_back(varlist[i].toUInt());
		return DeviceAttribute(name, ulv);
	      } 
	    case DEV_BOOLEAN:
	      if(typeDiscriminator.canConvert(QVariant::Bool))
	      {
		for(i = 0; i < varlist.size(); i++)
		  bv.push_back(varlist[i].toBool());
		return DeviceAttribute(name, bv);
	      }
	  }
	}
	else if(info.data_format == IMAGE)
	{
	  int xdim, ydim;
	  /* fare caso image */
	   if(!v.canConvert(QVariant::List))
	   {
	     perr("cannot convert into a list the data for the attribute named \"%s\"!", name.c_str()) ;
	     return DeviceAttribute();
	   }
	   
	   pok("Successfully convertable into a list");
	   QList<QVariant> varlist = v.toList();
	   ydim = varlist.size();
	   
	   int len = 0, cnt = 0;
	   foreach(QVariant qv, varlist)
	   {
	     if(qv.canConvert(QVariant::List))
	     {
	       QList<QVariant> sublist;
	       sublist = qv.toList();
	       if( (sublist.size() == 0) || (cnt > 0 && sublist.size() != len))
		 break;
	       else /* detect the data type */
		 typeDiscriminator = sublist[0];
	       len = sublist.size();
	       cnt++;
	     }
	   }
	   if(cnt == varlist.size())
	   {
	     xdim = len;
	     pinfo("\e[1;33;4mimage\e[0m detected: extracting matrix");
	     /* un esempio solo di estrazione di image ;-) */
	      switch(info.data_type)
	      {
	       case DEV_DOUBLE:
	       case DEV_FLOAT:
		 if(typeDiscriminator.canConvert(QVariant::Double))
		 {
		   foreach(QVariant qv, varlist)
		   {
		     QList<QVariant> sublist;
		     sublist = qv.toList();
		     foreach(QVariant subv, sublist)
		       dv.push_back(subv.toDouble());
		   }
		   return DeviceAttribute(name, dv, xdim, ydim);
		 }
	       default:
         pwarn("the tango data type %d is not yet implemented for the IMAGE format :(", info.data_type);
	      }
	   }
	}
	
	return DeviceAttribute();
	
}

DeviceData QVariantConverter::convertToDeviceData(QVariant &v)
{
  int i;
  QVariant typeDiscriminator;
  QList<QVariant> varlist;
  DeviceData dd;
  
  std::vector<double> dv;
  std::vector<std::string> sv;
  std::vector<short> shv;
  std::vector<unsigned short> ushv;
  std::vector<DevULong> ulv;
  std::vector<DevLong> lv;
  std::vector<bool> bv;
  
  if(!v.canConvert(QVariant::List))
  {
    std::string stds;
    DevLong dlong;
    DevULong dulong;
    DevShort ds;
    DevUShort us;
    switch(cmdinfo.in_type)
    {
      case DEV_DOUBLE:
      case DEV_FLOAT:
	if(v.canConvert(QVariant::Double))
	  dd << v.toDouble();
	break;
      case DEV_STRING:
	if(v.canConvert(QVariant::String))
	{
	  stds = v.toString().toStdString();
	  dd << stds;
	}
	break;
      case DEV_SHORT:
	if(v.canConvert(QVariant::Int))
	{
	  ds = (DevShort) (v.toInt());
	  dd << ds;
	}
	break;
      case DEV_USHORT:
	if(v.canConvert(QVariant::UInt))
	{
	  us = (DevUShort) (v.toUInt());
	  dd << us;
	}
	break;
      case DEV_ULONG:
	if(v.canConvert(QVariant::UInt))
	{
	  dulong = (DevULong) (v.toUInt());
	  dd << dulong;
	}
	break;
      case DEV_LONG:
	if(v.canConvert(QVariant::Int))
	{
	  dlong = (DevLong) (v.toInt());
	  dd << dlong;
	}
	break;
      case DEV_BOOLEAN:
	if(v.canConvert(QVariant::Bool))
	{
	    dd << v.toBool();
	}
	break;
      default:
    pwarn("Cannot convert the data type %ld into a DeviceData object.", cmdinfo.in_type);
	pwarn("Conversion not possible or not implemented");
	return DeviceData();
    }
    return dd;
  }
  else
  {
    pok("data array detected: trying to build the command \"%s\" now", name.c_str());
    varlist = v.toList();
    
    if(varlist.size() == 0)
    {
      pwarn("the size of the data vector is zero.");
      return DeviceData();
    }
    else /* use a QVariant to discriminate the data type in the switch/case below */
            typeDiscriminator = varlist[0];
    switch(cmdinfo.in_type)
    {
      case DEVVAR_DOUBLEARRAY:
      case DEVVAR_FLOATARRAY:
	if(typeDiscriminator.canConvert(QVariant::Double))
	{
	  for(i = 0; i < varlist.size(); i++)
	    dv.push_back(varlist[i].toDouble());
	  dd << dv;
        }
	break;
      case DEVVAR_STRINGARRAY:
	if(typeDiscriminator.canConvert(QVariant::String))
	{
	  for(i = 0; i < varlist.size(); i++)
	    sv.push_back(varlist[i].toString().toStdString());
	  dd << sv;
	}
	break;
      case DEVVAR_SHORTARRAY:
	if(typeDiscriminator.canConvert(QVariant::Int))
	{
	  for(i = 0; i < varlist.size(); i++)
	    shv.push_back(varlist[i].toInt());
	 dd << shv;
	}
	break;
      case DEVVAR_USHORTARRAY:
	if(typeDiscriminator.canConvert(QVariant::UInt))
	{
	  for(i = 0; i < varlist.size(); i++)
	    ushv.push_back(varlist[i].toInt());
	  dd << ushv;
	}
	break;
      case DEVVAR_LONGARRAY:
	if(typeDiscriminator.canConvert(QVariant::Int))
	{
	  for(i = 0; i < varlist.size(); i++)
	    lv.push_back(varlist[i].toInt());
	  dd << lv;
	}
	break;
      case DEVVAR_ULONGARRAY:
	if(typeDiscriminator.canConvert(QVariant::UInt))
	{
	  for(i = 0; i < varlist.size(); i++)
	    ulv.push_back(varlist[i].toUInt());
	  dd << ulv;
	} 
	break;
      case DEVVAR_BOOLEANARRAY:
	pwarn("vector of boolean not supported among the insert methods of DeviceData");
      default:
    pwarn("Cannot convert the data type %ld into a DeviceData object.", cmdinfo.in_type);
	pwarn("Conversion not possible or not implemented");
	return DeviceData();
    }
    return dd;
  }
  
  return DeviceData();
  
}

