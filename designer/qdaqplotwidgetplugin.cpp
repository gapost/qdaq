#include "gui/QDaqPlotWidget.h"

#include "qdaqplotwidgetplugin.h"

#include <QtPlugin>

QDaqPlotWidgetPlugin::QDaqPlotWidgetPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void QDaqPlotWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QDaqPlotWidgetPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QDaqPlotWidgetPlugin::createWidget(QWidget *parent)
{
    return new QDaqPlotWidget(parent);
}

QString QDaqPlotWidgetPlugin::name() const
{
    return QLatin1String("QDaqPlotWidget");
}

QString QDaqPlotWidgetPlugin::group() const
{
    return QLatin1String("QDaq Widgets");
}

QIcon QDaqPlotWidgetPlugin::icon() const
{
    return QIcon();
}

QString QDaqPlotWidgetPlugin::toolTip() const
{
    return QLatin1String("QDaq plot widget");
}

QString QDaqPlotWidgetPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QDaqPlotWidgetPlugin::isContainer() const
{
    return false;
}

QString QDaqPlotWidgetPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QDaqPlotWidget\" name=\"qDaqPlotWidget\">\n</widget>\n");
}

QString QDaqPlotWidgetPlugin::includeFile() const
{
    return QLatin1String("QDaqPotWidget.h");
}

