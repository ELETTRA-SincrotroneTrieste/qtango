#include "twidgetgroup.h"

#include <QObject>

//#include <QtDebug>
//#include <QTime>

TWidgetGroup::TWidgetGroup() : QObject(), QTangoComProxyReader(this)
{
	connect(qtangoComHandle(), 
		SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), 
		this, SLOT(init(const TangoConfigurationParameters *)));
	connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)),
	  Qt::DirectConnection);
}

TWidgetGroup::~TWidgetGroup()
{
	disconnect(readingThread, SIGNAL(configured()));
	unsetSource();
}

//void TWidgetGroup::setSource(QString, DeviceData, RefreshMode, int){}
		
int TWidgetGroup::addWidget(QObject *w)
{
	try {
		QTangoComProxyReader *v = dynamic_cast<QTangoComProxyReader*>(w);
		if (v)
		{
			viewers << w;
// 			v->setType(getType());
// 			AttributeInfoEx info = getAttributeInfo();
// 			info.data_format = SCALAR;
// 			v->setAttributeInfo(info);
			connect(this, SIGNAL(newDataAvailable()), w, SLOT(refresh()));
			return 0;
		}
		else
			return -1;
	} catch (...) {
		return -1;
	}
}

void TWidgetGroup::configure(const TangoConfigurationParameters * cp)
{
	startRefresh();
	foreach(QObject *w, viewers)
	{
		try {
			TDataViewer *v = dynamic_cast<TDataViewer*>(w);
			if (v)
			{
				v->setType(getType());
				AttributeInfoEx info = getAttributeInfo();
				info.data_format = SCALAR;
				v->setAttributeInfo(info);
			}
		} catch (...) {}
	}

}

void TWidgetGroup::setTangoValue(DeviceAttribute &devattr)
{
//	QTime t;
//	t.start();
	if (devattr.has_failed())
	{
		message = "read_attribute has failed";
		quality = ATTR_INVALID;
		foreach(QObject *w, viewers)
		{
			if (w)
			{
				try {
					TDataViewer *v = dynamic_cast<TDataViewer*>(w);
					if (v)
					{
						v->setMessage(message);
						v->setQuality(quality);
					}
				} catch (...) {}
			}
		}
		emit newDataAvailable();
		return;
	}
	
	quality = devattr.quality;
	
	switch(arg_type)
	{
		case DEV_SHORT:
		case DEVVAR_SHORTARRAY:	
		{
			vector<short> temp;
			devattr >> temp;
			QVector<short> data = QVector<short>::fromStdVector(temp);

			QVectorIterator<QPointer<QObject> > v(viewers);
			QVectorIterator<short> datum(data);
			while (v.hasNext() && datum.hasNext())
			{
				DeviceAttribute attr;
				attr << datum.next();
				attr.quality = devattr.quality;
				QPointer<QObject> ptr = v.next();
				if (ptr);
				{
					try {
						TDataViewer *v = dynamic_cast<TDataViewer*>((QObject*)ptr);
						if (v)
						{
							v->setMessage(message);
							v->setTangoValue(attr);
						}
					} catch (...) {}
				}
			}
			emit newDataAvailable();
			break;
		}
		case DEV_FLOAT:
                case DEVVAR_FLOATARRAY:	
		{
			vector<float> temp;
			devattr >> temp;
			QVector<float> data = QVector<float>::fromStdVector(temp);

			QVectorIterator<QPointer<QObject> > v(viewers);
			QVectorIterator<float> datum(data);
			while (v.hasNext() && datum.hasNext())
			{
				DeviceAttribute attr;
				attr << datum.next();
				attr.quality = devattr.quality;
				QPointer<QObject> ptr = v.next();
				if (ptr);
				{
					try {
						TDataViewer *v = dynamic_cast<TDataViewer*>((QObject*)ptr);
						if (v)
						{
							v->setMessage(message);
							v->setTangoValue(attr);
						}
					} catch (...) {}
				}
			}
			emit newDataAvailable();
			break;
		}

		case DEV_DOUBLE:
		case DEVVAR_DOUBLEARRAY:	
		{
			vector<double> temp;
			devattr >> temp;
			QVector<double> data = QVector<double>::fromStdVector(temp);

			QVectorIterator<QPointer<QObject> > v(viewers);
			QVectorIterator<double> datum(data);
			while (v.hasNext() && datum.hasNext())
			{
				DeviceAttribute attr;
				attr << datum.next();
				attr.quality = devattr.quality;
				QPointer<QObject> ptr = v.next();
				if (ptr);
				{
					TDataViewer *v = dynamic_cast<TDataViewer*>((QObject*)ptr);
					v->setMessage(message);
					v->setTangoValue(attr);
				}
			}
			emit newDataAvailable();
			break;
		}

		case DEV_USHORT:
                case DEVVAR_USHORTARRAY:	
		{
			vector<unsigned short> temp;
			devattr >> temp;
			QVector<unsigned short> data = QVector<unsigned short>::fromStdVector(temp);

			QVectorIterator<QPointer<QObject> > v(viewers);
			QVectorIterator<unsigned short> datum(data);
			while (v.hasNext() && datum.hasNext())
			{
				DeviceAttribute attr;
				attr << datum.next();
				attr.quality = devattr.quality;
				QPointer<QObject> ptr = v.next();
				if (ptr);
				{
					TDataViewer *v = dynamic_cast<TDataViewer*>((QObject*)ptr);
					v->setMessage(message);
					v->setTangoValue(attr);
				}
			}
			emit newDataAvailable();
			break;
		}

/*		case DEV_STRING:
		case DEVVAR_STRINGARRAY:	
		{
			vector<string> temp;
			devattr >> temp;
			QVector<string> data = QVector<string>::fromStdVector(temp);

			QVectorIterator<QPointer<QObject> > v(viewers);
			QVectorIterator<string> datum(data);
			while (v.hasNext() && datum.hasNext())
			{
				qDebug("%d", datum.peekNext());
				DeviceAttribute attr;
				attr << datum.next();
				attr.quality = devattr.quality;
				QPointer<QObject> ptr = v.next();
				if (ptr);
				{
					QObject *ptr = dynamic_cast<QObject*>((QObject*)ptr);
					//v->setMessage(message);
					v->setTangoValue(attr);
				//	v->update();
				}
			}
			qDebug("");
			break;
		}*/
		
		case DEV_BOOLEAN:
		case DEVVAR_BOOLEANARRAY:	
		{
			vector<bool> temp;
			devattr >> temp;
			QVector<bool> data = QVector<bool>::fromStdVector(temp);

			QVectorIterator<QPointer<QObject> > v(viewers);
			QVectorIterator<bool> datum(data);
			int i = 0;
			while (v.hasNext() && datum.hasNext())
			{
				DeviceAttribute attr;
				attr << datum.next();
				attr.quality = devattr.quality;
				QPointer<QObject> ptr = v.next();
				if (ptr);
				{
					TDataViewer *v = dynamic_cast<TDataViewer*>((QObject*)ptr);
					v->setMessage(message);
					v->setTangoValue(attr);
				}
			}
			emit newDataAvailable();
			break;
		}
			
		case DEV_LONG:
		case DEVVAR_ULONGARRAY:
		case DEV_ULONG:
		case DEVVAR_LONGARRAY:
		{
			vector<long> temp;
			devattr >> temp;
			QVector<long> data = QVector<long>::fromStdVector(temp);

			QVectorIterator<QPointer<QObject> > v(viewers);
			QVectorIterator<long> datum(data);
			while (v.hasNext() && datum.hasNext())
			{
				DeviceAttribute attr;
				attr << datum.next();
				attr.quality = devattr.quality;
				QPointer<QObject> ptr = v.next();
				if (ptr);
				{
					TDataViewer *v = dynamic_cast<TDataViewer*>((QObject*)ptr);
					v->setMessage(message);
					v->setTangoValue(attr);
				}
			}
			emit newDataAvailable();
			break;
		}
		default:
			message = "wrong data type";
			quality = ATTR_INVALID;
			foreach(QObject *w, viewers)
			{
				if (w)
				{
					TDataViewer *v = dynamic_cast<TDataViewer*>(w);
					v->setMessage(message);
					v->setQuality(quality);
				}
			}
			emit newDataAvailable();
			break;
	}
	//qDebug() << t.elapsed();
}		
	
void TWidgetGroup::setTangoValue(DeviceData&){};


