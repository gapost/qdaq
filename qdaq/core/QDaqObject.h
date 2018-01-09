#ifndef _RTOBJECT_H_
#define _RTOBJECT_H_

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QSet>
#include <QMetaType>
#include <QScriptable>

#include "QDaqGlobal.h"

#include "os_utils.h"

#define ERROR_QUEUE_DEPTH 5

namespace H5
{
class H5File;
class Group;
}

class QDaqRoot;
class QDaqObject;


typedef QList<QDaqObject*> QDaqObjectList;

/**
Structure containing error data.

Emitted every time an error occurs in an QDaqObject.
*/
struct RTLAB_BASE_EXPORT QDaqError
{
    /// Date/time of occurence
    QDateTime t;
    /// Name of object causing the error
    QString objectName;
    /// Error type description
    QString type;
    /// Extended description (optional)
    QString descr;


    QDaqError(const QDateTime& at, const QString& aname, const QString& atype) :
        t(at), objectName(aname), type(atype)
    {}
    QDaqError(const QDateTime& at, const QString& aname, const QString& atype, const QString& adesc) :
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


/** Base class of all RtLab objects.

\ingroup RtCore

*/
class RTLAB_BASE_EXPORT QDaqObject : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_PROPERTY(QString lastError READ lastError)

protected:
    /// Queue of last errors
    QList<QDaqError> error_queue;
	/// Push an error in the error queue
    void pushError(const QString& type, const QString& descr = QString());
    /// Get the last error that occured in this object
    QString lastError() const
    {
        return error_queue.isEmpty() ? QString() : error_queue.first().toString();
    }

	/// Throw a script error with message msg
	void throwScriptError(const QString& msg) const;

    /** Check if name is a legal name for an QDaqObject.
	Names should start with a letter and contain letters, numbers or the underscore _.
	This function also checks if there are any sibbling objects with the same name.
	*/
	bool checkName(const QString& name) const;
	static bool isNameValid(const QString& name, int* code = 0);
	bool isNameUnique(const QString& name) const;


public:
    /** Write the contents of the QDaqObject to a HDF5 file.
      In this base class implementation, properties are written as attributes.
      */
    virtual void writeH5(H5::H5File* file, QDaqObject* from, bool recursive = true);
    virtual bool readH5(H5::Group *file);

protected:
    // for handling children deletions
    virtual void childEvent ( QChildEvent * event );

public:
    /// A critical section for synching thread access to this object
    os::critical_section comm_lock;

protected:
    // the root object
    static QDaqRoot root_;

public:
    /// Obtain a pointer to the one-and-only QDaqRoot object.
    static QDaqRoot* root() { return &root_; }

protected:
    // this is a duplicate to QtObjects's list
    // so that we can handle child ordering stuff (insertBefore etc.)
    QDaqObjectList children_;

public:
    Q_INVOKABLE
    explicit QDaqObject(const QString& name);
    virtual ~QDaqObject(void);

	//virtual void attach();
	virtual void detach();

	void objectTree(QString& S, int level) const;

    /// Register this QDaqObject's types with a script engine.
	virtual void registerTypes(QScriptEngine* eng) { Q_UNUSED(eng); }

	/// Get the objects full name, e.g., dev.ifc1.obj1
	QString fullName() const;
    static QDaqObject* findByName(const QString& name);
    static QDaqObjectList findByWildcard(const QString& wildcard, QDaqObject* from = 0);

public slots:
	/// Print a backtrace of recent errors
	QString errorBacktrace() const;
	/// Print a string representation of the object
	//QString toString() const;
	/// Print the objects children hierarchy
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

    // Core DOM API 1. implementation
    /// Return the object's parent
    QDaqObject* parent() const { return qobject_cast<QDaqObject*>(QObject::parent()); }
    /// Return a list of children of this object
    QDaqObjectList children() const { return children_; }
    /// Returns true if the object has children
    bool hasChildren() const { return !children_.isEmpty(); }
    /// Adds a new child QDaqObject, as the last child.
    void appendChild(QDaqObject* obj);
    /// Adds a new child QDaqObject, as the last child.
    void insertBefore(QDaqObject* newobj, QDaqObject* existingobj);
    /// Clone a QDaqObject with its child objects.
    QDaqObject* clone();
    /// Remove a child and return a pointer to it.
    QDaqObject* removeChild(QDaqObject* obj);
    /// Replace a child and return a pointer to the old child.
    QDaqObject* replaceChild(QDaqObject* newobj, QDaqObject* oldobj);

signals:
	/// Fired when object is deleted
    void objectDeleted(QDaqObject* obj);
	/// Fired when object is created
    void objectCreated(QDaqObject* obj);
	/// Fired when properties have changed
	void propertiesChanged();
};

Q_DECLARE_METATYPE(QDaqObject*)
Q_DECLARE_METATYPE(QDaqObjectList)

int registerQDaqObjectStar(QScriptEngine* eng);


#endif
