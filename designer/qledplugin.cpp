#include "gui/QDaqLed.h"
#include "qledplugin.h"

#include <QtPlugin>

QLedPlugin::QLedPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void QLedPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QLedPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QLedPlugin::createWidget(QWidget *parent)
{
    return new QDaqLed(parent);
}

QString QLedPlugin::name() const
{
    return QLatin1String("QDaqLed");
}

QString QLedPlugin::group() const
{
    return QLatin1String("QDaq Widgets");
}

QIcon QLedPlugin::icon() const
{
    return QIcon();
}

QString QLedPlugin::toolTip() const
{
    return QLatin1String("A Led indicator/button");
}

QString QLedPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QLedPlugin::isContainer() const
{
    return false;
}

QString QLedPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QDaqLed\" name=\"qLed\">\n</widget>\n");
}

QString QLedPlugin::includeFile() const
{
    return QLatin1String("qled.h");
}

