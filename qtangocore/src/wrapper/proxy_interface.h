#ifndef PROXY_INTERFACE_H
#define PROXY_INTERFACE_H

#include <QString>
#include <QList>

class QTangoCommunicationHandle;
class TangoConfigurationParameters;
class ProxyInterfacePrivate;

#define PROXY_INTERFACE(object) (dynamic_cast<ProxyInterface* >(object))

#define HANDLE(proxy) (proxy->qtangoComHandle())

/** \brief common interface that must be exported by qtango readers and writers
 *
 * QTangoComProxyReader and QTangoComProxyWriter provide a default implementation of the 
 * pure virtual methods of this class. Creating custom widgets derived from a reader or 
 * a writer gives the possibility to reimplement the virtual methods.
 * So you can personalize, for example, the <em>html representation</em> of you reader 
 * (writer) and provide the name of the tangoPoint.
 * Html representation of communication handles is used by the TInfoTextBrowser text browser
 * to display information related to the tango attribute or command represented by the 
 * ProxyInterface.
 * It is also used by other classes who need to obtain a reference to the QTangoCommunicationHandle
 * without knowing if the object is a reader or a writer.
 */
class ProxyInterface
{
  public:
	/** \brief The class constructor.
	 *
	 * Constructs the ProxyInterface class, and does not need any parameter.
	 * @see QTangoComProxyReader
	 * @see QTangoComProxyWriter
	 */
	ProxyInterface();
	
	/** \brief This method must be implemented in subclasses and must return the communication handle
	 *
	 * @see QTangoComProxyReader
	 * @see QTangoComProxyWriter
	 *
	 * QTangoComProxyReader and QTangoComProxyWriter return their communication handle.
	 * If a reader has more than one handle, like for instance qtango TPlotRefreshConnector,
	 * you must decide what to return with this method. TPlotRefreshConnector returns the first
	 * handle of the list.
	 * @see handleList().
	 */
	virtual QTangoCommunicationHandle* qtangoComHandle() const = 0;
	
	/** \brief returns the configured source or target.
	 *
	 * This must be implemented in each subclass.
	 * Normally returns the source or the target of the communication handle, without
	 * input parameters, i.e. QTangoCommunicationHandle::realSource() or realTargets().
	 */
	virtual QString tangoPoint() const = 0;
	
	/** \brief must be implemented in a subclass and return true if the interface represents a reader.
	 *
	 */
	virtual bool isReader() const = 0;
	
	/** \brief must be implemented in a subclass and return true if the interface represents a writer.
	 *
	 */
	virtual bool isWriter()  const= 0;
	
	/** \brief Returns the list of the communication handles owned by a reader or writer.
	 *
	 * The default implementation returns the qtangoComHandle() return value of this class,
	 * but if a subclass has more than one handle, it must reimplement this method and return
	 * the list.
	 * This is the case of qtango TPlotRefreshConnector, for example.
	 */
	virtual QList<QTangoCommunicationHandle*> handleList() const;
	
	/** \brief Provides an html representation of the communication handle.
	 *
	 * The default implementation essentially creates <em>for each communication handle</em>,
	 * an <em>h3</em> header with a couple of 
	 * <em>p</em> paragraphs, one of them containing an unsorted list.
	 * If you want, reimplement this method to provide another representation.
	 */
	virtual QString htmlHandleRepresentation();
	
	/** \brief formats AttributeAlarmInfo,  AttributeEventInfo and TangoConfigurationParameters in html.
	 *
	 */
	QString htmlAttributeInfoRepresentation();
	
  private:
    ProxyInterfacePrivate *d_piptr;
	
};

#endif
