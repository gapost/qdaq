#include "qdaqfilters.h"

#include "QDaqRoot.h"
#include "QDaqSession.h"

#include "QDaqScriptAPI.h"
#include "qdaqfopdt.h"
#include "qdaqinterpolator.h"
#include "qdaqlinearcorrelator.h"
#include "qdaqpid.h"

QDaqFilters* QDaqFilters::filters_;

QDaqFilters::QDaqFilters(QObject *parent) : QObject(parent)
{
    filters_ = this;
    registerMetaTypes();
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
    QScriptEngine* e = s->scriptEngine();
    QDaqScriptAPI::registerClass(e, &QDaqFOPDT::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqInterpolator::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqLinearCorrelator::staticMetaObject);
    QDaqScriptAPI::registerClass(e, &QDaqPid::staticMetaObject);
}

void QDaqFilters::registerMetaTypes()
{
    qRegisterMetaType<QDaqFOPDT*>();
    qRegisterMetaType<QDaqInterpolator*>();
    qRegisterMetaType<QDaqLinearCorrelator*>();
    qRegisterMetaType<QDaqPid*>();
}
