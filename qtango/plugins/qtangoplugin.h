/* $Id: qtangoplugin.h,v 1.25 2014-08-26 14:55:11 giacomo Exp $
 *
 * Qt Designer plugin for Qtcontrols
 * this part is based in part on the work of
 * the Qwt project (http://qwt.sf.net).
 */

#ifndef QTANGO_PLUGIN_H
#define QTANGO_PLUGIN_H

#include <qglobal.h>

#if QT_VERSION < 0x050500
    #include <QDesignerCustomWidgetInterface>
#else
    #include <QtUiPlugin/QDesignerCustomWidgetInterface>
#endif

#include <QDesignerTaskMenuExtension>
#include <QExtensionFactory>

class EditSourceDialog;

/** \brief intercepts DropEvents and pops up a Dialog to configure the source
  *        or the target of the object.
  *
  * Introduced in 4.3.1 helps faster development of control panels.
  * Just drag and drop an attribute or a command from jive or from any other QTango panel
  * into any of the QTango widgets exported by the Qt designer and quickly configure its
  * source or target.
  */
class DropEventFilter : public QObject
{
    Q_OBJECT
public:
    DropEventFilter(QObject *parent);

 protected:
    bool eventFilter(QObject *obj, QEvent *event);

};

class CustomWidgetInterface: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT

    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    CustomWidgetInterface(QObject *parent);

    virtual bool isContainer() const { return false; }
    virtual bool isInitialized() const { return d_isInitialized; }
    virtual QIcon icon() const { return d_icon; }
    virtual QString codeTemplate() const { return d_codeTemplate; }
    virtual QString domXml() const { return d_domXml; }
    virtual QString group() const { return "QTango"; }
    virtual QString includeFile() const { return d_include; }
    virtual QString name() const { return d_name; }
    virtual QString toolTip() const { return d_toolTip; }
    virtual QString whatsThis() const { return d_whatsThis; }
    virtual void initialize(QDesignerFormEditorInterface *);

protected:
    QString d_name; 
    QString d_include; 
    QString d_toolTip; 
    QString d_whatsThis; 
    QString d_domXml; 
    QString d_codeTemplate;
    QIcon d_icon;

private:
    bool d_isInitialized;
};

class CustomWidgetCollectionInterface: public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
#endif

    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    CustomWidgetCollectionInterface(QObject *parent = NULL);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> d_plugins;
};

/* CustomWidgetInterface */

class TSimpleLabelInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TSimpleLabelInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TLabelInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TLabelInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TPropertyLabelInterface: public CustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TPropertyLabelInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TPixmapInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TPixmapInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};



class TCheckBoxInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TCheckBoxInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};



class TTableInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TTableInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TLedInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TLedInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TLineEditInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TLineEditInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TComboBoxInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TComboBoxInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TSpinBoxInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TSpinBoxInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TDoubleSpinBoxInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TDoubleSpinBoxInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};



class TLinearGaugeInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TLinearGaugeInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};



		
class TCircularGaugeInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TCircularGaugeInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};


class TRealtimePlotInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TRealtimePlotInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TPlotLightMarkerInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TPlotLightMarkerInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TNumericInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TNumericInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TApplyNumericInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TApplyNumericInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TSpectrumButtonInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TSpectrumButtonInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};


class TPushButtonInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TPushButtonInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TReaderWriterInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TReaderWriterInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TWidgetGroupInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TWidgetGroupInterface(QObject *parent);
    bool isContainer() const { return true; };
    virtual QWidget *createWidget(QWidget *parent);
};

class TWidgetInterface : public CustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TWidgetInterface(QObject *parent);
    bool isContainer() const { return true; };
    virtual QWidget *createWidget(QWidget *parent);
};

class QTangoInfoTextBrowserInterface : public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    QTangoInfoTextBrowserInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TImageWidgetInterface: public CustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TImageWidgetInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class TLogButtonInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TLogButtonInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};


//*////Added_code////
//class TImageInterface: public CustomWidgetInterface
//{
//    Q_OBJECT
//    Q_INTERFACES(QDesignerCustomWidgetInterface)

//public:
//    TImageInterface(QObject *parent);
//    //virtual QWidget *createWidget(QWidget *parent);
//    virtual QWidget *createWidget(QWidget *parent);
//};

//class TImageSnpInterface: public CustomWidgetInterface
//{
//    Q_OBJECT
//    Q_INTERFACES(QDesignerCustomWidgetInterface)

//public:
//    TImageSnpInterface(QObject *parent);
//    //virtual QWidget *createWidget(QWidget *parent);
//    virtual QWidget *createWidget(QWidget *parent);
//};
//////Added_code_end  */

/*

class TApplyButtonInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    TApplyButtonInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};
*/
/* fine widget  */

class TaskMenuFactory: public QExtensionFactory
{
Q_OBJECT
public:
	TaskMenuFactory(QExtensionManager *parent = 0);

protected:
	QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const;
};

class TaskMenuExtension: public QObject, public QDesignerTaskMenuExtension
{
Q_OBJECT
Q_INTERFACES(QDesignerTaskMenuExtension)

public:
	TaskMenuExtension(QWidget *widget, QObject *parent);

	QAction *preferredEditAction() const;
	QList<QAction *> taskActions() const;

private slots:
	void editTango();
	void editAttributes();
	
protected:
	QWidget *d_widget;
	QAction *d_editTangoAction;
	QAction *d_editAction;
	void setupSourceTargetDialog(QWidget *s);

	EditSourceDialog *editSourceDialog;
};



#endif
