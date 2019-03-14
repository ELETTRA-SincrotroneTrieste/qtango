#ifndef SPECTRUM_BUTTON_H
#define SPECTRUM_BUTTON_H

#include <qtangocore.h>
#include <spectrumwriter.h>
#include <QPushButton>
#include <qtango_wcommon.h>


class TSpectrumButtonPrivate;
class QTimer;

/** \brief A special push button that can write on a spectrum attribute when configured with SimpleDataProxies as writers
 * 
 * Create your SimpleDataProxies in your GUI (e.g. TSpinBox, TDoubleSpinBox, TLineEdit..), then register them 
 * with the property objectNameList.
 * The configuration of the writer takes place after some time, to avoid issues with the creation order of
 * the widgets that is not defined a priori when using Qt designer.
 */
class TSpectrumButton : public QPushButton, public SpectrumWriter, public QTangoWidgetCommon
{
  Q_OBJECT
  Q_PROPERTY(QString targets READ targets WRITE setTargets DESIGNABLE true)
  Q_PROPERTY(QStringList objectNameList READ objectNameList WRITE setObjectNameList DESIGNABLE true)
  Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)

  Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
  Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
  Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
  Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
  
  public:
    TSpectrumButton(QWidget *parent);
	
	/** \brief returns the list of the names of the configured objects
	 *
	 * @return the list of the names of the configured objects, or an empty string if 
	 *         no object is configured or if the last configuration attempt with 
	 *         setObjectNameList() failed.
	 */
	QStringList objectNameList() { return d_objectNameList; }
	
	/** \brief sets the targets on the button
	 *
	 * Sets the targets on the button. The connection takes place if the object list is not empty.
	 *
	 * @see targets
	 * @see setObjectNameList
	 */
	void setTargets(const QString& targets);
	
	/** \brief returns the tango targets configured with setTargets
	 *
	 * @see setTargets
	 * @see setObjectNameList
	 */
	QString targets() { return d_targets; }
    
  public slots:
    virtual void execute(); // when clicked, this SLOT 'll be invoked

   /** \brief sets a list of strings containing the object names of the elements to be taken into account when writing
   *
   * @param objNameList the string list of object names to register into the writer.
   * 
   * Each time this method is called, the current list of writers is cleared and the new one is setup.
   * The list of objects must be <em>SimpleDataProxy</em> elements.
   * <em>Note</em>: the objects with the name passed are searched in the <em>parent</em> widget by means
   * of the <em>findChildren()</em> method, which looks for <em>QWidgets</em> and casts them 
   * into <em>SimpleDataProxies</em>. If one of the specified objects is not found, the whole operation
   * fails, a popup is shown and no writer will be registered.
   * If no object is found in the <em>parent</em> widget, the method still looks for it in the <em>parent's
   * parent</em>, then in the <em>parent's parent parent</em>, and so on, with the limit of the <em>non 
   * recursive <em>findChildren()</em></em> method, until the last <em>parent</em> is <strong>NULL</strong>.
   * <br/><em>Note</em>: the real activation of the writer takes place after a second by means of a timer, 
   * to wait for the creation  of all data proxies in case of GUIS created by means of the Qt designer 
   * (which does not determine the order of creation of its elements).
   */
   void setObjectNameList(const QStringList &objNameList);
   
   /** \brief reimplemented from spectrumwriter. Takes care whenever an object is going to be deleted.
    *
	* This calls SpectrumWriter::addElement, making it safe to delete an element after 
	* it has been added to the list of data proxies.
	*/
   void addElement(QObject *proxy);
   
   /** \brief Reimplemented from SpectrumWriter. Guarantees safety when an object inside the list is deleted.
    *
	* This method takes care of the SimpleDataProxies present in the list that might be destroyed.
	*/
   void setElements(const QList <QObject *>& list);
   
  protected slots:
	/* buff! I need it to configure the button when I am sure that all the writer elements 
	 * have been created. The order in which the designer creates the widgets in the 
	 * generated ui_*.h is the order in which the user adds them in the design phase.
	 * More generally, we cannot do any assumption on the objects creation order, but we
	 * can assume that it is safe to configure the objects after some time has elapsed.
	 */
	void activate();
   
  private slots:
	void dataProxyDeleted(QObject *);
	
  private:
	
	QStringList d_objectNameList;
	QString d_targets;
	bool d_configured;

    TSpectrumButtonPrivate *d_ptr;
};



#endif
