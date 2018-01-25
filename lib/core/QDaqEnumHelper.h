#ifndef _RTENUMHELPER_H_
#define _RTENUMHELPER_H_

#include <QScriptEngine>
#include <QMetaType>
#include <QMetaEnum>

#define Q_SCRIPT_ENUM(T, QC) \
Q_DECLARE_METATYPE(QC::T) \
QScriptValue __toScript_##T(QScriptEngine *eng, const QC::T& p) \
{ \
	Q_UNUSED(eng); \
	int i = QC::staticMetaObject.indexOfEnumerator(#T); \
	QMetaEnum me = QC::staticMetaObject.enumerator(i); \
	return QScriptValue( me.valueToKey( (int)p ) ); \
} \
void __fromScript_##T(const QScriptValue &value, QC::T &p) \
{ \
	QByteArray str = value.toString().toLatin1(); \
	int i = QC::staticMetaObject.indexOfEnumerator(#T); \
	QMetaEnum me = QC::staticMetaObject.enumerator(i); \
	p = (QC::T)me.keyToValue(str.constData()); \
} \
int qScriptRegister##T(QScriptEngine* eng) \
{ \
	return qScriptRegisterMetaType<QC::T>(eng, \
		__toScript_##T, \
		__fromScript_##T \
		); \
}

#endif

