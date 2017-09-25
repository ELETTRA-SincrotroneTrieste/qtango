#include "read_data.h"

Timestamp::Timestamp(QString nam, unsigned h, unsigned m, unsigned s, unsigned ms, 
	bool err, bool ev, QString msg, QString obji, unsigned idx)
{
	hours = h;
	min = m;
	sec = s;
	msec = ms;	
	error = err;
	event = ev;
	message = msg;
	objinfo = obji;
	name = nam;
	index = idx;
}

Timestamp::Timestamp()
{
	hours = min = sec = msec = 0;
	error = false;
	event = 0;
}

ReadData::ReadData(QString nam)
{
	name = nam;
}

ReadData::ReadData(QString nam, QList<Timestamp> &ts)
{
	name = nam;
	timestamps = ts;
}

QList<Timestamp> ReadData::findTimestamp(uint h, uint m, uint s, uint ms)
{
	QList<Timestamp> ts;
	foreach(Timestamp t, timestamps)
	{
		if(t.hours == h && t.min == m && t.sec == s && t.msec == ms)
			ts << t;
	}
	return ts;
}






