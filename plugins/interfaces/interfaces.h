#ifndef INTERFACES_H
#define INTERFACES_H

#include "interfaces_global.h"

#include "qdaqplugin.h"

#include <QObject>

class INTERFACESSHARED_EXPORT Interfaces :
        public QObject,
        public QDaqPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqPluginIID FILE "qdaqinterfaces.json")
    Q_INTERFACES(QDaqPlugin)

public:
    Interfaces();

    virtual QList<const QMetaObject *> pluginClasses() const;
};

#endif // INTERFACES_H
