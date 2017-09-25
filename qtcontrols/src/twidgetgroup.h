#ifndef  _TWIDGET_GROUP
#define  _TWIDGET_GROUP

#include <com_proxy_reader.h>
#include <QVector>
#include <QPointer>
#include <QObject>

/**
 * \brief a viewer to group widgets
 *
 * this viewer groups together different viewers and allows to split 
 * data from a Tango spectrum into various scalar data and develop them 
 * to single widgets
 *
 * \par Example
 * \verbatim
TWidgetGroup tvg;
for (int i = 0; i < 16; i++)
{
	TLabel *tl = new TLabel(this);
	tvg.addWidget(tl);
}
tvg.setSource("some/test/device/spectrum_attribute");
\endverbatim
 * 
 */
class TWidgetGroup : public QObject, public QTangoComProxyReader
{
	Q_OBJECT
public:
	TWidgetGroup();
	~TWidgetGroup();
	
	int addWidget(QObject*);

	void setTangoValue(DeviceAttribute&);
	void setTangoValue(DeviceData&);
	
	
signals:
	void newDataAvailable();

protected:
	QVector<QPointer<QObject> > viewers;

protected slots:
	void configure();	
};

#endif
