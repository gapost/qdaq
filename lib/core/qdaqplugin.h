#ifndef QDAQPLUGIN_H
#define QDAQPLUGIN_H

#include <QtPlugin>
#include <QMetaObject>

class QDaqPlugin
{
public:
    virtual ~QDaqPlugin() {}


    virtual QList<const QMetaObject *> pluginClasses() const = 0;
};

#define QDaqPluginIID "org.qdaq.plugin"

Q_DECLARE_INTERFACE(QDaqPlugin, QDaqPluginIID)

#endif // QDAQPLUGIN_H
