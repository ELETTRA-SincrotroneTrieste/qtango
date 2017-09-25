#include "source_metadata.h"
#include <QCoreApplication>
#include <QStringList>
#include <QWidget>
#include <QtDebug>

SourceMetadata::SourceMetadata(QWidget *parent, const QString& info, const QString& help)
{
  Q_ASSERT(parent != NULL);
  d_parentWidget = parent;
  qDebug() << parent << "name " << parent->objectName();
  d_objectName = parent->objectName();
  QString appName = QCoreApplication::instance()->applicationName();
  if(appName.isEmpty())
  {
	appName = QCoreApplication::instance()->arguments().at(0);
	d_owner = appName.remove("./");
  }
  else 
	d_owner = QCoreApplication::instance()->applicationName();
  d_info = info;
  d_help = help;
}

QString SourceMetadata::toHtml()
{
  d_objectName = d_parentWidget->objectName();
  QString html = QString("<ul><li>Application: \"%1\"</li><li>name \"%2\"</li><li>class: \"%3\"</li>").
	arg(d_owner).arg(d_objectName).arg(d_parentWidget->metaObject()->className());
  html += "</ul>";
  if(!d_info.isEmpty())
	html += QString("<p><em>Info:</em><p>%1</p></p>").arg(d_info);
  if(!d_help.isEmpty())
	html += QString("<p><em>Info:</em><p>%1</p></p>").arg(d_help);
  return html;
}

