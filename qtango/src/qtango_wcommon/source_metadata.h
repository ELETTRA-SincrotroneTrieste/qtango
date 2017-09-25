#ifndef QTANGOSOURCE_METADATA_H
#define QTANGOSOURCE_METADATA_H

#include <QString>

class QWidget;

class SourceMetadata
{
  public:
	SourceMetadata(QWidget *parent, const QString& info = QString(), const QString& help = QString());
	
	QString owner() { return d_owner; }
	QString info() { return d_info; }
	QString help() { return d_help; }
	
	const QString objectName() const { return d_objectName; }
	
	void setInfo(const QString& info) { d_info = info; }
	void setHelp(const QString& help) { d_help = help; }
	void changeOwner(const QString& owner) { d_owner = owner; }
	void changeObjectName(const QString& name) { d_objectName = name; }
	
	QString toHtml(); 
	
  private:
	QString d_owner, d_info, d_help, d_objectName;
	QWidget *d_parentWidget;
};

#endif
