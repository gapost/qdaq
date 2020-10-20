#ifndef QDAQSCRIPTAPI_H
#define QDAQSCRIPTAPI_H

#include <QObject>
#include <QScriptable>
#include <QScriptValue>
#include <QVariant>

class QScriptEngine;
class QDaqScriptEngine;
class QDaqObject;
class QElapsedTimer;
class QDaqSession;

class QDaqScriptAPI : public QObject, protected QScriptable
{
    Q_OBJECT
public:
    explicit QDaqScriptAPI(QObject *parent = 0);
    virtual ~QDaqScriptAPI();

    static QScriptValue toScriptValue(QScriptEngine *eng, QDaqObject* const  &obj, int ownership = 2);
    static void fromScriptValue(const QScriptValue &value, QDaqObject* &obj);
    static QVariant toVariant(QScriptEngine *eng, const QScriptValue &value);
    static int initAPI(QDaqScriptEngine *daqEngine);
    static int registerClass(QScriptEngine* eng, const QMetaObject* metaObject);

    void setSession(QDaqSession* s);
    QDaqSession* session() const { return session_; }
    QDaqScriptEngine* daqengine() const;

signals:
    void stdOut(const QString&);
    void stdErr(const QString&);
    void endSession();
    void abortWait();

public slots:
    QString version();
    void exit();
    QScriptValue exec(const QString& fname);
//    void print(const QString& str); // there is a native qt print() that causes errors
    void log(const QString& str);
    void wait(uint ms);
    void textSave(const QString& str, const QString& fname);
    QString textLoad(const QString& fname);

    void importExtension(const QString& name);
    QStringList availableExtensions();

    bool h5write(const QDaqObject* obj, const QString& fname);
    QDaqObject*  h5read(const QString& fname);

    // file and folder
    QString pwd();
    bool cd(const QString& path);
    QStringList dir(const QStringList& filters = QStringList());
    QStringList dir(const QString& filter);
    bool isDir(const QString& name);



    // set debugging on (enable Qt script debugger)
    void debug(bool on);

    // timing
    void tic();
    double toc();

    // system call
    QString system(const QString& comm);
    // check if it is win32 or linux
    bool ispc()
    {
#ifdef __linux__
        return false;
#else
        return true;
#endif
    }

    // scripting helpers
    QString info(QScriptValue v);

private:
    QElapsedTimer* stopWatch_;
    QDaqSession* session_;
};

#endif // QDAQSCRIPTAPI_H
