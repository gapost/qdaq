#ifndef QDAQFILTERPLUGIN_H
#define QDAQFILTERPLUGIN_H

#include <QtPlugin>

class QDaqFilterPlugin
{
public:
    virtual ~QDaqFilterPlugin() {}

    virtual QString errorMsg() = 0;

    virtual bool init() = 0;
    virtual bool operator()(const double* vin, double* vout) = 0;

    virtual int nInputChannels() const = 0;
    virtual int nOutputChannels() const = 0;
};

/// An identifier to be used in IID metadata of filter plugins
#define QDaqFilterPlugin_iid "org.qdaq.filterplugin"

Q_DECLARE_INTERFACE(QDaqFilterPlugin, QDaqFilterPlugin_iid)

#endif // QDAQFILTERPLUGIN_H
