#ifndef BYTEARRAYPROTOTYPE_H
#define BYTEARRAYPROTOTYPE_H

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtScript/QScriptable>
#include <QtScript/QScriptValue>


class ByteArrayPrototype : public QObject, public QScriptable
{
Q_OBJECT
public:
    ByteArrayPrototype(QObject *parent = 0);
    ~ByteArrayPrototype();

public slots:
    void chop(int n);
    bool equals(const QByteArray &other);
    QByteArray left(int len) const;
    QByteArray mid(int pos, int len = -1) const;
    QScriptValue remove(int pos, int len);
    QByteArray right(int len) const;
    QByteArray simplified() const;
    QByteArray toBase64() const;
    QByteArray toLower() const;
    QByteArray toUpper() const;
    QByteArray trimmed() const;
    void truncate(int pos);
    QString toLatin1String() const;
    QScriptValue valueOf() const;

    double readDoubleLE(int offset = 0) const;
    double readDoubleBE(int offset = 0) const;
    float readFloatLE(int offset = 0) const;
    float readFloatBE(int offset = 0) const;
    int readInt32LE(int offset = 0) const;
    int readInt32BE(int offset = 0) const;
    uint readUInt32LE(int offset = 0) const;
    uint readUInt32BE(int offset = 0) const;
    int readInt16LE(int offset = 0) const;
    int readInt16BE(int offset = 0) const;
    uint readUInt16LE(int offset = 0) const;
    uint readUInt16BE(int offset = 0) const;
    int readInt8(int offset = 0) const;

    void writeDoubleLE(double v, int offset = 0);
    void writeDoubleBE(double v, int offset = 0);
    void writeFloatLE(float v, int offset = 0);
    void writeFloatBE(float v, int offset = 0);
    void writeInt32LE(int v, int offset = 0);
    void writeInt32BE(int v, int offset = 0);
    void writeUInt32LE(uint v, int offset = 0);
    void writeUInt32BE(uint v, int offset = 0);
    void writeInt16LE(int v, int offset = 0);
    void writeInt16BE(int v, int offset = 0);
    void writeUInt16LE(uint v, int offset = 0);
    void writeUInt16BE(uint v, int offset = 0);
    void writeInt8(int v, int offset = 0);

private:
    QByteArray *thisByteArray() const;

    bool checkRange(int offset, int sz) const;
};
//! [0]


#endif
