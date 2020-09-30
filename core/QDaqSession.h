#ifndef _RTSESSION_H_
#define _RTSESSION_H_

#include "QDaqGlobal.h"

#include "QDaqTypes.h"

#include <QObject>
#include <QScriptValue>
#include <QScriptProgram>
#include <QStringList>
#include <QSet>
#include <QElapsedTimer>


class QScriptEngine;
class QScriptEngineDebugger;
class QScriptContext;
class QTimer;
class QDaqLogFile;
class QDaqObject;


class QDAQ_EXPORT QDaqScriptEngine : public QObject
{    
    Q_OBJECT

protected:
	QScriptEngine* engine_;

public:
    explicit QDaqScriptEngine(QObject* parent);

    QScriptEngine* getEngine() const { return engine_; }

	bool canEvaluate(const QString&);
	bool isEvaluating() const;
    bool evaluate(const QScriptProgram& program, QString& ret, QDaqObject* thisObj = 0);
    bool evaluate(const QString& program, QString &ret, QDaqObject* thisObj = 0);
	void abortEvaluation();

public slots:
    void sleep(int ms);

private:
	static QScriptValue scriptConstructor(QScriptContext *context, QScriptEngine *engine, const QMetaObject *metaObj);

};

class QDAQ_EXPORT QDaqSession : public QDaqScriptEngine
{
    Q_OBJECT

protected:
    QScriptEngineDebugger* debugger_;
    //QTimer* wait_timer_;
    //bool wait_aborted_;
    QElapsedTimer watch_;

    // console index = 0 - ROOT,1,2,3,...
    int idx_;
    // set contains actice index values
    static QSet<int> idx_set;

    QDaqLogFile* logFile_;

private:

    friend class QDaqRoot;

    QDaqSession(QObject* parent = 0);

public:

    virtual ~QDaqSession(void);
    void evaluate(const QString& program);
    void abortEvaluation();
    int index() const { return idx_; }
    static int nextAvailableIndex();

private slots:

public slots:
    QString version();
    void quit();
    void exec(const QString& fname);
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
    void tic() { watch_.start(); }
    double toc() { return 0.000001*watch_.nsecsElapsed(); }

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

protected slots:
    void log_in(const QString& str)  { log__(0,str); }
    void log_out(const QString& str) { log__(1,str); }
    void log_err(const QString& str) { log__(2,str); }

protected:
    void log__(int fd, const QString& str);

signals:
    void stdOut(const QString&);
    void stdErr(const QString&);
    void endSession();
    void abortWait();

};



#endif


