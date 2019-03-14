#ifndef _TWIDGET_GROUP
#define _TWIDGET_GROUP

#include <readersgroup.h>
#include <qtango_wcommon.h>
#include <QGroupBox>

class QObject;
class TWidgetGroupPrivate;

/**
 * \brief a viewer to group widgets
 *
 * this viewer groups together different viewers and allows to split 
 * data from a Tango spectrum into various scalar data and develop them 
 * to single widgets
 *
 * This class is left for compatibility reasons: you could directly use 
 * the ReaderGroup qtangocore class, since it is not necessary that the
 * readers are really widgets. It's actually enough to be QTangoComProxyReaders.
 *
 * \par Example
 * \verbatim
TWidgetGroup tvg;
for (int i = 0; i < 16; i++)
{
	TLabel *tl = new TLabel(this);
        tvg.addReader(tl, i);
}
tvg.setSource("some/test/device/spectrum_attribute");
\endverbatim
 *
 * Since qtango version <em>4.x</em> it is possible to use Qt designer to setup a widget 
 * containing readers that take data from spectrum attributes.
 * This is realized by setting, via the designer, the <em>objectNameList</em> property and the 
 * tango <em>source</em> property. The  <em>objectNameList</em> property is a list of 
 * strings representing the object names of the readers, which must be of type <em>QTangoComProxyReader</em>.
 * 
 */
class TWidgetGroup : public QGroupBox, public QTangoWidgetCommon, public ProxyInterface
{

Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
Q_PROPERTY(QStringList objectNameList READ objectNameList WRITE setObjectNameList DESIGNABLE true)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)
Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
  
Q_OBJECT
public:
	TWidgetGroup(QWidget *parent);
	
	virtual void setSource(const QString&);
	
	QString source() { return d_source; }
	
	/** \brief returns the list of the names of the configured objects
	 *
	 * @return the list of the names of the configured objects, or an empty string if 
	 *         no object is configured or if the last configuration attempt with 
	 *         setObjectNameList() failed.
	 */
	QStringList objectNameList() { return d_objectNameList; }

   /** \brief sets a list of strings containing the object names of the elements to be taken into account when writing
   *
   * @param objNameList the string list of object names to register into the writer.
   * 
   * Each time this method is called, the current list of writers is cleared and the new one is setup.
   * The list of objects must be <em>QTangoComProxyReader</em> elements.
   * <em>Note</em>: the objects with the name passed are searched in the <em>parent</em> widget by means
   * of the <em>findChildren()</em> method, which looks for <em>QWidgets</em> and casts them 
   * into <em>QTangoComProxyReader</em>. If one of the specified objects is not found, the whole operation
   * fails, a popup is shown and no writer will be registered.
   * If no object is found in the <em>parent</em> widget, the method still looks for it in the <em>parent's
   * parent</em>, then in the <em>parent's parent parent</em>, and so on, with the limit of the <em>non 
   * recursive <em>findChildren()</em></em> method, until the last <em>parent</em> is <strong>NULL</strong>.
   */
   void setObjectNameList(const QStringList &objNameList);
   
   void setDesignerMode(bool en);
   
   QString tangoPoint() const { return d_source; }
   bool isReader()  const{ return true; }
   bool isWriter() const { return !isReader(); }
   QTangoCommunicationHandle *qtangoComHandle() const { return d_readersGroup->qtangoComHandle(); }
   
    protected slots:
	/* buff! I need it to configure the button when I am sure that all the writer elements 
	 * have been created. The order in which the designer creates the widgets in the 
	 * generated ui_*.h is the order in which the user adds them in the design phase.
	 * More generally, we cannot do any assumption on the objects creation order, but we
	 * can assume that it is safe to configure the objects after some time has elapsed.
	 */
	void activate();
	
	bool designerMode() { return d_designerMode; }
   
  private:
	
	QStringList d_objectNameList;
	QString d_source;
	ReadersGroup *d_readersGroup;
	bool d_designerMode;

    TWidgetGroupPrivate *d_ptr;
};

#endif	
