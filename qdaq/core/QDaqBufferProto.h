#ifndef _QDAQBUFFERPROTO_H_
#define _QDAQBUFFERPROTO_H_

#include "QDaqTypes.h"

typedef QDaqBuffer<double> QDaqDoubleBuffer;

Q_DECLARE_METATYPE(QDaqDoubleBuffer*)

class RTLAB_BASE_EXPORT QDaqDoubleBufferPrototype : public QObject, public QScriptable
{
    Q_OBJECT
    Q_PROPERTY(int size READ size)
public:
    QDaqDoubleBufferPrototype(QObject *parent = 0);

    int size() const;

    QDaqDoubleBuffer* getObject() const;

    static void setup(QScriptEngine* eng);

public slots:
    //QString toString() const;
    //QString toStringList(QChar sep = ' ') const;
    QDaqDoubleVector toArray() const;
    double get(int i) const;
    double vmin() const;
    double vmax() const;
    double mean() const;
    double std() const;
};

#endif

