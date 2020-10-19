#ifndef _QDAQROOT_H_
#define _QDAQROOT_H_

#include "QDaqObject.h"

#include <QHash>
#include <QMetaObject>
#include <QStringList>


class QDaqLogFile;

class QDaqSession;

//struct QDaqPluginManager
//{
//    typedef QHash<QString, const QMetaObject*> object_map_t;

//    object_map_t object_map_;

//    QList<const QMetaObject *> pluginClasses() const;

//    void loadPlugins();
//    void loadFrom(const QString& path);
//};

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
class QDAQ_EXPORT QDaqRoot : public QDaqObject
{
	Q_OBJECT

    /// The application root directory.
    Q_PROPERTY(QString rootDir READ rootDir)
    /// The directory where log files are written.
    Q_PROPERTY(QString logDir READ logDir)

protected:
    QString rootDir_, logDir_;
    QDaqLogFile* errorLog_;
    QDaqSession* rootSession_;
    QDaqErrorQueue error_queue_;

public:
    QDaqRoot(void);
    virtual ~QDaqRoot(void);

    QString rootDir() const { return rootDir_; }
    QString logDir() const { return logDir_; }

    /** Called to post an error at the QDaq error queue.
     *
     * This function is thread safe.
     *
     */
    void postError(const QDaqError& e) { emit error(e); }


    /// Returns a pointer to the root script session.
    QDaqSession* rootSession() { return rootSession_; }

    /// Vreates a new script session.
    QDaqSession* newSession();

    /// Returns a pointer to the QDaq error queue.
    const QDaqErrorQueue* errorQueue() const { return &error_queue_; }


public slots:


private slots:
    // connected (queued connection) to signal error()
    void onError(const QDaqError& err);

signals:
    // used internally by this class
    void error(const QDaqError& e);
    /// Fired when object is attached
    void objectAttached(QDaqObject* obj);
    /// Fired when object is detached
    void objectDetached(QDaqObject* obj);
    /// Fired when a new session is created
    void newSession(QDaqSession* s);

private:

    //typedef QHash<QString, const QMetaObject*> object_map_t;

    //object_map_t object_map_;

    //QDaqPluginManager pluginManager;

public:


};






#endif


