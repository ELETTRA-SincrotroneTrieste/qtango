#include "estyleloader.h"
#include <macros.h>
#include <QSettings>
#include <QFile>
#include <QTextStream>

EStyleLoader::EStyleLoader(const QString& filename)
{
  QSettings s;
  QString fileName = filename;
  char *styleFromEnv = getenv("QTANGO_STYLESHEET");
  if(styleFromEnv && fileName.isEmpty())
	fileName = QString(styleFromEnv);
  if(fileName.isEmpty())
	fileName = s.value("QTANGO_STYLESHEET", QString(SHAREDIR) + QString("/qtango/stylesheets/qtango.css")).toString();
  
  QFile file(fileName);
  if(!fileName.isEmpty() && file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
	 QTextStream in(&file);
     while (!in.atEnd()) 
	 {
         d_styleSheet += in.readLine();
     }
  }
  else
	pwarn("EStyleLoader: failed to open file \"%s\" in read only mode: %s", qstoc(fileName), qstoc(file.errorString()));
}


