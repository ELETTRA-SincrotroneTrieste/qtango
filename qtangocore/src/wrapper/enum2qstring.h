#include <tango.h>
#include <QString>

/** \brief a class to convert tango enum values into readable QStrings
 *
 */
class Enum2String
{
  public:
	static QString CmdArgType2String(const Tango::CmdArgType i);
	static QString AttrDataFormat2String(const Tango::AttrDataFormat a);
	static QString AttrWriteType2String(const Tango::AttrWriteType t);	
};



