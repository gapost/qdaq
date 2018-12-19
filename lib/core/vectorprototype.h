#ifndef VECTORPROTOTYPE_H
#define VECTORPROTOTYPE_H

#include "QDaqTypes.h"
#include <QtCore/QObject>
#include <QtScript/QScriptable>
#include <QtScript/QScriptValue>

/**
 * @brief The prototype for the Vector class.
 * @ingroup ScriptAPI
 *
 * The Vector javascript class represents an vector of floating point numbers.
 * It is essentially a QtScript wrapper for the QDaqVector C++ class.
 * Thus, when Qt-slots of QDaq classes are called from QtScript code,
 * a QDaqVector passed as an argument or as return value is converted to
 * a JS object of Vector class.
 *
 * Elements of a Vector can be accesed by the [] operator and the "length"
 * property gives the number of elements.
 *
 * A Vector can be created in QDaq scripts by the new operator
 * in 3 possible ways:
 @code{.js}
  var b1 = new Vector(10); // An empty Vector of capacity 10
  var b2 = new Vector([1,2,3]); // A Vector from a numeric Array
  var b3 = new Vector(b2); // b3 is a copy of b2
  b2[0] // returns 1
  b3.length // returns 3
 @endcode
 *
 * The VectorPrototype class defines a number of usefull functions
 * for handling
 * Vector objects in script code.
 *
 */
class VectorPrototype : public QObject, public QScriptable
{
    Q_OBJECT


public:
    VectorPrototype(QObject *parent = 0);
    ~VectorPrototype();

public slots:

    /// Returns true if this Vector is equal to other.
    bool equals(const QDaqVector &other);

    /// Return the data stored by the object
    QScriptValue valueOf() const;

    QScriptValue toArray() const;

    void clear();

    /// Push a vector at the end
    void push(const QScriptValue& val);

    double min() const;
    double max() const;
    double mean() const;
    double std() const;


    QString toString() const;

private:
    QDaqVector *thisVector() const;

    bool checkRange(int offset, int sz) const;
};
//! [0]


#endif
