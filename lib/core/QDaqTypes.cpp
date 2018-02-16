#include "QDaqTypes.h"

#include <QScriptEngine>

#include "bytearrayclass.h"

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

int registerVectorTypes(QScriptEngine* eng)
{
    ByteArrayClass *byteArrayClass = new ByteArrayClass(eng);
    eng->globalObject().setProperty("ByteArray", byteArrayClass->constructor());

    return qScriptRegisterMetaType<QDaqIntVector>(eng,toScriptValue,fromScriptValue) &
        qScriptRegisterMetaType<QDaqUintVector>(eng,toScriptValue,fromScriptValue) &
        qScriptRegisterMetaType<QDaqVector>(eng,toScriptValue,fromScriptValue);
}




