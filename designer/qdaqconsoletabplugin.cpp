#include "QDaqConsole.h"
#include "qdaqconsoletabplugin.h"

#include <QtPlugin>

QDaqConsoleTabPlugin::QDaqConsoleTabPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}


void QDaqConsoleTabPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QDaqConsoleTabPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QDaqConsoleTabPlugin::createWidget(QWidget *parent)
{
    return new QDaqConsoleTab(parent);
}

QString QDaqConsoleTabPlugin::name() const
{
    return QLatin1String("QDaqLed");
}

QString QDaqConsoleTabPlugin::group() const
{
    return QLatin1String("QDaq Widgets");
}

QIcon QDaqConsoleTabPlugin::icon() const
{
    return QIcon();
}

QString QDaqConsoleTabPlugin::toolTip() const
{
    return QLatin1String("A Led indicator/button");
}

QString QDaqConsoleTabPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QDaqConsoleTabPlugin::isContainer() const
{
    return false;
}

QString QDaqConsoleTabPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QDaqLed\" name=\"qLed\">\n</widget>\n");
}

QString QDaqConsoleTabPlugin::includeFile() const
{
    return QLatin1String("qled.h");
}
