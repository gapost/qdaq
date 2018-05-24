#ifndef FILTERFACTORY_H
#define FILTERFACTORY_H

#include "filters_global.h"

#include "QDaqFilterPlugin.h"

class FILTERSSHARED_EXPORT FilterFactory :
        public QObject,
        public QDaqFilterPluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDaqFilterPluginFactory_iid FILE "filters.json")
    Q_INTERFACES(QDaqFilterPluginFactory)

public:
    FilterFactory();

    virtual QStringList availablePlugins();

    virtual QObject* createPlugin(const QString& iid);
};

#endif // FILTERFACTORY_H
