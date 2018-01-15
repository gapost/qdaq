#ifndef _RTROOT_H_
#define _RTROOT_H_

#include "QDaqObject.h"

#include <QHash>
#include <QMetaObject>
#include <QStringList>

class QScriptContext;
class QScriptEngine;


class RtMainWindow;

/** The QDaQ root object.

All QDaqObject are descendands of QDaqRoot.

QDaqRoot is a static object which can be accessed by QDaqObject::root().

\ingroup QDaq-Core

*/
class RTLAB_BASE_EXPORT QDaqRoot : public QDaqObject
{
	Q_OBJECT

public:
    QDaqRoot(void);
    virtual ~QDaqRoot(void);

    QString h5write(const QString& filename, const QString& comment);

    QString xml();

    /** Create a QDaqObject with specified name and class.
	  Use appendChild or insertBefore to get the new object in the
	  RtLab framework.
	*/
    QDaqObject* createObject(const QString& name, const QString& className);

    void postError(const QDaqError& e) { Q_UNUSED(e) }

public slots:

    /// Return the names of registered object classes.
	QStringList classNames();
    /// Serialize contents to a HDF5 file.
    void h5write(const QString& filename);
    /// Read object contents from HDF5 file.
    void h5read(const QString& filename);


private:

	/*typedef QScriptValue (*ScriptConstructorFunctionSignature)(QScriptContext *, QScriptEngine *, void *);

	struct MetaObjectData
	{
		const QMetaObject* metaObject;
		ScriptConstructorFunctionSignature constructor;
	};*/

    typedef QHash<QString, const QMetaObject*> object_map_t;

    object_map_t object_map_;

public:
    /**Register a QDaqObject class type.
	  This allows:
		- serialization of objects to h5, ...
		- creation of objects from QtScript with new operator
	  */
	void registerClass(const QMetaObject* metaObj);
	/// Returns list of registered metaObjects
	QList<const QMetaObject *> registeredClasses() const;

};




#endif


