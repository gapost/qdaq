#include "QDaqVector.h"
#include "QDaqObject.h"

#include <QScriptEngine>

#include "bytearrayclass.h"
#include "vectorclass.h"

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

QScriptValue QDaqTypes::toScriptValue(QScriptEngine *eng, QDaqObject * const &obj, int ownership)
{
    return eng->newQObject(obj, QScriptEngine::ValueOwnership(ownership),
                           QScriptEngine::ExcludeDeleteLater |
                           QScriptEngine::PreferExistingWrapperObject );
}

QScriptValue QDaqTypes::toScriptValue(QScriptEngine *eng, const QScriptValue& scriptObj, QDaqObject * const &obj, int ownership)
{
    return eng->newQObject(scriptObj, obj, QScriptEngine::ValueOwnership(ownership),
                           QScriptEngine::ExcludeDeleteLater |
                           QScriptEngine::PreferExistingWrapperObject );
}

void QDaqTypes::fromScriptValue(const QScriptValue &value, QDaqObject*& obj)
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
    return QDaqTypes::toScriptValue(eng,obj);
}

void fromScriptValueQDaqObjectStar(const QScriptValue &value, QDaqObjectStar &obj)
{
    QDaqTypes::fromScriptValue(value, obj);
}

bool QDaqTypes::isBool(const QVariant& v)
{
    return (int)v.type() == (int)QMetaType::Bool;
}

bool QDaqTypes::isNumeric(const QVariant& v)
{
    int id = v.type();
    return id==QMetaType::Int || id==QMetaType::UInt ||
            id==QMetaType::Long || id==QMetaType::ULong || id==QMetaType::LongLong ||
            id==QMetaType::ULongLong || id==QMetaType::Short || id==QMetaType::UShort ||
            id==QMetaType::Float || id==QMetaType::Double;
}

bool QDaqTypes::isString(const QVariant& v)
{
    int id = v.type();
    return id==QMetaType::QString || id==QMetaType::QByteArray ||
            id==QMetaType::QChar || id==QMetaType::Char || id==QMetaType::SChar ||
            id==QMetaType::UChar;
}

bool QDaqTypes::isStringList(const QVariant& v)
{
    int id = v.type();
    if (id == QMetaType::QStringList || id == QMetaType::QByteArrayList) return true;
    if (id == QMetaType::QVariantList && isString(v.value<QVariantList>())) return true;
    return false;
}

bool QDaqTypes::isVector(const QVariant& v)
{
    int id = v.userType();
    if (id == qMetaTypeId<QDaqVector>()) return true;
    if (id == QMetaType::QVariantList && isNumeric(v.value<QVariantList>())) return true;
    return false;
}

QDaqVector QDaqTypes::toVector(const QVariant & v)
{
    int id = v.userType();
    if (id == qMetaTypeId<QDaqVector>()) return v.value<QDaqVector>();
    if (id == QMetaType::QVariantList) {
        QVariantList vl = v.value<QVariantList>();
        if (isNumeric(vl)) {
            int n = vl.length();
            QDaqVector x(n);
            for(int i=0; i<n; ++i) x[i] = vl.at(i).toDouble();
            return x;
        }
    }
    return QDaqVector();
}

QStringList QDaqTypes::toStringList(const QVariant & v)
{
    QStringList L;
    int id = v.type();
    if (id==QMetaType::QStringList) return v.toStringList();
    if (id==QMetaType::QByteArrayList)
    {
        QByteArrayList balist = v.value<QByteArrayList>();
        foreach(QByteArray ba, balist) L << QString(ba);
        return L;
    }
    if (id==QMetaType::QVariantList) {
        QVariantList vl = v.value<QVariantList>();
        if (isString(vl)) {
            foreach(const QVariant& var, vl)
                L << var.toString();
            return L;
        }
    }
    return L;
}

bool QDaqTypes::isBool(const QVariantList& L)
{
    foreach(const QVariant& v, L) {
        if (!isBool(v)) return false;
    }
    return true;
}

bool QDaqTypes::isNumeric(const QVariantList& L)
{
    foreach(const QVariant& v, L) {
        if (!isNumeric(v)) return false;
    }
    return true;
}

bool QDaqTypes::isString(const QVariantList& L)
{
    foreach(const QVariant& v, L) {
        if (!isString(v)) return false;
    }
    return true;
}

int QDaqTypes::registerWithJS(QScriptEngine* eng)
{
    ByteArrayClass *byteArrayClass = new ByteArrayClass(eng);
    eng->globalObject().setProperty("ByteArray", byteArrayClass->constructor());

    VectorClass *vectorClass = new VectorClass(eng);
    eng->globalObject().setProperty("Vector", vectorClass->constructor());


    return qScriptRegisterMetaType<QDaqObjectStar>(eng,toScriptValueQDaqObjectStar,fromScriptValueQDaqObjectStar) &
        qScriptRegisterMetaType<QDaqObjectList>(eng,::toScriptValue,::fromScriptValue) &
        qScriptRegisterMetaType<QColor>(eng,::toScriptValue,::fromScriptValue) &
        qScriptRegisterMetaType<QPointF>(eng,::toScriptValue,::fromScriptValue);

}




