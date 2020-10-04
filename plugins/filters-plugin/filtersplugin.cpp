#include "filtersplugin.h"

#include "QDaqScriptAPI.h"
#include "qdaqfopdt.h"
#include "qdaqinterpolator.h"
#include "qdaqlinearcorrelator.h"
#include "qdaqpid.h"

QDaqFiltersPlugin::QDaqFiltersPlugin(QObject *parent) :
    QScriptExtensionPlugin(parent)
{
}

QStringList QDaqFiltersPlugin::keys() const
{
    QStringList lst;
    lst << "qdaq-filters";
    lst << "qdaq";
    return lst;
}

void QDaqFiltersPlugin::initialize(const QString &key, QScriptEngine *e)
{
    if (key=="qdaq") {}
    else if (key=="qdaq-filters") {
        // init
        QDaqScriptAPI::registerClass(e, &QDaqFOPDT::staticMetaObject);
        QDaqScriptAPI::registerClass(e, &QDaqInterpolator::staticMetaObject);
        QDaqScriptAPI::registerClass(e, &QDaqLinearCorrelator::staticMetaObject);
        QDaqScriptAPI::registerClass(e, &QDaqPid::staticMetaObject);
    }
    else {

         return;
     }
}
