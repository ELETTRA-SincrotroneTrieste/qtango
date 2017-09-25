#include "data_loader.h"

#include <QFile>
#include <QList>
#include "macros.h"
#include <QStringList>
#include <QMessageBox>
#include <QApplication>
#include <QTextStream>

DataLoader::DataLoader(QString fileName)
{
	QFile file(fileName);
	QString line;
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
	{
		QMessageBox::warning(0, "QTangoCore statistics data loader",
				QString("Cannot read file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
	}
	else
	{

		QTextStream in(&file);
		QApplication::setOverrideCursor(Qt::WaitCursor);

		/* first line is for the header */
		d_header = in.readLine();
		
		while (!in.atEnd()) {
			line = in.readLine();
			processLine(line);
		}
		QApplication::restoreOverrideCursor();
	}
}

void DataLoader::processLine(const QString &line)
{
	QList<Timestamp> timestamps;
	QStringList parts;
	QString element, message, objinfo;
	QString dev;
	bool err = false, event = false;
	if(line.contains("\t") && !line.startsWith("#") && line.length() > 4)
	{
		parts = line.split("\t", QString::SkipEmptyParts);
		if(parts.size() > 1)
		{
			dev = parts.at(0);
			for(int i = 1; i < parts.size(); i++)
			{
				element = parts.at(i);
				if(element.count("::") == 3 && element.count(",") == 4)
				{
					QStringList elParts = element.split(",");
					if(elParts.size() == 5)
					{
						QString time = elParts.at(0);
						if(elParts.at(1) == "err")
							err = true;
						if(elParts.at(2) == "event")
							event = true;
						message = elParts.at(3);
						objinfo = elParts.at(4);
						QStringList timeParts = time.split("::");
						if(timeParts.size() == 4)
						{
							Timestamp t(dev, timeParts.at(0).toUInt(),
								timeParts.at(1).toUInt(),
								timeParts.at(2).toUInt(),
								timeParts.at(3).toUInt(),
								err, event, message, objinfo, i);
							
							timestamps.push_back(t);
						}
						else
							printf("line \%s\" timestamp \" \e[1;31m%s\e[0m\" not valid", qstoc(line), qstoc(time));
					}
				}
				else
					printf("line \"%s\": bad format", qstoc(line));
			}
		}
	}
	if(dev != QString() && timestamps.size() > 0)
	{
		ReadData rd(dev);
		rd.timestamps = timestamps;
		d_data.push_back(rd);
	}
		
}

void DataLoader::printResults()
{
	foreach(ReadData rd, d_data)
	{
		printf("\"%s\":\n{\n", qstoc(rd.name));
		foreach(Timestamp t, rd.timestamps)
			printf("- \e[1;32m%d:%d:%d:%d\e[0m - err: %d event %d\n",
			      t.hours, t.min, t.sec, t.msec, t.error, t.event);
		printf("}\n");
	}
}













