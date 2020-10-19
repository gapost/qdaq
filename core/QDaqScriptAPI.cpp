#include "QDaqScriptAPI.h"
#include "QDaqSession.h"
#include "qdaqh5file.h"

#include <QCoreApplication>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QIODevice>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDir>
#include <QProcess>
#include <QEventLoop>
#include <QDebug>
#include <QThread>
#include <QMetaProperty>

#include <QLibraryInfo>

#include "bytearrayclass.h"
#include "vectorclass.h"

#include <QColor>
#include <QPointF>

#include "QDaqJob.h"
#include "QDaqChannel.h"
#include "QDaqDataBuffer.h"
#include "QDaqDevice.h"

/*
 * Conversion of some data from/to qscript
 *
 * QColor, QPointF
 *
 */

QScriptValue toScriptValue(QScriptEngine *engine, const QColor &clr)
{
    Q_UNUSED(engine);
    return QScriptValue(clr.name());
}

void fromScriptValue(const QScriptValue &obj, QColor &clr)
{
    clr.setNamedColor(obj.toString());

}

QScriptValue toScriptValue(QScriptEngine *engine, const QPointF &p)
{
    QScriptValue a = engine->newArray();
    a.setProperty(quint32(0), QScriptValue(p.x()));
    a.setProperty(quint32(1), QScriptValue(p.y()));
    return a;
}

void fromScriptValue(const QScriptValue &obj, QPointF &p)
{
    p.setX(obj.property(quint32(0)).toNumber());
    p.setY(obj.property(quint32(1)).toNumber());
}


template <class Container>
QScriptValue toScriptValueContainer(QScriptEngine *eng, const Container &cont)
{
    QScriptValue a = eng->newArray();
    typename Container::const_iterator begin = cont.begin();
    typename Container::const_iterator end = cont.end();
    typename Container::const_iterator it;
    for (it = begin; it != end; ++it)
        a.setProperty(quint32(it - begin), qScriptValueFromValue(eng, *it));
    return a;
}

template <class Container>
void fromScriptValueContainer(const QScriptValue &value, Container &cont)
{
    quint32 len = value.property("length").toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        typedef typename Container::value_type ContainerValue;
        cont.push_back(qscriptvalue_cast<ContainerValue>(item));
    }
}

QScriptValue toScriptValue(QScriptEngine *eng, const QDaqObjectList& L)
{
    QScriptValue V = eng->newArray();
    QDaqObjectList::const_iterator begin = L.begin();
    QDaqObjectList::const_iterator end = L.end();
    QDaqObjectList::const_iterator it;
    for (it = begin; it != end; ++it)
        V.setProperty(quint32(it - begin), qScriptValueFromValue(eng, *it));
    return V;
}

void fromScriptValue(const QScriptValue &value, QDaqObjectList& L)
{
    if (value.isArray()) {
        quint32 len = value.property("length").toUInt32();
        for (quint32 i = 0; i < len; ++i) {
            QScriptValue item = value.property(i);
            L.push_back(qscriptvalue_cast<QDaqObject*>(item));
        }
        return;
    }

    if (value.isQObject()) {
        QDaqObject* obj = qobject_cast<QDaqObject*>(value.toQObject());
        if (obj) L.push_back(obj);
    }
}

QScriptValue QDaqScriptAPI::toScriptValue(QScriptEngine *eng, QDaqObject * const &obj, int ownership)
{
    return eng->newQObject(obj, QScriptEngine::ValueOwnership(ownership),
                           QScriptEngine::ExcludeDeleteLater |
                           QScriptEngine::PreferExistingWrapperObject );
}

//QScriptValue toScriptValue(QScriptEngine *eng, const QScriptValue& scriptObj, QDaqObject * const &obj, int ownership)
//{
//    return eng->newQObject(scriptObj, obj, QScriptEngine::ValueOwnership(ownership),
//                           QScriptEngine::ExcludeDeleteLater |
//                           QScriptEngine::PreferExistingWrapperObject );
//}

void QDaqScriptAPI::fromScriptValue(const QScriptValue &value, QDaqObject*& obj)
{
    obj = qobject_cast<QDaqObject*>(value.toQObject());
}

typedef QDaqObject* QDaqObjectStar;


QScriptValue toScriptValueQDaqObjectStar(QScriptEngine *eng, const QDaqObjectStar& obj)
{
    return QDaqScriptAPI::toScriptValue(eng,obj);
}

void fromScriptValueQDaqObjectStar(const QScriptValue &value, QDaqObjectStar &obj)
{
    QDaqScriptAPI::fromScriptValue(value, obj);
}


QScriptValue scriptConstructor(QScriptContext *context, QScriptEngine *engine, const QMetaObject* metaObject)
{
    if (context->isCalledAsConstructor())
    {
        QString name;
        if (context->argumentCount()==1 &&
                context->argument(0).isString())
            name = context->argument(0).toString();
        if (name.isEmpty())
            return context->throwError(QScriptContext::SyntaxError,
                                "QDaqObject constructor must have one string argument (the object name)");

        QDaqObject* obj = (QDaqObject*)(metaObject->newInstance(Q_ARG(QString,name)));
        if (!obj)
            return context->throwError(QString("%1(name=%2) could not be created").arg(metaObject->className()).arg(name));

        return QDaqScriptAPI::toScriptValue(engine, obj);
    }
    else return context->throwError(QScriptContext::SyntaxError,
                                    QString("%1() called without'new'").arg(metaObject->className()));
}

QScriptValue sleepfunc(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine);
    if (context->argumentCount()!=1) {
        return context->throwError(QScriptContext::SyntaxError,
                            "sleep must be called with 1 argument\n"
                            "  Usage: sleep(ms)");
    }

    int msecs = context->argument(0).toUInt32();
    QThread::msleep(msecs);
    return QScriptValue(QScriptValue::UndefinedValue);
}

int QDaqScriptAPI::registerClass(QScriptEngine* eng, const QMetaObject* metaObject)
{
    QScriptEngine::FunctionWithArgSignature cptr =
            reinterpret_cast<QScriptEngine::FunctionWithArgSignature>(scriptConstructor);
    QScriptValue ctor = eng->newFunction(cptr, (void *)metaObject);
    QScriptValue scriptClass = eng->newQMetaObject(metaObject, ctor);

    eng->globalObject().setProperty(metaObject->className(), scriptClass);

    return 1;
}

int QDaqScriptAPI::initAPI(QScriptEngine *eng)
{
    // Register sleep func
    QScriptValue v = eng->newFunction(sleepfunc);
    eng->globalObject().setProperty("sleep", v);

    // Register all types and conversions
    ByteArrayClass *byteArrayClass = new ByteArrayClass(eng);
    eng->globalObject().setProperty("ByteArray", byteArrayClass->constructor());

    VectorClass *vectorClass = new VectorClass(eng);
    eng->globalObject().setProperty("Vector", vectorClass->constructor());


    int ret = qScriptRegisterMetaType<QDaqObjectStar>(eng,toScriptValueQDaqObjectStar,fromScriptValueQDaqObjectStar) &
        qScriptRegisterMetaType<QDaqObjectList>(eng,::toScriptValue,::fromScriptValue) &
        qScriptRegisterMetaType<QColor>(eng,::toScriptValue,::fromScriptValue) &
        qScriptRegisterMetaType<QPointF>(eng,::toScriptValue,::fromScriptValue);


    ret &= registerClass(eng, &QDaqObject::staticMetaObject);
    ret &= registerClass(eng, &QDaqJob::staticMetaObject);
    ret &= registerClass(eng, &QDaqLoop::staticMetaObject);
    ret &= registerClass(eng, &QDaqChannel::staticMetaObject);
    ret &= registerClass(eng, &QDaqDataBuffer::staticMetaObject);
    ret &= registerClass(eng, &QDaqDevice::staticMetaObject);

    return ret;

}

QVariant QDaqScriptAPI::toVariant(QScriptEngine *eng, const QScriptValue &value)
{
    // Do some QDaq specific type conversions
    QVariant V;
    bool ok = false;

    // check for QDaq JS classes Vector and ByteArray
    if (eng) {
        QScriptValue ByteArray = eng->globalObject().property("ByteArray");
        if (ByteArray.isValid() && value.instanceOf(ByteArray)) {
            QByteArray ba = qscriptvalue_cast<QByteArray>(value);
            V = QVariant::fromValue(ba);
            ok = true;
        }
        if (!ok) {
            QScriptValue Vector = eng->globalObject().property("Vector");
            if (Vector.isValid() && value.instanceOf(Vector)) {
                QDaqVector v = qscriptvalue_cast<QDaqVector>(value);
                V = QVariant::fromValue(v);
                ok = true;
            }
        }
    }

    // check for QDaqObject
    if (!ok && value.isQObject()) {
        QDaqObject* obj(0);
        fromScriptValue(value,obj);
        if (obj) {
            V = QVariant::fromValue(obj);
            ok = true;
        }
    }

    // check for QDaqObject list
    if (!ok && value.isArray()) {
        quint32 len = value.property("length").toUInt32();
        bool isObjLst = true;
        QDaqObjectList L;
        for (quint32 i = 0; i < len; ++i) {
            QScriptValue p = value.property(i);
            if (!p.isQObject()) { isObjLst=false; break; }
            QDaqObject* obj(0);
            fromScriptValue(p,obj);
            if (obj==0) { isObjLst=false; break; }
            else L << obj;
        }
        if (isObjLst) {
            V = QVariant::fromValue(L);
            ok = true;
        }
    }


    // if all failed do the default Qt conversion
    if (!ok) V = value.toVariant();

    return V;
}

QDaqScriptAPI::QDaqScriptAPI(QObject *parent) : QObject(parent),
    stopWatch_(new QElapsedTimer()), session_(0)
{

}

void QDaqScriptAPI::setSession(QDaqSession *s)
{
    session_ = s;
}

QDaqScriptAPI::~QDaqScriptAPI()
{
    delete stopWatch_;
}

QDaqScriptEngine* QDaqScriptAPI::daqengine() const
{
    if (session_) return session_->daqEngine();
    return 0;
}

// timing
void QDaqScriptAPI::tic()
{
    stopWatch_->start();
}
double QDaqScriptAPI::toc()
{
    return 0.000001*stopWatch_->nsecsElapsed();
}


void QDaqScriptAPI::wait(uint ms)
{
    QEventLoop loop;
    connect(this,SIGNAL(abortWait()),&loop,SLOT(quit()));
    QTimer::singleShot(ms,Qt::PreciseTimer,&loop,SLOT(quit()));
    loop.exec();

    /*
     * wait problem
     *
     * Either the above implementation or the one below
     * with QCoreApplication::processEvents have the same issue:
     * If another session begins a wait, then first their wait has to
     * end (either timeout or user abort) and then this one.
     *
     * This is a known problem generally (the term is "loop folding")
     * and may lead to other serious issues
     *
     * The implementation below (with processEvents) works ok however CPU goes to 100%
     *
     * TODO : find solution!
     *
     * Current solution (Oct2020)
     *
     * Only the root session is executed in the main thread. Other sessions
     * go in their own QThread+Event Loop and do not have access to the GUI.
     *
     * Thus the above local event loop interferes with the main app event loop
     * only if the wait() is used in the root session - which should be avoided
     *
     * When wait() is called from a normal (not root) session, we enter a local
     * loop in the session's event loop. It is expected that this should not cause
     * any trouble since this event loop is only for the session's funtioning.
     *
     * I tried also an implementation of a wait() with real thread blocking
     * (using QWaitCondition) but then the session event loop does not process events,
     * including e.g. a user request for aborting the wait
     *
     */

//    QElapsedTimer tmr;
//    tmr.start();
//    wait_aborted_ = false;
//    while(tmr.elapsed()<ms && !wait_aborted_)
//        QCoreApplication::processEvents(QEventLoop::AllEvents,200);


}

QScriptValue QDaqScriptAPI::exec(const QString &fname)
{
    QFile file(fname);
    if (file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        QTextStream qin(&file);
        QString program = qin.readAll();

        // TODO: syntax check the program

        QScriptContext* ctx = context();

        ctx->setActivationObject(ctx->parentContext()->activationObject());
        ctx->setThisObject(ctx->parentContext()->thisObject());

        return engine()->evaluate(program,fname);
    }
    else {
        context()->throwError(QScriptContext::ReferenceError,"File not found.");
        return QScriptValue(QScriptValue::UndefinedValue);
    }
}



void QDaqScriptAPI::log(const QString& str)
{
     emit stdOut(str + '\n');
}

void QDaqScriptAPI::textSave(const QString &str, const QString &fname)
{
    QFile file(fname);
    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream qout(&file);
        qout << str;
    }
    else context()->throwError("File could not be opened.");
}

QString QDaqScriptAPI::textLoad(const QString &fname)
{
    QString str;
    QFile file(fname);
    if (file.open(QFile::ReadOnly))
    {
        QTextStream qin(&file);
        str = qin.readAll();
    }
    else context()->throwError("File could not be opened.");
    return str;
}

void QDaqScriptAPI::importExtension(const QString &name)
{
    QScriptValue ret = engine()->importExtension(name);
    if (engine()->hasUncaughtException() && session_)
        emit stdErr(ret.toString() + '\n');
}

QStringList QDaqScriptAPI::availableExtensions()
{
//    QDir pluginsDir = QDir(QLibraryInfo::location(QLibraryInfo::PluginsPath));

//    QStringList libraryPaths = qApp->libraryPaths();

    return engine()->availableExtensions();
}

void QDaqScriptAPI::exit()
{
    emit endSession();
}

QString QDaqScriptAPI::pwd()
{
    return QDir::currentPath();
}

bool QDaqScriptAPI::cd(const QString &path)
{
    QDir dir = QDir::current();
    bool ret = dir.cd(path);
    if (ret) QDir::setCurrent(dir.path());
    return ret;
}

QStringList QDaqScriptAPI::dir(const QStringList& filters)
{
    return QDir::current().entryList(filters);
}
QStringList QDaqScriptAPI::dir(const QString& filter)
{
    return dir(QStringList(filter));
}
bool QDaqScriptAPI::isDir(const QString& name)
{
    QFileInfo fi(name);
    return fi.isDir();
}
void QDaqScriptAPI::debug(bool on)
{
    session_->debug(on);
}
QString QDaqScriptAPI::system(const QString &comm)
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
bool QDaqScriptAPI::h5write(const QDaqObject *obj, const QString &fname)
{
    QDaqH5File f;
    bool ret = f.h5write(obj,fname);
    if (!f.warnings().isEmpty()) session_->stdErr(f.warnings().join(QChar('\n'))+QChar('\n'));
    if (!ret) context()->throwError(QString("Error writing file: %1.").arg(f.lastError()));
    return true;
}
QDaqObject* QDaqScriptAPI::h5read(const QString &fname)
{
    QDaqH5File f;
    QDaqObject* o = f.h5read(fname);
    if (!f.warnings().isEmpty()) session_->stdErr(f.warnings().join(QChar('\n'))+QChar('\n'));
    if (!o) context()->throwError(QString("Error reading file: %1.").arg(f.lastError()));
    return o;
}




QString QDaqScriptAPI::info(QScriptValue v)
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

QString QDaqScriptAPI::version()
{
    QString S = QString("QDaq ver.: %1\n").arg(QDaq::Version());
    S += QString("Qt compile ver.: %1\n").arg(QDaq::QtVersion());
    S += QString("Qt run-time ver.: %1\n").arg(qVersion());
    return S;
}
