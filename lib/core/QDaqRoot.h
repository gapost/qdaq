#ifndef _QDAQROOT_H_
#define _QDAQROOT_H_

#include "QDaqObject.h"

#include <QHash>
#include <QMetaObject>
#include <QStringList>
#include <QWidgetList>

class QDaqLogFile;
class QDaqIDE;
class QDaqSession;

/** QDaq root object class.
 *
 * At the top of the QDaq tree structure there is an object of type QDaqRoot.
 * It is a subclass of QDaqObject.
 *
 * The QDaq root is a single static object which can be accessed by QDaqObject::root().
 *
 * The QDaq root should be instantiated in the main function, after QApplication.
 *
 @code
 int main()
 {
   QApplication app;
   QDaqRoot qdaq;

   // do something

   return app.exec();
 }
 @endcode
 *
 * The name of QDaq root is "qdaq".
 *
 * A number of static items available to all QDaq objects are defined under QDaqRoot:
 *   - The error queue, available by errorQueue()
 *   - a list of all application windows can be obtained by calling daqWindows()
 *   - the IDE window can be obtained by ideWindow()
 *   - the root script session, rootSession()
 *
 * @ingroup Core
 *
 */
class RTLAB_BASE_EXPORT QDaqRoot : public QDaqObject
{
	Q_OBJECT

    /// The application root directory.
    Q_PROPERTY(QString rootDir READ rootDir)
    /// The directory where log files are written.
    Q_PROPERTY(QString logDir READ logDir)

protected:
    QString rootDir_, logDir_;
    QDaqLogFile* errorLog_;
    QDaqIDE* ideWindow_;
    QDaqSession* rootSession_;
    QDaqErrorQueue error_queue_;

public:
    QDaqRoot(void);
    virtual ~QDaqRoot(void);

    QString rootDir() const { return rootDir_; }
    QString logDir() const { return logDir_; }

    QString xml();

    /** Create a QDaqObject with name name and class className.
     *
     * Calls the constructor of className with name as the single argument.
     *
     * The class with name className must be previously be registered
     * with registerClass.
     *
     * Typically used by QDaqScriptEngine to create QDaq objects in script code.
     *
     */
    QDaqObject* createObject(const QString& name, const QString& className);

    /** Called to post an error at the QDaq error queue.
     *
     * This function is thread safe.
     *
     */
    void postError(const QDaqError& e) { emit error(e); }

    void addDaqWindow(QWidget* w);
    void removeDaqWindow(QWidget* w);

    /// Return a list of QDaq top level windows.
    QWidgetList daqWindows() const { return daqWindows_; }

    /// Return a pointer to the QDaq IDE window.
    QDaqIDE* ideWindow() { return ideWindow_; }

    /// Create the QDaq IDE window and return a pointer to it.
    QDaqIDE* createIdeWindow();

    /// Returns a pointer to the root script session.
    QDaqSession* rootSession() { return rootSession_; }

    /// Returns a pointer to the QDaq error queue.
    const QDaqErrorQueue* errorQueue() const { return &error_queue_; }


public slots:

    /// Return the names of registered object classes.
	QStringList classNames();

private slots:
    // connected (queued connection) to signal error()
    void onError(const QDaqError& err);

signals:
    // used internally by this class
    void error(const QDaqError& e);
    /// Fired when a top level window is opened or closed
    void daqWindowsChanged();
    /// Fired when object is attached
    void objectAttached(QDaqObject* obj);
    /// Fired when object is detached
    void objectDetached(QDaqObject* obj);

private:

    QWidgetList daqWindows_;

    typedef QHash<QString, const QMetaObject*> object_map_t;

    object_map_t object_map_;

public:
    /**
     * @brief Register a QDaqObject-derived class type
     * Registered classes can be created by createObject().
     * @param metaObj meta-object of the class
     */
	void registerClass(const QMetaObject* metaObj);
    /// Returns list of registered class metaObjects
	QList<const QMetaObject *> registeredClasses() const;

};




#endif


