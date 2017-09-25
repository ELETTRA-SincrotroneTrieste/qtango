#include "tlog.h"

TLog::TLog(const DevFailed &exception)
{
  d_devFailed = exception;
  d_usingErrorList = false;
}

TLog::TLog(const Tango::DevErrorList& errors)
{
  d_errList = errors;
  d_usingErrorList = true;
}

TLog::TLog()
{
  
}

QPair<time_t, DevErrorList> TLog::toTimeErrlistPair(const DevErrorList& errors)
{
  time_t now = time(NULL);
  QPair<time_t, DevErrorList> err(now, errors);
  return err;
}

QPair<time_t, DevErrorList> TLog::toTimeErrlistPair()
{
  time_t now = time(NULL);
  if(d_usingErrorList)
  {
	QPair<time_t, DevErrorList> err(now, d_errList);
	return err;
  }
  else
  {
	QPair<time_t, DevErrorList> err(now, d_devFailed.errors);
	return err;
  }
}

QString TLog::formatMessage(QPair<time_t, DevErrorList> errors)
{
  QString mess(ctime(&errors.first));
  if (!mess.isEmpty())
	mess.remove(mess.size()-1, 1);
  mess += ":\n";
  for (unsigned long i = 0;i < errors.second.length();i++)
  {
	  mess += "\nDesc : ";
	  mess += errors.second[i].desc.in();
	  mess += "\nOrigin : ";
	  mess += errors.second[i].origin.in();
	  mess += "\nReason : ";
	  mess += errors.second[i].reason.in();
	  mess += "\nSeverity : ";
	  switch (errors.second[i].severity)
	  {
		  case Tango::WARN:
			  mess += "WARNING ";
			  break;
		  case Tango::ERR :
			  mess += "ERROR ";
			  break;
		  case Tango::PANIC :
			  mess += "PANIC ";
			  break;
		  default:
			  mess+=  "Unknown severity code ";
			  break;
	  }
	  mess += "\n\n";
  }
  mess += "---------------------------------------------------------\n";
  return mess;
}

QString TLog::formatMessage(const DevErrorList& errors)
{
  time_t now = time(NULL);
  QPair<time_t, DevErrorList> errCouple(now, errors);
  return formatMessage(errCouple);
}

QString TLog::formatMessage()
{
  QString message;
  time_t now = time(NULL);
  if(d_usingErrorList)
  {
	QPair<time_t, DevErrorList> errCouple(now, d_errList);
	message = formatMessage(errCouple);
  }
  else
  {
	QPair<time_t, DevErrorList> errCouple(now, d_devFailed.errors);
	message = formatMessage(errCouple);
  }
  return message;
}




