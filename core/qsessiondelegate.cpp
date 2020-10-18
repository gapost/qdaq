#include "qsessiondelegate.h"
#include "QDaqScriptAPI.h"
#include "QDaqSession.h"
#include <QCoreApplication>
#include <QThread>
#include <QDebug>

QSessionDelegate::QSessionDelegate() : thread_(0), session_(0)
{

}

QSessionDelegate::~QSessionDelegate()
{
    if (session_ && session_->daqEngine()->isEvaluating())
    {
        emit abort();
    }
    if (thread_) {
        thread_->quit();
        thread_->wait();
        delete thread_;
    }
}

void QSessionDelegate::init(bool ownthread, QDaqSession *s)
{
    session_ = s;
    QDaqScriptAPI* api = s->api_;

    Qt::ConnectionType type = ownthread ? Qt::QueuedConnection : Qt::DirectConnection;

    // if we must run in a separate thread, initialize the QThread
    // object and move me and the api object to the thread
    if (ownthread) {
        thread_ = new QThread();
        api->moveToThread(thread_);
        moveToThread(thread_);
    }

    // connect the stdout, stderr streams from me and the api to the slots
    connect(api,&QDaqScriptAPI::stdErr,this,&QSessionDelegate::onStdErr,type);
    connect(api,&QDaqScriptAPI::stdOut,this,&QSessionDelegate::onStdOut,type);
    connect(this,&QSessionDelegate::stdErr,this,&QSessionDelegate::onStdErr,type);
    connect(this,&QSessionDelegate::stdOut,this,&QSessionDelegate::onStdOut,type);

    connect(api,&QDaqScriptAPI::endSession,this,&QSessionDelegate::onEndSession,type);

    connect(this,&QSessionDelegate::eval,this,&QSessionDelegate::onEval,type);
    connect(this,&QSessionDelegate::abort,this,&QSessionDelegate::onAbort,type);
    connect(this,&QSessionDelegate::evalFinished,this,&QSessionDelegate::onEvalFinished,type);

    // start the thread event loop
    if (ownthread) {
        thread_->start();
    }

}

void QSessionDelegate::onStdOut(const QString& str)
{
    session_->log__(1,str);
    emit session_->stdOut(str);
}

void QSessionDelegate::onStdErr(const QString& str)
{
    session_->log__(2,str);
    emit session_->stdErr(str);
}

void QSessionDelegate::onEvalFinished()
{
    emit session_->requestInput(">> ");
}

void QSessionDelegate::onEndSession()
{
    emit session_->endSession();
}

void QSessionDelegate::onEval(const QString &code)
{
    QDaqScriptEngine* e = session_->daqEngine();
    QString msg;
    bool ret = e->evaluate(code,msg);
    if (!msg.isEmpty())
    {
        if (ret) emit stdOut(msg + "\n");
        else emit stdErr(msg+"\n");
    }
    emit evalFinished();
}

void QSessionDelegate::onAbort()
{
    QDaqScriptEngine* e = session_->daqEngine();
    if (!e->isEvaluating()) return;
    e->abortEvaluation();
    qDebug() << "called abortEvaluation";
    qDebug() << "emit abotWait";
    emit session_->api_->abortWait();
}
