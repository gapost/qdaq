#include "QDaqBufferProto.h"

//#include <QScriptValue>
#include <QScriptEngine>

QDaqBufferProto::QDaqBufferProto(QObject *parent)
    : QObject(parent)
{
}
int QDaqBufferProto::size() const
{
    QDaqBuffer* obj = getObject();
    if (obj) return obj->size();
    else return 0;
}
QDaqBuffer* QDaqBufferProto::getObject() const
{
    return qscriptvalue_cast<QDaqBuffer*>(thisObject());
}
double QDaqBufferProto::get(int i) const
{
    QDaqBuffer* obj = getObject();
    if (obj && i>=0 && i<obj->size())
        return obj->get(i);
    else return 0;
}
double QDaqBufferProto::mean() const
{
    QDaqBuffer* obj = getObject();
    if (obj)
        return obj->mean();
    else return 0;
}
double QDaqBufferProto::std() const
{
    QDaqBuffer* obj = getObject();
    if (obj)
        return obj->std();
    else return 0;
}
double QDaqBufferProto::vmax() const
{
    QDaqBuffer* obj = getObject();
    if (obj)
        return obj->vmax();
    else return 0;
}
double QDaqBufferProto::vmin() const
{
    QDaqBuffer* obj = getObject();
    if (obj)
        return obj->vmin();
    else return 0;
}
QDaqDoubleVector QDaqBufferProto::toArray() const
{
    QDaqBuffer* obj = getObject();
    if (obj)
        return QDaqDoubleVector(obj->vector());
    return QDaqDoubleVector();
}
void QDaqBufferProto::setup(QScriptEngine *eng)
{
    QDaqBufferProto *proto = new QDaqBufferProto();
    eng->setDefaultPrototype(qMetaTypeId<QDaqBuffer*>(),
        eng->newQObject(proto, QScriptEngine::ScriptOwnership,
                           QScriptEngine::ExcludeSuperClassContents));
}
