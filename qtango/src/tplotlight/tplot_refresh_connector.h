#ifndef PLOT_REFRESHER_H
#define PLOT_REFRESHER_H

#include "plot_com_proxy_reader.h"
#include <qtango_wcommon.h>
#include <proxy_interface.h>
#include <QString>
#include <QColor>
#include <QList>

class QTimer;
class TPlotRefreshConnectorPrivate;

/** \brief This class is used by the TPlotLightMarker to get data from one or more tango source points.
  *
  * This class is inherited by TPlotLightMarker. The documentation is provided to describe the
  * behaviour of the methods used by TPlotLightMarker itself.
  *
  * @see TPlotLightMarker
  */
class TPlotRefreshConnector : public QTangoWidgetCommon, public ProxyInterface
{
	/* must implement ProxyInterface virtual methods */
	public:
		TPlotRefreshConnector(QWidget *parent);
		~TPlotRefreshConnector();
		/** \brief sets a list of sources. For each source a curve is attached to the plot.
		 * @param sources list of tango sources separated by a ';'
		 */
		virtual void setSources(const QString& sources);

                /** Returns the source (or the sources if more than one, separated by ";"
                  *
                  * @return the Tango source point(s)
                  *
                  * @see setSources
                  * @see setSourcesList
                  * @see sourcesList
                  */
		QString sources() { return d_sources; }
		
                /** \brief set sources as a list of strings.
                  *
                  * @param sl QStringList of tango points to be read.
                  *
                  * @see setSources
                  * @see sources
                  * @see sourcesList
                  */
		void setSourcesList(QStringList &sl);

                /** \brief returns the sources as a list of QString.
                  *
                  * @return the tango source points.
                  */
		QStringList sourcesList();
		
                /** \brief returns the status of the fillFromHistory property
                  *
                  * The TPlotLightMarker is able to fill the first points of the plot with the data
                  * available from the attribute history. For this to work, it is necessary that the
                  * sources are attributes and the tango attributes have the polling enabled.
                  *
                  * @return true if the plot fills the first N points from the attribute history, if available.
                  * @return false if the plot starts reading without getting the attribute history.
                  */
		bool fillFromHistoryEnabled() { return d_fillFromHistoryEnabled; }

                /** \brief enables or disables the fillFromHistory property.
                  *
                  * @see fillFromHistoryEnabled
                  */
		void setFillFromHistoryEnabled(bool en) { d_fillFromHistoryEnabled = en; }
		
		void unsetSources();
		
		/** \brief pauses acquisition.
		 *
		 * Changes the refresh period setting it to 10 hours.
		 */
		virtual void pauseAcquisition();
		
		/** \brief Restarts the acquisition, when previously paused.
		 *
		 * Restores the period of the refresh connector to each reader.
		 */
		virtual void restartAcquisition();
		
		void setPeriod(int period);
		void setRefreshMode(RefreshMode rm);
		void setAutoConfiguration(bool ac) { d_autoConfiguration = ac; }
		
		bool autoConfiguration() { return d_autoConfiguration; }
		int period() { return d_period; }
		RefreshMode refreshMode() { return d_refreshMode; }

		void setDesignerMode(bool s);
		bool designerMode();
		
		int sourcesCount() { return m_readersMap.size(); }
		
                /** \brief Returns true if the provided source is among the readers.
                  *
                  * A true return value does not imply that the source is correctly connected to the tango point.
                  */
		bool sourceConfigured(const QString& src) { return m_readersMap.values().contains(src); }
		
		/* implementation of ProxyInterface virtual methods */
		QTangoCommunicationHandle* qtangoComHandle() const;
		QList<QTangoCommunicationHandle*> handleList() const;
		bool isReader() const { return true;}
		bool isWriter() const { return false; }
		virtual QString tangoPoint() const;
		/* end of the virtual methods implementation */
		
		bool hideEventEnabled();
		
		void setHideEventEnabled(bool en);
		
  protected:
	/** \brief reimplemented from QTangoWidgetCommon: tries to split the passed source to extract one or more sources.
	 *
	 * @param the tango source
	 *
	 * Tries to extract the tango source from the passed parameter, and pops up a menu asking if the source is 
	 * to replace or add.
	 */
	void changeTangoPoint(const QString &source);
	
	void _addSource(const QString &s);
	
	QString pastedSource() { return d_pastedSource; }
	
	PlotComProxyReader *reader(const QString &sourceName);
	
	void plotHideEvent();
	
	void plotShowEvent();

        QTimer* d_plotRefreshTimer;

        /* map that stores couples of readers and readers name.
         * Currently, the automatic management of the readers introduces into the map
         * the reader and its source, from which the arguments are removed, if present.
         * This is done by m_insertReader() below.
         */
        QMap<PlotComProxyReader *, QString> m_readersMap;
	
  protected slots:
	/* y axis configured from tango source */
	void setAutoConfigured();
	
	private:
	  
	void configureAndConnectReader(PlotComProxyReader *r);
  
        QWidget *d_parentWidget;

	int d_period;
	bool d_autoConfiguration, d_hideEventEnabled;
	bool d_fillFromHistoryEnabled;
	RefreshMode d_refreshMode;
	QString d_sources;
        QString d_pastedSource;

    private:
        /* inserts the reader r into the map, removing args from s, if s contains
         * arguments. For example s = "a/b/c->getData(1,100)" becomes a/b/c->getData
         * before being inserted into the map. The source is not the wildcard replaced
         * source.
         */
        void m_insertReader(PlotComProxyReader *, const QString&);

        /* given a source name, it looks into m_readersMap for a reader associated to that source.
         * In the search, if the source contains arguments, such as a/b/c->get(N,M), the arguments
         * are removed. This works in conjunction with m_insertReader.
         */
        PlotComProxyReader *m_readerFromSource(const QString& source);

        QString m_makeCurveTitleFromSource(const QString& source);

      TPlotRefreshConnectorPrivate *d_ptr;
};

#endif
