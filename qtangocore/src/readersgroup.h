#ifndef _READERS_GROUP
#define _READERS_GROUP

#include <QVector>
#include <QPointer>
#include <QObject>
#include <QMap>
#include <com_proxy_reader.h>

class ReadersGroupPrivate;

/**
 * \brief a set to group readers
 *
 * this `big` reader groups together different readers and allows to split 
 * data from a Tango spectrum into various scalar data and develop them 
 * to single readers.
 * Normally it is used by QTango widgets as follows:
 *
 * \par Example
 * \verbatim
ReadersGroup tvg;
for (int i = 0; i < 16; i++)
{
	TLabel *tl = new TLabel(this);
        tvg.addReader(tl, i);
}
tvg.setSource("some/test/device/spectrum_attribute");
\endverbatim
 * 
 */
class ReadersGroup : public QObject, public QTangoComProxyReader
{
Q_OBJECT
public:
	ReadersGroup(QObject *parent);
	
	void setSource(QString, RefreshMode = AUTO_REFRESH, int = 1000);

	int addReader(QObject*, int index);
	
	QList<QObject *> readers() { return d_viewersMap.keys(); }
	
signals:
	void newDataAvailable();

protected:
	QMap <QObject *, int> d_viewersMap;

protected slots:
	void refresh(const TVariant&);
	
  private slots:
	void viewerDestroyed(QObject *);
	void updateInfo();
	void configurationAvailable(const TangoConfigurationParameters *);

private:
    ReadersGroupPrivate *d_ptr;
};

#endif	
