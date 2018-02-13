#include "QDaqObject.h"
#include "QDaqRoot.h"
#include <QScriptContext>
#include <QScriptEngine>
#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QVariant>

QDaqObject::QDaqObject(const QString& name) :
QObject(0)
{
    setObjectName(name);
    qDebug() << "QDaqObject constructor" << path() << "@" << (void*)this;
}

QDaqObject::~QDaqObject(void)
{
    QDaqObjectList lst = children();
    foreach(QDaqObject* obj, lst)
    {
        obj->setParent(0);
        delete obj;
    }

    qDebug() << "destroying" << path() << "@" << (void*)this;
}

/** Attach this QDaqObject to the QDaq-framework.
This function should be called right after the constructor.
It establishes the required links to other objects of the framework. 
This has to be done after the constructor so that the object's C++ pointer
is fully qualified.
*/
void QDaqObject::attach()
{
    qDebug() << "attaching" << path() << "@" << (void*)this;
    foreach(QDaqObject* obj, children_) obj->attach();
    root()->objectCreation(this,true);
}
/** Detach this QDaqObject from the QDaq-framework.
This function should always be called before the destructor.
It removes links to this object from other objects in the framework.
It stops QDaqLoop objects, disarms QDaqJob objects, etc. so that they can
be safely deleted.
*/
void QDaqObject::detach()
{
    qDebug() << "detaching" << path() << "@" << (void*)this;
    root()->objectCreation(this,false);
    foreach(QDaqObject* obj, children_) obj->detach();   
}
/** Return true if this QDaqObject is attached to the QDaq-framework.
*/
bool QDaqObject::isAttached() const
{
    if (this==root()) return true; // root is always atached
    QDaqObject* p = parent();
    while(p && p!=root()) p = p->parent();
    return p;
}

QString QDaqObject::errorBacktrace() const
{
    QList<QDaqError> lastErrors = root()->errorQueue()->objectBackTrace(this);
	QString S;
	int i=0;
    foreach(const QDaqError& e, lastErrors)
	{
		if (i) S += '\n';
		S += e.toString();
		i++;
	}
	return S;
}

void QDaqObject::throwScriptError(const QString& msg) const
{
    QScriptContext* ctx = context();
    if (ctx) ctx->throwError(msg);
    pushError("throwScriptError",msg);
}


void QDaqObject::objectTree(QString& S, int level) const
{
	QString pre;
	if (level)
	{
		int k = level;
		pre.prepend("|--"); k--;
		while (k)
		{
			pre.prepend("|  "); k--;
		}
	}
	S += pre + objectName() + '\n';
	level += 1;
    foreach(QObject* obj, children_)
	{
		QDaqObject* rtobj = qobject_cast<QDaqObject*>(obj);
		if (rtobj) rtobj->objectTree(S, level);
	}
}

bool QDaqObject::checkName(const QString& name) const
{
    static const char* errmsg[] = {
		"Empty string is not allowed for object name.",
		"Object names must start with a letter.",
		"Object names may only contain letters, numbers and the underscore."
	};
	int cd;
	if (!isNameValid(name,&cd))
	{
		throwScriptError(errmsg[cd]);
		return false;
	}
	if (!isNameUnique(name))
	{
		throwScriptError("Name is used by another child object.");
		return false;

	}
	return true;
}

bool QDaqObject::isNameUnique(const QString& name) const
{
	QDaqObject* obj;
	foreach(obj, children_)
	{
		if (obj->objectName()==name)
		{
			return false;
		}
	}
	return true;
}

bool QDaqObject::isNameValid(const QString& name, int *c)
{
	int ret;
	int* code = c ? c : &ret;

	int n = name.length();
	if (n==0) { *code=0; return false; }
	int i = 0;
	if (!name.at(i++).isLetter()) { *code=1; return false; }

	while(i<n)
	{
		const QChar ch = name.at(i++);
		if (!(ch.isLetterOrNumber() || ch=='_'))
			{ *code=2; return false; }
	}

	return true;
}

QString QDaqObject::path() const
{
	QString name = objectName();
	QDaqObject* p = parent();
    while(p) // && p!=&root_)
	{
		name.prepend('.');
		name.prepend(p->objectName());
		p = p->parent();
	}
	return name;
}

QDaqObject* QDaqObject::findByName(const QString& name)
{
	QStringList tokens = name.split('.');
	if (tokens.isEmpty()) return 0;

    QDaqObject* o = root();

    if (tokens.front()=="qdaq")
	{
		tokens.pop_front();
		if (tokens.isEmpty()) return o;
	}

	do
	{
		QString str = tokens.front();
		tokens.pop_front();
		QDaqObject* child = 0;
        foreach(QDaqObject* p, o->children_)
		{
            if (p->objectName()==str)
			{
				child = p;
				break;
			}
		}
		o = child;
	}
	while (!tokens.isEmpty() && o);
	return o;
}

void findByWildcardHelper(const QRegExp& rx, QDaqObjectList& lst, const QDaqObject* from)
{
    foreach(QDaqObject* o, from->children())
	{
        if (rx.exactMatch(o->path())) lst.push_back(o);
        findByWildcardHelper(rx,lst,o);
	}
}

QDaqObjectList QDaqObject::findByWildcard(const QString& wildcard, const QDaqObject *from)
{
    QDaqObjectList lst;

    if (from==0) from = root();

	QRegExp rx(wildcard);
	rx.setPatternSyntax(QRegExp::Wildcard);

	findByWildcardHelper(rx, lst, from);

	return lst;
}

void QDaqObject::pushError(const QString& type, const QString& descr) const
{
    QDaqError e(path(), type, descr);
    root()->postError(e);
}

void listPropertiesHelper(const QDaqObject* m_object, QString& S, const QMetaObject* metaObject, int& level)
{
	const QMetaObject* super = metaObject->superClass();

	//if (!super) return;

	if (super) listPropertiesHelper(m_object, S, super, level);

	//if (level) S += '\n';
	//S += metaObject->className();
	//S += " properties:\n";

	QString P;
	for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) 
	{
		QMetaProperty metaProperty = metaObject->property(idx);

		P += "  ";
		P += metaProperty.name();
		P += " : ";


		if (!metaProperty.isReadable()) P += "< Non Readable >";
		else if (metaProperty.isEnumType()) 
		{
			if (metaProperty.isFlagType()) P += "< Flags >";
			else
			{
				QMetaEnum metaEnum = metaProperty.enumerator();
				int i = *reinterpret_cast<const int *>(metaProperty.read(m_object).constData());
				P += metaEnum.valueToKey(i);
			}
		}
		else P += metaProperty.read(m_object).toString();
		P += '\n';
	}

	if (!P.isEmpty())
	{
		S += metaObject->className();
		S += '\n';
		S += P;
	}

	level++;
}

QString QDaqObject::listProperties() const
{
	QString S;
	int level = 0;
	listPropertiesHelper(this, S, metaObject(), level);

    QList<QByteArray> names = dynamicPropertyNames();

    S += QString("Dynamic properties: %1\n").arg(names.size());

    for(int i=0; i<names.size(); ++i)
    {
        QVariant V = property(names.at(i));
        S += "  ";
        S += QString(names.at(i));
        S += " : ";
        S += QString("<variant typeid=%1>,").arg((int)V.type());
        S += QString("<typename=%1>,").arg(V.typeName());
        S += V.toString();
        S += "\n";
    }

	return S;
}

void listFunctionsHelper(QString& S, const QMetaObject* metaObject, int& level)
{
	const QMetaObject* super = metaObject->superClass();

	// if no superclass (I am QObject) return
	//if (!super) return;

	if (super)
		listFunctionsHelper(S, super, level);

	QString P;
	for(int i=metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
	{
		QMetaMethod m = metaObject->method(i);
		if (m.methodType()==QMetaMethod::Slot)
		{
			P += "  ";
            P += m.methodSignature();
			P += '\n';
		}
		else if (m.methodType()==QMetaMethod::Signal)
		{
			P += "  ";
            P += m.methodSignature();
			P += " [signal]";
			P += '\n';
		}
	}

	if (!P.isEmpty())
	{
		S += metaObject->className();
		S += '\n';
		S += P;
	}

	level++;
}

QString QDaqObject::listFunctions() const
{
	QString S;
	int level = 0;
	listFunctionsHelper(S, metaObject(), level);
	return S;
}

void QDaqObject::appendChild(QDaqObject* obj)
{
    if (!obj) {
       throwScriptError("The argument is not a valid object.");
	   return;
    }
    if (!checkName(obj->objectName())) return;

	if (obj->parent()) {
		throwScriptError("This object is a child of another object.");
		return;
	}

    obj->setParent(this);
}

void QDaqObject::insertBefore(QDaqObject *newobj, QDaqObject *existingobj)
{
    if (!newobj) {
       throwScriptError("The 1st argument is not a valid object.");
	   return;
    }

    if (!checkName(newobj->objectName())) return;

	if (newobj->parent()) {
		throwScriptError("The 1st object is a child of another object.");
		return;
	}

    int i = children_.indexOf(existingobj);
    if (i<0)
    {
		this->throwScriptError("2nd argument is not a child object.");
        return;
    }

    newobj->setParent(this);
    // put the new object in the correct order
    children_.removeLast();
    children_.insert(i,newobj);
}

void QDaqObject::childEvent(QChildEvent *event)
{
    QDaqObject* obj = qobject_cast<QDaqObject*>(event->child()); // maybe reinterpret??
    if (obj) {
        int i = children_.indexOf(obj);

        if (event->added() && i<0) {
            children_.append(obj);
            setProperty(obj->objectName().toLatin1().constData(),QVariant::fromValue(obj));
            if (isAttached()) obj->attach();
        }
        if (event->removed() && i>=0) {
            if (isAttached()) obj->detach();
            setProperty(obj->objectName().toLatin1().constData(),QVariant());
            children_.removeAt(i);           
        }
    }

	QObject::childEvent(event);
}

QDaqObject* QDaqObject::removeChild(QDaqObject *obj)
{
    if (!obj)
    {
        throwScriptError("The argument is not a valid object.");
        return 0;
    }

    QDaqObject* childObj = findChild<QDaqObject*>(obj->objectName());
    if (!childObj)
    {
        throwScriptError("The argument is not a valid child object.");
        return 0;
    }
    childObj->setParent(0);
    return childObj;
}

QDaqObject* QDaqObject::replaceChild(QDaqObject *newobj, QDaqObject *oldobj)
{
    if (!newobj)
    {
        throwScriptError("The 1st argument is not a valid object.");
        return 0;
    }
    if (newobj->isAttached())
    {
        throwScriptError("The 1st argument is already in the qdaq tree.");
        return 0;
    }
    if (!oldobj)
    {
        throwScriptError("The 2nd argument is not a valid object.");
        return 0;
    }

    int i = children_.indexOf(oldobj);
    if (i<0)
    {
        throwScriptError("The 2nd argument is not a valid child object.");
        return 0;
    }

    if ((newobj->objectName()!=oldobj->objectName()) &&
            !checkName(newobj->objectName())) return 0;


    oldobj->setParent(0);
    newobj->setParent(this);
    // put the new object in the correct order
    children_.removeLast();
    children_.insert(i,newobj);

    return oldobj;
}

QDaqObject* QDaqObject::clone()
{
    if (this == root())
    {
        throwScriptError("Cannot clone the root");
        return 0;
    }

    const QMetaObject* metaObj = metaObject();
    QDaqObject* _clone = (QDaqObject*)(metaObj->newInstance(Q_ARG(QString,objectName())));

    if (!_clone)
    {
        throwScriptError("Cloning failed!");
        return 0;
    }

    for(int i= 1; i<metaObj->propertyCount(); i++)
    {
        QMetaProperty p = metaObj->property(i);
        if (p.isWritable())
            p.write(_clone, p.read(this));
    }

    foreach(QDaqObject* obj, children_)
    {
        QDaqObject* newobj = obj->clone();
        _clone->appendChild(newobj);
    }

    return _clone;
}

typedef QDaqObject* QDaqObjectStar;
typedef QObject* QObjectStar;

QScriptValue toScriptValue(QScriptEngine *eng, const QDaqObjectStar& obj)
{
    return eng->newQObject(obj,
                           QScriptEngine::AutoOwnership,
                           QScriptEngine::ExcludeDeleteLater |
                           QScriptEngine::AutoCreateDynamicProperties |
                           QScriptEngine::PreferExistingWrapperObject |
                           QScriptEngine::ExcludeChildObjects);
}

void fromScriptValue(const QScriptValue &value, QDaqObjectStar& obj)
{
    obj = qobject_cast<QDaqObject*>(value.toQObject());
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
    quint32 len = value.property("length").toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        L.push_back(qscriptvalue_cast<QDaqObject*>(item));
    }
}

int registerQDaqObjectStar(QScriptEngine* eng)
{
    int i1 = qScriptRegisterMetaType<QDaqObjectStar>(eng,toScriptValue,fromScriptValue);
    int i2 = qScriptRegisterMetaType<QDaqObjectList>(eng,toScriptValue,fromScriptValue);
    return i1 && i2;
}


QDaqErrorQueue::QDaqErrorQueue(QObject *parent) : QObject(parent)
{

}

void QDaqErrorQueue::push(const QDaqError& item)
{
    queue_.push_front(item);
    emit errorAdded();
    if (queue_.size()>ERROR_QUEUE_DEPTH) {
        queue_.takeLast();
        emit errorRemoved();
    }
}

QList<QDaqError> QDaqErrorQueue::objectBackTrace(const QDaqObject* obj, int maxItems) const
{
    QList<QDaqError> errors;
    QString name = obj->path();
    foreach(const QDaqError& e, queue_)
    {
        if (e.objectName==name) errors.push_back(e);
        if (errors.size()==maxItems) break;
    }

    return errors;
}















