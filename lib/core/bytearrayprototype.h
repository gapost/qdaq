#ifndef BYTEARRAYPROTOTYPE_H
#define BYTEARRAYPROTOTYPE_H

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtScript/QScriptable>
#include <QtScript/QScriptValue>

/**
 * @brief The prototype for the ByteArray class.
 * @ingroup ScriptAPI
 *
 * The ByteArray javascript class represents an array of bytes.
 * It is essentially a QtScript wrapper for the QByteArray C++ class.
 * Thus, when Qt-slots of QDaq classes are called from QtScript code,
 * a QByteArray passed as an argument or as return value is converted to
 * a JS object of ByteArray class.
 *
 * Elements of a ByteArray can be accesed by the [] operator and the "length"
 * property gives the number of elements.
 *
 * A ByteArray can be created in QDaq scripts by the new operator
 * in 3 possible ways:
 @code{.js}
  var b1 = new ByteArray(10); // An empty ByteArray of length 10
  var b2 = new ByteArray("abcdef"); // A ByteArray from a string
  var b3 = new ByteArray(b2); // b3 is a copy of b2
  b2[3] // returns ASCII code of character 'c'
  b3.length // returns 6
 @endcode
 *
 * The ByteArrayPrototype class defines a number of usefull functions
 * for handling
 * ByteArray objects in script code.
 *
 */
class ByteArrayPrototype : public QObject, public QScriptable
{
Q_OBJECT
public:
    ByteArrayPrototype(QObject *parent = 0);
    ~ByteArrayPrototype();

public slots:
    /// Removes n bytes from the end of the byte array.
    void chop(int n);
    /// Truncates the array at pos.
    void truncate(int pos);
    /// Returns true if this ByteArray is equal to other.
    bool equals(const QByteArray &other);
    /// Returns a ByteArray with the len leftmost bytes.
    QByteArray left(int len) const;
    /// Returns the middle part of the array from pos with length len.
    QByteArray mid(int pos, int len = -1) const;
    /// Removes len bytes starting at pos
    QScriptValue remove(int pos, int len);    
    /// Returns the rightmost len bytes
    QByteArray right(int len) const;

    /// Convert to a String
    QString toLatin1String() const;
    /// Return the data stored by the object
    QScriptValue valueOf() const;


    /**
     * @name Functions for reading numbers in binary form out of a ByteArray.
     *
     * Similar to functions in
     * <a href="https://nodejs.org/api/buffer.html#buffer_buf_readdoublebe_offset_noassert">
     * node.js Buffer class.</a>
     *
     * It is assumed that the number to be read
     * is written in binary representation in the ByteArray. The functions
     * access the buffer bytes and convert to the corresponding numeric
     * value.
     *
     * The functions support double (64bit), float(32bit),
     * int (32bit, 16bit, signed and unsigned) and the 8bit
     * signed int. The unsigned 8bit value is the native storage type
     * of the class and can be read with the [] operator.
     *
     * Little- and big-endian number formats (LE or BE in the function name)
     * are supported.
     *
     * An offset in bytes may be passed as an argument. It defines
     * the index of the byte where the stored number starts.
     * If not given it defaults to 0.
     *
     * If the read operation exeeds the buffer length a RangeError
     * is thrown in the QScriptEngine.
     *
     */
    ///@{
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
    ///@}


    /**
     * @name Functions for writing numbers in binary format into the ByteArray
     *
     * Similar to functions in
     * <a href="https://nodejs.org/api/buffer.html#buffer_buf_writedoublele_value_offset_noassert">
     * node.js Buffer class.</a>
     *
     * The number passed as a 1st argument is written
     * in binary representation in the ByteArray.
     *
     * The functions support double (64bit), float(32bit),
     * int (32bit, 16bit, signed and unsigned) and the 8bit
     * signed int. The unsigned 8bit value is the native storage type
     * of the class and can be written using the [] operator.
     *
     * Little- and big-endian number formats (LE or BE in the function name)
     * are supported.
     *
     * An offset in bytes may be given as an argument. It defines
     * the index of the starting byte where the number is stored.
     *
     * If the write operation exeeds the buffer length a RangeError
     * is thrown in the QScriptEngine.
     *
     */
    ///@{
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
    ///@}

private:
    QByteArray *thisByteArray() const;

    bool checkRange(int offset, int sz) const;
};
//! [0]


#endif
