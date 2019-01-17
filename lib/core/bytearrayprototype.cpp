#include "bytearrayprototype.h"
#include <QtScript/QScriptEngine>

#include "QDaqTypes.h"

Q_DECLARE_METATYPE(QByteArray*)

ByteArrayPrototype::ByteArrayPrototype(QObject *parent)
    : QObject(parent)
{
}

ByteArrayPrototype::~ByteArrayPrototype()
{
}

QByteArray *ByteArrayPrototype::thisByteArray() const
{
    return qscriptvalue_cast<QByteArray*>(thisObject().data());
}

void ByteArrayPrototype::chop(int n)
{
    thisByteArray()->chop(n);
}

bool ByteArrayPrototype::equals(const QByteArray &other)
{
    return *thisByteArray() == other;
}

QByteArray ByteArrayPrototype::left(int len) const
{
    return thisByteArray()->left(len);
}


QByteArray ByteArrayPrototype::mid(int pos, int len) const
{
    return thisByteArray()->mid(pos, len);
}

QScriptValue ByteArrayPrototype::remove(int pos, int len)
{
    thisByteArray()->remove(pos, len);
    return thisObject();
}


QByteArray ByteArrayPrototype::right(int len) const
{
    return thisByteArray()->right(len);
}

void ByteArrayPrototype::truncate(int pos)
{
    thisByteArray()->truncate(pos);
}

QString ByteArrayPrototype::toLatin1String() const
{
    return QString::fromLatin1(*thisByteArray());
}


QScriptValue ByteArrayPrototype::valueOf() const
{
    return thisObject().data();
}

// Type write/read
template<class T>
T readLE(const char* p, int offset)
{
    return *(reinterpret_cast<const T*>(p + offset));
}
template<class T>
T readBE(const char* p, int offset)
{
    union {
        T v;
        char c[sizeof(T)];
    } V;
    const char* q = p + offset + sizeof(T) - 1;
    char* d = V.c;
    while (q>=p) *d++ = *q--;
    return V.v;
}
template<class T>
void writeLE(const T& v, char* p, int offset)
{
    memcpy(p+offset,&v,sizeof(T));
}
template<class T>
void writeBE(const T& v, char* p, int offset)
{
    union {
        T v;
        char c[sizeof(T)];
    } V;
    V.v = v;
    char* q = p + offset + sizeof(T) - 1;
    char* d = V.c;
    while (q>=p) *q-- = *d++;
}

bool ByteArrayPrototype::checkRange(int offset, int sz) const
{
    int len = thisByteArray()->length();
    if (offset<0 || len<sz || offset>len-sz)
    {
        context()->throwError(QScriptContext::RangeError,tr("Index out of range"));
        return false;
    } else return true;
}

double ByteArrayPrototype::readDoubleBE(int offset) const
{
    if (!checkRange(offset,sizeof(double))) return 0;
    return readBE<double>(thisByteArray()->constData(),offset);
}
double ByteArrayPrototype::readDoubleLE(int offset) const
{
    if (!checkRange(offset,sizeof(double))) return 0;
    return readLE<double>(thisByteArray()->constData(),offset);
}
float ByteArrayPrototype::readFloatBE(int offset) const
{
    if (!checkRange(offset,sizeof(float))) return 0;
    return readBE<float>(thisByteArray()->constData(),offset);
}
float ByteArrayPrototype::readFloatLE(int offset) const
{
    if (!checkRange(offset,sizeof(float))) return 0;
    return readLE<float>(thisByteArray()->constData(),offset);
}

void ByteArrayPrototype::writeDoubleBE(double v, int offset)
{
    if (!checkRange(offset,sizeof(double))) return;
    writeBE(v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeDoubleLE(double v, int offset)
{
    if (!checkRange(offset,sizeof(double))) return;
    writeLE(v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeFloatBE(float v, int offset)
{
    if (!checkRange(offset,sizeof(float))) return;
    writeBE(v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeFloatLE(float v, int offset)
{
    if (!checkRange(offset,sizeof(float))) return;
    writeLE(v,thisByteArray()->data(),offset);
}
// Int 32
int ByteArrayPrototype::readInt32LE(int offset) const
{
    if (!checkRange(offset,sizeof(int32_t))) return 0;
    return readLE<int32_t>(thisByteArray()->constData(),offset);
}
int ByteArrayPrototype::readInt32BE(int offset) const
{
    if (!checkRange(offset,sizeof(int32_t))) return 0;
    return readBE<int32_t>(thisByteArray()->constData(),offset);
}
void ByteArrayPrototype::writeInt32LE(int v, int offset)
{
    if (!checkRange(offset,sizeof(int32_t))) return;
    writeLE((int32_t)v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeInt32BE(int v, int offset)
{
    if (!checkRange(offset,sizeof(int32_t))) return;
    writeBE((int32_t)v,thisByteArray()->data(),offset);
}
// UInt32
uint ByteArrayPrototype::readUInt32LE(int offset) const
{
    if (!checkRange(offset,sizeof(uint32_t))) return 0;
    return readLE<uint32_t>(thisByteArray()->constData(),offset);
}
uint ByteArrayPrototype::readUInt32BE(int offset) const
{
    if (!checkRange(offset,sizeof(uint32_t))) return 0;
    return readBE<uint32_t>(thisByteArray()->constData(),offset);
}
void ByteArrayPrototype::writeUInt32LE(uint v, int offset)
{
    if (!checkRange(offset,sizeof(uint32_t))) return;
    writeLE((uint32_t)v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeUInt32BE(uint v, int offset)
{
    if (!checkRange(offset,sizeof(uint32_t))) return;
    writeBE((uint32_t)v,thisByteArray()->data(),offset);
}
// Int 16
int ByteArrayPrototype::readInt16LE(int offset) const
{
    if (!checkRange(offset,sizeof(int16_t))) return 0;
    return readLE<int16_t>(thisByteArray()->constData(),offset);
}
int ByteArrayPrototype::readInt16BE(int offset) const
{
    if (!checkRange(offset,sizeof(int16_t))) return 0;
    return readBE<int16_t>(thisByteArray()->constData(),offset);
}
void ByteArrayPrototype::writeInt16LE(int v, int offset)
{
    if (!checkRange(offset,sizeof(int16_t))) return;
    writeLE((int16_t)(v & 0xFFFF),thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeInt16BE(int v, int offset)
{
    if (!checkRange(offset,sizeof(int16_t))) return;
    writeBE((int16_t)(v & 0xFFFF),thisByteArray()->data(),offset);
}
// UInt16
uint ByteArrayPrototype::readUInt16LE(int offset) const
{
    if (!checkRange(offset,sizeof(uint16_t))) return 0;
    return readLE<uint16_t>(thisByteArray()->constData(),offset);
}
uint ByteArrayPrototype::readUInt16BE(int offset) const
{
    if (!checkRange(offset,sizeof(uint16_t))) return 0;
    return readBE<uint16_t>(thisByteArray()->constData(),offset);
}
void ByteArrayPrototype::writeUInt16LE(uint v, int offset)
{
    if (!checkRange(offset,sizeof(uint16_t))) return;
    writeLE((uint16_t)(v & 0xFFFF),thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeUInt16BE(uint v, int offset)
{
    if (!checkRange(offset,sizeof(uint16_t))) return;
    writeBE((uint16_t)(v & 0xFFFF),thisByteArray()->data(),offset);
}
// Int 8
int ByteArrayPrototype::readInt8(int offset) const
{
    if (!checkRange(offset,sizeof(int8_t))) return 0;
    return thisByteArray()->constData()[offset];
}
void ByteArrayPrototype::writeInt8(int v, int offset)
{
    if (!checkRange(offset,sizeof(int8_t))) return;
    thisByteArray()->data()[offset] = (char)(v & 0xFF);
}
