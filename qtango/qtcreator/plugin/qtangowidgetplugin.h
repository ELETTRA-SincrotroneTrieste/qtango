#pragma once

#include "qtangowidget_global.h"

#include <extensionsystem/iplugin.h>

namespace QTangoWidget {
namespace Internal {

class QTangoWidgetPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QTangoWidget.json")

public:
    QTangoWidgetPlugin();
    ~QTangoWidgetPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void triggerAction();
};

} // namespace Internal
} // namespace QTangoWidget
