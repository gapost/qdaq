#ifndef _RTROOT_H_
#define _RTROOT_H_

#include "QDaqObject.h"

#include <QHash>
#include <QMetaObject>
#include <QStringList>

class QScriptContext;
class QScriptEngine;
class QDaqLogFile;


/** The QDaq root object.

All QDaqObject are descendands of QDaqRoot.

QDaqRoot is a static object which can be accessed by QDaqObject::root().

\ingroup QDaq-Core

*/
class RTLAB_BASE_EXPORT QDaqRoot : public QDaqObject
{
	Q_OBJECT

    Q_PROPERTY(QString rootDir READ rootDir)
    Q_PROPERTY(QString logDir READ logDir)

protected:
    QString rootDir_, logDir_;
    QDaqLogFile* errorLog_;

public:
    QDaqRoot(void);
    virtual ~QDaqRoot(void);

    QString rootDir() const { return rootDir_; }
    QString logDir() const { return logDir_; }

    QString h5write(const QString& filename, const QString& comment);

    QString xml();

    /** Create a QDaqObject with specified name and class.
	  Use appendChild or insertBefore to get the new object in the
	  RtLab framework.
	*/
    QDaqObject* createObject(const QString& name, const QString& className);

    void objectCreation(QDaqObject* obj, bool c);

    void postError(const QDaqError& e) { emit error(e); }

public slots:

    /// Return the names of registered object classes.
	QStringList classNames();
    /// Serialize contents to a HDF5 file.
    void h5write(const QString& filename);
    /// Read object contents from HDF5 file.
    void h5read(const QString& filename);

private slots:
    void onError(const QDaqError& err);

signals:
    void error(const QDaqError& e);


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


