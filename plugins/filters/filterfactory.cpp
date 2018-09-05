#include "filterfactory.h"
#include "qdaqfopdt.h"
#include "qdaqinterpolator.h"
#include "qdaqpid.h"
#include "qdaqlinearcorrelator.h"


FilterFactory::FilterFactory() : QObject()
{
}

QList<const QMetaObject *> FilterFactory::pluginClasses() const
{
    QList<const QMetaObject *> lst;
    lst << &QDaqFOPDT::staticMetaObject;
    lst << &QDaqPid::staticMetaObject;
    lst << &QDaqInterpolator::staticMetaObject;
    lst << &QDaqLinearCorrelator::staticMetaObject;
    return lst;
}
