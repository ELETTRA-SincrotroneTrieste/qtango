#ifndef INT_DELEGATE_H
#define INT_DELEGATE_H

#include "number_delegate.h"

class IntDelegate : public NumberDelegate
{
	public:
		IntDelegate() : NumberDelegate() {};
		
		virtual ~IntDelegate() {};

		virtual void setValue(int) = 0;
		virtual void setMinimum(int) = 0;
		virtual void setMaximum(int) = 0;
};


#endif
