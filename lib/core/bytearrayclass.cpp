#include <QtScript/QScriptClassPropertyIterator>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptEngine>
#include "bytearrayclass.h"
#include "bytearrayprototype.h"

#include <stdlib.h>

Q_DECLARE_METATYPE(QByteArray*)
Q_DECLARE_METATYPE(ByteArrayClass*)

class ByteArrayClassPropertyIterator : public QScriptClassPropertyIterator
{
public:
    ByteArrayClassPropertyIterator(const QScriptValue &object);
    ~ByteArrayClassPropertyIterator();

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
ByteArrayClass::ByteArrayClass(QScriptEngine *engine)
    : QObject(engine), QScriptClass(engine)
{
    qScriptRegisterMetaType<QByteArray>(engine, toScriptValue, fromScriptValue);

    length = engine->toStringHandle(QLatin1String("length"));

    proto = engine->newQObject(new ByteArrayPrototype(this),
                               QScriptEngine::QtOwnership,
                               QScriptEngine::SkipMethodsInEnumeration |
                               QScriptEngine::ExcludeSuperClassMethods |
                               QScriptEngine::ExcludeSuperClassProperties);
    QScriptValue global = engine->globalObject();
    proto.setPrototype(global.property("Object").property("prototype"));

    ctor = engine->newFunction(construct, proto);
    ctor.setData(engine->toScriptValue(this));
}
//! [0]

ByteArrayClass::~ByteArrayClass()
{
}

//! [3]
QScriptClass::QueryFlags ByteArrayClass::queryProperty(const QScriptValue &object,
                                                       const QScriptString &name,
                                                       QueryFlags flags, uint *id)
{
    QByteArray *ba = qscriptvalue_cast<QByteArray*>(object.data());
    if (!ba)
        return 0;
    if (name == length) {
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
QScriptValue ByteArrayClass::property(const QScriptValue &object,
                                      const QScriptString &name, uint id)
{
    QByteArray *ba = qscriptvalue_cast<QByteArray*>(object.data());
    if (!ba)
        return QScriptValue();
    if (name == length) {
        return ba->length();
    } else {
        qint32 pos = id;
        if ((pos < 0) || (pos >= ba->size()))
            return QScriptValue();
        return uint(ba->at(pos)) & 255;
    }
    return QScriptValue();
}
//! [4]

//! [5]
void ByteArrayClass::setProperty(QScriptValue &object,
                                 const QScriptString &name,
                                 uint id, const QScriptValue &value)
{
    QByteArray *ba = qscriptvalue_cast<QByteArray*>(object.data());
    if (!ba)
        return;
    if (name == length) {
        resize(*ba, value.toInt32());
    } else {
        qint32 pos = id;
        if (pos < 0)
            return;
        if (ba->size() <= pos)
            resize(*ba, pos + 1);
        (*ba)[pos] = char(value.toInt32());
    }
}
//! [5]

//! [6]
QScriptValue::PropertyFlags ByteArrayClass::propertyFlags(
    const QScriptValue &/*object*/, const QScriptString &name, uint /*id*/)
{
    if (name == length) {
        return QScriptValue::Undeletable
            | QScriptValue::SkipInEnumeration;
    }
    return QScriptValue::Undeletable;
}
//! [6]

//! [7]
QScriptClassPropertyIterator *ByteArrayClass::newIterator(const QScriptValue &object)
{
    return new ByteArrayClassPropertyIterator(object);
}
//! [7]

QString ByteArrayClass::name() const
{
    return QLatin1String("ByteArray");
}

QScriptValue ByteArrayClass::prototype() const
{
    return proto;
}

QScriptValue ByteArrayClass::constructor()
{
    return ctor;
}

//! [10]
QScriptValue ByteArrayClass::newInstance(int size)
{
    engine()->reportAdditionalMemoryCost(size);
    return newInstance(QByteArray(size, /*ch=*/0));
}
//! [10]

//! [1]
QScriptValue ByteArrayClass::newInstance(const QByteArray &ba)
{
    QScriptValue data = engine()->newVariant(QVariant::fromValue(ba));
    return engine()->newObject(this, data);
}
//! [1]

//! [2]
QScriptValue ByteArrayClass::construct(QScriptContext *ctx, QScriptEngine *)
{
    ByteArrayClass *cls = qscriptvalue_cast<ByteArrayClass*>(ctx->callee().data());
    if (!cls)
        return QScriptValue();
    QScriptValue arg = ctx->argument(0);
    if (arg.instanceOf(ctx->callee()))
        return cls->newInstance(qscriptvalue_cast<QByteArray>(arg));
    else if (arg.isArray()) {
        quint32 len = arg.property("length").toUInt32();
        QByteArray ba((int)len,char(0));
        for (quint32 i = 0; i < len; ++i) {
            uint v = arg.property(i).toUInt32() & 255;
            ba[i] = char(v);
        }
        return cls->newInstance(ba);
    }
    else if (arg.isString()) {
        QString s = arg.toString();
        QByteArray ba = s.toLatin1();
        return cls->newInstance(ba);
    }
    else if (arg.isNumber()) {
        int size = arg.toInt32();
        return cls->newInstance(size);
    }
    return QScriptValue();
}
//! [2]

QScriptValue ByteArrayClass::toScriptValue(QScriptEngine *eng, const QByteArray &ba)
{
    QScriptValue ctor = eng->globalObject().property("ByteArray");
    ByteArrayClass *cls = qscriptvalue_cast<ByteArrayClass*>(ctor.data());
    if (!cls)
        return eng->newVariant(QVariant::fromValue(ba));
    return cls->newInstance(ba);
}

void ByteArrayClass::fromScriptValue(const QScriptValue &obj, QByteArray &ba)
{
    ba = qvariant_cast<QByteArray>(obj.data().toVariant());
}

//! [9]
void ByteArrayClass::resize(QByteArray &ba, int newSize)
{
    int oldSize = ba.size();
    ba.resize(newSize);
    if (newSize > oldSize)
        engine()->reportAdditionalMemoryCost(newSize - oldSize);
}
//! [9]



ByteArrayClassPropertyIterator::ByteArrayClassPropertyIterator(const QScriptValue &object)
    : QScriptClassPropertyIterator(object)
{
    toFront();
}

ByteArrayClassPropertyIterator::~ByteArrayClassPropertyIterator()
{
}

//! [8]
bool ByteArrayClassPropertyIterator::hasNext() const
{
    QByteArray *ba = qscriptvalue_cast<QByteArray*>(object().data());
    return m_index < ba->size();
}

void ByteArrayClassPropertyIterator::next()
{
    m_last = m_index;
    ++m_index;
}

bool ByteArrayClassPropertyIterator::hasPrevious() const
{
    return (m_index > 0);
}

void ByteArrayClassPropertyIterator::previous()
{
    --m_index;
    m_last = m_index;
}

void ByteArrayClassPropertyIterator::toFront()
{
    m_index = 0;
    m_last = -1;
}

void ByteArrayClassPropertyIterator::toBack()
{
    QByteArray *ba = qscriptvalue_cast<QByteArray*>(object().data());
    m_index = ba->size();
    m_last = -1;
}

QScriptString ByteArrayClassPropertyIterator::name() const
{
    return object().engine()->toStringHandle(QString::number(m_last));
}

uint ByteArrayClassPropertyIterator::id() const
{
    return m_last;
}
//! [8]
