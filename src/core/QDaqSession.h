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
class QDaqScriptAPI;
class QSessionDelegate;


class QDAQ_EXPORT QDaqScriptEngine : public QObject
{    
    Q_OBJECT

public:
    enum EngineType {
        LoopEngine,
        SessionEngine,
        RootEngine
    };

protected:
	QScriptEngine* engine_;
    EngineType type_;


public:
    explicit QDaqScriptEngine(QObject* parent, EngineType t);

    QScriptEngine* getEngine() const { return engine_; }

	bool canEvaluate(const QString&);
	bool isEvaluating() const;
    bool evaluate(const QScriptProgram& program, QString& ret, QDaqObject* thisObj = 0);
    bool evaluate(const QString& program, QString &ret, QDaqObject* thisObj = 0);
	void abortEvaluation();
    EngineType type() const { return type_; }
};

class QDAQ_EXPORT QDaqSession : public QObject
{
    Q_OBJECT

    QScriptEngineDebugger* debugger_;
    QDaqScriptEngine* engine_;
    QDaqScriptAPI* api_;

    friend class QSessionDelegate;
    QSessionDelegate* delegate_;

    // console index = 0 - ROOT,1,2,3,...
    int idx_;
    // set contains actice index values
    static QSet<int> idx_set;

    QDaqLogFile* logFile_;

    friend class QDaqRoot;
    QDaqSession(QObject* parent = 0);

public:

    virtual ~QDaqSession(void);

    int index() const { return idx_; }
    static int nextAvailableIndex();

    QDaqScriptEngine* daqEngine() const { return engine_; }
    QScriptEngine* scriptEngine() const
    {
        if (engine_) return engine_->getEngine();
        return 0;
    }

    // set debugging on (enable Qt script debugger)
    void debug(bool on);

    bool waitForFinished(uint ms);

public slots:

    // accepting console request to eval code
    void eval(const QString& code);
    void abort();

signals:

    // signals sent to console
    void stdOut(const QString&);
    void stdErr(const QString&);
    void endSession();
    void requestInput(const QString& prompt);

private:
    void log__(int fd, const QString& str);
};



#endif


