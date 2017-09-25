#ifndef TLOG_H
#define TLOG_H

#include <tango.h>
#include <QPair>
#include <QString>
#include <time.h>

using namespace Tango;

/** \brief utility class to manage Tango exception messages and prepare them to be 
 *         used with the TUtil singleton class
 *
 * This class, introduced in QTango <em>4.x</em> provides utility methods to format 
 * Tango exceptions into QStrings and produces objects ready to be inserted in the 
 * log list managed by the qtangocore TUtil class.
 * Using this class inside your application, and above all inside DeviceThreads, 
 * improves its performance: TUtil static singleton is invoked to store log information
 * only when such information is prepared by TLog. This reduces the portion of code that
 * must be locked against thread concurrency inside TUtil.
 *
 * @see TUtil::log
 *
 * \par Example
 * \code 
 * // ...
 * catch(Tango::DevFailed &e)
 * {
 *   TLog logInfo(e); 
 *   // TUtil::addLog acquires the lock to protect the list of logs against concurrent threads,
 *   // but we use TLog inside our thread to do the formatting and the couple creation.
 *   TUtil::addLog(logInfo.toTimeErrlistPair(), logInfo.formatMessage());
 * }
 * \endcode
 *
 * Examples can be found in taction.cpp
 *
 * If you have a DevErrorList, for instance provided by AttrConfEventData <em>errors</em> member,
 * or by EventData <em>errors</em> member, then you can instantiate a TLog class without any parameter
 * and then simply use formatMessage(const DevErrorList&) to obtain a string representation of the
 * DevErrorList. At the moment, to keep this class light, a TLog constructor accepting a DevErrorList
 * as input argument is not provided. Instead, use formatMessage(const DevErrorList& errors) and
 * toTimeErrlistPair(const DevErrorList& errors).
 * 
 * \par Example
 * \code
 *  void TVariant::setMessage(const DevErrorList& el)
 * {
  *  TLog log;
  *  d_message = log.formatMessage(el);
 *   TUtil::instance()->addLog(log.toTimeErrlistPair(el), d_message);
 * }
 *
 * \endcode
 *
 */

class TLog
{
  public:
    /** \brief The class constructor. 
	 *
	 * Constructs a TLog object
	 *
	 * @param exception the Tango::DevFailed, passed by reference.
	 *
	 * Use this in conjunction with formatMessage() to obtain a string representing all the 
	 * errors listed inside DevFailed Tango structure.
	 *
	 */
	TLog(const Tango::DevFailed &exception);
	
	/** \brief constructs a TLog object with the given error list.
	 *
	 * @param errors a Tango::DevErrorList structure passed as const reference.
	 * 
	 * When TLog is initialized with a parameter of type DevErrorList, the formatMessage()
	 * method will format the ErrorList provided. Using the constructor with DevFailed 
	 * parameter allows formatMessage() to include all the errors in the list stored by
	 * DevFailed.
	 *
	 */
	TLog(const Tango::DevErrorList& errors);
	
	/** \brief a constructor without parameters. You can use it to create the object 
	 *         and then use formatMessage with a DevErrorList as argument.
	 *
	 */
	TLog();
  
	/** \brief returns a pair of time_t and DevErrorList ready to use with TUtil::log
	 *
	 * This method gets the current time and extracts the DevErrorList from the Tango exception.
	 *
	 * @return a couple containing the time and the DevErrorList pertaining to the exception.
	 *         This can be directly passed to TUtil::log.
	 */
	QPair<time_t, DevErrorList> toTimeErrlistPair();

	/** \brief formats the information contained into TLog into a readable string
	 * 
	 * @return a nice formatted QString
	 *
	 * If the TLog class was contstructed with a DevFailed as parameter, all the errors in the 
	 * error list contained into DevFailed are included in the formatted string returned.
	 * Otherwise, only the DevErrorList provided inside the TLog constructor is formatted by this method.
	 */
	QString formatMessage();
	
	/** \brief Given a DevErrorList, it formats a string.
	 *
	 * This method can be simply used to format into a readable string a DevErrorList.
	 *
	 * @param errors a Tango DevErrorList you want to format into a readable string.
	 */
	QString formatMessage(const DevErrorList& errors);
	
	/** \brief returns a pair of time_t and DevErrorList ready to use with TUtil::log
	 *
	 * This method gets the current time and places it aside the DevErrorList passed as input
	 * argument. This might be used in conjunction with the TLog constructor without parameters
	 * if you have a DevErrorList but not a full DevFailed Tango class.
	 *
	 * @return a couple containing the time and the DevErrorList pertaining to the exception.
	 *         This can be directly passed to TUtil::log.
	 *
	 * Examples can be found in TVariant::setMessage(const DevErrorList &), tvariant.cpp and in 
	 * TAction::push_event(EventData*), taction.cpp.
	 */
	QPair<time_t, DevErrorList> toTimeErrlistPair(const DevErrorList& errors);
	
	/** \brief formats a message given a couple time_t, DevErrorList as input argument.
	 *
	 * This is used internally, but can also be called after creating a TLog object with the 
	 * empty constructor, as TLogDialog class does in tlogdialog.cpp
	 */
	QString formatMessage(QPair<time_t, DevErrorList> errors);
	
  private:
	Tango::DevFailed d_devFailed;
	Tango::DevErrorList d_errList;
	bool d_usingErrorList;
};

#endif
