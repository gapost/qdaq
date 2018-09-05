#ifndef FILTERFACTORY_H
#define FILTERFACTORY_H

#include "filters_global.h"

#include "qdaqplugin.h"

#include <QObject>

class FILTERSSHARED_EXPORT FilterFactory :
        public QObject,
        public QDaqPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqPluginIID FILE "filters.json")
    Q_INTERFACES(QDaqPlugin)

public:
    FilterFactory();

    virtual QList<const QMetaObject *> pluginClasses() const;
};

#endif // FILTERFACTORY_H
