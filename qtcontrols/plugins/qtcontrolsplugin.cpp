/* first of all ! */
#include <QtControls>
#include <qtcontrolsplugin.h>
/* and then... */
#include <qglobal.h>
#include <QtPlugin>

CustomWidgetInterface::CustomWidgetInterface(QObject *parent): QObject(parent), d_isInitialized(false)
{
}

void CustomWidgetInterface::initialize(QDesignerFormEditorInterface *)
{
    if (d_isInitialized)
        return;

    d_isInitialized = true;
}

EFlagInterface::EFlagInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "EFlag";
    d_include = "eflag.h";
    d_icon = QPixmap(":pixmaps/eflag.png");
    d_domXml = 
        "<widget class=\"EFlag\" name=\"eFlag\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>160</width>\n"
        "   <height>90</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *EFlagInterface::createWidget(QWidget *parent)
{
    return new EFlag(parent);
}

ELedInterface::ELedInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "ELed";
    d_include = "eled.h";
    d_icon = QPixmap(":pixmaps/eled.png");
    d_domXml = 
        "<widget class=\"ELed\" name=\"eLed\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>30</width>\n"
        "   <height>30</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *ELedInterface::createWidget(QWidget *parent)
{
    return new ELed(parent);
}

ELinearGaugeInterface::ELinearGaugeInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "ELinearGauge";
    d_include = "egauge.h";
    d_icon = QPixmap(":pixmaps/elineargauge.png");
    d_domXml = 
        "<widget class=\"ELinearGauge\" name=\"eLinearGauge\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>40</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *ELinearGaugeInterface::createWidget(QWidget *parent)
{
    return new ELinearGauge(parent);
}

ECircularGaugeInterface::ECircularGaugeInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "ECircularGauge";
    d_include = "egauge.h";
    d_icon = QPixmap(":pixmaps/ecirculargauge.png");
    d_domXml = 
        "<widget class=\"ECircularGauge\" name=\"eCircularGauge\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *ECircularGaugeInterface::createWidget(QWidget *parent)
{
    return new ECircularGauge(parent);
}

ENumericInterface::ENumericInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "ENumeric";
    d_include = "enumeric.h";
    d_icon = QPixmap(":pixmaps/enumeric.png");
    d_domXml = 
        "<widget class=\"ENumeric\" name=\"eNumeric\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>50</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *ENumericInterface::createWidget(QWidget *parent)
{
    return new ENumeric(parent, 4, 2);
}

EApplyNumericInterface::EApplyNumericInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "EApplyNumeric";
    d_include = "eapplynumeric.h";
    d_icon = QPixmap(":pixmaps/enumeric.png");
    d_domXml = 
        "<widget class=\"EApplyNumeric\" name=\"eApplyNumeric\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>160</width>\n"
        "   <height>70</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *EApplyNumericInterface::createWidget(QWidget *parent)
{
    return new EApplyNumeric(parent, 4, 2);
}

EApplyButtonInterface::EApplyButtonInterface(QObject *parent): CustomWidgetInterface(parent)
{
    d_name = "EApplyButton";
    d_include = "eapplybutton.h";
    d_icon = QPixmap(":pixmaps/eapplybutton.png");
    d_domXml = 
        "<widget class=\"EApplyButton\" name=\"eApplyButton\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>87</width>\n"
        "   <height>28</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *EApplyButtonInterface::createWidget(QWidget *parent)
{
    return new EApplyButton(parent);
}

ELabelInterface::ELabelInterface(QObject* parent) : CustomWidgetInterface(parent)
{
	d_name = "ELabel";
    d_include = "elabel.h";
	d_icon = QPixmap(":pixmaps/elabel.png");
	d_domXml =
 	"<widget class=\"ELabel\" name=\"eLabel\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>87</width>\n"
        "   <height>28</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";

}

QWidget* ELabelInterface::createWidget(QWidget* parent)
{
	return new ELabel(parent);
}

EApplicationButtonInterface::EApplicationButtonInterface(QObject* parent) : CustomWidgetInterface(parent)
{
	d_name = "EApplicationButton";
    d_include = "eapplicationbutton.h";
	d_icon = QPixmap(":pixmaps/eapplybutton.png");
	d_domXml =
 	"<widget class=\"EApplicationButton\" name=\"eApplicationButton\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>87</width>\n"
        "   <height>28</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";

}

QWidget* EApplicationButtonInterface::createWidget(QWidget* parent)
{
	return new EApplicationButton(parent);
}

EArrowInterface::EArrowInterface(QObject* parent) : CustomWidgetInterface(parent)
{
	d_name = "EArrow";
    d_include = "earrow.h";
	d_icon = QPixmap(":pixmaps/earrow.png");
	d_domXml =
 	"<widget class=\"EArrow\" name=\"eArrow\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>80</width>\n"
        "   <height>80</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *EArrowInterface::createWidget(QWidget* parent)
{
	return new EArrow(parent);
}


EPlotLightMarkerInterface::EPlotLightMarkerInterface(QObject* parent) : CustomWidgetInterface(parent)
{
	d_name = "EPlotLightMarker";
    d_include = "eplotlight_marker.h";
	d_icon = QPixmap(":pixmaps/graph.png");
	d_domXml =
 	"<widget class=\"EPlotLightMarker\" name=\"ePlotLightMarker\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>120</width>\n"
        "   <height>80</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget* EPlotLightMarkerInterface::createWidget(QWidget* parent)
{
	return new EPlotLightMarker(parent);
}


EPlotLightInterface::EPlotLightInterface(QObject* parent) : CustomWidgetInterface(parent)
{
	d_name = "EPlotLight";
    d_include = "eplotlight.h";
	d_icon = QPixmap(":pixmaps/graph.png");
	d_domXml =
 	"<widget class=\"EPlotLight\" name=\"ePlotLight\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>120</width>\n"
        "   <height>80</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget* EPlotLightInterface::createWidget(QWidget* parent)
{
	return new EPlotLight(parent);
}

EImageInterface::EImageInterface(QObject* parent) : CustomWidgetInterface(parent)
{
    d_name = "EImageWidget";
    d_include = "eimagewidget.h";
    d_icon = QPixmap(":pixmaps/eimage.png");
    d_domXml =
    "<widget class=\"EImageWidget\" name=\"eImageWidget\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>120</width>\n"
        "   <height>80</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget* EImageInterface::createWidget(QWidget* parent)
{
    return new EImageWidget(parent);
}



// EWidgetInterface::EWidgetInterface(QObject* parent) : CustomWidgetInterface(parent)
// {
// 	d_name = "EWidget";
// 	d_include = "EWidget";
// 	d_icon = QPixmap(":pixmaps/");
// 	d_domXml =
//  	"<widget class=\"EWidget\" name=\"eWidget\">\n"
//         " <property name=\"geometry\">\n"
//         "  <rect>\n"
//         "   <x>0</x>\n"
//         "   <y>0</y>\n"
//         "   <width>300</width>\n"
//         "   <height>350</height>\n"
//         "  </rect>\n"
//         " </property>\n"
//         "</widget>\n";
// }
// 
// QWidget* EWidgetInterface::createWidget(QWidget* parent)
// {
// 	return new EWidget(parent);
// }


EStackedWidgetContainerInterface::EStackedWidgetContainerInterface(QObject* parent) : CustomWidgetInterface(parent)
{
	d_name = "EStackedWidgetContainer";
    d_include = "estackedwidget_container.h";
	d_icon = QPixmap(":pixmaps/");
	d_domXml =
 	"<widget class=\"EStackedWidgetContainer\" name=\"eStackedWidgetContainer\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>120</width>\n"
        "   <height>80</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget* EStackedWidgetContainerInterface::createWidget(QWidget* parent)
{
	return new EStackedWidgetContainer(parent);
}

EContextHelpInterface::EContextHelpInterface(QObject* parent) : CustomWidgetInterface(parent)
{
	d_name = "EContextHelp";
    d_include = "econtexthelp.h";
	d_icon = QPixmap(":pixmaps/econtexthelp.png");
	d_domXml =
 	"<widget class=\"EContextHelp\" name=\"eContextHelp\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>110</width>\n"
        "   <height>28</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *EContextHelpInterface::createWidget(QWidget* parent)
{
	return new EContextHelp(parent);
}


CustomWidgetCollectionInterface::CustomWidgetCollectionInterface(QObject *parent): QObject(parent)
{
    d_plugins.append(new ELabelInterface(this) );
    d_plugins.append(new EFlagInterface(this));
    d_plugins.append(new ELedInterface(this));
    d_plugins.append(new ELinearGaugeInterface(this));
    d_plugins.append(new ECircularGaugeInterface(this));
    d_plugins.append(new ENumericInterface(this));
    d_plugins.append(new EApplyNumericInterface(this));
    d_plugins.append(new EApplyButtonInterface(this));
    d_plugins.append(new EApplicationButtonInterface(this));
    d_plugins.append(new EArrowInterface(this));
    d_plugins.append(new EContextHelpInterface(this));
    d_plugins.append(new EPlotLightInterface(this));
    d_plugins.append(new EPlotLightMarkerInterface(this));
    d_plugins.append(new EStackedWidgetContainerInterface(this));
    d_plugins.append(new EImageInterface(this));
//     d_plugins.append(new EWidgetInterface(this));
}

QList<QDesignerCustomWidgetInterface*> CustomWidgetCollectionInterface::customWidgets(void) const
{
    return d_plugins;
}

#if QT_VERSION < 0x050000
    Q_EXPORT_PLUGIN2(QtControls, CustomWidgetCollectionInterface)
#endif
