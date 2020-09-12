#ifndef CORE_SCRIPT_INTERFACE_H
#define CORE_SCRIPT_INTERFACE_H

#include <QScriptValue>

class QScriptEngine;
class QDaqObject;

QScriptValue toScriptValue(QScriptEngine *eng, QDaqObject* const  &obj, int ownership = 2);
QScriptValue toScriptValue(QScriptEngine *eng, const QScriptValue& scriptObj, QDaqObject* const &obj, int ownership = 2);
void fromScriptValue(const QScriptValue &value, QDaqObject* &obj);

int core_script_interface_init(QScriptEngine *eng);

int core_script_register_class(QScriptEngine* eng, const QMetaObject* metaObject);

#endif // CORE_SCRIPT_INTERFACE_H
