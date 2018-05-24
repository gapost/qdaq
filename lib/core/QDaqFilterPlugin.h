#ifndef QDAQFILTERPLUGIN_H
#define QDAQFILTERPLUGIN_H

#include <QtPlugin>
#include <QList>

class QDaqFilterPlugin
{
public:

    virtual ~QDaqFilterPlugin() {}

    virtual QString iid() = 0;

    virtual QString errorMsg() = 0;

    virtual bool init() = 0;
    virtual bool operator()(const double* vin, double* vout) = 0;

    virtual int nInputChannels() const = 0;
    virtual int nOutputChannels() const = 0;
};

class QDaqFilterPluginFactory
{
public:
    virtual ~QDaqFilterPluginFactory() {}

    virtual QStringList availablePlugins() = 0;

    virtual QObject* createPlugin(const QString& iid) = 0;
};

/// An identifier to be used in IID metadata of filter plugins
#define QDaqFilterPlugin_iid "org.qdaq.filterplugin"
#define QDaqFilterPluginFactory_iid "org.qdaq.filterpluginfactory"

Q_DECLARE_INTERFACE(QDaqFilterPlugin, QDaqFilterPlugin_iid)

Q_DECLARE_INTERFACE(QDaqFilterPluginFactory, QDaqFilterPluginFactory_iid)

#endif // QDAQFILTERPLUGIN_H
