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
 * The Vector javascript class represents an array of floating point numbers.
 * It is essentially a QtScript wrapper for the QDaqVector C++ class.
 * Thus, when Qt-slots of QDaq classes are called from QtScript code,
 * a QDaqVector passed as an argument or as return value is converted to
 * a JS object of class Vector.
 *
 * Elements of a Vector can be accesed by the [] operator and the "length"
 * property gives the number of elements.
 *
 * New elements are added at the end of the Vector with the function push().
 *
 * The property called "capacity" defines for how many
 * elements there is memory allocated.
 *
 * The "circular" property is a boolean value. If it is true then the Vector is a ring or
 * circular buffer with maximum size equal to its capacity. In a circular vector, when the capacity
 * has been reached, insertion of a new element causes deletion of the oldest element.
 *
 * A Vector can be created in QDaq scripts by the new operator
 * in 3 possible ways:
 @code{.js}
  var x1 = new Vector(10); // An empty Vector of capacity 10
  var x2 = new Vector([1,2,3]); // A Vector from a numeric Array
  var x3 = new Vector(x2); // x3 is a copy of x2
  x2[0] // returns 1
  x3.length // returns 3
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

    /// Push a number, a Vector or a numeric Array at the end
    void push(const QScriptValue& val);
    /// Remove the last element
    void pop();
    /// Resize to n elements keeping the first n
    void resize(int n);

    double min() const;
    double max() const;
    double mean() const;
    double std() const;

    QDaqVector clone() const { return thisVector()->clone(); }


    QString toString() const;

private:
    QDaqVector *thisVector() const;

    bool checkRange(int offset, int sz) const;
};
//! [0]


#endif
