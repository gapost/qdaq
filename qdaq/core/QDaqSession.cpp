#include "QDaqSession.h"
#include "QDaqRoot.h"
#include "QDaqTypes.h"
#include "QDaqLogFile.h"

#include <QDebug>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QFile>
//#include <QTextStream>
#include <QTimer>
//#include <QCoreApplication>
#include <QApplication>
#include <QScriptValueIterator>
#include <QDir>
#include <QProcess>
#include <QWidget>
#include <QtUiTools/QUiLoader>



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

    // register QDaqObject* with the engine (and QDaqObjectList)
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

QSet<int> QDaqSession::idx_set;

/// \brief QDaqSession::QDaqSession
/// \param parent
///
///
QDaqSession::QDaqSession(QObject *parent) : QDaqScriptEngine(parent)
{
    debugger_ = new QScriptEngineDebugger(this);

    wait_timer_ = new QTimer(this);
    wait_timer_->setSingleShot(true);

    // find a free index number
    idx_ = nextAvailableIndex();
    idx_set.insert(idx_);

    QString objName = QString("session%1").arg(idx_);
    setObjectName(objName);

    // create log file
    if (idx_) {
        logFile_ = new QDaqLogFile(true,' ',this);
        logFile_->open(QDaqLogFile::getDecoratedName(objName));

        connect(this,SIGNAL(stdOut(QString)),this,SLOT(log_out(QString)));
        connect(this,SIGNAL(stdErr(QString)),this,SLOT(log_err(QString)));
    }

    QScriptValue uiObj = engine_->newObject();

    int iw = 1;
    foreach (QWidget* w, QApplication::topLevelWidgets()) {
        QScriptValue wObj = engine_->newQObject(w);
        QString wname = w->objectName();
        //QString className =  w->metaObject()->className();
        if (!wname.isEmpty()) {
            //wname = QString("widget%1").arg(iw);
            uiObj.setProperty(wname,wObj);
        }
        iw++;
    }

    engine_->globalObject().setProperty("ui",uiObj);

    connect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(onFocusChanged(QWidget*,QWidget*)));
}

QDaqSession::~QDaqSession( void)
{
    Q_ASSERT(!isEvaluating());

    // close logfile, remove from index set
    if (idx_) {
        if (logFile_) delete logFile_;
        idx_set.remove(idx_);
    }
}

int QDaqSession::nextAvailableIndex() const
{
    int i = 1;
    while (idx_set.contains(i)) i++;
    return i;
}

void QDaqSession::evaluate(const QString& program)
{
    if (logFile_) log_in(program);
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
void QDaqSession::debug(bool on)
{
    if (on) debugger_->attachTo(engine_);
    else debugger_->detach();
}
QString QDaqSession::system(const QString &comm)
{
    QProcess p;
    //comm.split(QChar(' '),QString::SkipEmptyParts);
    p.start(comm);
    /*bool ret = */p.waitForFinished(1000);
    QByteArray pout = p.readAllStandardOutput();
    QByteArray perr = p.readAllStandardError();
    QByteArray pall = p.readAll();
    return QString(pout);
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
        QString buff(pre);
        buff += ln;
        //if (buff.endsWith('\n')) buff.remove(buff.size()-1,1);
        *logFile_ << buff;
    }

}
bool QDaqSession::h5write(const QDaqObject *obj, const QString &fname)
{
    QDaqObject::h5write(obj,fname);
    return true;
}
QDaqObject* QDaqSession::h5read(const QString &fname)
{
    return QDaqObject::h5read(fname);
}
QWidget* QDaqSession::loadUi(const QString &fname)
{
    QFile file(fname);

    if (!file.open(QFile::ReadOnly))
    {
        engine_->currentContext()->throwError(QString("Ui file %1 could not be opened.").arg(fname));
        return 0;
    }

    QUiLoader loader;
    // loader working dir = ui file dir
    QFileInfo fi(file);
    loader.setWorkingDirectory(fi.absoluteDir());
    QWidget* w = loader.load(&file);
    file.close();
    if (!w)
    {
        engine_->currentContext()->throwError(
                    QString("Error while loading file %1.\n%2").arg(fname).arg(loader.errorString())
                    );
    }

    return w;
}
void QDaqSession::onFocusChanged(QWidget* old, QWidget* now)
{
    QWidgetList wl = QApplication::topLevelWidgets();
    int j=0;
    if (now && now->objectName()=="deltaControl")
    {
        j++;
    }
    if (wl.contains(now))
    {
        j++;
    }

}

