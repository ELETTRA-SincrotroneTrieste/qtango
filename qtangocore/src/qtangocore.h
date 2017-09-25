#ifndef _QTANGOCORE_H
#define _QTANGOCORE_H

/**
 * \mainpage QTangoCore - QtCore & Tango
 *
 * QTangoCore is a framework built on top of  
 * <a href="http://www.qtsoftware.com/products/platform/qt-for-linux-x11">Trolltech's QT </a>, version 4  
 * and  the <a href="http://www.tango-controls.org">Tango control system</a>
 * It implements a multi threaded architecture, device centric and easy to use
 * for the Qt programmer.
 *
 * @author Giacomo Strangolino and Vincenzo Forchi` - 2008-2009
 * @version 5.x.y
 *
 * @see <a href="http://doc.trolltech.com/4.6/">Qt documentation</a>.
 * @see <a href="../../../qtcontrols/doc/html/index.html">qtcontrols</a> documentation.
 * @see <a href="../../../qtango/doc/html/index.html">qtango</a> widgets documentation.
 * @see <a href="../QTangoCorePresentation.pdf">QTangoCore presentation</a>, pdf document
 * 
 * * **
 * <h1>Version 5.3.6</h1>
 * <p>
 *  The source is taken from what would have been the 4.3.6 release.
 *  Private pointers have been added to all relevant classes, so that they can be
 *  extended in the future by means of private classes without breaking the binary
 *  compatibility between releases.
 * </p>
 *
 * <h3>Version 4.x</h3>
 * <p>
 * Version <em>4.x</em> of the <em>QTangoCore</em> framework introduces some important features, which are described 
 * in continuation.
 * <ul>
 * <li><strong>Reconnection to the device at startup</strong>: when a reader or writer fails to connect to a Tango device,
 * it starts a <em>retry</em> cycle that tries to establish a link with the device server, after one, three, five seconds 
 * and then every ten seconds;
 * </li>
 * <li>introduction of a class called <strong>ProxyInterface</strong>. It provides a <em>common interface to readers and 
 * writers</em>, aimed at unifying the representation of some of their properties. It consists of three pure virtual methods
 * that must be implemented by readers or writers to export the tango point and the isReader or isWriter properties.
 * Moreover, it provides a list of the QTangoCommunicationHandle owned by the QTangoComProxyReader or QTangoComProxyWriter
 * and an <em>html representation</em> of the proxy interface and the <em>AttributeInfo</em> represented.
 *
 * @see ProxyInterface
 * @see ProxyInterface::htmlHandleRepresentation()
 *
 * This interface is used by the QTango <em>informational Text browser</em> (TInfoTextBrowser) and from any other class that
 * needs to access its methods and does not want to know if the object represents a reader or a writer.
 * </li>
 *
 * <li>Introduction of the class <strong>TLog</strong>. This class is aimed at managing Tango <em>DevFailed</em> exceptions
 * and format them into string messages. It can also manage Tango <em>DevErrorList</em> objects. This utility class provides
 * string formatting and prepares exception information to be used in conjunction with <em>TUtil addLog</em> method.
 * Actually, the TUtil::addLog method acquires a lock before enqueueing the messages that have to be displayed later on with
 * the log dialog, since many concurrent threads might want to add their messages to the queue.
 * <br/>Using TLog class inside each single thread limits the blocks of code that need to be lock protected inside TUtil.
 * <br/>Thus the user is discouraged from using anymore TUtil::formatMessage methods and TUtil::log methods, since they are
 * not supported and might not behave well.</li>
 *
 * <li><strong>QTangoComProxyReader</strong> <em>does no more implement Show/Hide events</em>. Widget events are now directly
 * managed by the QTango widgets. You might want  to have a look at QTango QTangoWidgetCommon class to learn how to write 
 * a QTango widget fully compatible with the QTangoCore library. QTangoWidgetCommon offers a common interface for all classes
 * that need to interact with QTangoCore functionalities related with show and hide events, mouse clicks, mouse enter and leave
 * events, source or targets copy and paste actions, drag and drop and so on. All these features are described in detail in the
 * <a href="../../../qtango/doc/html/index.html">qtango introduction</a> and especially in each single QTango class documentation.
 * Some older panels with <em>custom widgets</em> which used to simply inherit from QTangoComProxyWriter might not be fully 
 * compatible with this change, but the modification is truly aimed at simplifying and clearing the QTango widgets code.
 * </li>
 *
 * <li><strong>Getting attribute properties becomes easy</strong>: QTangoComProxyReader and Writer now have a method named
 * setDesiredAttributePropertyList(), accepting as parameter a QStringList. The string list represents the attribute property
 * names that a reader or writer wants to obtain at connection time, i.e. during <em>auto configuration</em>. When auto 
 * configuration is complete, you will get from the TangoConfigurationParameters object the wanted couples <em>property name/
 * property value</em>. @see TangoConfigurationParameters::attributePropertyValue()
 * Just remember to call setDesiredAttributePropertyList() on your reader or writer before calling setSource().
 * </li>
 *
 * 
 * <li><strong>Attribute configuration change events</strong>:
 * QTango supports the <em>attribute configuration change</em> events, which are available through the Tango library.
 * The <em>attribute configuration change</em> are <strong>enabled by default for all readers</strong> and, if 
 * available, all attribute configuration changes are notified by the QTangoCommunicationHandle::attributeAutoConfigured
 * signal. In the current version it is not possible to disable this feature <em>per reader</em>. On the other hand, it
 * is possible to <em>disable this feature globally</em> by means of an environment variable called <br/>
 * <em>QTANGO_ATTR_CONF_CHANGE_DISABLED</em><br/>.
 * <em>export QTANGO_ATTR_CONF_CHANGE_DISABLED=1</em> will <em>disable</em> attribute configuration change event subscription
 * at reader source configuration. This might lead to a faster startup of the QTango application if the event channel is not
 * available. Leaving <em>QTANGO_ATTR_CONF_CHANGE_DISABLED</em> unset or set to <em>0</em> will imply the default behaviour.
 *
 * @see QTangoComProxyReader
 *
 * </li>
 *
 * <li><strong>QTangoCore 4.x is more efficient</strong>: the startup procedure of QTango based applications 
 * is now much faster than before: configuration of the tango source points takes now completely place in the
 * separate thread that manages the communication with the tango device (attribute configuration, event subscription,
 * first read_attribute and so on). The graphical interface, living in the <em>GUI</em> thread, is fully responsive
 * from the very startup, thus improving the user experience. Configuration calls have been minimized to ensure
 * still faster response and bandwith optimization.
 * </li>
 *
 * <li><strong>synchronous and asynchronous targets execution</strong>: it is now easy to configure a <em>target</em>
 * so that its execution takes place in a different thread than the main one: this is useful for actions that 
 * require a long time to be completed. The configuration is exported through a designer property of the target.
 * </li>
 *
 * </ul>
 *
 * 
 *
 * </p>
 *
 * <p>
 * QTangoCore is the qt and Tango communication library. It is a multi threaded library. This version corrects some
 * bugs that in previous releases might have led to unexpected behaviour in a scenario made up of a large number of
 * concurrent threads. So an upgrade to this version is strongly recommended.
 * </p>
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */


#include <qtangocore_common.h>
#include <tvariant.h>
#include <tutil.h>
#include <QtDebug>

#endif
