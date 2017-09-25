#include "enum2qstring.h"
#include <QStringList>


QString Enum2String::CmdArgType2String(const Tango::CmdArgType i)
{
  if(i < Tango::DEV_INT) /* watch for fix: dev encoded missing in CmdArgTypeName */
	return QString(Tango::CmdArgTypeName[i]);
  return "index error";
}
	
QString Enum2String::AttrDataFormat2String(const Tango::AttrDataFormat a)
{
  QStringList attrDataFormatName  =  QStringList() << "SCALAR" << "SPECTRUM" << "IMAGE" << "FORMAT UNKNOWN";
  if(a < attrDataFormatName.size())
	return attrDataFormatName.at(a);
  return "index error";
}

QString Enum2String::AttrWriteType2String(const Tango::AttrWriteType t)
{
  QStringList attrWriteTypeName =  QStringList() << "READ" << "READ_WITH_WRITE" << "WRITE" << "READ_WRITE";
  if(t < attrWriteTypeName.size())
	return attrWriteTypeName.at(t);
  return "index error";
}

