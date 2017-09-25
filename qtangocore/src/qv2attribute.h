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

/* $Id: qv2attribute.h,v 1.4 2008-09-03 10:16:57 giacomo Exp $ */

#ifndef QV_TO_ATTRIBUTE_H
#define QV_TO_ATTRIBUTE_H

#include <tango.h>
#include <QVariant>

using namespace Tango;

class QVariantConverter
{
	public:
		QVariantConverter(const QString& attname, const AttributeInfoEx& info);
		
		QVariantConverter(const QString& attname, const CommandInfo& info);
		
		/** \brief tries to build a DeviceAttribute with the QVariant provided
		*   according to the attribute data type and format detected by means
		*   of the info.
		*
		* @return The DeviceAttribute built or an empty DeviceAttribute (test with is_empty()
		* tango method!) if the QVariant conversion was unsuccessful.
		*
		* For scalar attributes, QVariant can be a scalar QVariant of any supported Tango data type;
		* For spectrum attributes, the QVariant must contain a QList:
		* 	
		*	QVariantConverter qvc("test/device/1/double_spectrum", attributeInfo);
		*	QList<double> double_spectrum;
		*	QVariant double_list(double_spectrum);
		*	DeviceAttribute double_sp_attribute = qvc.convert(double_list);
		*
		* For image attribute, QVariant must contain 2 nested QLists as in the example below:
		*
		* QList<QVariant> ul, ul2; // an image of unsigned long, for instance 
		* ul << 1 << 2 << 3 << 4 << 5;
		* ul2 << 10 << 20 << 30 << 40 << 50; // ul1.size() MUST be EQUAL to ul2.size()
		* QList <QVariant> ulnest;
		* ulnest << QVariant(ul) <<  QVariant(ul)  <<  QVariant(ul2);
		* v = QVariant(ulnest); // create the QVariant with the `image' made up of the nested lists
		* DeviceAttribute d_attrargin = converter.convert(v);
		*
		*/
		DeviceAttribute convertToAttribute(QVariant &v);
		
		DeviceData convertToDeviceData(QVariant &v);
		
	private:
		std::string name;
		AttributeInfoEx info;
		CommandInfo cmdinfo;
};


#endif


