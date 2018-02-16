#include "bytearrayprototype.h"
#include <QtScript/QScriptEngine>

Q_DECLARE_METATYPE(QByteArray*)

ByteArrayPrototype::ByteArrayPrototype(QObject *parent)
    : QObject(parent)
{
}

ByteArrayPrototype::~ByteArrayPrototype()
{
}

//! [0]
QByteArray *ByteArrayPrototype::thisByteArray() const
{
    return qscriptvalue_cast<QByteArray*>(thisObject().data());
}
//! [0]

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

//! [1]
QByteArray ByteArrayPrototype::mid(int pos, int len) const
{
    return thisByteArray()->mid(pos, len);
}

QScriptValue ByteArrayPrototype::remove(int pos, int len)
{
    thisByteArray()->remove(pos, len);
    return thisObject();
}
//! [1]

QByteArray ByteArrayPrototype::right(int len) const
{
    return thisByteArray()->right(len);
}

QByteArray ByteArrayPrototype::simplified() const
{
    return thisByteArray()->simplified();
}

QByteArray ByteArrayPrototype::toBase64() const
{
    return thisByteArray()->toBase64();
}

QByteArray ByteArrayPrototype::toLower() const
{
    return thisByteArray()->toLower();
}

QByteArray ByteArrayPrototype::toUpper() const
{
    return thisByteArray()->toUpper();
}

QByteArray ByteArrayPrototype::trimmed() const
{
    return thisByteArray()->trimmed();
}

void ByteArrayPrototype::truncate(int pos)
{
    thisByteArray()->truncate(pos);
}

QString ByteArrayPrototype::toLatin1String() const
{
    return QString::fromLatin1(*thisByteArray());
}

//! [2]
QScriptValue ByteArrayPrototype::valueOf() const
{
    return thisObject().data();
}
//! [2]

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
int32_t ByteArrayPrototype::readInt32LE(int offset) const
{
    if (!checkRange(offset,sizeof(int32_t))) return 0;
    return readLE<int32_t>(thisByteArray()->constData(),offset);
}
int32_t ByteArrayPrototype::readInt32BE(int offset) const
{
    if (!checkRange(offset,sizeof(int32_t))) return 0;
    return readBE<int32_t>(thisByteArray()->constData(),offset);
}
void ByteArrayPrototype::writeInt32LE(int32_t v, int offset)
{
    if (!checkRange(offset,sizeof(int32_t))) return;
    writeLE(v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeInt32BE(int32_t v, int offset)
{
    if (!checkRange(offset,sizeof(int32_t))) return;
    writeBE(v,thisByteArray()->data(),offset);
}
// UInt32
uint32_t ByteArrayPrototype::readUInt32LE(int offset) const
{
    if (!checkRange(offset,sizeof(uint32_t))) return 0;
    return readLE<uint32_t>(thisByteArray()->constData(),offset);
}
uint32_t ByteArrayPrototype::readUInt32BE(int offset) const
{
    if (!checkRange(offset,sizeof(uint32_t))) return 0;
    return readBE<uint32_t>(thisByteArray()->constData(),offset);
}
void ByteArrayPrototype::writeUInt32LE(uint32_t v, int offset)
{
    if (!checkRange(offset,sizeof(uint32_t))) return;
    writeLE(v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeUInt32BE(uint32_t v, int offset)
{
    if (!checkRange(offset,sizeof(uint32_t))) return;
    writeBE(v,thisByteArray()->data(),offset);
}
// Int 16
int16_t ByteArrayPrototype::readInt16LE(int offset) const
{
    if (!checkRange(offset,sizeof(int16_t))) return 0;
    return readLE<int16_t>(thisByteArray()->constData(),offset);
}
int16_t ByteArrayPrototype::readInt16BE(int offset) const
{
    if (!checkRange(offset,sizeof(int16_t))) return 0;
    return readBE<int16_t>(thisByteArray()->constData(),offset);
}
void ByteArrayPrototype::writeInt16LE(int16_t v, int offset)
{
    if (!checkRange(offset,sizeof(int16_t))) return;
    writeLE(v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeInt16BE(int16_t v, int offset)
{
    if (!checkRange(offset,sizeof(int16_t))) return;
    writeBE(v,thisByteArray()->data(),offset);
}
// UInt16
uint16_t ByteArrayPrototype::readUInt16LE(int offset) const
{
    if (!checkRange(offset,sizeof(uint16_t))) return 0;
    return readLE<uint16_t>(thisByteArray()->constData(),offset);
}
uint16_t ByteArrayPrototype::readUInt16BE(int offset) const
{
    if (!checkRange(offset,sizeof(uint16_t))) return 0;
    return readBE<uint16_t>(thisByteArray()->constData(),offset);
}
void ByteArrayPrototype::writeUInt16LE(uint16_t v, int offset)
{
    if (!checkRange(offset,sizeof(uint16_t))) return;
    writeLE(v,thisByteArray()->data(),offset);
}
void ByteArrayPrototype::writeUInt16BE(uint16_t v, int offset)
{
    if (!checkRange(offset,sizeof(uint16_t))) return;
    writeBE(v,thisByteArray()->data(),offset);
}
// Int 8
int8_t ByteArrayPrototype::readInt8(int offset) const
{
    if (!checkRange(offset,sizeof(int8_t))) return 0;
    return thisByteArray()->constData()[offset];
}
void ByteArrayPrototype::writeInt8(int8_t v, int offset)
{
    if (!checkRange(offset,sizeof(int8_t))) return;
    thisByteArray()->data()[offset] = v;
}
