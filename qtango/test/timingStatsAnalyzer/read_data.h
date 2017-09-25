#ifndef READ_DATA
#define READ_DATA

#include <QString>
#include <QList>

class Timestamp
{
	public:
		Timestamp(QString name, unsigned h, unsigned m, unsigned s, unsigned ms, 
			  bool err, bool ev, QString msg, QString objinfo, unsigned idx);
		Timestamp();
		QString message;
		QString objinfo;
		QString name;
		unsigned hours, min, sec, msec, index;
		bool error, event;
};

class ReadData 
{
	public:
		ReadData(QString nam);
		ReadData(QString nam, QList<Timestamp> &ts);
		
		QList<Timestamp> findTimestamp(uint h, uint m, uint s, uint ms);
		
		QList<Timestamp> timestamps;
		QString name;

};


#endif
