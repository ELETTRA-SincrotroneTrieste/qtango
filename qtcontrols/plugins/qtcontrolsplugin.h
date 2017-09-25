/* 
 * Qt Designer plugin for Qtcontrols
 * this part is based in part on the work of
 * the Qwt project (http://qwt.sf.net).
 */

#ifndef QTCONTROLS_PLUGIN_H
#define QTCONTROLS_PLUGIN_H

#include <qglobal.h>

#if QT_VERSION < 0x050500
    #include <QDesignerCustomWidgetInterface>
#else
    #include <QtUiPlugin/QDesignerCustomWidgetInterface>
#endif

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
    virtual QString group() const { return "QtControls"; }
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
class EFlagInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    EFlagInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class ELedInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    ELedInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class ELinearGaugeInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    ELinearGaugeInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class ECircularGaugeInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    ECircularGaugeInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class ENumericInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    ENumericInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class EApplyButtonInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    EApplyButtonInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class EApplyNumericInterface: public CustomWidgetInterface 
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    EApplyNumericInterface(QObject *parent);
    virtual QWidget *createWidget(QWidget *parent);
};

class ELabelInterface : public CustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
	
public:
    ELabelInterface(QObject* parent);
    virtual QWidget* createWidget(QWidget* parent);
};

class EApplicationButtonInterface : public CustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
	
public:
    EApplicationButtonInterface(QObject* parent);
    virtual QWidget* createWidget(QWidget* parent);
};

class EArrowInterface : public CustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
	
public:
    EArrowInterface(QObject* parent);
    virtual QWidget* createWidget(QWidget* parent);
};

class EPlotLightMarkerInterface : public CustomWidgetInterface
{
   Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
	
  public:
    EPlotLightMarkerInterface(QObject* parent);
    virtual QWidget* createWidget(QWidget* parent);
};

class EPlotLightInterface : public CustomWidgetInterface
{
   Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
	
  public:
    EPlotLightInterface(QObject* parent);
    virtual QWidget* createWidget(QWidget* parent);
};

class EImageInterface : public CustomWidgetInterface
{
   Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    EImageInterface(QObject* parent);
    virtual QWidget* createWidget(QWidget* parent);
};

class EStackedWidgetContainerInterface  : public CustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
      EStackedWidgetContainerInterface(QObject* parent);
      bool isContainer() const { return true; }
      virtual QWidget* createWidget(QWidget* parent);
};

// class EWidgetInterface  : public CustomWidgetInterface
// {
//     Q_OBJECT
//     Q_INTERFACES(QDesignerCustomWidgetInterface)
//   public:
//       EWidgetInterface(QObject* parent);
//       virtual QWidget* createWidget(QWidget* parent);
// };

class EContextHelpInterface : public CustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
	
public:
    EContextHelpInterface(QObject* parent);
    virtual QWidget* createWidget(QWidget* parent);
};

/* fine widget  */

#endif
