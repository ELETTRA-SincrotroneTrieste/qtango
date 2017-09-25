#ifndef PLOT_COM_PROXY_READER_H
#define PLOT_COM_PROXY_READER_H

#include <com_proxy_reader.h>
#include <QString>

class PlotComProxyReaderPrivate;

class PlotComProxyReader :  public QObject, public QTangoComProxyReader
{
	Q_OBJECT
	public:
		PlotComProxyReader(QObject *parent, QString name);
		QString name() { return d_name; }
		
		signals:
			void newData(const TVariant &value);
			void sourceUpperBound(const double u);
			void sourceLowerBound(const double l);
			void sourceConfigurationAvailable(const  TangoConfigurationParameters *);
			
  public slots:
	void refresh(const TVariant &); /* public for TrendPlot */
	
	void disconnectNewDataSignal();
	
	void connectNewDataSignal();
	
	
  protected slots:
			void sourceConfigured(const TangoConfigurationParameters *cp);
			
	private:
	  void fillFromHistory(const Tango::AttributeInfo &ainfo);
	  void d_triggerRefresh(const TVariant &);
	  
		QString d_name;
		bool d_filledFromHistory;

     PlotComProxyReaderPrivate *d_ptr;
};


#endif
