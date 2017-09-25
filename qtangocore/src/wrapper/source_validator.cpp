#include "source_validator.h"
#include "qtangocore_macros.h"


QTangoSourceValidator::QTangoSourceValidator(QObject *parent) :
	QRegExpValidator(parent)
{
	/* NOT escaped */
	// (([A-Za-z_0-9]*[:]{1}[0-9]+[/])?(([A-Za-z_0-9]+/[A-Za-z_0-9]+/[A-Za-z_0-9]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9]+)|(\$[1-9]*([/]{1,1}|[->]{2})[A-Za-z_0-9]+)){1}([\(]{1}[&A-Za-z_0-9\.]+[\)]{1})?[;]?){1,}
	
	/* escaped */
	// $1->double_scalar(&ui.spinbox);test/device/1/double_scalar;$1/double_spectrum(&tspinbox2);test/device/4->DevDouble
	//"((([A-Za-z_0-9]+/[A-Za-z_0-9]+/[A-Za-z_0-9]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9]+)|(\\$[1-9]*([/]{1,1}|[->]{2})[A-Za-z_0-9]+)){1}([\\(]{1}[&A-Za-z_0-9/@#\\.]+[\\)]{1})?[;]?){1,}"
		QRegExp re("(([A-Za-z_0-9\\.]*[:]{1}[0-9]+[/])?(([A-Za-z_0-9\\$\\.\\-]+/[A-Za-z_0-9]+/[A-Za-z_0-9\\$\\.\\-]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9\\$\\.\\-]+)|(\\$[1-9]*([/]{1,1}|[->]{2})[A-Za-z_0-9\\.\\-]+)){1}([\\(]{1}[&A-Za-z_0-9/@#,\\.\\-]+[\\)]{1})?[;]?){1,}");
		setRegExp(re);
}

QValidator::State QTangoSourceValidator::validate(QString &s, int & pos) const
{
	return QRegExpValidator::validate(s, pos);
}

void QTangoSourceValidator::fixup(QString &in) 
{
    if((in.contains('$') && (in.count('/') != 1)) || ((!in.contains('/') && !in.contains("->")) ))
		d_msg = QString("If the string contains \"$\", this must be followed by an integer and then\n"
				"   \"/attribute_name\" or \"->command name\"");
	else if(!in.contains('$'))
	{
		if(in.contains(':')) /* hokuto:20000 ... */
		{
			if(in.contains("->") &&  in.count('/') != 3)
				d_msg = QString("the correct pattern is tango_host:PORT/device/family/member->command_name");
			else if(!in.contains("->") && (in.count('/') != 4 ))
				d_msg = QString("the correct pattern is tango_host:PORT/device/family/member/attribute_name");
		}
		else
		{
			if(in.contains("->") &&  in.count('/') != 2)
				d_msg =  QString("the correct pattern is device/family/member->command_name for a command");
			else if(!in.contains("->") && (in.count('/') != 3 ))
				d_msg =  QString("the correct pattern is device/family/member/attribute_name for an attribute");
		}
	}
	QRegExpValidator::fixup(in);
}

void QTangoSourceValidator::setRegexp2()
{
	/* not escaped */
	//  (([A-Za-z_0-9]*[:]{1}[0-9]+[/])?(([A-Za-z_0-9]+/[A-Za-z_0-9]+/[A-Za-z_0-9]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9]+)){1}([\(]{1}[&A-Za-z_0-9\.]+[\)]{1})?[;]?){1,}
	QRegExp re("(([A-Za-z_0-9\\$\\.\\-]*[:]{1}[0-9]+[/])?(([A-Za-z_0-9\\-\\$\\.]+/[A-Za-z_0-9\\$\\.\\-]+/[A-Za-z_0-9\\$\\.\\-]+([/]{1,1}|[->]{2,2})[A-Za-z_0-9\\$\\.\\-]+)){1}([\\(]{1}[&A-Za-z_0-9/#@\\.,\\-]+[\\)]{1})?[;]?){1,}");
	setRegExp(re);
}


