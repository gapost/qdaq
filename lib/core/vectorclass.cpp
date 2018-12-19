#include <QtScript/QScriptClassPropertyIterator>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptEngine>
#include "vectorclass.h"
#include "vectorprototype.h"

#include <stdlib.h>

Q_DECLARE_METATYPE(QDaqVector*)
Q_DECLARE_METATYPE(VectorClass*)

class VectorClassPropertyIterator : public QScriptClassPropertyIterator
{
public:
    VectorClassPropertyIterator(const QScriptValue &object);
    ~VectorClassPropertyIterator();

    bool hasNext() const;
    void next();

    bool hasPrevious() const;
    void previous();

    void toFront();
    void toBack();

    QScriptString name() const;
    uint id() const;

private:
    int m_index;
    int m_last;
};

//! [0]
VectorClass::VectorClass(QScriptEngine *engine)
    : QObject(engine), QScriptClass(engine)
{
    qScriptRegisterMetaType<QDaqVector>(engine, toScriptValue, fromScriptValue);

    length = engine->toStringHandle(QLatin1String("length"));
    circular = engine->toStringHandle(QLatin1String("circular"));
    capacity = engine->toStringHandle(QLatin1String("capacity"));

    proto = engine->newQObject(new VectorPrototype(this),
                               QScriptEngine::QtOwnership,
    //                           QScriptEngine::SkipMethodsInEnumeration |
                               QScriptEngine::ExcludeSuperClassMethods |
                               QScriptEngine::ExcludeSuperClassProperties);
    QScriptValue global = engine->globalObject();
    proto.setPrototype(global.property("Object").property("prototype"));

    ctor = engine->newFunction(construct, proto);
    ctor.setData(engine->toScriptValue(this));
}
//! [0]

VectorClass::~VectorClass()
{
}

//! [3]
QScriptClass::QueryFlags VectorClass::queryProperty(const QScriptValue &object,
                                                       const QScriptString &name,
                                                       QueryFlags flags, uint *id)
{
    QDaqVector *ba = qscriptvalue_cast<QDaqVector*>(object.data());
    if (!ba)
        return 0;
    if (name == length ||
            name == circular ||
            name == capacity) {
        return flags;
    } else {
        bool isArrayIndex;
        qint32 pos = name.toArrayIndex(&isArrayIndex);
        if (!isArrayIndex)
            return 0;
        *id = pos;
        if ((flags & HandlesReadAccess) && (pos >= ba->size()))
            flags &= ~HandlesReadAccess;
        return flags;
    }
}
//! [3]

//! [4]
QScriptValue VectorClass::property(const QScriptValue &object,
                                      const QScriptString &name, uint id)
{
    QDaqVector *vec = qscriptvalue_cast<QDaqVector*>(object.data());
    if (!vec)
        return QScriptValue();
    if (name == length) {
        return vec->size();
    } else if (name == circular) {
        return vec->isCircular();
    } else if (name == capacity) {
        return vec->capacity();
    } else {
        qint32 pos = id;
        if ((pos < 0) || (pos >= vec->size()))
            return QScriptValue();
        return vec->get(pos);
    }
    return QScriptValue();
}
//! [4]

//! [5]
void VectorClass::setProperty(QScriptValue &object,
                                 const QScriptString &name,
                                 uint id, const QScriptValue &value)
{
    QDaqVector *vec = qscriptvalue_cast<QDaqVector*>(object.data());
    if (!vec)
        return;
    if (name == length) {
        // resize(*vec, value.toInt32());
    } else if (name == circular) {
        vec->setCircular(value.toBool());
    } else if (name == capacity) {
        vec->setCapacity(value.toInt32());
    } else {
        qint32 pos = id;
        if (pos < 0)
            return;
        if (vec->size() <= pos)
            resize(*vec, pos + 1);
        (*vec)[pos] = value.toNumber();
    }
}
//! [5]

//! [6]
QScriptValue::PropertyFlags VectorClass::propertyFlags(
    const QScriptValue &/*object*/, const QScriptString &name, uint /*id*/)
{
//    if (name == length  ||
//            name == circular ||
//            name == capacity) {
//        return QScriptValue::Undeletable
//            | QScriptValue::SkipInEnumeration;
//    }
    return QScriptValue::Undeletable;
}
//! [6]

//! [7]
QScriptClassPropertyIterator *VectorClass::newIterator(const QScriptValue &object)
{
    return new VectorClassPropertyIterator(object);
}
//! [7]

QString VectorClass::name() const
{
    return QLatin1String("Vector");
}

QScriptValue VectorClass::prototype() const
{
    return proto;
}

QScriptValue VectorClass::constructor()
{
    return ctor;
}

//! [10]
QScriptValue VectorClass::newInstance(int size)
{
    engine()->reportAdditionalMemoryCost(size);
    return newInstance(QDaqVector(size));
}
//! [10]

//! [1]
QScriptValue VectorClass::newInstance(const QDaqVector &ba)
{
    QScriptValue data = engine()->newVariant(QVariant::fromValue(ba));
    return engine()->newObject(this, data);
}
//! [1]

//! [2]
QScriptValue VectorClass::construct(QScriptContext *ctx, QScriptEngine *)
{
    VectorClass *cls = qscriptvalue_cast<VectorClass*>(ctx->callee().data());
    if (!cls)
        return QScriptValue();
    QScriptValue arg = ctx->argument(0);
    if (arg.instanceOf(ctx->callee()))
        return cls->newInstance(qscriptvalue_cast<QDaqVector>(arg));
    else if (arg.isArray()) {
        quint32 len = arg.property("length").toUInt32();
        QDaqVector vec((int)len);
        for (quint32 i = 0; i < len; ++i)
            vec << arg.property(i).toNumber();
        return cls->newInstance(vec);
    }
    else if (arg.isNumber()) {
        int size = arg.toInt32();
        return cls->newInstance(size);
    }
    return QScriptValue();
}
//! [2]

QScriptValue VectorClass::toScriptValue(QScriptEngine *eng, const QDaqVector &ba)
{
    QScriptValue ctor = eng->globalObject().property("Vector");
    VectorClass *cls = qscriptvalue_cast<VectorClass*>(ctor.data());
    if (!cls)
        return eng->newVariant(QVariant::fromValue(ba));
    return cls->newInstance(ba);
}

void VectorClass::fromScriptValue(const QScriptValue &obj, QDaqVector &ba)
{
    ba = qvariant_cast<QDaqVector>(obj.data().toVariant());
}

//! [9]
void VectorClass::resize(QDaqVector &ba, int newSize)
{
    int oldSize = ba.size();
    ba.setCapacity(newSize);
    if (newSize > oldSize)
        engine()->reportAdditionalMemoryCost(newSize - oldSize);
}
//! [9]



VectorClassPropertyIterator::VectorClassPropertyIterator(const QScriptValue &object)
    : QScriptClassPropertyIterator(object)
{
    toFront();
}

VectorClassPropertyIterator::~VectorClassPropertyIterator()
{
}

//! [8]
bool VectorClassPropertyIterator::hasNext() const
{
    QDaqVector *ba = qscriptvalue_cast<QDaqVector*>(object().data());
    return m_index < ba->size();
}

void VectorClassPropertyIterator::next()
{
    m_last = m_index;
    ++m_index;
}

bool VectorClassPropertyIterator::hasPrevious() const
{
    return (m_index > 0);
}

void VectorClassPropertyIterator::previous()
{
    --m_index;
    m_last = m_index;
}

void VectorClassPropertyIterator::toFront()
{
    m_index = 0;
    m_last = -1;
}

void VectorClassPropertyIterator::toBack()
{
    QDaqVector *ba = qscriptvalue_cast<QDaqVector*>(object().data());
    m_index = ba->size();
    m_last = -1;
}

QScriptString VectorClassPropertyIterator::name() const
{
    return object().engine()->toStringHandle(QString::number(m_last));
}

uint VectorClassPropertyIterator::id() const
{
    return m_last;
}
//! [8]
