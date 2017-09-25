/***************************************************************************
*   Copyright (C) 2008 by  Vincenzo Forchi`	  *
*     *
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

#ifndef _QVARIANTH
#define _QVARIANTH

#include <qtangocore_common.h>
#include <QString>
#include <QColor>
#include <QVector>
#include <QMap>
#include <QTextStream>
#include <QMetaType>
#include <configuration.h>
#include <eng_notation.h>
#include <timagedata.h>

class TVariantPrivate;

struct timeval;
 
using namespace Tango;

class TVariant
{
public:
	TVariant(DeviceAttribute&, const AttributeInfo&, const QString& tangoPoint);
	TVariant(DeviceData&, const CommandInfo &, const QString& tangoPoint);
	TVariant(const QString& tangoPoint);
    TVariant(const TVariant &o);
	TVariant();
	~TVariant();

    TVariant & operator=(const TVariant & other); // copy assignment
	
	/** \brief returns the tango point, source or target */
	QString tangoPoint() const  { return d_tangoPoint; }
	
	void setTangoPoint(const QString& name) { d_tangoPoint = name; }
	
	void init();
	void setDeviceAttribute(DeviceAttribute&, const AttributeInfo&, bool byEvent = false);
	void setDeviceData(DeviceData&, const CommandInfo&);

	bool isCommand() const { return !isAttribute; }
	
    CmdArgType type() const  { return d_type; }
	
	AttributeInfo getAttributeInfo()  const { return d_attrInfo; }
	CommandInfo getCommandInfo()  const { return	d_cmdInfo; }

    int dimX() const;
    int dimY() const;
    int w_dimX() const;
    int w_dimY() const;

	bool	canConvertToState() const;
	bool	canConvertToString() const;
	bool	canConvertToInt() const;
	bool	canConvertToUInt() const;
	bool	canConvertToDouble() const;
	bool	canConvertToBool() const;
	
	bool	canConvertToStringVector() const;
	bool	canConvertToIntVector() const;
	bool	canConvertToUIntVector() const;
    bool	canConvertToDoubleVector() const;
	bool	canConvertToBoolVector() const;

    bool    canConvertToDoubleImage(bool strict = false) const;
    bool    canConvertToUCharImage(bool strict = false) const;
    bool    canConvertToUIntImage(bool strict = false) const;
    bool    canConvertToBoolImage(bool strict = false) const;

	DevState		toState() const;
	QString			toString(bool read = true, bool forceNoUnit = false) const;
	int				toInt(bool = true) const;
	unsigned int	toUInt(bool = true) const;
	double			toDouble(bool = true) const;
	bool			toBool(bool = true) const;
	
	QVector<QString>	toStringVector(bool = true) const;
	QVector<int>		toIntVector(bool = true) const;
	QVector<unsigned int>	toUIntVector(bool = true) const;
    QVector<double>		toDoubleVector(bool = true) const;
	QVector<bool>		toBoolVector(bool = true) const;
    QVector<DevState>       toStateVector(bool = true) const;


    TImageData<double>		toDoubleImage(bool read = true) const;
    TImageData<unsigned char> toUCharImage(bool read = true) const;
    TImageData<unsigned int> toUIntImage(bool read = true) const;
    TImageData<bool> toUBoolImage(bool read = true) const;


	void	setMessage(const DevErrorList&);
	void	setMessage(DeviceAttribute&, bool byEvent = false);
	void	setMessage(QString);
	void	setMessage();
	
	/** \brief sets a flag indicating that the execution of an action failed.
	 * 
	 * This is used by TAction inside execute() and push_event()
	 * 
	 * @see message
	 * @see executionFailed
	 */
	void setExecutionFailed(bool failed) { d_executionFailed = failed; }
	
	/** \brief returns the executionFailed flag.
	 *
	 * Used by TAction.
	 * You can check this flag to see if the write_attribute(), read_attribute() or command_inout()
	 * failed. You can then retreive the message().
	 * 
	 * @see message
	 * @see executionFailed
	 */
	bool executionFailed() const { return d_executionFailed; }
	
    QString	message() const { return d_message; }
	
    void		setQuality(AttrQuality q) { d_quality = q; }
    AttrQuality	quality() const { return d_quality; }
	
	/**
	 * get reading time (if reading an attribute) or current time (if executing a command)
	 */
	const struct timeval* timeReadRef() const { return &d_timeRead; } 
	
	struct timeval timeRead() const { return d_timeRead; }

	template<typename T> QString format(string, T) const;

protected:
	void	manipulate(QTextStream &, QString &) const;

	CmdArgType	d_type;
	AttrQuality	d_quality;
	bool		isAttribute;
	bool 		d_executionFailed;

	QVector<QString>		dataReadStringVector;
	QVector<double>			dataReadDoubleVector;
	QVector<unsigned int>	dataReadUIntVector;
	QVector<int>			dataReadIntVector;
	QVector<bool>			dataReadBoolVector;
	QVector<DevState>       dataReadStateVector;

	QVector<QString>		dataSetStringVector;
	QVector<double>			dataSetDoubleVector;
	QVector<unsigned int>	dataSetUIntVector;
	QVector<int>			dataSetIntVector;
	QVector<bool>			dataSetBoolVector;

	QString			d_message;
	QString 		d_tangoPoint;

	AttributeInfo	d_attrInfo;
	CommandInfo		d_cmdInfo;
	
	struct timeval d_timeRead;

private:
    TVariantPrivate *d_ptr;

    template<typename Tout, typename Tin>
    QVector<Tout> convert(const QVector<Tin>& in ) const;
};

Q_DECLARE_METATYPE(TVariant)

#endif
