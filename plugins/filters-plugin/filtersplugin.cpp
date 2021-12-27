#include "filtersplugin.h"

#include <QScriptEngine>

#include "QDaqScriptAPI.h"
#include "QDaqSession.h"

#include "qdaqfilters.h"

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
        // register QDaqFilter types with meta-object system
        QDaqFilters::registerMetaTypes();
        // If it is the root QDaq script engine, register the constructors
        QDaqScriptEngine* daqEngine = qobject_cast<QDaqScriptEngine*>(e->parent());
        if (daqEngine && daqEngine->type()==QDaqScriptEngine::RootEngine)
        {
            QDaqFilters::initScriptInterface(daqEngine);
        }
    }
    else {

         return;
     }
}


