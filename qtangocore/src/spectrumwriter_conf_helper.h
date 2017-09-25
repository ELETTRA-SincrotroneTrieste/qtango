#ifndef SPECTRUMWRITER_CONF_HELPER
#define SPECTRUMWRITER_CONF_HELPER

#include <QString>
#include <QObject>

class SimpleDataProxy;
class SpectrumWriter;
class TangoConfigurationParameters;

/** \brief used privately by SpectrumWriter
 *
 */
class ConfigHelper : public QObject
{
	friend class SpectrumWriter;
	
	Q_OBJECT
	private:
		ConfigHelper();
		
		 /* called by SpectrumWriter setTargets before it calls QTangoComProxyWriter::setTargets() */
		 void registerElements(const QList <SimpleDataProxy *>& list) { d_proxyList = list; }
		 
		 void clearElements() { d_proxyList.clear(); }
		 
		QList <SimpleDataProxy *> d_proxyList;
		
		void setAttributeName(const QString &n) { d_attName = n; }
	
  public slots:
	void updateInfo(const TangoConfigurationParameters *cp);
	
	private slots:
	
   		 void configure(const TangoConfigurationParameters *cp);
		
	private:
		QString d_attName;
};


#endif 