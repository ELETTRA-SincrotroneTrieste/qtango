#ifndef TPIXMAP_H
#define TPIXMAP_H

#include <com_proxy_reader.h>
#include <qtango_wcommon.h>
#include <QtCore>
#include <QApplication>
#include <QPixmap>
#include <QLabel>
#include <QString>
#include <QPair>

class TPixmapPrivate;

/** \brief A class to display different pixmaps depending on the state of a device.
 * This class is based on a QLabel and displays a pixmap which changes when a 
 * tango device state changes.
 * You create it invoking the constructor `TPixmap', optionally passing the 
 * parent and the window flags to it, and then you associate each state of
 * your device with a pixmap of your choice with `setStatePixmap()', where
 * you specify the Tango state and the corresponding file name of the pixmap
 * you want to be shown.
 * The quality of the attribute is also considered...
 * <p>
 * <em>Changes in qtango 4.x</em>
 * <ul>
 * <li>
 * Pixmap does not blink anymore when the quality of the attribute is WARNING, ALARM, MOVING or something like that..
 * Instead, a coloured rectangle is drawn over the pixmap with transparency;
 * </li>
 * <li><em>defaultPixmap</em> designer property is now exported to set the <em>file name</em> of the desired 
 * default pixmap; </li>
 * <li><em>stateList</em> designer property is exported to save a list of states to associate to a list of 
 * <strong>pixmap names</strong>;</li>
 * <li><em>pixmapList</em>  designer property is exported to save a list of pixmap to associate to a list of
 * states or integer values.</li>
 * </ul>
 * </p>
 */

class TPixmap : public QLabel, public QTangoComProxyReader, public QTangoWidgetCommon
{
	Q_OBJECT

Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)
Q_PROPERTY(bool helperApplicationEnabled READ helperApplicationEnabled WRITE setHelperApplicationEnabled)
Q_PROPERTY(QString helperApplication   READ helperApplication        WRITE setHelperApplication)
Q_PROPERTY(bool communicationStatusChangesWidgetEnable READ communicationStatusChangesWidgetEnable WRITE setCommunicationStatusChangesWidgetEnable DESIGNABLE true)

Q_PROPERTY(QStringList stateList READ stateList WRITE setStateList DESIGNABLE true)
Q_PROPERTY(QStringList pixmapList READ pixmapList WRITE setPixmapList DESIGNABLE true)
Q_PROPERTY(QString defaultPixmap READ defaultPixmap WRITE setDefaultPixmap DESIGNABLE true)

Q_PROPERTY(bool enterLeaveEventsEnabled READ enterLeaveEventsEnabled WRITE setEnterLeaveEventsEnabled DESIGNABLE true)
Q_PROPERTY(int enterEventDelay READ enterEventDelay WRITE setEnterEventDelay DESIGNABLE enterLeaveEventsEnabled)
Q_PROPERTY(bool copyActionEnabled READ copyActionEnabled WRITE  setCopyActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool pasteActionEnabled READ pasteActionEnabled WRITE setPasteActionEnabled  DESIGNABLE true)
Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled DESIGNABLE true)
Q_PROPERTY(QString additionalInfo READ  additionalInfo WRITE setAdditionalInfo)
	
	public:

	/** The default constructor: constructs a null pixmap */  
    TPixmap(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~TPixmap();

	/** Associates a state to a pixmap. Calls qt `load()' function to set the pixmap file */
	void setStatePixmap(int state, const QString & filename);
	
	/** Associates a Tango specific state to a Pixmap */
	void setStatePixmap(Tango::DevState state, const QString &filename);
	
	/** sets the default pixmap to show when the current state is 
	  * not associated to a pixmap.
	  */
	void setDefaultPixmap(const QString &filename);
	
	QString defaultPixmap() { return default_pixmap; }

	/** Returns the state of the object */
	int getState() { return state; }

	/* Called by apply_state, saves the current pixmap being shown */
	void SavePixmap(QString p) { saved_pixmap = p; }
	/* Returns the currently saved pixmap */
	QString GetSavedPixmap() { return saved_pixmap; }
	
	/** \brief a designable property to define a list of Tango states or integers
	 * 
	 * Set a list of tango states in the form ON, OFF, RUNNING... or a list of integer values
	 * to associate to a list of pixmaps using setPixmapList()
	 *
	 * @see setPixmapList
	 * <strong>note</strong>: configuration is updated whenever the state list size becomes equal to
	 * the pixmap list size. 
	 */
	void setStateList(const QStringList &);
	
	/** \brief returns the list of states defined 
	 *
	 * <em>Note</em>: for the configuration to work, it is necessary to setup with
	 * setPixmapList() and setStateList() a set of states and pixmap file names of the
	 * same size.
	 * @see setPixmapList
	 * @see setStateList
	 */
	QStringList stateList() { return d_stateList; }
	
	/** \brief a designable property to define a list of Tango pixmap file names
	 * 
	 * Set a list of pixmap file names in the form of absolute or relative paths 
	 * to associate to a list of tango states using setStateList()
	 *
	 * @see setStateList
	 *
	 * <strong>note</strong>: configuration is updated whenever the state list size becomes equal to
	 * the pixmap list size. 
	 */
	void setPixmapList(const QStringList &names);
	
	/** \brief returns the list of the pixmap file names which you want to associate to the list of states
	 *
	 * <em>Note</em>: for the configuration to work, it is necessary to setup with
	 * setPixmapList() and setStateList() a set of states and pixmap file names of the
	 * same size.
	 * @see setPixmapList
	 * @see setStateList
	 */
	QStringList pixmapList() { return d_pixmapList; }

	protected:
	
	void paintEvent(QPaintEvent *);

	private:
	
	/* Returns the state associated to the pixmap filename, -1 on failure */
	int stateForPixmap(QString filename);

	/* Returns the file name of the pixmap associated to the state, an empty string ("") if
	 * no pixmap is associated to that state.
	 */
	QString pixmapForState(int state);

	protected slots:
	void refresh(const TVariant &v);
	
	protected:
	void apply_state(unsigned int state);

	/* Variables */
	int state, previous_quality;

	bool nullpix; /* True if current pixmap is null. Used in timerEvent */
	QList <QPair <int, QString> > assoc;
	QString saved_pixmap, default_pixmap;
	
	QStringList d_stateList, d_pixmapList;
	
	void setPixmapByName(const QString &name);
	
	/* called by setPixmapList() and setStateList(), when the sizes of the 
	 * lists are equal, sets up the association among states and pixmaps.
	 */
	void updateConfiguration();

private:
    TPixmapPrivate *d_ptr;
};



#endif
