#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <QString>
#include "read_data.h"


class DataLoader
{
	public:
		DataLoader(QString filename);
		
		QList<ReadData> data() { return d_data; }
		
		void printResults();
		
		QString header() { return d_header; }
		
	private:
		void processLine(const QString &);
		
		QList<ReadData> d_data;
		
		QString d_header;
};






#endif
