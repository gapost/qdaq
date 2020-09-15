#ifndef _QDAQTYPES_H_
#define _QDAQTYPES_H_

#include "QDaqGlobal.h"
#include "QDaqVector.h"

#include "math_util.h"

#include <QMetaType>
//#include <QScriptValue>

//class QScriptEngine;
class QDaqObject;

struct QDAQ_EXPORT QDaqTypes {
//    static int registerWithJS(QScriptEngine* eng);

    static bool isNumeric(const QVariant&);
    static bool isString(const QVariant&);
    static bool isBool(const QVariant&);

    static bool isStringList(const QVariant&);
    static bool isVector(const QVariant&);

    static QStringList toStringList(const QVariant&);
    static QDaqVector toVector(const QVariant&);

    static bool isNumeric(const QVariantList&);
    static bool isString(const QVariantList&);
    static bool isBool(const QVariantList&);

//    static QScriptValue toScriptValue(QScriptEngine *eng, QDaqObject* const  &obj, int ownership = 2);
//    static QScriptValue toScriptValue(QScriptEngine *eng, const QScriptValue& scriptObj, QDaqObject* const &obj, int ownership = 2);
//    static void fromScriptValue(const QScriptValue &value, QDaqObject* &obj);
};

#include <QString>
#include <QDateTime>

/** Time representation in QDaq.

  @ingroup Core
  @ingroup Types

  Time is represented internaly as a double number
  corresponding to seconds since 1 Jan 1970
  as returned by the POSIX ftime function.
  It is recorded with millisecond resolution.
  */

class QDAQ_EXPORT QDaqTimeValue
{
    double v_;

public:
    /// default constructor
    QDaqTimeValue() : v_(0.0)
    {}
    /// construct from double
    explicit QDaqTimeValue(double d) : v_(d)
    {}
    /// copy constructor
    QDaqTimeValue(const QDaqTimeValue& rhs) : v_(rhs.v_)
    {}

    /// copy operator
    QDaqTimeValue& operator=(const QDaqTimeValue& rhs)
    {
        v_ = rhs.v_;
        return *this;
    }

    /// type conversion operator to double
    operator double() { return v_; }

    /// convert to QDateTime
    operator QDateTime()
    {
        return QDateTime::fromMSecsSinceEpoch(1000*v_);
    }

    /// return current time in QDaqTimeValue format
    static QDaqTimeValue now()
    {
        return QDaqTimeValue(0.001*(QDateTime::currentMSecsSinceEpoch()));
    }

    /// convert to string
    QString toString()
    {
        QTime T = QDateTime(*this).time();
        return T.toString("hh:mm:ss.zzz");
    }
};

#endif

