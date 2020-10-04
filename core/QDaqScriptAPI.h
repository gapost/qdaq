#ifndef QDAQSCRIPTAPI_H
#define QDAQSCRIPTAPI_H

#include <QObject>
#include <QScriptValue>
#include <QVariant>

class QScriptEngine;
class QDaqObject;

class QDaqScriptAPI : public QObject
{
    Q_OBJECT
public:
    explicit QDaqScriptAPI(QObject *parent = 0);

    static QScriptValue toScriptValue(QScriptEngine *eng, QDaqObject* const  &obj, int ownership = 2);
    //static QScriptValue toScriptValue(QScriptEngine *eng, const QScriptValue& scriptObj, QDaqObject* const &obj, int ownership = 2);
    static void fromScriptValue(const QScriptValue &value, QDaqObject* &obj);

    static QVariant toVariant(QScriptEngine *eng, const QScriptValue &value);

    static int initAPI(QScriptEngine *eng);

    static int registerClass(QScriptEngine* eng, const QMetaObject* metaObject);

signals:

public slots:
};

#endif // QDAQSCRIPTAPI_H
