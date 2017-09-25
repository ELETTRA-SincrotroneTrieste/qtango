#ifndef QTANGO_HDB_H
#define QTANGO_HDB_H

#include <QString>
class QTangoHdbPrivate;

class QTangoHdb
{
  public:
	QTangoHdb();
	
	void setHdbActionEnabled(bool en) { d_hdbActionEnabled = en; }
	
	bool hdbActionEnabled() { return d_hdbActionEnabled; }
	
	bool openLink(const QString &source);

  private:
	QString d_environment;
	bool d_hdbActionEnabled;

    QTangoHdbPrivate *d_ptr;
	
};

#endif
