#include "QDaqRoot.h"
#include "QDaqJob.h"
#include "QDaqLogFile.h"
#include "QDaqChannel.h"
#include "QDaqDataBuffer.h"
#include "QDaqSession.h"
#include "QDaqIde.h"
#include "QDaqInterface.h"
#include "QDaqDevice.h"

#include <QCoreApplication>
#include <QDir>

QDaqRoot* QDaqObject::root_;

QDaqRoot::QDaqRoot(void) : QDaqObject("qdaq"), ideWindow_(0)
{
    root_ = this;

    qRegisterMetaType<QDaqError>();

    registerClass(&QDaqObject::staticMetaObject);

    registerClass(&QDaqJob::staticMetaObject);
    registerClass(&QDaqLoop::staticMetaObject);

    registerClass(&QDaqChannel::staticMetaObject);
    registerClass(&QDaqTestChannel::staticMetaObject);
    registerClass(&QDaqTimeChannel::staticMetaObject);

    registerClass(&QDaqDataBuffer::staticMetaObject);

    // DAQ objects/devices
    registerClass(&QDaqTcpip::staticMetaObject);
    registerClass(&QDaqSerial::staticMetaObject);
    registerClass(&QDaqModbusTcp::staticMetaObject);
    registerClass(&QDaqModbusRtu::staticMetaObject);
    registerClass(&QDaqDevice::staticMetaObject);

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

QDaqRoot::~QDaqRoot(void)
{
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

QDaqIDE* QDaqRoot::createIdeWindow()
{
    if (!ideWindow_) {
        ideWindow_ = new QDaqIDE;
    }
    return ideWindow_;
}

QString QDaqRoot::xml()
{
    QString ret;
    return ret;
}

QDaqObject* QDaqRoot::createObject(const QString& name, const QString& className)
{
    QDaqObject* obj = 0;
    object_map_t::iterator it = object_map_.find(className);
    const QMetaObject* metaObj = (it==object_map_.end()) ? 0 : *it;
    if (metaObj)
    {
        obj = (QDaqObject*)(metaObj->newInstance(Q_ARG(QString,name)));
		if (!obj)
			this->throwScriptError("The object could not be created");
    }
    return obj;
}

QStringList QDaqRoot::classNames()
{
    QStringList lst;
    for(object_map_t::iterator it = object_map_.begin(); it!=object_map_.end(); it++)
        lst << it.key();
    return lst;
}

void QDaqRoot::registerClass(const QMetaObject* metaObj)
{
    object_map_[metaObj->className()] = metaObj;
}

QList<const QMetaObject*> QDaqRoot::registeredClasses() const
{
	QList<const QMetaObject*> lst;
	for(object_map_t::const_iterator it = object_map_.begin(); it!=object_map_.end(); it++)
			lst << it.value();
	return lst;
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

void QDaqRoot::objectCreation(QDaqObject* obj, bool create)
{
    if (obj==this) return;
    if (obj->parent()==0) return;

    if (create)
        emit objectCreated(obj);
    else
        emit objectDeleted(obj);
}
void QDaqRoot::addDaqWindow(QWidget* w)
{
    if (!daqWindows_.contains(w)) {
        daqWindows_.push_back(w);
        emit daqWindowsChanged();
    }
}

void QDaqRoot::removeDaqWindow(QWidget* w)
{
    if (daqWindows_.contains(w)) {
        daqWindows_.removeOne(w);
        emit daqWindowsChanged();
    }
}


