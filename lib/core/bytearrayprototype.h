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
    int32_t readInt32LE(int offset = 0) const;
    int32_t readInt32BE(int offset = 0) const;
    uint32_t readUInt32LE(int offset = 0) const;
    uint32_t readUInt32BE(int offset = 0) const;
    int16_t readInt16LE(int offset = 0) const;
    int16_t readInt16BE(int offset = 0) const;
    uint16_t readUInt16LE(int offset = 0) const;
    uint16_t readUInt16BE(int offset = 0) const;
    int8_t readInt8(int offset = 0) const;

    void writeDoubleLE(double v, int offset = 0);
    void writeDoubleBE(double v, int offset = 0);
    void writeFloatLE(float v, int offset = 0);
    void writeFloatBE(float v, int offset = 0);
    void writeInt32LE(int32_t v, int offset = 0);
    void writeInt32BE(int32_t v, int offset = 0);
    void writeUInt32LE(uint32_t v, int offset = 0);
    void writeUInt32BE(uint32_t v, int offset = 0);
    void writeInt16LE(int16_t v, int offset = 0);
    void writeInt16BE(int16_t v, int offset = 0);
    void writeUInt16LE(uint16_t v, int offset = 0);
    void writeUInt16BE(uint16_t v, int offset = 0);
    void writeInt8(int8_t v, int offset = 0);

private:
    QByteArray *thisByteArray() const;

    bool checkRange(int offset, int sz) const;
};
//! [0]


#endif
