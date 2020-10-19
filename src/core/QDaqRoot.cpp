#include "QDaqRoot.h"
#include "QDaqLogFile.h"
#include "QDaqJob.h"
#include "QDaqSession.h"
#include "QDaqChannel.h"
#include "QDaqDataBuffer.h"
#include "QDaqDevice.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QLibraryInfo>
#include <QDebug>

QDaqRoot* QDaqObject::root_;

QDaqRoot::QDaqRoot(void) : QDaqObject("qdaq")
{
    root_ = this;

    qRegisterMetaType<QDaqError>();

    // These are needed for instantiating objects during serialization
    qRegisterMetaType<QDaqObject*>();
    qRegisterMetaType<QDaqJob*>();
    qRegisterMetaType<QDaqLoop*>();
    qRegisterMetaType<QDaqChannel*>();
    qRegisterMetaType<QDaqDataBuffer*>();
    qRegisterMetaType<QDaqDevice*>();

    // root dir = current dir when app starts
    QDir pwd = QDir::current();
    rootDir_ = pwd.absolutePath();

    // log dir = rootDir/log
    if (!pwd.cd("log")) {
        if (pwd.mkdir("log")) pwd.cd("log");
    }
    logDir_ = pwd.absolutePath();

    // create error log file
    errorLog_ = new QDaqLogFile(false,',',this);
    errorLog_->open(QDaqLogFile::getDecoratedName("error"));

    connect(this,SIGNAL(error(QDaqError)),this,SLOT(onError(QDaqError)));

    rootSession_ = new QDaqSession(this);

}

void jobDisarmHelper(QDaqObjectList lst)
{
    foreach(QDaqObject* obj, lst)
    {
        if (obj->hasChildren()) jobDisarmHelper(obj->children());
        QDaqLoop* j = qobject_cast<QDaqLoop*>(obj);
        if (j) j->disarm();
    }
}

QDaqRoot::~QDaqRoot(void)
{
    jobDisarmHelper(children());
    //foreach(QDaqObject* obj, children()) obj->detach();
    /*{QDaqIDE*
		QDaqObject* rtobj = qobject_cast<QDaqObject*>(obj);
		if (rtobj) 
		{
			rtobj->detach();
			delete rtobj;
		}
    }*/
}


void QDaqRoot::onError(const QDaqError &err)
{
    error_queue_.push(err);
    if (errorLog_) *errorLog_ <<
           QString("%1,%2,%3,%4,%5")
               .arg(err.t.toString("dd.MM.yyyy"))
               .arg(err.t.toString("hh:mm:ss.zzz"))
               .arg(err.objectName)
               .arg(err.type).arg(err.descr);
}

QDaqSession* QDaqRoot::newSession()
{
    QDaqSession* s = new QDaqSession(this);
    emit newSession(s);
    return s;
}



