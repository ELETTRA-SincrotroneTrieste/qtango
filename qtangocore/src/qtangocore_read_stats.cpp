#include "qtangocore_read_stats.h"
#include <macros.h>
#include <QApplication>
#include <QStringList>
#include <QFile>
#include <QDate>
#include <QTextStream>
#include <QtDebug>

StatsData::StatsData(QString name, QString objinfo, QTime t, bool event, bool error, QString message)
{
  d_name = name;
  d_timestamp = t;
  d_event = event;
  d_error = error;
  d_message = message;
  d_empty = false;
  d_objinfo = objinfo;
}


QTangoCoreReadStats* QTangoCoreReadStats::_instance = NULL;

QTangoCoreReadStats* QTangoCoreReadStats::instance()
{
  if(!_instance)
    _instance = new QTangoCoreReadStats();
  return _instance;
}

QTangoCoreReadStats::QTangoCoreReadStats()
{
  d_enabled = d_pollEnabled = d_eventEnabled = false;
  d_startTime = QTime::currentTime();
  
  QStringList args = qApp->arguments();
  if(args.size() > 0)
  {
	d_applicationName = qApp->arguments().at(0);
	foreach(QString arg, args)
	{
		if(arg == "--dump-stats")
		{
			d_enabled = true;
			d_pollEnabled = true;
			d_eventEnabled = true;
			printf("\e[1;32m*\e[0m enabling statistics for \"%s\" application\n", qstoc(d_applicationName));
		}
		//     else if(arg == "--noevents")
		//     {
		//       d_eventEnabled = false;
		//       printf("\e[1;32m*\e[0m excluding events from statistics\n", qstoc(qApp->arguments().at(0)));
		//     }
		//     else if(arg == "--nopolled")
		//     {
		// 	    d_pollEnabled = false;
		// 	    printf("\e[1;32m*\e[0m excluding non-event elements from statistics\n", qstoc(qApp->arguments().at(0)));
		//     }
	}
  }
}

void QTangoCoreReadStats::addAttributeStat(const QString &attName, const QString & objinformation,bool event, 
      bool error, const QString& msg)
{
  d_mutex.lock();
  if(d_enabled)
  {
    QTime ts = QTime::currentTime();
    QString escapedMsg = msg;
//     escapedMsg.replace(" ", "\\s");
    StatsData sd(attName, objinformation, ts, event, error, escapedMsg);
    d_statsMap[attName].push_back(sd);
  }
  d_mutex.unlock();
}

QList <StatsData> QTangoCoreReadStats::statsForName(const QString &name)
{
  d_mutex.lock();
  QList <StatsData> sd;
  if(d_enabled)
  {
     sd = d_statsMap.value(name);
  }
  d_mutex.unlock();
  return sd;
}

bool QTangoCoreReadStats::dumpStats()
{
     if(!d_enabled)
      return false;
	 
     d_mutex.lock();
     
	 int dataCnt;
     QString line;
     QString err = "-", ev = "-";
     QString fileName = QString("%1-stats-%2-%3.csv").arg(d_applicationName).arg( d_startTime.toString("hh.mm.ss")).
      arg(QTime::currentTime().toString("hh.mm.ss"));
     /* if file name contains `/', take the last part */
     if(fileName.contains("/") && fileName.split("/", QString::SkipEmptyParts).size() > 1 )
	     fileName = fileName.split("/", QString::SkipEmptyParts).last();
     QFile file(fileName);
     if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
     {
	     QString error = QString("QTangoCoreReadStats: failed to save statistics on file"
		"\"%1\":\n%2").arg(fileName).arg(file.errorString());
	     printf("\e[1;31m*\e[0m %s\n", qstoc(error));
		 
		 d_mutex.unlock();
         
		 return false;
     }

     printf("\e[1;32m* \e[0msaving statistics on file \"%s\"", qstoc(fileName));
     QTextStream out(&file);
     
     QString header = QString("%1 stats %2-%3 (%4)\n").arg(d_applicationName).arg(d_startTime.toString("hh:mm:ss")).
      arg(QTime::currentTime().toString("hh.mm.ss")).arg(QDate::currentDate().toString("yy-MM-dd"));
      
     out << header;
     
     QList<QString> names = d_statsMap.keys();
     foreach(QString n, names)
     {
      dataCnt = 0;
      QList <StatsData> data = d_statsMap.value(n);
      line = QString("%1\t").arg(n);
      foreach(StatsData d, data)
      {
	if(!d.isEmpty())
	{
	  dataCnt++;
	  if(d.error())
	    err = "err";
	  if(d.event())
	    ev = "event";
	  line += QString("%1,%2,%3,\"%4\",%5\t").arg(d.timestamp().toString("hh::mm::ss::zzz")).arg(err).arg(ev).arg(d.message()).
										arg(d.objInfo());
	}
      }
      line += "\n";
      if(dataCnt > 0)
		out << line;
     }

     file.close();
     d_savedFilename = fileName;
     d_mutex.unlock();
     return true;
}









