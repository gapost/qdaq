#ifndef VECTORCLASS_H
#define VECTORCLASS_H

#include "QDaqTypes.h"

#include <QtCore/QObject>
#include <QtScript/QScriptClass>
#include <QtScript/QScriptString>

QT_BEGIN_NAMESPACE
class QScriptContext;
QT_END_NAMESPACE

/*
 * Vector class definition from Qt examples
 */
class VectorClass : public QObject, public QScriptClass
{
    Q_OBJECT
public:
    VectorClass(QScriptEngine *engine);
    ~VectorClass();

    QScriptValue constructor();

    QScriptValue newInstance(int size = 0);
    QScriptValue newInstance(const QDaqVector &ba);

    QueryFlags queryProperty(const QScriptValue &object,
                             const QScriptString &name,
                             QueryFlags flags, uint *id);

    QScriptValue property(const QScriptValue &object,
                          const QScriptString &name, uint id);

    void setProperty(QScriptValue &object, const QScriptString &name,
                     uint id, const QScriptValue &value);

    QScriptValue::PropertyFlags propertyFlags(
        const QScriptValue &object, const QScriptString &name, uint id);

    QScriptClassPropertyIterator *newIterator(const QScriptValue &object);

    QString name() const;

    QScriptValue prototype() const;

private:
    static QScriptValue construct(QScriptContext *ctx, QScriptEngine *eng);

    static QScriptValue toScriptValue(QScriptEngine *eng, const QDaqVector &ba);
    static void fromScriptValue(const QScriptValue &obj, QDaqVector &ba);

    void resize(QDaqVector &ba, int newSize);

    QScriptString length, circular, capacity;
    QScriptValue proto;
    QScriptValue ctor;
};

#endif
