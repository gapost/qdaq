#include "QDaqTypes.h"
#include "QDaqObject.h"

#include <QScriptEngine>

#include "bytearrayclass.h"

#include <QColor>
#include <QPointF>

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
QScriptValue toScriptValue(QScriptEngine *eng, const Container &cont)
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
void fromScriptValue(const QScriptValue &value, Container &cont)
{
    quint32 len = value.property("length").toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        typedef typename Container::value_type ContainerValue;
        cont.push_back(qscriptvalue_cast<ContainerValue>(item));
    }
}

typedef QDaqObject* QDaqObjectStar;
typedef QObject* QObjectStar;

QScriptValue toScriptValue(QScriptEngine *eng, const QDaqObjectStar& obj)
{
    return eng->newQObject(obj,
                           QScriptEngine::AutoOwnership,
                           QScriptEngine::ExcludeDeleteLater |
                           QScriptEngine::AutoCreateDynamicProperties |
                           QScriptEngine::PreferExistingWrapperObject |
                           QScriptEngine::ExcludeChildObjects);
}

void fromScriptValue(const QScriptValue &value, QDaqObjectStar& obj)
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
    quint32 len = value.property("length").toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        L.push_back(qscriptvalue_cast<QDaqObject*>(item));
    }
}

int registerQDaqTypes(QScriptEngine* eng)
{
    ByteArrayClass *byteArrayClass = new ByteArrayClass(eng);
    eng->globalObject().setProperty("ByteArray", byteArrayClass->constructor());

    return qScriptRegisterMetaType<QDaqObjectStar>(eng,toScriptValue,fromScriptValue) &
        qScriptRegisterMetaType<QDaqObjectList>(eng,toScriptValue,fromScriptValue) &
        qScriptRegisterMetaType<QDaqIntVector>(eng,toScriptValue,fromScriptValue) &
        qScriptRegisterMetaType<QDaqUintVector>(eng,toScriptValue,fromScriptValue) &
        qScriptRegisterMetaType<QDaqVector>(eng,toScriptValue,fromScriptValue) &
        qScriptRegisterMetaType<QColor>(eng,toScriptValue,fromScriptValue) &
        qScriptRegisterMetaType<QPointF>(eng,toScriptValue,fromScriptValue);

}




