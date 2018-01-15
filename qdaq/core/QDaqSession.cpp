#include "QDaqSession.h"
#include "QDaqRoot.h"
#include <QDebug>
#include <QScriptEngine>
#include <QFile>
//#include <QTextStream>
#include <QTimer>
#include <QCoreApplication>
#include <QScriptValueIterator>
#include <QDir>

#include "QDaqTypes.h"

QScriptValue QDaqScriptEngine::scriptConstructor(QScriptContext *context, QScriptEngine *engine, const QMetaObject* metaObject)
{
	QString name;
	if (context->argumentCount()==1 &&
			context->argument(0).isString())
		name = context->argument(0).toString();
	if (name.isEmpty()) {
		context->throwError(QScriptContext::SyntaxError,
							"Give only a String by object creation");
		return QScriptValue();
	}
    QDaqObject* obj = QDaqObject::root()->createObject(name,metaObject->className());
	if (context->isCalledAsConstructor())
		return engine->newQObject(context->thisObject(), obj, QScriptEngine::AutoOwnership, QScriptEngine::ExcludeDeleteLater);

	QScriptValue scriptObj = engine->newQObject(obj, QScriptEngine::AutoOwnership, QScriptEngine::ExcludeDeleteLater);
	scriptObj.setPrototype(context->callee().property(QString::fromLatin1("prototype")));
	return scriptObj;
}

#define PROC_EVENTS_INTERVAL 250

QDaqScriptEngine::QDaqScriptEngine(QObject *parent) : QObject(parent)
{
	engine_ = new QScriptEngine(this);
	engine_->setProcessEventsInterval ( PROC_EVENTS_INTERVAL );

	QScriptValue self = engine_->newQObject(
		this, QScriptEngine::QtOwnership,
		QScriptEngine::ExcludeSuperClassContents);

	QScriptValue globObj = engine_->globalObject();
	{
		QScriptValueIterator it(globObj);
		while (it.hasNext()) {
			it.next();
			self.setProperty(it.scriptName(), it.value(), it.flags());
		}
	}

    QDaqObject* qdaq = QDaqObject::root();

    QScriptValue rootObj = engine_->newQObject(qdaq,
		QScriptEngine::QtOwnership,
		QScriptEngine::ExcludeDeleteLater //| QScriptEngine::ExcludeSuperClassContents
		);

    self.setProperty("qdaq", rootObj, QScriptValue::Undeletable);

	engine_->setGlobalObject(self);

    // register QDaqObject* with the engine (and RtObjectList)
    registerQDaqObjectStar(engine_);

	// register basic types with the engine
    registerVectorTypes(engine_);

	// register root classes
    QList<const QMetaObject*> rtClasses = QDaqObject::root()->registeredClasses();
	foreach(const QMetaObject* metaObject, rtClasses)
	{
		QScriptEngine::FunctionWithArgSignature cptr =
				reinterpret_cast<QScriptEngine::FunctionWithArgSignature>(scriptConstructor);
		QScriptValue ctor = engine_->newFunction(cptr, (void *)metaObject);
		QScriptValue scriptClass = engine_->newQMetaObject(metaObject, ctor);

		engine_->globalObject().setProperty(metaObject->className(), scriptClass);
	}

	engine_->collectGarbage();
}
bool QDaqScriptEngine::evaluate(const QString& program, QString& ret)
{
	QScriptValue result = engine_->evaluate(program);
	if (!result.isUndefined()) ret = result.toString();
	if (engine_->hasUncaughtException()) {
		QStringList backtrace = engine_->uncaughtExceptionBacktrace();
		ret += '\n';
		ret += backtrace.join("\n");
		return false;
	}
	return (result.isError()) ? false : true;
}

bool QDaqScriptEngine::evaluate(const QScriptProgram& program, QString& ret)
{
	QScriptValue result = engine_->evaluate(program);
	if (!result.isUndefined()) ret = result.toString();
	if (engine_->hasUncaughtException())
	{
		QStringList backtrace = engine_->uncaughtExceptionBacktrace();
		ret += '\n';
		ret += backtrace.join("\n");
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
QDaqSession::QDaqSession(QObject *parent) : QDaqScriptEngine(parent)
{
    wait_timer_ = new QTimer(this);
    wait_timer_->setSingleShot(true);

    int n = ids.size();
    id_ = 1;
    if (n)
    {
        int idmax = *(ids.crbegin());
        if (n==idmax)
            id_ = idmax;
        else
        {
            for(; id_<=idmax; id_++)
            {
                if (ids.find(id_)==ids.end()) break;
            }
        }
    }

    ids.insert(id_);
}

QDaqSession::~QDaqSession( void)
{
    Q_ASSERT(!isEvaluating());

    ids.erase(id_);
}

void QDaqSession::evaluate(const QString& program)
{
    QString result;
    if ( QDaqScriptEngine::evaluate(program, result) )
    {
        if (!result.isEmpty()) emit stdOut(result);
    }
    else
    {
        emit stdErr(result);
    }
}
void QDaqSession::wait(uint ms)
{
    wait_timer_->start(ms);
    wait_aborted_ = false;
    do
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, PROC_EVENTS_INTERVAL);
    while (wait_timer_->isActive());
    if (wait_aborted_)
        engine_->currentContext()->throwError(QScriptContext::UnknownError,"Wait Aborted.");
}
void QDaqSession::abortEvaluation()
{
    wait_timer_->stop();
    wait_aborted_ = true;
    engine_->abortEvaluation();
}
void QDaqSession::exec(const QString &fname)
{
    QFile file(fname);
    if (file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        QTextStream qin(&file);
        QString program = qin.readAll();
        engine_->evaluate(program,fname);
    }
    else engine_->currentContext()->throwError(QScriptContext::ReferenceError,"File not found.");
}
void QDaqSession::print(const QString& str)
{
    stdOut(str);
    if (!str.endsWith('\n')) stdOut(QString('\n'));
}
void QDaqSession::textSave(const QString &str, const QString &fname)
{
    QFile file(fname);
    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream qout(&file);
        qout << str;
    }
    else engine_->currentContext()->throwError("File could not be opened.");
}

QString QDaqSession::textLoad(const QString &fname)
{
    QString str;
    QFile file(fname);
    if (file.open(QFile::ReadOnly))
    {
        QTextStream qin(&file);
        str = qin.readAll();
    }
    else engine_->currentContext()->throwError("File could not be opened.");
    return str;
}

void QDaqSession::quit()
{
    emit endSession();
}

QString QDaqSession::pwd()
{
    return QDir::currentPath();
}

bool QDaqSession::cd(const QString &path)
{
    QDir dir = QDir::current();
    bool ret = dir.cd(path);
    if (ret) QDir::setCurrent(dir.path());
    return ret;
}

QStringList QDaqSession::dir(const QStringList& filters)
{
    return QDir::current().entryList(filters);
}
QStringList QDaqSession::dir(const QString& filter)
{
    return dir(QStringList(filter));
}
bool QDaqSession::isDir(const QString& name)
{
    QFileInfo fi(name);
    return fi.isDir();
}

