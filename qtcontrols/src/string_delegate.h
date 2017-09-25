#ifndef STRING_DELEGATE_H
#define STRING_DELEGATE_H

#include <QString>

class StringDelegate
{
	public:
		StringDelegate() {};
		virtual ~StringDelegate() {};
		virtual void setValue(const QString &) = 0;
};

#endif
