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

template<class T>
class QDaqBufferImpl
{
public:
    enum StorageType { Open, Fixed, Circular };

    typedef QVector<T> container_t;

private:
    typedef QDaqBufferImpl<T> _Self;

    /// memory buffer
    container_t buffer;
    /// vector size
    int sz;
    /// vector capacity
    int cp;
    /// vector type
    StorageType type_;
    /// pointer to next position for circular vectors
    int tail;
    /// min & max values
    T x1, x2;
    /// flag set if min & max need recalc
    bool recalcBounds;

    void normalize_()
    {
        if (type_==Circular && sz!=0 && sz==cp && tail>0)
        {
            QVector<T> temp(sz);
            const T* head = buffer.data() + tail;
            int sz1 = sz-tail;
            memcpy(temp.data(), head, sz1*sizeof(T));
            memcpy(temp.data()+sz1, buffer.data(), (sz-sz1)*sizeof(T));
            memcpy(buffer.data(), temp.data(), sz*sizeof(T));
            tail = 0;
        }
    }

    int idx_(int i) const
    {
        return (type_==Circular && sz==cp) ? ((tail+i) % sz) : i;
    }

    void set_(int i, const T& v)
    {
        buffer[i] = v;
    }

    void calcBounds_()
    {
        int n(size());
        if (n>0)
        {
            x1 = x2 = get(0);
            for(int i=1; i<n; ++i)
            {
                double v = get(i);
                if (v<x1) x1 = v;
                if (v>x2) x2 = v;
            }
        }
        else x1 = x2 = 0.;
        recalcBounds = false;
    }

public:
    explicit QDaqBufferImpl(quint32 acap = 0) : buffer((int)acap),
        sz(0), cp(acap), type_(Fixed), tail(0),
        x1(0), x2(0), recalcBounds(true)
    {
    }
    QDaqBufferImpl(const _Self& rhs) : buffer(rhs.buffer),
        sz(rhs.sz), cp(rhs.cp), type_(rhs.type_), tail(rhs.tail),
        x1(rhs.x1), x2(rhs.x2), recalcBounds(rhs.recalcBounds)
    {
    }
    ~QDaqBufferImpl(void)
    {
    }

    _Self& operator=(const _Self& rhs)
    {
        buffer = rhs.buffer;
        sz = rhs.sz;
        cp = rhs.cp;
        type_ = rhs.type_;
        tail = rhs.tail;
        x1 = rhs.x1;
        x2 = rhs.x2;
        recalcBounds = rhs.recalcBounds;
        return (*this);
    }

    StorageType type() const { return type_; }
    void setType(StorageType newt) { if (newt!=type_){ normalize_(); type_ = newt; } }
    int capacity() const { return (int)cp; }
    int size() const { return (int)sz; }

    void setCapacity(int c)
    {
        if (c==cp) return;

        normalize_();
        buffer.resize(c);
        switch (type_)
        {
        case Fixed:
        case Open:
            if (sz>c) sz = c;
            recalcBounds = true;
            break;
        case Circular:
            if (c>cp)
            {
                if (sz==cp) tail = sz;
            }
            else // (c<cp)
            {
                if (sz>c)
                {
                    sz = c; tail = 0;
                    recalcBounds = true;
                }
            }
            break;
        }
        cp = c;
    }
    void clear()
    {
        sz = 0;
        tail = 0;
        recalcBounds = true;
    }

    const T& get(int i) const
    {
        return buffer[idx_(i)];
    }
    const T& operator[](int i) const
    {
        return get(i);
    }
    void push(const T& v)
    {
        recalcBounds = true;
        switch (type_)
        {
        case Open:
            if (sz==cp) buffer.resize(++cp);
            set_(sz++,v);
            break;
        case Fixed:
            if (sz<cp) set_(sz++,v);
            break;
        case Circular:
            set_(tail++,v);
            if (sz<cp) sz++;
            if (sz==cp) tail %= sz;
            break;
        }
    }
    _Self& operator<<(const T& v)
    {
        push(v); return (*this);
    }
    const T* data() const
    {
        const_cast< _Self * >( this )->normalize_();
        return buffer.constData();
    }
    container_t vector() const
    {
        const_cast< _Self * >( this )->normalize_();
        return buffer.mid(0,sz);
    }

    double vmin() const
    {
        if (recalcBounds)
            const_cast< _Self * >( this )->calcBounds_();
        return x1;
    }
    double vmax() const
    {
        if (recalcBounds)
            const_cast< _Self * >( this )->calcBounds_();
        return x2;
    }
    double mean() const
    {
        double s(0.0);
        int n(size());
        for(int i=0; i<n; ++i) s += get(i);
        return s/n;
    }
    double std() const
    {
        double s1(0.0), s2(0.0);
        int n(size());
        for(int i=0; i<n; ++i) {
            double v = get(i);
            s1 += v; s2 += v*v;
        }
        s1 /= n;
        s2 /= n;
        s1 = s2 - s1*s1;
        if (s1<=0.0) return 0.0;
        else return sqrt(s1);
    }
};

typedef QDaqBufferImpl<double> QDaqBuffer;

Q_DECLARE_METATYPE(QDaqBuffer*)

#endif

