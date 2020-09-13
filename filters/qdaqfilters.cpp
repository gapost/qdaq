#include "qdaqfilters.h"

#include "QDaqRoot.h"
#include "QDaqSession.h"

#include "core_script_interface.h"
#include "qdaqfopdt.h"
#include "qdaqinterpolator.h"
#include "qdaqlinearcorrelator.h"
#include "qdaqpid.h"

QDaqFilters* QDaqFilters::filters_;

QDaqFilters::QDaqFilters(QObject *parent) : QObject(parent)
{
    filters_ = this;

    initScriptInterface( QDaqObject::root()->rootSession() );

    connect(QDaqObject::root(),SIGNAL(newSession(QDaqSession*)),
            this,SLOT(onNewSession(QDaqSession*)));
}

void QDaqFilters::onNewSession(QDaqSession *s)
{
    initScriptInterface(s);
}

void QDaqFilters::initScriptInterface(QDaqSession *s)
{
    QScriptEngine* e = s->getEngine();
    core_script_register_class(e, &QDaqFOPDT::staticMetaObject);
    core_script_register_class(e, &QDaqInterpolator::staticMetaObject);
    core_script_register_class(e, &QDaqLinearCorrelator::staticMetaObject);
    core_script_register_class(e, &QDaqPid::staticMetaObject);
}
