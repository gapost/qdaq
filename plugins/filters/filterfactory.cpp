#include "filterfactory.h"
#include "qdaqfopdt.h"
#include "qdaqinterpolator.h"
#include "qdaqpid.h"
#include "qdaqlinearcorrelator.h"

template<typename T>
QString getID()
{
    T obj;
    return obj.iid();
}

template<typename T>
QObject* createPluginHelper(const QString& iid)
{
    T* obj = new T();
    if (obj->iid()==iid) return obj;
    else {
        delete obj;
        return 0;
    }
}

FilterFactory::FilterFactory() : QDaqFilterPluginFactory()
{
}

QStringList FilterFactory::availablePlugins()
{
    QStringList lst;
    lst << getID<QDaqFOPDT>();
    lst << getID<QDaqPid>();
    lst << getID<QDaqInterpolator>();
    lst << getID<QDaqLinearCorrelator>();
    return lst;
}

QObject* FilterFactory::createPlugin(const QString &iid)
{
    QObject* filter;

    filter = createPluginHelper<QDaqFOPDT>(iid);
    if (filter) return filter;
    filter = createPluginHelper<QDaqPid>(iid);
    if (filter) return filter;
    filter = createPluginHelper<QDaqInterpolator>(iid);
    if (filter) return filter;
    filter = createPluginHelper<QDaqLinearCorrelator>(iid);
    if (filter) return filter;

    return 0;
}
