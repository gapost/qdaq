#ifndef _QDAQBUFFERPROTO_H_
#define _QDAQBUFFERPROTO_H_

#include "QDaqTypes.h"

#include <QObject>
#include <QScriptable>

class RTLAB_BASE_EXPORT QDaqBufferProto : public QObject, public QScriptable
{
    Q_OBJECT
    Q_PROPERTY(int size READ size)
public:
    QDaqBufferProto(QObject *parent = 0);

    int size() const;

    QDaqBuffer* getObject() const;

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

