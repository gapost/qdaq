#include "qledplugin.h"
#include "qdaqplotwidgetplugin.h"
#include "qdaqwidgetsplugin.h"

QDaqWidgetsPlugin::QDaqWidgetsPlugin(QObject *parent)
    : QObject(parent)
{
    m_widgets.append(new QLedPlugin(this));
    m_widgets.append(new QDaqPlotWidgetPlugin(this));

}

QList<QDesignerCustomWidgetInterface*> QDaqWidgetsPlugin::customWidgets() const
{
    return m_widgets;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qdaqwidgetspluginplugin, QDaqWidgetsPlugin)
#endif // QT_VERSION < 0x050000
