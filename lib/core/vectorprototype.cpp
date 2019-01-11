#include "vectorprototype.h"
#include <QtScript/QScriptEngine>

Q_DECLARE_METATYPE(QDaqVector*)

VectorPrototype::VectorPrototype(QObject *parent)
    : QObject(parent)
{
}

VectorPrototype::~VectorPrototype()
{
}

QDaqVector *VectorPrototype::thisVector() const
{
    QDaqVector * V = qscriptvalue_cast<QDaqVector*>(thisObject().data());
    Q_ASSERT(V);
    return V;
}

bool VectorPrototype::equals(const QDaqVector &other)
{
    return *thisVector() == other;
}

QString VectorPrototype::toString() const
{
    return QString("[object Vector]");
}

void VectorPrototype::push(const QScriptValue &val)
{
    QDaqVector* V = thisVector();
    if (V->isCircular() && V->capacity()==0) {
        context()->throwError(QScriptContext::RangeError,"Pushing to a null capacity circular vector.");
        return;
    }
    if (val.isNumber())
        thisVector()->push(val.toNumber());
    else if (val.isArray()) {
        quint32 n = val.property("length").toUInt32();
        QDaqVector* vec = thisVector();
        for(quint32 i=0; i<n; i++) {
            QScriptValue vi = val.property(i);
            if (vi.isNumber()) vec->push(vi.toNumber());
            else break;
        }
    }
    else if (val.instanceOf(engine()->globalObject().property("Vector"))) {
        QDaqVector * vec = thisVector();
        QDaqVector * rhs = qscriptvalue_cast<QDaqVector*>(val.data());
        if (rhs) vec->push(*rhs);
    }
}

void VectorPrototype::pop()
{
    thisVector()->pop();
}

void VectorPrototype::resize(int n)
{
    thisVector()->resize(n);
}

QScriptValue VectorPrototype::toArray() const
{
    QDaqVector* vec = thisVector();
    QScriptValue v = engine()->newArray(vec->size());
    for(int i=0; i<vec->size(); i++)
        v.setProperty(quint32(i),qScriptValueFromValue(engine(),vec->get(i)));
    return v;
}

void VectorPrototype::clear()
{
    thisVector()->clear();
}

double VectorPrototype::min() const
{
    return thisVector()->vmin();
}
double VectorPrototype::max() const
{
    return thisVector()->vmax();
}
double VectorPrototype::mean() const
{
    return thisVector()->mean();
}
double VectorPrototype::std() const
{
    return thisVector()->std();
}

QScriptValue VectorPrototype::valueOf() const
{
    return thisObject().data();
}

bool VectorPrototype::checkRange(int offset, int sz) const
{
    int len = thisVector()->size();
    if (offset<0 || len<sz || offset>len-sz)
    {
        context()->throwError(QScriptContext::RangeError,tr("Index out of range"));
        return false;
    } else return true;
}


