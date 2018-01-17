#include "QDaqTypes.h"

QDaqDoubleBufferPrototype::QDaqDoubleBufferPrototype(QObject *parent)
    : QObject(parent)
{
}
int QDaqDoubleBufferPrototype::size() const
{
    QDaqDoubleBuffer* obj = getObject();
    if (obj) return obj->size();
    else return 0;
}
QDaqDoubleBuffer* QDaqDoubleBufferPrototype::getObject() const
{
    return qscriptvalue_cast<QDaqDoubleBuffer*>(thisObject());
}
double QDaqDoubleBufferPrototype::get(int i) const
{
    QDaqDoubleBuffer* obj = getObject();
    if (obj && i>=0 && i<obj->size())
        return obj->get(i);
    else return 0;
}
double QDaqDoubleBufferPrototype::mean() const
{
    QDaqDoubleBuffer* obj = getObject();
    if (obj)
        return obj->mean();
    else return 0;
}
double QDaqDoubleBufferPrototype::std() const
{
    QDaqDoubleBuffer* obj = getObject();
    if (obj)
        return obj->std();
    else return 0;
}
double QDaqDoubleBufferPrototype::vmax() const
{
    QDaqDoubleBuffer* obj = getObject();
    if (obj)
        return obj->vmax();
    else return 0;
}
double QDaqDoubleBufferPrototype::vmin() const
{
    QDaqDoubleBuffer* obj = getObject();
    if (obj)
        return obj->vmin();
    else return 0;
}
RtDoubleVector QDaqDoubleBufferPrototype::toArray() const
{
    QDaqDoubleBuffer* obj = getObject();
    if (obj)
        return QDaqDoubleVector(obj->vector());
    return QDaqDoubleVector();
}
void QDaqDoubleBufferPrototype::setup(QScriptEngine *eng)
{
    QDaqDoubleBufferPrototype *proto = new QDaqDoubleBufferPrototype();
    eng->setDefaultPrototype(qMetaTypeId<QDaqDoubleBuffer*>(),
        eng->newQObject(proto, QScriptEngine::ScriptOwnership,
                           QScriptEngine::ExcludeSuperClassContents));
}
