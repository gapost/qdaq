#include "QDaqSession.h"
#include "QDaqRoot.h"

#include "QDaqLogFile.h"
#include "qdaqh5file.h"

#include "core_script_interface.h"

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

QDaqScriptEngine::QDaqScriptEngine(QObject *parent) : QObject(parent)
{
	engine_ = new QScriptEngine(this);
	engine_->setProcessEventsInterval ( PROC_EVENTS_INTERVAL );

	QScriptValue self = engine_->newQObject(
		this, QScriptEngine::QtOwnership,
        QScriptEngine::ExcludeDeleteLater);

	QScriptValue globObj = engine_->globalObject();
	{
		QScriptValueIterator it(globObj);
		while (it.hasNext()) {
			it.next();
			self.setProperty(it.scriptName(), it.value(), it.flags());
		}
	}

    QDaqObject* qdaq = QDaqObject::root();

    QScriptValue rootObj = toScriptValue(engine_, qdaq, QScriptEngine::QtOwnership);

    self.setProperty("qdaq", rootObj, QScriptValue::Undeletable | QScriptValue::ReadOnly);

	engine_->setGlobalObject(self);

    // init core scripting interface
    core_script_interface_init(engine_);

	engine_->collectGarbage();
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

	if (!result.isUndefined()) ret = result.toString();
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
void QDaqScriptEngine::sleep(int ms)
{
    QThread::msleep(ms);
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

    logFile_ = new QDaqLogFile(true,' ',this);
    logFile_->open(QDaqLogFile::getDecoratedName(objName));

    connect(this,SIGNAL(stdOut(QString)),this,SLOT(log_out(QString)));
    connect(this,SIGNAL(stdErr(QString)),this,SLOT(log_err(QString)));

    log_out(QString("*** %1 Start ***").arg(objectName()));

}

QDaqSession::~QDaqSession( void)
{
    Q_ASSERT(!isEvaluating());

    // close logfile, remove from index set
    log_out(QString("*** %1 End ***").arg(objectName()));
    if (logFile_) delete logFile_;
    idx_set.remove(idx_);

}

int QDaqSession::nextAvailableIndex()
{
    int i = 0;
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

        // TODO: syntax check the program

        QScriptContext* ctx = engine_->currentContext();

        ctx->setActivationObject(ctx->parentContext()->activationObject());
        ctx->setThisObject(ctx->parentContext()->thisObject());

        QScriptValue ret = engine_->evaluate(program,fname);


        if (engine_->hasUncaughtException()) {
            // TODO : better reporting
            QString s = ret.toString();
            QStringList backtrace = engine_->uncaughtExceptionBacktrace();

            engine_->currentContext()->throwValue(engine_->uncaughtException());
        }
    }
    else engine_->currentContext()->throwError(QScriptContext::ReferenceError,"File not found.");
}
//void QDaqSession::print(const QString& str)
void QDaqSession::log(const QString& str)
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

void QDaqSession::importExtension(const QString &name)
{
    QScriptValue ret = engine_->importExtension(name);
    if (engine_->hasUncaughtException())
        stdErr(ret.toString());
}

QStringList QDaqSession::availableExtensions()
{
//    QDir pluginsDir = QDir(QLibraryInfo::location(QLibraryInfo::PluginsPath));

//    QStringList libraryPaths = qApp->libraryPaths();

    return engine_->availableExtensions();
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

        if (fd==2) qDebug().noquote() << ln;
    }

}
bool QDaqSession::h5write(const QDaqObject *obj, const QString &fname)
{
    QDaqH5File f;
    bool ret = f.h5write(obj,fname);
    if (!f.warnings().isEmpty()) emit stdErr(f.warnings().join(QChar('\n'))+QChar('\n'));
    if (!ret) engine_->currentContext()->throwError(QString("Error writing file: %1.").arg(f.lastError()));
    return true;
}
QDaqObject* QDaqSession::h5read(const QString &fname)
{
    QDaqH5File f;
    QDaqObject* o = f.h5read(fname);
    if (!f.warnings().isEmpty()) emit stdErr(f.warnings().join(QChar('\n'))+QChar('\n'));
    if (!o) engine_->currentContext()->throwError(QString("Error reading file: %1.").arg(f.lastError()));
    return o;
}




QString QDaqSession::info(QScriptValue v)
{
    if (!v.isValid()) return QString("invalid");

    if (v.isBool())
        return QString("Boolean: %1").arg(v.toString());

    if (v.isNumber())
        return QString("Number: %1").arg(v.toNumber());

    if (v.isString())
        return QString("String: %1").arg(v.toString());

    if (v.isDate())
        return QString("Date: %1").arg(v.toString());

    if (v.isNull())
        return QString("null");

    if (v.isUndefined())
        return QString("undefined");

    if (v.isFunction())
        return QString("Function");

    if (v.isVariant()) {
        QVariant var = v.toVariant();
        return QString("Variant (%1): %2").arg(var.typeName()).arg(var.toString());
    }

    if (v.isQObject()) {
        QObject* obj = v.toQObject();
        const QMetaObject* metaobj = obj->metaObject();
        QString S =  QString(metaobj->className());
        S += "\n";
        for(int i=0; i< metaobj->propertyCount(); i++)
        {
            QMetaProperty metaProperty = metaobj->property(i);
            QVariant var = metaProperty.read(obj);
            S += QString("  %1 (%2): %3\n").arg(metaProperty.name())
                    .arg(var.typeName())
                    .arg(var.toString());
        }
        if (!obj->dynamicPropertyNames().isEmpty()) {
            S += "Dynamic Properties\n";
            foreach(const QByteArray& ba, obj->dynamicPropertyNames())
            {
                QVariant var = obj->property(ba.constData());
                S += QString("  %1 (%2): %3\n").arg(ba.constData())
                        .arg(var.typeName())
                        .arg(var.toString());
            }
        }
        return S;
    }

    if (v.isArray())
        return QString("Array: %1").arg(v.toString());


    if (v.isObject())
        return QString("Object: %1").arg(v.toString());

    return QString();



//    QString S;
//    QScriptValue obj(v); // the object to iterate over
//    while (obj.isObject()) {
//        QScriptValueIterator it(obj);
//        while (it.hasNext()) {
//            it.next();
//            //if (it.flags() & QScriptValue::SkipInEnumeration)
//            //    continue;
//            S += it.name();
//            S += QString("(%1): ").arg((int)it.flags());
//            if (it.value().isFunction()) S += "Function";
//            else if (it.value().isQMetaObject()) S += "QMetaObject";
//            else if (it.value().isQObject()) S += "QObject";
//            else S += it.value().toString();
//            S += "\n";
//        }
//        obj = obj.prototype();
//    }
//    return S;
}

QString QDaqSession::version()
{
    QString S = QString("QDaq ver.: %1\n").arg(QDaq::Version());
    S += QString("Qt compile ver.: %1\n").arg(QDaq::QtVersion());
    S += QString("Qt run-time ver.: %1\n").arg(qVersion());
    return S;
}





