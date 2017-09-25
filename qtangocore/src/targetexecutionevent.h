#ifndef TARGETEXECUTIONEVENT_H
#define TARGETEXECUTIONEVENT_H

#include <QEvent>
#include <time.h>
#include <QString>

class TargetExecutionEvent : public QEvent
{
public:
    enum Type { TargetExecution = QEvent::User + 1 };

    TargetExecutionEvent(const QString& target, const time_t timestamp, const bool success = true,
                         const QString& exception = QString(), const QString& data = QString());

    /** \brief the timestamp of the execution
      *
      * Represents the date and time when the user clicked or applied a value
      * invoking the execute method of a QTangoComProxyWriter.
      */
    time_t timestamp;

    /** \brief returns the status of the execute call.
      *
      * @return true the execution succeeded.
      * @return false the execution failed.
      *
      */
    bool success;

    /** \brief returns the tango target.
      *
      * @return the tango target, as usual in the form a/b/c/Attribute or a/b/c->Command
      *
      */
    QString target;

    /** \brief The exception string, which is filled in if success is false.
      *
      */
    QString exception;

    /** \brief This is unused.
     *
     * In the future it might contain additional information, such as objectName.
     */
    QString data;
};

#endif // TARGETEXECUTIONEVENT_H
