/***************************************************************************
*   Copyright (C) 2008 by Giacomo Strangolino	   *
*   giacomo.strangolino@elettra.trieste.it   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

/* $Id: com_proxy_writer.h,v 1.26 2013-07-03 13:54:42 giacomo Exp $ */


#ifndef  QTANGO_COM_PROXY_WRITER_H
#define QTANGO_COM_PROXY_WRITER_H

#include <QString>
#include <QVariant>

#include "proxy_interface.h"
#include "communication_handle.h"

/** \brief Base class to inherit from when creating a Tango writer. 
 *
 * Each Tango writer should inherit and
 * implement the getData() method returning a string representation of the data. The proxy writer offers a
 * base implementation of the setTargets() method, to associate the tango point to write to. The default 
 * implementation of the execute() method calls TAction's execute method, which in turn calls write_attribute()
 * or command_inout() for the configured target.
 *
 * The classical usage of QTangoComProxyWriter is to create a class inheriting from it and call the execute()
 * method inside a Qt slot to connect a particular event (for instance a mouse click event) to the execution
 * of the tango write_attribute() or command_inout().
 * <br/>
 * qtangoComHandle() method is the handle to the Tango connection. It is a QObject that makes possible to
 * be notified when attribute configuration is available, through the <em>signal</em> 
 * QTangoCommunicationHandle::attributeAutoConfigured(const TangoConfigurationParameters *)
 * connect it to your initialization slot to receive configuration events (for attributes only), to setup for
 * instance correct values for minimum and maximum. <br/>Remember that attribute configuration is possible if the 
 * corresponding tango attribute has been configured into the database, via jive, for example.
 * <br/>
 * <h2>Input arguments taken from SimpleDataProxy objects</h2>
 * <p>
  * Input arguments to be passed to the execution of commands or to the write attributes can be taken from other
  * objects implementing a SimpleDataProxy. A simple data proxy is an object that provides a string representation
  * of its data. For instance, a label displaying a double value or a string can be used to obtain
  * its displayed value and use it as input argument in a proxy writer. <br/>
  * Examples of SimpleDataProxy are represented by QTango TLineEdit and TDoubleSpinBox.
 * </p>
 * @see SimpleDataProxy
 * @see setTargets()
 * 
 * <br/>Examples of QTangoComProxyWriters are QTango TPushButton and TApplyNumeric.
 * \par Example of a writer header file
 * \code
 * class SimpleWriter : public QObject, public QTangoComProxyWriter
{
  Q_OBJECT // QObject because we normally need SIGNAL/SLOT framework
     public:
	  SimpleWriter(QObject *parent);
	  
     protected slots:
	  // in case of commands, the immediate read back is possible.
	  // in case of attributes, this is not.
          // - optional -
	  void refresh(const TVariant &v);
	  
	 // OPTIONAL: also for a writer it is possible to know the limits for a
	 // Tango device attribute.
	 // Not available for commands 
	 void init(const TangoConfigurationParameters *);
	 
	 void write();
	  
	 // necessary to re implement this. Make a dummy implementation if unuseful
	 QString getData() { return QString("Unused"); }
 }

 * \endcode

 * \par Example of a writer implementation
 * \code
  SimpleWriter::SimpleWriter(QObject *parent) : QObject(parent),
     QTangoComProxyWriter(parent)
{
     // enable auto configuration (optional)
     setAutoConfiguration(true);
     // - optional - auto configure if possible, when ready
     connect(qtangoComHandle(), SIGNAL(attributeAutoConfigured(const TangoConfigurationParameters *)), 
	  this, SLOT(init(const TangoConfigurationParameters *))); 
     // -optional - only for commands, the argout readback is possible:
     connect(qtangoComHandle(), SIGNAL(newData(const TVariant &)), this,
		   SLOT(refresh(const TVariant&)));
}

void SimpleWriter::write()
{   
     double value = 10.0;
     QVariant variant(value); 
     execute(variant);
}

// for commands only: read argout after a command was imparted
void SimpleWriter::refresh(const TVariant &v)
{
     double readVal;
     if(v.canConvertToDouble())
     {
	  readVal = v.toDouble();
          // do what you like with readVal
     }
}

 * \endcode

 *
 * <h3>Writing Qt Designer plugins implementing QTangoComProxyWriter</h3>
 * <p>
 * 	Inside a Qt designer plugin, special care must be taken in some cases.
 *      As to the writers, a source misconfigured could lead to a widget disable during the design phase.
 *      As a consequence, saving the form would produce an interface with the widget disabled.
 *      The method setDesignerMode() does this for you, without affecting the behaviour of the final widget.
 *	
 *
 * @see setDesignerMode
 *
 * </p>
 *
 */

/*
 * One might want to read QTangoComProxyReader's virtual base class inheritance notes. File: com_proxy_reader.h.
 * Such considerations apply to QTangoComProxyWriter too.
 */
class QTangoComProxyWriter : virtual public ProxyInterface
{

	public:
	    /** \brief proxy writer constructor
		 *
		 * Constructs a proxy writer with the parent specified. Each writer class should inherit 
		 * from the QTangoComProxyWriter and <em>always specify a valid parent</em>, to allow 
		 * proper destruction of the writer and the communication handle.
		 */
		QTangoComProxyWriter(QObject *parent);
		
		
		virtual ~QTangoComProxyWriter();
		
		/** \brief Associate a tango point (device attribute or command name) to a writer
		 * @param target a string representing the device name and the device attribute name or command name
		 *
		 * It is possible to provide a <em>specific target</em>, for example
		 * <ul><li>"test/device/1/double_scalar(10.0)"</li><li>
		 * "test/device/1->DevShort(12)</li></ul>
		 * or to indicate only attribute or command name, leaving the qtango core assign the device names
		 * taking them from the command line:
		 * <ul><li>"$1/double_scalar(10.1)</li>
		 * <li>"$1->DevLong(111)"</li></ul>
		 * It is also possible to take the input argument from another object (or widget). In that case, 
		 * the target string must contain the <em>&</em> token before the object name specified inside
		 * the brackets:
		 * <ul><li>setTargets("test/device/1/double_scalar(&displayObjectName)");</li>
		 * <li>setTargets("$1/double_scalar(&displayObjectName"));</li>
		 * <li>setTargets("$1->DevUshort(&displayObjectName)");</li></ul>
		 The default implementation connects to QTangoCore through the handle */
		virtual void setTargets(QString target);
		
		/** \brief removes the configured targets
		 *
		 * The default implementation disconnects from QTangoCore through the handle 
		*/
		virtual void clearTargets();
		
		/** \brief You can setup a list of attribute properties that will be read by the auto configuration
		 *         system. You will get them through TangoConfigurationParameters inside the attributeAutoConfigured()
		 *         signal.
		 *
		 * @param attributeProperties a QStringList containing the list of attribute properties you want to obtain
		 *        within the TangoConfigurationParameters object passed inside the attributeAutoConfigured() signal method.
		 *
		 * @see TangoConfigurationParameters::attributePropertyValue()
		 *
		 * <h3>Note</h3><p>You must call it inside your code <strong>before calling setSource()</strong>.
		 * <br/>Otherwise, auto configuration system won't be able to retrieve those attribute properties
		 * at connection time.</p>
		 */
		void setDesiredAttributePropertyList(const QStringList &attProps);
		
		/** \brief Returns the list of attribute properties you setup with setDesiredAttributePropertyList
		 *
		 * @see setDesiredAttributePropertyList
		 */
		QStringList desiredAttributePropertyList();

        /** \brief sets the timeout, in milliseconds, to be applied to the Database object
         *         before calling get_attribute_property in QTangoAutoConfiguration.
         *
         * @param dbTimeoutMillis, the timeout, in milliseconds, to be applied to get the desired
         *        attribute properties.
         *
         * @par Note
         * The timeout is applied only to the desiredAttributePropertyList properties, in
         * the auto configuration step, on the Database object. The timeout only affects the
         * Tango::Database::get_device_attribute_property method.
         */
        void setAttributePropertyDbTimeoutMillis(int dbTimeoutMillis);

        int attributePropertyDbTimeoutMillis() const;

		/** \brief method that can be implemented and connected to the newData signal to read tango commands argout 
		 *
		 * Reimplement this method in you own class and connect it as SLOT to the QTangoCommunicationHandle::newData() signal
		 * if you are interested in evaluating commands' argout after their execution.
		 * The usage and the signal/slot connection is created in the same way it is in the QTangoComProxyReader::refresh() case.
		 * @see QTangoComProxyReader::newData() (signal)
		 * @see QTangoComProxyReader::refresh()
		 * 
		 * <p><strong>Note</strong><br/> the refresh() method must be declared as a SLOT in your QObject.</p>
		 */
		virtual void refresh(const TVariant &) {};
		
		/** \brief returns the data handled by the oibject represented as string.
		 *
		 * This method should always be implemented, at least in the current QTangoCore release.
		 * If you do not need a string representation of the data contained in your object, return an
		 * empty QString.
		 * 
		 * @return a QString representation of the value contained in the object
		 */
		virtual QString getData() { return QString(); }
		
		/** \brief the execution method, that writes an attribute or imparts a command to the tango device specified as target
		 * 
		 */
		virtual QList<TVariant> execute(QVariant v = QVariant());
		
		/**
		 * \brief returns the target point(s) configured for the writer
		 *
		 * @return The string representing the tango target or targets for the writer. 
		 * For example:
		 * test/device/1/short_scalar or hostname:PORT/test/device/1/short_scalar, or
		 * $1/double_scalar, if the wildcard <em>$</em> was used. 
		 * 
		 * <h3>Note.</h3><p>  When <em>$1, $2...</em> wildcards are used, this method does 
		 * not return the <em>real</em> target 
		 * computed by the communication handle, for example via the replaceWildcards()
		 * method. On the other hand, the value returned by 
		 * targets() is the same returned by realTargets() if a tango target point was
		 * explicitly set, for instance <em>test/device/1/double_scalar</em>.</p>
		 * @see realTargets()
		 * @see QTangoCommunicationHandle
		 */
		QString targets() const ;
		
		/** \brief the tango point. Equivalent to targets() but without .
		 *
		 * This must be implemented since we are a ProxyInterface.
		 * @return the targets().
		 * Users: CopyPaste, QTangoHdb, and generally all ProxyInterface users.
		 *
		 */
		QString tangoPoint() const;
		
		/** \brief returns false 
		 * 
		 * Must be implemented since we are a ProxyInterface 
		 */
		bool isReader()  const { return false; }
		
		/** \brief returns true 
		 * 
		 * Must be implemented since we are a ProxyInterface 
		 */
		bool isWriter() const  { return !isReader(); }
		
		/**
		 * \brief returns the target currently used by the communication handle connected to tango
		 *
		 * @return the real target used by the qtango communication handle, which is equal to the value
		 *         returned by the method targets() if the target was explicitly and completely specified,
		 *         for instance: <em>tango/device/test/double_scalar</em>. On the other hand, when 
		 *         wildcards (<em>$1, $2, ...</em>) are used, the method returns the value of the target
		 *         <em>after the wildcards are replaced</em> or, it is equivalent, the value of the target
		 *         currently used by the qtango communication handle.
		 * If you are interested in retrieving the target value you provided in your code when you called setTargets(),
		 * use the method targets() instead.
		 *
		 * @see targets()
		 * @see setTargets()
		 * @see QTangoCommunicationHandle
		 */
		QString realTargets() const;

		QTangoCommunicationHandle* qtangoComHandle()  const { return d_qtangoHandle; }
		
		void hideEvent();
		
		void showEvent();
		
		int stepsCount();
		
		/* auto configuration */
		bool getAutoConfiguration();
		
		/** \brief auto configuration of a writer 
		 *
		 *
		 * <h3>Important note</h3>
		 * <p>
		 *	Should you not like to receive auto configuration events from the QTangoCommunicationHandle, disable
		 *  this property. If a tango attribute is configured 
		 *	into the Tango database with <em>minimum and maximum values</em>, then auto configuration is
		 *	possible. Writers will then auto configure themselves
		 *	with those minimum and maximum values.
		 *	If you want to set minimum and maximum values by yourself (by code or with the Qt designer), be sure 
		 * 	that the source is not configured into the tango database with minimum and maximum values.
		 * 	<br/>
		 *	<strong>Note</strong>: the same principle is applied to the <strong>readers</strong>, implementing 
		 *	QTangoComProxyReader.
		 * </p>
		 * <p>	
		 *	If you do not configure the tango database with a range, but you provide a standard measurement unit, then
		 * 	your writer will receive a configuration event with that data.
		 * </p>
		 * <p>
		 * Disabling auto configuration will ignore configuration events.
		 * </p>
		 */
		void setAutoConfiguration(bool en, bool confChangeEventEnabled = false);
		
		/** \brief returns true if auto configuration is enabled, false otherwise.
		 *
		 * @return true auto configuration of the widget is applied according to the tango database for the 
		 *         provided source set with setSource().
		 *
		 * @return false auto configuration is ignored and you are able to set custom minimum and maximum 
		 *         values, together with all the other parameters that your widget can display as to the
		 *         tango configuration parameters (measurement units, alarm and warn thresholds, and so on).
		 */
		bool autoConfiguration() const;
		
		/** \brief returns the connection error message deriving from a setSource failure
		 *
		 * When setSource() fails, the QTangoCommunicationHandle sets an error message that can be retrieved by
		 * this method.
		 * @return the setSource() error message.
		 */
		QString connectionErrorMessage();
		
		/** \brief tells the reader that it is being used inside the Qt designer (i.e. plugin)
		 *
		 * Calling this method with true, tells the reader that it is being used inside the designer.
		 * The reader might consequently configure itself in a special way. For example, inside the
		 * Qt designer, hide/show events must be disabled since TActions are shared among the same 
		 * sources.
		 * Please <strong>remember to call this inside any Qt Designer plugin implementing QTangoComProxyReader</strong>.
		 * This mode is obviously disabled by default (i.e. false).
		 *
		 * @see setHideEventEnabled
		 */
		void setDesignerMode(bool);
		
		bool designerMode();
		
	  /** \brief returns the target execution mode, synchronous, asynchronous or synchronous without stopping on errors.
	   *
	   * Selects the target execution mode:
	   * <ul><li><em>Synchronous</em>: the execution of the target takes place in synchronous mode (the default):
	   *   the execution (<em>command_inout()</em> or <em>write_attribute()</em>) takes place in the main
	   *   execution thread, blocking the application until the call returns;</li>
	   * <li><em>Asynchronous</em>: the execution takes place in the background, i.e. in the <em>Device</em> thread.
	   *  </li><li><em>DontStopOnErrors</em>: this is a <em>synchronous</em> mode, that does not stop execution of 
	   * multiple targets when an error is encountered. Indeed, the default behaviour is to stop after the first 
	   * error (in <em>Synchronous</em> mode).</li></ul>
	   *
	   */
		QTangoCommunicationHandle::ExecutionMode executionMode();
		
		/** \brief sets the execution mode
		 *
		 * sets the execution mode of the writer to mode.
		 * 
		 * @see ExecutionMode
		 */
                void setExecutionMode(QTangoCommunicationHandle::ExecutionMode mode);
		
	protected:
		/* protected because the widgets that inherit from this proxy, can use it as a Q_PROPERTY 
		 * (for the designer)
		*/
		QString d_helperApplication;
		
	private:
		
		QTangoCommunicationHandle* d_qtangoHandle;
		QString d_rawTargets;
};


#endif
