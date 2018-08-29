#ifndef _RTSESSION_H_
#define _RTSESSION_H_

#include "QDaqGlobal.h"

#include "QDaqTypes.h"

#include "os_utils.h"

#include <QObject>
#include <QScriptValue>
#include <QScriptProgram>
#include <QStringList>
#include <QSet>

class QScriptEngine;
class QScriptEngineDebugger;
class QScriptContext;
class QTimer;
class QDaqLogFile;
class QDaqObject;
class QDaqChannel;
//class QDaqBuffer;
class QComboBox;
class QListWidget;

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
    QTimer* wait_timer_;
    bool wait_aborted_;
    os::stopwatch watch_;

    // console index = 0 - ROOT,1,2,3,...
    int idx_;
    // set contains actice index values
    static QSet<int> idx_set;

    QDaqLogFile* logFile_;

public:
    QDaqSession(QObject* parent = 0);
    virtual ~QDaqSession(void);
    void evaluate(const QString& program);
    void abortEvaluation();
    int index() const { return idx_; }
    static int nextAvailableIndex();

private slots:
    void onUiChanged();

public slots:
    QString version();
    void quit();
    void exec(const QString& fname);
    void print(const QString& str);
    void wait(uint ms);
    void textSave(const QString& str, const QString& fname);
    QString textLoad(const QString& fname);

    bool h5write(const QDaqObject* obj, const QString& fname);
    QDaqObject*  h5read(const QString& fname);

    // file and folder
    QString pwd();
    bool cd(const QString& path);
    QStringList dir(const QStringList& filters = QStringList());
    QStringList dir(const QString& filter);
    bool isDir(const QString& name);

    // widgets
    QWidget* loadUi(const QString& fname);
    QWidget* loadTopLevelUi(const QString& fname, const QString &uiName);
    QString pluginPaths();
    QString availableWidgets();
    void bind(QDaqChannel* ch, QWidget* w);
    void bind(QDaqObject* obj, const QString& propertyName, QWidget* w, bool readOnly = false);
    void addItems(QComboBox* cb, const QStringList& lst);
    void addItems(QListWidget* cb, const QStringList& lst);

    // QDaqBuffer handling
    double mean(const QDaqBuffer& b) const { return b.mean(); }
    double std(const QDaqBuffer& b) const { return b.std(); }
    QDaqVector toVector(const QDaqBuffer& b) const { return b.toVector(); }

    // set debugging on (enable Qt script debugger)
    void debug(bool on);

    // timing
    void tic() { watch_.start(); }
    double toc() { /*watch_.stop();*/ return watch_.sec(); }

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

};



#endif


