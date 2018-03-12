#ifndef _QDAQOBJECT_H_
#define _QDAQOBJECT_H_

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QSet>
#include <QMetaType>
#include <QScriptable>

#include "QDaqGlobal.h"

#include "os_utils.h"

namespace H5
{
class CommonFG;
class Group;
}

class QDaqRoot;
class QDaqObject;

/** A QList of QDaqObject pointers.
 *
 * @ingroup Core
 *
 */
typedef QList<QDaqObject*> QDaqObjectList;

/** Structure containing error data.
 *
 * @ingroup Core
 *
 * It is emitted every time an error occurs in a QDaqObject.
 * Errors are gathered centrally on a QDaqErrorQueue object
 * that lives in QDaqRoot.
 * The error queue is accesible through QDaqObject::root()->errorQueue().
 *
*/
struct RTLAB_BASE_EXPORT QDaqError
{
    /// Date/time of occurence
    QDateTime t;
    /// (Full)Name of object causing the error
    QString objectName;
    /// Error type description
    QString type;
    /// Extended description (optional)
    QString descr;

    QDaqError()
    {}

    QDaqError(const QString& aname, const QString& atype, const QString& adesc = QString(), const QDateTime& at = QDateTime::currentDateTime()) :
        t(at), objectName(aname), type(atype), descr(adesc)
    {}
    QDaqError(const QDaqError& e) :
        t(e.t), objectName(e.objectName), type(e.type), descr(e.descr)
    {}
    QString toString() const
    {
        return QString("%1\t%2\t%3\t%4").arg(t.toString()).arg(objectName).arg(type).arg(descr);
    }

};

/** Base class of all QDaq objects.
 *
 * @ingroup Core
 * @ingroup ScriptAPI
 *
 * QDaqObject is derived from QObject and thus inherits all the Qt machinery,
 * i.e., properties, meta-objects, signals \& slots, etc.
 *
 * QDaq objects are organized in a tree-like structure like QObjects. However,
 * the children of a QDaqObject are ordered ,whereas those of a QObject are not.
 * The ordering is necessary for QDaq
 * because QDaq objects take part in loops where the order of execution is essential.
 *
 * To create the QDaq object tree structure, the QDaqObject class offers an interface
 * similar to the Node interface of Document Object Model (DOM) Level 1. The interface is
 * defined by this \ref domapi "group of functions".
 *
 * Initially a QDaqObject is created without a parent
 * and only its name needs to be specified.
 * The object can than be incorporated into the QDaq hierarchy by one of the
 * \ref domapi "DOM API functions".
 *
 * There is a static single root object at the top of the tree structure.
 * The root object is accesible by QDaqObject::root() and it of type QDaqRoot.
 *
 * Descendants of QDaqObject can be serialized to
 * <a HREF="https://support.hdfgroup.org/HDF5/">HDF5 files.</a>
 * The complete tree of QDaq objects can be written/read by means of
 * writeH5() and readH5(), respectively. HDF5 serialization is done
 * according to the following rules:
 *   - Each object is written in the HDF5 file as a H5 group. The name of the
 * object becomes the name of the group.
 *   - Properties and data of the object are written as datasets of the group.
 *     QDaqObject descendants reimplement writeH5()/readH5() where necessary.
 *   - Child objects are written as sub-groups
 *
 */
class RTLAB_BASE_EXPORT QDaqObject : public QObject, protected QScriptable
{
	Q_OBJECT

protected:

	/// Push an error in the error queue
    void pushError(const QString& type, const QString& descr = QString()) const;

	/// Throw a script error with message msg
	void throwScriptError(const QString& msg) const;

    /** Check if name is a legal name for an QDaqObject.
    * Names should start with a letter and contain letters, numbers or the underscore _.
    * This function also checks if there are any sibbling objects with the same name.
	*/
	bool checkName(const QString& name) const;
	static bool isNameValid(const QString& name, int* code = 0);
	bool isNameUnique(const QString& name) const;


public:
    /// Write contents of the object to a H5 group
    virtual void writeH5(H5::Group* file) const;
    virtual void readH5(H5::Group *file);

    /// Serialize contents to a HDF5 file.
    static void h5write(const QDaqObject* obj, const QString& filename);
    /// Read object contents from HDF5 file.
    static QDaqObject* h5read(const QString& filename);

protected:
    // for handling children events
    virtual void childEvent ( QChildEvent * event );

public:
    /// A critical section for synching thread access to this object
    os::critical_section comm_lock;

protected:
    // the root object
    static QDaqRoot* root_;

public:
    /// Obtain a pointer to the one-and-only QDaqRoot object.
    static QDaqRoot* root() { return root_; }

protected:
    // this is a duplicate to QtObjects's list
    // so that we can handle child ordering stuff (insertBefore etc.)
    QDaqObjectList children_;

public:
    /** Construct a QDaqObject with a name.
     *
     * The name is actually the objectName property of the QObject super-class.
     *
     */
    Q_INVOKABLE explicit QDaqObject(const QString& name);
    virtual ~QDaqObject(void);

    /** Attach this QDaqObject to the QDaq tree.
     *
     * This function is called when the object becomes part of the QDaq tree.
     *
     * In the base class implementation this function signals the root object
     * that a new QDaqObject has been inserted in the tree and then
     * calls attach() for each child QDaqObject.
     *
     * Subclasses may reimplement this function to perform specific initialization.
     */
    virtual void attach();

    /** Detach this QDaqObject from the QDaq tree.
     *
     * This function is called just before the object becomes detached from the QDaq tree.
     *
     * In the base class implementation it first calls detach() for each child
     * QDaqObject and then signal root that the object is removed from the qdaq tree.
     *
     * Subclasses may reimplement this function to perform needed actions before detaching.
     *
     */
	virtual void detach();

    /// Returns true is this object is attached to the QDaq tree.
    bool isAttached() const;

    // helper function neede for building the tree string representation
	void objectTree(QString& S, int level) const;

    /** Register this QDaqObject's types with a QScriptEngine.
     *
     * This funtion is called when the QDaqObject is exposed to a script engine
     * and is used to register specific types needed by the object (enums etc).
     *
     */
	virtual void registerTypes(QScriptEngine* eng) { Q_UNUSED(eng); }

    /** Return the object's path in the QDaq tree.
     *
     * Example: qdaq.x.y.z.obj1
     * If the object is not attached to the tree, the function
     * returns the name of the object.
     *
     */
    QString path() const;

    static QDaqObject* findByName(const QString& name);
    static QDaqObjectList findByWildcard(const QString& wildcard, const QDaqObject* from = 0);

public slots:
    /** @addtogroup ScriptAPI
     *
     *  More documentation for the first group.
     *  @{
     */

    /// Print a backtrace of recent errors in this QDaqObject
	QString errorBacktrace() const;
	/// Print a string representation of the object
    // QString toString() const;
    /// Output in a string the object hierarchy beneath this object.
	QString objectTree() const
	{
		QString S;
		int ind = 0;
		objectTree(S,ind);
		return S;
	}
	/// List the objects properties
	QString listProperties() const;
	/// List the objects scriptable functions
	QString listFunctions() const;


    /**
     * \name DOM Level 1 Node interface
     * \anchor domapi
     *
     * This group of QDaqObject slots implement the Node interface
     * of DOM API Level 1.
     *
     * The functions are slots, thus available in script code.
     *
     */
    ///@{
    /// Return the object's parent or null if the object does not have a parent.
    QDaqObject* parent() const { return qobject_cast<QDaqObject*>(QObject::parent()); }
    /// Return a list of children of this object
    QDaqObjectList children() const { return children_; }
    /// Returns true if the object has children
    bool hasChildren() const { return !children_.isEmpty(); }
    /// Adds a new child QDaqObject, as the last child and returns a pointer to it.
    QDaqObject* appendChild(QDaqObject* obj);
    /// Adds a new child QDaqObject, before an existing child and returns a pointer to it.
    QDaqObject* insertBefore(QDaqObject* newobj, QDaqObject* existingobj);
    /// Clone a QDaqObject with its child objects.
    QDaqObject* clone();
    /// Remove a child and return a pointer to it.
    QDaqObject* removeChild(QDaqObject* obj);
    /// Replace a child and return a pointer to the old child.
    QDaqObject* replaceChild(QDaqObject* newobj, QDaqObject* oldobj);
    /// Find objects by a wildcard string wc
    QDaqObjectList find(const QString& wc) const
    {
        return QDaqObject::findByWildcard(wc,this);
    }
    ///@}

    /** @} */ // end of ScriptAPI

signals:
	/// Fired when object is deleted
    void objectDeleted(QDaqObject* obj);
	/// Fired when object is created
    void objectCreated(QDaqObject* obj);
	/// Fired when properties have changed
	void propertiesChanged();
    /// Fired when widgets need update
    void updateWidgets();
};

Q_DECLARE_METATYPE(QDaqError)
Q_DECLARE_METATYPE(QDaqObject*)
Q_DECLARE_METATYPE(QDaqObjectList)

int registerQDaqObjectStar(QScriptEngine* eng);

#define ERROR_QUEUE_DEPTH 1000

class QDaqErrorQueue : public QObject
{
    Q_OBJECT
    /// Queue of QDaq errors
    QList<QDaqError> queue_;
public:
    explicit QDaqErrorQueue(QObject *parent = 0);
    void push(const QDaqError& item);
    QList<QDaqError> errorQueue() const { return queue_; }
    const QDaqError& head() const { return queue_.first(); }
    QList<QDaqError> objectBackTrace(const QDaqObject* obj, int maxItems = 4) const;
signals:
    void errorAdded();
    void errorRemoved();
public slots:
};


#endif
