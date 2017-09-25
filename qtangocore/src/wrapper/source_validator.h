#ifndef SOURCE_VALIDATOR_H
#define SOURCE_VALIDATOR_H

#include <QString>
#include <QRegExpValidator>

class QTangoSourceValidator  : public QRegExpValidator
{
	public:
		QTangoSourceValidator(QObject *parent = NULL);
		QString message() { return d_msg; }
		QValidator::State validate(QString &, int &) const;
		void fixup(QString &in);
		
		void setRegexp2();
		
	protected:
		
		
	private:
		QString d_msg;
};

#endif
