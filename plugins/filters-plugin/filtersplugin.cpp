#include "filtersplugin.h"

#include "core_script_interface.h"
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
        core_script_register_class(e, &QDaqFOPDT::staticMetaObject);
        core_script_register_class(e, &QDaqInterpolator::staticMetaObject);
        core_script_register_class(e, &QDaqLinearCorrelator::staticMetaObject);
        core_script_register_class(e, &QDaqPid::staticMetaObject);
    }
    else {

         return;
     }
}
