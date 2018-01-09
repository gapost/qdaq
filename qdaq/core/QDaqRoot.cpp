#include "QDaqRoot.h"

#include <QCoreApplication>

QDaqRoot QDaqObject::root_;

QDaqRoot::QDaqRoot(void) : QDaqObject("qdaq")
{
}

QDaqRoot::~QDaqRoot(void)
{
    foreach(QDaqObject* obj, children()) obj->detach();
    /*{
		QDaqObject* rtobj = qobject_cast<QDaqObject*>(obj);
		if (rtobj) 
		{
			rtobj->detach();
			delete rtobj;
		}
    }*/
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



