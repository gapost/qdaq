#include "QDaqSession.h"
#include "QDaqRoot.h"

#include "QDaqLogFile.h"
#include "qdaqh5file.h"

#include "qsessiondelegate.h"
#include "QDaqScriptAPI.h"

#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QCoreApplication>
#include <QScriptValueIterator>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QThread>
#include <QMetaProperty>

#include <QLibraryInfo>

#define PROC_EVENTS_INTERVAL 250

QDaqScriptEngine::QDaqScriptEngine(QObject *parent, EngineType t) : QObject(parent), type_(t)
{
	engine_ = new QScriptEngine(this);
	engine_->setProcessEventsInterval ( PROC_EVENTS_INTERVAL );

    QScriptValue globalObj = engine_->globalObject();
    QDaqObject* qdaq = QDaqObject::root();
    QScriptValue rootObj = QDaqScriptAPI::toScriptValue(engine_, qdaq, QScriptEngine::QtOwnership);

    globalObj.setProperty("qdaq", rootObj, QScriptValue::Undeletable | QScriptValue::ReadOnly);

    // init core scripting interface
    QDaqScriptAPI::initAPI(this);
}
bool QDaqScriptEngine::evaluate(const QString& program, QString& ret, QDaqObject* thisObj)
{
    QScriptValue result;
    if (thisObj) {
        QScriptContext* ctx = engine_->pushContext();
        ctx->setThisObject(engine_->toScriptValue(thisObj));
        result = engine_->evaluate(program);
        engine_->popContext();
    }
    else result = engine_->evaluate(program);

	if (!result.isUndefined()) ret = result.toString();
	if (engine_->hasUncaughtException()) {
		QStringList backtrace = engine_->uncaughtExceptionBacktrace();
		ret += '\n';
		ret += backtrace.join("\n");

        QDaqError e("scriptEngine","evaluation error",ret);
        QDaqObject::root()->postError(e);

		return false;
	}
	return (result.isError()) ? false : true;
}

bool QDaqScriptEngine::evaluate(const QScriptProgram& program, QString& ret, QDaqObject* thisObj)
{
    QScriptValue result;
    if (thisObj) {
        QScriptContext* ctx = engine_->pushContext();
        ctx->setThisObject(engine_->toScriptValue(thisObj));
        result = engine_->evaluate(program);
        engine_->popContext();
    }
    else result = engine_->evaluate(program);

    if (result.isValid() && !result.isUndefined()) ret = result.toString();
	if (engine_->hasUncaughtException())
	{
		QStringList backtrace = engine_->uncaughtExceptionBacktrace();
		ret += '\n';
		ret += backtrace.join("\n");

        QDaqError e("scriptEngine","evaluation error",ret);
        QDaqObject::root()->postError(e);

		return false;
	}
	return (result.isError()) ? false : true;
}
bool QDaqScriptEngine::canEvaluate(const QString& program)
{
	return engine_->canEvaluate(program);
}
bool QDaqScriptEngine::isEvaluating() const
{
	return engine_->isEvaluating();
}
void QDaqScriptEngine::abortEvaluation()
{
	engine_->abortEvaluation();
}


////////////////// QDaqSession /////////////////////////////

QSet<int> QDaqSession::idx_set;

/// \brief QDaqSession::QDaqSession
/// \param parent
///
///
QDaqSession::QDaqSession(QObject *parent) : QObject(parent)
{
    // find a free index number
    idx_ = nextAvailableIndex();
    idx_set.insert(idx_);
    setObjectName(QString("session%1").arg(idx_));

    // create objects
    engine_ = new QDaqScriptEngine(this, idx_ ? QDaqScriptEngine::SessionEngine : QDaqScriptEngine::RootEngine);
    delegate_ = new QSessionDelegate();

    if (idx_) // not ROOT
    {
         // normal sessions run in own thread
        // engine_->moveToThread(thread_);
        debugger_ = 0;
    }
    else
    {
        //thread_ = 0; // ROOT session runs in app thread
        debugger_ = new QScriptEngineDebugger(this); // debugger only in ROOT
    }

    QScriptEngine* e = engine_->getEngine();
    api_ = new QDaqScriptAPI();
    api_->setSession(this);
    QScriptValue apiObject = e->newQObject(
                api_, QScriptEngine::QtOwnership,
                QScriptEngine::ExcludeDeleteLater | QScriptEngine::ExcludeSuperClassContents);
    QScriptValue globObj = e->globalObject();
    {
        QScriptValueIterator it(globObj);
        while (it.hasNext()) {
            it.next();
            apiObject.setProperty(it.scriptName(), it.value(), it.flags());
        }
    }
    e->setGlobalObject(apiObject);
    e->collectGarbage();

    delegate_->init(idx_,this);

    // create log file
    logFile_ = new QDaqLogFile(true,' ',this);
    logFile_->open(QDaqLogFile::getDecoratedName(this->objectName()));
    log__(1,QString("*** %1 Start ***").arg(objectName()));

}

QDaqSession::~QDaqSession( void)
{
    delete delegate_;
    delete engine_;
    delete api_;

    // close logfile, remove from index set
    log__(1,QString("*** %1 End ***").arg(objectName()));
    if (logFile_) delete logFile_;

    idx_set.remove(idx_);

}

int QDaqSession::nextAvailableIndex()
{
    int i = 0;
    while (idx_set.contains(i)) i++;
    return i;
}

bool QDaqSession::waitForFinished(uint ms)
{
    QEventLoop loop;
    connect(this,SIGNAL(abortWait()),&loop,SLOT(quit()));
    if (ms>0) QTimer::singleShot(ms,&loop,SLOT(quit()));
    loop.exec();
    return true;
}

void QDaqSession::eval(const QString& code)
{
    if (logFile_) log__(0,code);
    emit delegate_->eval(code);
}

void QDaqSession::abort()
{
    qDebug() << "Request abort to " << this->objectName();
    emit delegate_->abort();
}

void QDaqSession::debug(bool on)
{
    if (!debugger_) return;
    if (on) debugger_->attachTo(engine_->getEngine());
    else debugger_->detach();
}

void QDaqSession::log__(int fd, const QString &str)
{
    if (!logFile_) return;

    QString pre;
    if (fd==0) pre = ">> ";
    else if (fd==2) pre = "err: ";

    QStringList lines = str.split('\n', QString::SkipEmptyParts);
    foreach(const QString& ln, lines)
    {
        *logFile_ << pre;
        *logFile_ << ln;
        if (fd==2) qDebug().noquote() << ln;
    }

}






