#include "QDaqVector.h"
#include "QDaqObject.h"

#include <QScriptEngine>

#include "bytearrayclass.h"
#include "vectorclass.h"

#include <QColor>
#include <QPointF>



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






