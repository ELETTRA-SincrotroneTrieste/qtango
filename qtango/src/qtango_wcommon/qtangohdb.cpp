#include "qtangohdb.h"
#include <macros.h>
#include <QDesktopServices>
#include <QUrl>
#include <QRegExp>

QTangoHdb::QTangoHdb()
{
  d_hdbActionEnabled = true;
  /* take the hdb host from the environment */
  char *env = getenv("HDB_PHPSCRIPT_PATH");
  if(env)
	d_environment = QString(env);
}

bool QTangoHdb::openLink(const QString &sourceName)
{
  QString src = sourceName;
  /* remove argin expression from source (targets) */
  src.remove(QRegExp("\\(.*\\)"));
  if(src.endsWith(";"))
	src.remove(src.length() - 1);
  
  pinfo("QTangoHdb::openLink: historical db data request for \"%s\"\n", qstoc(src));
//   src.replace("/", "%2F"); /* not needed openUrl does it */
  /* the string comes from ProxyInterface::tangoPoint() and should be clean */
  QString link = QString(d_environment + "?attr=%1&amp;start_date=last%2024%20hours").arg(src);
  pinfo("QTangoHdb::openLink: opening \"%s\"", qstoc(link));
  if(d_environment.isEmpty())
  {
	perr("QTangoHdb: HDB_PHPSCRIPT_PATH is not set: set it properly so that it");
	perr("points to the correct location of the php script for hdb. For example:");
	perr("http://192.168.205.101/docs/fermi/hdb.php");
	return false;
  }
  return QDesktopServices::openUrl(QUrl(link));
}

