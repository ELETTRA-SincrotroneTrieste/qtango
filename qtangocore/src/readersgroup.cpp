#include "readersgroup.h"
#include <QObject>
#include <QWidget>
#include <QMetaObject>
#include <QMetaProperty>

ReadersGroup::ReadersGroup(QObject *parent) : QObject(parent), QTangoComProxyReader(this)
{
	
}

void ReadersGroup::setSource(QString s, RefreshMode m, int p)
{
	connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), 
		this, SLOT(configurationAvailable(const TangoConfigurationParameters *)), Qt::DirectConnection);
	connect(qtangoComHandle(), SIGNAL(newData(const TVariant&)), this, SLOT(refresh(const TVariant&)),
		 Qt::DirectConnection);
	connect(qtangoComHandle(), SIGNAL(connectionErrorMessage(const QString&)), this,
		SLOT(updateInfo()));
	setPeriod(p);
	setRefreshMode(m);
	QTangoComProxyReader::setSource(s);
	updateInfo();
}
		
int ReadersGroup::addReader(QObject *w, int index)
{
	try {
		QTangoComProxyReader *v = dynamic_cast<QTangoComProxyReader*>(w);
		if (v)
		{
		  /* be notified when an object of ours is going to be deleted! */
		  connect(w, SIGNAL(destroyed(QObject *)), this, SLOT(viewerDestroyed(QObject *)));
		  d_viewersMap.insert(w, index);
		  updateInfo();
		  return 0;
		}
		else
			return -1;
	} catch (...) {
		return -1;
	}
}

void ReadersGroup::viewerDestroyed(QObject *obj)
{
  if(d_viewersMap.contains(obj))
  {
	pinfo("ReadersGroup: viewer object \"%s\" destroyed while in readers group: removing from list",
	  qstoc(obj->objectName()));
	d_viewersMap.remove(obj);
  }
}

void ReadersGroup::updateInfo()
{
  QList<QObject *> rs = readers() ;
  foreach(QObject *o, rs)
  {
	int propIdx = o->metaObject()->indexOfProperty("additionalInfo");
	if(propIdx > -1)
	{
	  QString additionalInfo = o->metaObject()->property(propIdx).read(o).toString();
	  /* remove our previous information, if present */
	  additionalInfo.remove(QRegExp("<div class=\"readersGroupAdditionalInfo\">.*</div>"));
	  QString groupAdditionalInfo = QString("<div class=\"readersGroupAdditionalInfo\">"
		"<h4>Element %1 of Readers group \"%2\"</h4><p>"
		"<ul><li><em>source</em>: <cite>%3</cite>;</li></ul></p>"
		"<p>%4</p></div>").arg(d_viewersMap.value(o)).arg(objectName()).arg(realSource()).arg(htmlHandleRepresentation());
	  o->metaObject()->property(propIdx).write(o, additionalInfo + groupAdditionalInfo);
	}
  }
}

void ReadersGroup::configurationAvailable(const TangoConfigurationParameters *)
{
  updateInfo();
}

void ReadersGroup::refresh(const TVariant &v)
{
	QVector<TVariant > variants;
	int index, i;
	
	if(v.quality() == ATTR_INVALID)
	{
		QString message = "read_attribute has failed";
		foreach(QObject *w, d_viewersMap.keys())
		{
			if (w)
			{
				QTangoComProxyReader *reader = dynamic_cast<QTangoComProxyReader*>(w);
				if (reader)
				{
					TVariant scalarVar;
					scalarVar.setQuality(v.quality());
                                        scalarVar.setMessage(v.message());
					reader->refresh(scalarVar);
				}
			}
		}
	}
	else if(v.getAttributeInfo().data_format != SPECTRUM)
	{
		perr("ReadersGroup accepts only spectrum attributes as source.");
		perr("Cannot refresh widgets in the group!");
	}
	else
	{
		AttributeInfo attInfo = v.getAttributeInfo();
		/* the attribute of each widget in the group has all the properties of the ReadersGroup source,
		 * except the format, which is scalar.
		 */
		attInfo.data_format = SCALAR;
		
		switch(v.type())
		{
			case DEV_SHORT:
			case DEVVAR_SHORTARRAY:	
				if(v.canConvertToIntVector())
				{
					QVector<int> temp;
					temp = v.toIntVector();
					foreach(int element, temp)
					{
						DeviceAttribute attr;
						short sh = (short) element;
						attr << sh;
						attr.quality = v.quality();
						TVariant elementVariant(attr, attInfo, source());
						elementVariant.setQuality(attr.quality);
						elementVariant.setMessage(v.message());
						variants.push_back(elementVariant);
					}
					
				}
				else
				{
					QString msg = "data conversion error: spectrum of short but data not convertable";
					DeviceAttribute attr;
					short sh = 0;
					attr  << sh;
					attr.quality = ATTR_INVALID;
					TVariant elementVariant(attr, attInfo, source());
					elementVariant.setQuality(attr.quality);
					elementVariant.setMessage(msg);
					variants.push_back(elementVariant);
				}
				break;
			case DEV_USHORT:
                	case DEVVAR_USHORTARRAY:
				if(v.canConvertToUIntVector())
				{
					QVector<unsigned int> temp;
					temp = v.toUIntVector();
					foreach(unsigned int element, temp)
					{
						DeviceAttribute attr;
						unsigned short ush = (unsigned short) element;
						attr << ush;
						attr.quality = v.quality();
						TVariant elementVariant(attr, attInfo, source());
						elementVariant.setQuality(attr.quality);
						elementVariant.setMessage(v.message());
						variants.push_back(elementVariant);
					}
					
				}
				else
				{
					QString msg = "data conversion error: spectrum of unsigned short but data not convertable";
					DeviceAttribute attr;
					unsigned short ush = 0;
					attr  << ush;
					attr.quality = ATTR_INVALID;
					TVariant elementVariant(attr, attInfo, source());
					elementVariant.setQuality(attr.quality);
					elementVariant.setMessage(msg);
					variants.push_back(elementVariant);
				}
				break;
				
			case DEV_DOUBLE:
			case DEVVAR_DOUBLEARRAY:
				if(v.canConvertToDoubleVector())
				{
					QVector<double> temp;
                    temp = v.toDoubleVector();
					foreach(double element, temp)
                    {
						DeviceAttribute attr;
                        DevDouble dd = (DevDouble) element;
						attr << dd;
						attr.quality = v.quality();
						TVariant elementVariant(attr, attInfo, source());
						elementVariant.setQuality(attr.quality);
						elementVariant.setMessage(v.message());
						variants.push_back(elementVariant);
					}
					
				}
				else
				{
					QString msg = "data conversion error: spectrum of double but data not convertable";
					DeviceAttribute attr;
					DevDouble dd = 0;
					attr  << dd;
					attr.quality = ATTR_INVALID;
					TVariant elementVariant(attr, attInfo, source());
					elementVariant.setQuality(attr.quality);
					elementVariant.setMessage(msg);
					variants.push_back(elementVariant);
				}
				break;
			case DEV_FLOAT:
			case DEVVAR_FLOATARRAY:
				if(v.canConvertToDoubleVector())
				{
					QVector<double> temp;
					temp = v.toDoubleVector();
					foreach(double element, temp)
					{
						DeviceAttribute attr;
						float df = (float) element;
						attr << df;
						attr.quality = v.quality();
						TVariant elementVariant(attr, attInfo, source());
						elementVariant.setQuality(attr.quality);
						elementVariant.setMessage(v.message());
						variants.push_back(elementVariant);
					}
					
				}
				else
				{
					QString msg = "data conversion error: spectrum of float but data not convertable (to double)";
					DeviceAttribute attr;
					float df = 0;
					attr  << df;
					attr.quality = ATTR_INVALID;
					TVariant elementVariant(attr, attInfo, source());
					elementVariant.setQuality(attr.quality);
					elementVariant.setMessage(msg);
					variants.push_back(elementVariant);
				}
				break;	
				
			case DEV_STRING:
			case DEVVAR_STRINGARRAY:
				if(v.canConvertToStringVector())
				{
					QVector<QString> temp;
					temp = v.toStringVector();
					foreach(QString element, temp)
					{
						DeviceAttribute attr;
						std::string s = element.toStdString();
						attr << s;
						attr.quality = v.quality();
						TVariant elementVariant(attr, attInfo, source());
						elementVariant.setQuality(attr.quality);
						elementVariant.setMessage(v.message());
						variants.push_back(elementVariant);
					}
					
				}
				else
				{
					QString msg = "data conversion error: spectrum of float but data not convertable (to double)";
					DeviceAttribute attr;
					string df = "####";
					attr  << df;
					attr.quality = ATTR_INVALID;
					TVariant elementVariant(attr, attInfo, source());
					elementVariant.setQuality(attr.quality);
					elementVariant.setMessage(msg);
					variants.push_back(elementVariant);
				}
				break;
			case DEV_BOOLEAN:
			case DEVVAR_BOOLEANARRAY:
				if(v.canConvertToBoolVector())
				{
					QVector<bool> temp;
					temp = v.toBoolVector();
					foreach(bool element, temp)
					{
						DeviceAttribute attr;
						attr << element;
						attr.quality = v.quality();
						TVariant elementVariant(attr, attInfo, source());
						elementVariant.setQuality(attr.quality);
						elementVariant.setMessage(v.message());
						variants.push_back(elementVariant);
					}
					
				}
				else
				{
					QString msg = "data conversion error: spectrum of float but data not convertable (to double)";
					DeviceAttribute attr;
					bool b = false;
					attr  << b;
					attr.quality = ATTR_INVALID;
					TVariant elementVariant(attr, attInfo, source());
					elementVariant.setQuality(attr.quality);
					elementVariant.setMessage(msg);
					variants.push_back(elementVariant);
				}
				break;
			case DEV_LONG:
			case DEVVAR_LONGARRAY:
                                if(v.canConvertToIntVector())
				{
					QVector<int> temp;
					temp = v.toIntVector();
					foreach(int element, temp)
					{
						DeviceAttribute attr;
						DevLong dl = (DevLong) element;
						attr << dl;
						attr.quality = v.quality();
						TVariant elementVariant(attr, attInfo, source());
						elementVariant.setQuality(attr.quality);
						elementVariant.setMessage(v.message());
						variants.push_back(elementVariant);
					}
					
				}
				else
				{
					QString msg = "data conversion error: spectrum of long but data not convertable (to int)";
					DeviceAttribute attr;
					DevLong dl = 0;
					attr  << dl;
					attr.quality = ATTR_INVALID;
					TVariant elementVariant(attr, attInfo, source());
					elementVariant.setQuality(attr.quality);
					elementVariant.setMessage(msg);
					variants.push_back(elementVariant);
				}
				break;
			case DEV_ULONG:
			case DEVVAR_ULONGARRAY:
                                if(v.canConvertToUIntVector())
				{
					QVector<unsigned int> temp;
					temp = v.toUIntVector();
					foreach(unsigned int element, temp)
					{
						DeviceAttribute attr;
						DevULong dul = (DevULong) element;
						attr << dul;
						attr.quality = v.quality();
						TVariant elementVariant(attr, attInfo, source());
						elementVariant.setQuality(attr.quality);
						elementVariant.setMessage(v.message());
						variants.push_back(elementVariant);
					}
					
				}
				else
				{
					QString msg = "data conversion error: spectrum of unsigned long but data not convertable (to unsigned int)";
					DeviceAttribute attr;
					DevULong dul = 0;
					attr  << dul;
					attr.quality = ATTR_INVALID;
					TVariant elementVariant(attr, attInfo, source());
					elementVariant.setQuality(attr.quality);
					elementVariant.setMessage(msg);
					variants.push_back(elementVariant);
				}
				break;
                        case DEV_STATE:
                            if (v.canConvertToState()) {
                                QVector<DevState> temp;
                                temp = v.toStateVector();
                                foreach(DevState element, temp) {
                                    DeviceAttribute attr;
                                    attr << element;
                                    attr.quality = v.quality();
                                    TVariant elementVariant(attr, attInfo, source());
                                    elementVariant.setQuality(attr.quality);
                                    elementVariant.setMessage(v.message());
                                    variants.push_back(elementVariant);
                                }
                            } else {
                                QString msg = "data conversion error: spectrum of DevState but data not convertable (to State)";
                                DeviceAttribute attr;
                                DevState ds = Tango::UNKNOWN;
                                attr  << ds;
                                attr.quality = ATTR_INVALID;
                                TVariant elementVariant(attr, attInfo, source());
                                elementVariant.setQuality(attr.quality);
                                elementVariant.setMessage(msg);
                                variants.push_back(elementVariant);
                            }
                            break;
                        default:
				QString mess = "wrong data type";
				DeviceAttribute attr;
				attr.quality = ATTR_INVALID;
				TVariant elementVariant(attr, attInfo, source());
				elementVariant.setQuality(attr.quality);
				elementVariant.setMessage(mess);
				variants.push_back(elementVariant);
				break;
				
		}
		
		i = 0;
		foreach(QObject *o,  d_viewersMap.keys())
		{
			QTangoComProxyReader *reader = dynamic_cast<QTangoComProxyReader *>(o);
			if(reader)
            {
				index = d_viewersMap.value(o);
                if(index != -1 && variants.size()  > index)
                    reader->refresh(variants.at(index));
                else if( i < variants.size())
                    reader->refresh(variants.at(i));
				else
                                        perr("ReadersGroup refresh(): index %d exceeds vector dimensions: %d (\"%s\")", index,
                                             variants.size(), qstoc(realSource()));
				i++;	
			}
			else
				perr("cannot convert group member \"%s\" into a tango com reader",
					       qstoc(o->objectName()));
			
		}
	}
	updateInfo();
}

