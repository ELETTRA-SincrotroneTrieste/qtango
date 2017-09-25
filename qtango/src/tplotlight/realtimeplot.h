#ifndef REALTIME_PLOT_H
#define REALTIME_PLOT_H

#include "tplotlight_marker.h"
#include <QDateTime>

class TRealtimePlotPrivate;

/** \brief A TPlotLightMarker specialized in managing real time commands.
 *
 * This class inherits from TPLotLightMarker and is tailored to work with commands.
 * Command syntax only is accepted by setSources, and the configuration dialog 
 * available through the right click menu allows the configuration of the real time
 * parameters that caracterize the real time commands.
 * The expected argout returned by the command is of type <em>spectrum</em>.
 */
class TRealtimePlot : public TPlotLightMarker
{
  Q_OBJECT
  
  /* these are not designable, because they are intended to be used by the real time plot configuration 
   * widget (RtConfWidget). See public access methods for the notes.
   */
  Q_PROPERTY(int numberOfPoints READ numberOfPoints WRITE setNumberOfPoints DESIGNABLE false)
  Q_PROPERTY(int mode READ mode WRITE setMode DESIGNABLE false)
  Q_PROPERTY(int bunch1 READ bunch1 WRITE setBunch1 DESIGNABLE false)
  Q_PROPERTY(int bunch2 READ bunch2 WRITE setBunch2 DESIGNABLE false)
  Q_PROPERTY(QDateTime date1 READ date1 WRITE setDate1 DESIGNABLE false)
  Q_PROPERTY(QDateTime date2 READ date1 WRITE setDate2 DESIGNABLE false)
  
  public:
	
  TRealtimePlot(QWidget *parent);
  
  /** \brief reimplemented from TPlotRefreshConnector's, takes the sources provided and 
   *         build suitable sources for realtime commands.
   *
   * @param srcs the real time commands, separated by <em>;</em>.
   * 
   * The real time plot accepts only commands as sources, such as <em>test/device/rtime->GetVector</em>.
   * Moreover, we suppose that the real time commands return an array, i.e. they have <strong>spectrum</strong> argouts.
   * This implies that the <em>time scale draw</em> property of the EPlotLight is disabled and on the X axis we have 
   * integer points from 0 to N, where N is the size of the command argout.
   * Thus <em>adding</em> a source by code or by a <em>drag and drop</em> operation presents a different behaviour with respect
   * to TPlotLightMarker working with attributes (see addPastedSource() implementation).
   * <h3>Features</h3>
   * <p>
   * <ul><li>the source contains compatible real time command argins <em>test/device/rtime->GetVector(0,100)</em>: ok;</li>
   * <li>the source contains only the command name: <em>test/device/rtime->GetVector</em>: a <strong>default</strong>
   *   argin is appended.</li>
   * </ul>
   * </p>
   * <p> So, if you provide a source like <em>a/b/c->GetData(0,1000);a/b/c->GetData2</em>, the result will be
   * <em>a/b/c->GetData(0,1000);a/b/c->GetData2(0,1000)</em>.
   * </p>
   * <p> With the sources built like that, TPlotLightMarker::setSources() will be called.
   * </p>
   * <h3>How the default argin is chosen</h3>
   * <p>
   * If the source names never contain argins, then the default argin is set to (0,1000), that means mode 0 and 1000 points.
   * \example 
   * \code
   * plot->setSources("a/b/c->GetData;d/e/f->GetPos;a/b/c->GetFoo");
   * \endcode
   * the sources become "a/b/c->GetData(0,100), d/e/f->GetPos(0,100), a/b/c->GetFoo(0,100)"
   *
   * </p>
   * <p>
   * If at least one of the source(s) contain an argin, <em>the last provided is applied to all the sources</em>:
   * \example
   * \code
   * plot->setSources("a/b/c->GetData;d/e/f->GetPos(0,150);a/b/c->GetFoo");
   * \endcode
   * the sources become "a/b/c->GetData(0,150), d/e/f->GetPos(0,150), a/b/c->GetFoo(0,150)"
   *
   * </p>
   * <p>
   * If some sources are configured and a new source is dropped into the plot:
   * <ul><li>the source does not contain an argin: <em>a/b/c->GetData</em>: the number of points and the 
   *         mode is taken from the current values;</li>
   * <li>the source contains an argin: <em>a/b/c/->GetData(1,200)</em>: all the previously configured sources
   *     will change their mode and point number to 1,200.</li>
   * </ul>
   * This means that the <em>last source with a provided argin <strong>predominates</strong></em>.
   * This choice is based on the assumption that
   * <ol><li>the number of points on the x axis must be the same for all the curves in the plot;</li>
   * <li>the modes of all the real time sources must be the same.</li>
   * </ol>
   * A different behaviour is thought to be misleading and confusing: in that case two or more different plots
   * should be used.
   * </p>
   * @see TPlotLightMarker::setSources
   *
   */
  virtual void setSources(const QString &srcs);
  
  /** \brief returns the sources configured, together with information about frequency, mode and number of points
   *
   * Reimplemented from EPlotLightBase::plotTitle()
   */
   virtual QString plotTitle();
  
  public:
	
	/* access methods for the Q_PROPERTIES follow. Be sure, in the configuration dialogs or whenever
	 * you use these methods, that the setMode() must be called last, because it only triggers a real
	 * update of the qtangocore configured sources.
	 */
	
	int numberOfPoints() { return d_nPoints; }
	int mode() { return d_mode; }
	QDateTime date1() { return d_date1; }
	QDateTime date2() { return d_date2; }
	int bunch1() { return d_b1; }
	int bunch2() { return d_b2; }
	
	/** \brief changes the mode of the configured sources
	 *
	 * <strong>Note</strong>: call this <strong>after all the following</strong> methods,
	 * since setMode really changes the read mode of the plot (unsets the sources and 
	 * sets them again), taking the parameters from the other properties.
         * <p><ul>Available modes
	 * <li><strong>0</strong>: number of points mode: the configured number of points is read every refresh;</li>
	 * <li><strong>1</strong>: <em>bunch1 to bunch N</em> mode;</li>
         * <li><strong>2</strong>: <em>timestamp</em> mode.</li>
	 * </ul>
	 * The configuration parameters for the chosen mode (bunch1, bunch2, date1, date2) are Qt properties.
	 * They can be changed programmatically by the corresponding set... methods or by the right click 
	 * dialog.
	 * </p>
	 */
	void setMode(int m);

  public slots:
	void setNumberOfPoints(int n);
    void setDate1(const QDateTime &d1);
    void setDate2(const QDateTime &d2);
    void setBunch1(int b);
    void setBunch2(int b2);
    void updateParams();
	
	/* ^-- end of the Qt Properties access methods --^ */
	
   
  protected slots:
	/** \brief executes the configuration dialog.
	 *
	 * Reimplemented from EPlotLightBase.
	 */
	virtual void configurePlot();
	
	/** \brief adds the pasted source. Reimplemented from TPlotRefreshConnector.
	 *
	 * In TPlotLightMarker tango scalar attributes can be configured. In that scenario,
	 * when a new attribute is added, it is important to begin plotting the new curve
	 * without erasing or losing data of the previous one(s).
	 * Here, at every refresh we read a 
	 */
	virtual void addPastedSource();
	
  private:
	
	/** \brief from the list of sources, extracts the default argin from the last one
	 *
	 * @return true if at least one source contains an argin
	 * @return false if no source contains an argin.
	 * 
	 * if at least one source contains an argin, this method returns true and saves the
	 * extracted argin in d_defaultArgin.
	 * If not, returns false and d_defaultArgin remains untouched
	 */
	bool extractDefaultArginFromSourcesList(const QStringList &sources);
	
	/** \brief given the argin (x,y), (x,y,z), ... updates mode, number of points, bunch1, bunchN, dates..
	 *
	 */
	void updatePropertiesFromArgin();
	
	/** 
	 * foreach string passed as argument in the list, `(x,y)' argin is removed
	 * and the sources will all be of type a/b/c->d
	 */
	void clearArginsFromSourcesList(QStringList &srcs);
	
	QString d_defaultArgin;
	
	  int d_nPoints, d_mode, d_b1, d_b2;
	  QDateTime d_date1, d_date2;

     TRealtimePlotPrivate *d_ptr;
};





#endif
