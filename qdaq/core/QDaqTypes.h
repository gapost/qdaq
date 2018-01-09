#ifndef _RTTYPES_H_
#define _RTTYPES_H_

#include "QDaqGlobal.h"

#include <QVector>
#include <QMetaType>
#include <QScriptable>

typedef QVector<int> QDaqIntVector;
typedef QVector<unsigned int> QDaqUintVector;
typedef QVector<double> QDaqDoubleVector;

Q_DECLARE_METATYPE(QDaqIntVector)
Q_DECLARE_METATYPE(QDaqUintVector)
Q_DECLARE_METATYPE(QDaqDoubleVector)

class QScriptEngine;

int registerVectorTypes(QScriptEngine* eng);

#include <QString>
#include <QDateTime>

/** Time representation in RtLab.

  \ingroup RtCore

  Time is represented internaly as a double number
  corresponding to seconds since 1 Jan 1970
  as returned by the POSIX ftime function.
  It is recorded with millisecond resolution.
  */

class QDaqTimeValue
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

