#include "core_script_interface.h"

#include <QScriptEngine>

#include "bytearrayclass.h"
#include "vectorclass.h"

#include <QColor>
#include <QPointF>

#include "QDaqJob.h"
#include "QDaqChannel.h"
#include "QDaqDataBuffer.h"
#include "QDaqDevice.h"

QScriptValue toScriptValue(QScriptEngine *engine, const QColor &clr)
{
    Q_UNUSED(engine);
    return QScriptValue(clr.name());
}

void fromScriptValue(const QScriptValue &obj, QColor &clr)
{
    clr.setNamedColor(obj.toString());

}

QScriptValue toScriptValue(QScriptEngine *engine, const QPointF &p)
{
    QScriptValue a = engine->newArray();
    a.setProperty(quint32(0), QScriptValue(p.x()));
    a.setProperty(quint32(1), QScriptValue(p.y()));
    return a;
}

void fromScriptValue(const QScriptValue &obj, QPointF &p)
{
    p.setX(obj.property(quint32(0)).toNumber());
    p.setY(obj.property(quint32(1)).toNumber());
}


template <class Container>
QScriptValue toScriptValueContainer(QScriptEngine *eng, const Container &cont)
{
    QScriptValue a = eng->newArray();
    typename Container::const_iterator begin = cont.begin();
    typename Container::const_iterator end = cont.end();
    typename Container::const_iterator it;
    for (it = begin; it != end; ++it)
        a.setProperty(quint32(it - begin), qScriptValueFromValue(eng, *it));
    return a;
}

template <class Container>
void fromScriptValueContainer(const QScriptValue &value, Container &cont)
{
    quint32 len = value.property("length").toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        typedef typename Container::value_type ContainerValue;
        cont.push_back(qscriptvalue_cast<ContainerValue>(item));
    }
}

QScriptValue toScriptValue(QScriptEngine *eng, QDaqObject * const &obj, int ownership)
{
    return eng->newQObject(obj, QScriptEngine::ValueOwnership(ownership),
                           QScriptEngine::ExcludeDeleteLater |
                           QScriptEngine::PreferExistingWrapperObject );
}

QScriptValue toScriptValue(QScriptEngine *eng, const QScriptValue& scriptObj, QDaqObject * const &obj, int ownership)
{
    return eng->newQObject(scriptObj, obj, QScriptEngine::ValueOwnership(ownership),
                           QScriptEngine::ExcludeDeleteLater |
                           QScriptEngine::PreferExistingWrapperObject );
}

void fromScriptValue(const QScriptValue &value, QDaqObject*& obj)
{
    obj = qobject_cast<QDaqObject*>(value.toQObject());
}

QScriptValue toScriptValue(QScriptEngine *eng, const QDaqObjectList& L)
{
    QScriptValue V = eng->newArray();
    QDaqObjectList::const_iterator begin = L.begin();
    QDaqObjectList::const_iterator end = L.end();
    QDaqObjectList::const_iterator it;
    for (it = begin; it != end; ++it)
        V.setProperty(quint32(it - begin), qScriptValueFromValue(eng, *it));
    return V;
}

void fromScriptValue(const QScriptValue &value, QDaqObjectList& L)
{
    if (value.isArray()) {
        quint32 len = value.property("length").toUInt32();
        for (quint32 i = 0; i < len; ++i) {
            QScriptValue item = value.property(i);
            L.push_back(qscriptvalue_cast<QDaqObject*>(item));
        }
        return;
    }

    if (value.isQObject()) {
        QDaqObject* obj = qobject_cast<QDaqObject*>(value.toQObject());
        if (obj) L.push_back(obj);
    }
}

typedef QDaqObject* QDaqObjectStar;


QScriptValue toScriptValueQDaqObjectStar(QScriptEngine *eng, const QDaqObjectStar& obj)
{
    return toScriptValue(eng,obj);
}

void fromScriptValueQDaqObjectStar(const QScriptValue &value, QDaqObjectStar &obj)
{
    fromScriptValue(value, obj);
}

QScriptValue scriptConstructor(QScriptContext *context, QScriptEngine *engine, const QMetaObject* metaObject)
{
    if (context->isCalledAsConstructor())
    {
        QString name;
        if (context->argumentCount()==1 &&
                context->argument(0).isString())
            name = context->argument(0).toString();
        if (name.isEmpty())
            return context->throwError(QScriptContext::SyntaxError,
                                "Give only a String by object creation");

        QDaqObject* obj = (QDaqObject*)(metaObject->newInstance(Q_ARG(QString,name)));
        if (!obj)
            return context->throwError(QString("%1(name=%2) could not be created").arg(metaObject->className()).arg(name));

        return toScriptValue(engine, obj);
    }
    else return context->throwError(QScriptContext::SyntaxError,
                                    QString("%1() called without'new'").arg(metaObject->className()));
}

int core_script_register_class(QScriptEngine* eng, const QMetaObject* metaObject)
{
    QScriptEngine::FunctionWithArgSignature cptr =
            reinterpret_cast<QScriptEngine::FunctionWithArgSignature>(scriptConstructor);
    QScriptValue ctor = eng->newFunction(cptr, (void *)metaObject);
    QScriptValue scriptClass = eng->newQMetaObject(metaObject, ctor);

    eng->globalObject().setProperty(metaObject->className(), scriptClass);

    return 1;
}

int core_script_interface_init(QScriptEngine *eng)
{
    // Register all types and conversions
    ByteArrayClass *byteArrayClass = new ByteArrayClass(eng);
    eng->globalObject().setProperty("ByteArray", byteArrayClass->constructor());

    VectorClass *vectorClass = new VectorClass(eng);
    eng->globalObject().setProperty("Vector", vectorClass->constructor());


    int ret = qScriptRegisterMetaType<QDaqObjectStar>(eng,toScriptValueQDaqObjectStar,fromScriptValueQDaqObjectStar) &
        qScriptRegisterMetaType<QDaqObjectList>(eng,::toScriptValue,::fromScriptValue) &
        qScriptRegisterMetaType<QColor>(eng,::toScriptValue,::fromScriptValue) &
        qScriptRegisterMetaType<QPointF>(eng,::toScriptValue,::fromScriptValue);


    ret &= core_script_register_class(eng, &QDaqObject::staticMetaObject);
    ret &= core_script_register_class(eng, &QDaqJob::staticMetaObject);
    ret &= core_script_register_class(eng, &QDaqLoop::staticMetaObject);
    ret &= core_script_register_class(eng, &QDaqChannel::staticMetaObject);
    ret &= core_script_register_class(eng, &QDaqDataBuffer::staticMetaObject);
    ret &= core_script_register_class(eng, &QDaqDevice::staticMetaObject);

    return ret;

}
