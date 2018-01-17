#include <QCoreApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QHeaderView>
#include <QMetaObject>
#include <QMetaMethod>

#include "QDaqObjectController.h"

#include "QDaqObjectBrowser.h"
#include "QDaqRoot.h"

QDaqObjectBrowser::QDaqObjectBrowser(QWidget* p) : QWidget(p)
{
	treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(2);
    QStringList headers;
    headers << "Object" << "Class";
    treeWidget->setHeaderLabels(headers);
    treeWidget->setAlternatingRowColors(true);

    currentObject = new QLineEdit(this);

    tabWidget = new QTabWidget(this);
    propertyBrowser = new QDaqPropertyBrowser();
    tabWidget->addTab(propertyBrowser, "Properties");
    functionBrowser = new QDaqFunctionBrowser();
    tabWidget->addTab(functionBrowser, "Functions");

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget(currentObject);
    vlayout->addWidget(tabWidget);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(treeWidget);
    hlayout->addLayout(vlayout);
    setLayout(hlayout);

	foreach(QObject* o, QDaqObject::root()->children())
	{
		if (QDaqObject* rto = qobject_cast<QDaqObject*>(o))
			insertObject(treeWidget->invisibleRootItem(), rto, true);
	}

	connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		this, SLOT(slotCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)) );

	connect(this, SIGNAL(updateItem(QTreeWidgetItem* )),
		this, SLOT(slotUpdateItem(QTreeWidgetItem* )) , Qt::QueuedConnection);

    connect(this, SIGNAL(currentObjectChanged(QDaqObject*)),
        propertyBrowser, SLOT(setQDaqObject(QDaqObject*)));
    connect(this, SIGNAL(currentObjectChanged(QDaqObject*)),
        functionBrowser, SLOT(setQDaqObject(QDaqObject*)));

    // object creation is queued so that object is fully created
    connect(QDaqObject::root(), SIGNAL(objectCreated(QDaqObject*)),
        this, SLOT(slotInsertObject(QDaqObject*))); //, Qt::QueuedConnection);
    // object deletion should be normal so that all objects have the chance to deref
    connect(QDaqObject::root(), SIGNAL(objectDeleted(QDaqObject*)),
        this, SLOT(slotRemoveObject(QDaqObject*))); //, Qt::QueuedConnection);

    //connect(currentObject,SIGNAL(editingFinished()),this,SLOT(setByUser()));

}

QDaqObjectBrowser::~QDaqObjectBrowser(void)
{
}

void QDaqObjectBrowser::change(QDaqObject* obj, bool create)
{
	if (create)
	{
		QTreeWidgetItem* parentitem = objects2items.value(obj->parent());
		if (parentitem) 
		{
			insertObject(parentitem, obj);
			//treeWidget->expandItem(parentitem);
		}
	}
	else removeObject(obj);
}

void QDaqObjectBrowser::insertObject(QTreeWidgetItem* parent, QDaqObject* obj, bool recursive)
{
	QStringList nodedata;
	nodedata <<  obj->objectName() << obj->metaObject()->className();
	QTreeWidgetItem* node = new QTreeWidgetItem(parent, nodedata);
	objects2items.insert(obj,node);
	items2objects.insert(node,obj);
	//treeWidget->expandItem(node);
	emit updateItem(node);

	if (recursive)
	{
		foreach(QObject* o, obj->children())
		{
			if (QDaqObject* rtchild = qobject_cast<QDaqObject*>(o))
				insertObject(node, rtchild);
		}
	}

}

void QDaqObjectBrowser::removeObject(QDaqObject* obj, bool recursive)
{
		QTreeWidgetItem* item = objects2items.value(obj);
		if (item) 
		{
			objects2items.remove(obj);
			items2objects.remove(item);

			if (recursive)
			{
				QList<QTreeWidgetItem *> items = item->takeChildren();
				foreach(QTreeWidgetItem* i, items)
				{
					QDaqObject* obj1 = items2objects.value(i);
					if (obj1) removeObject(obj1);
				}
			}
			delete item;
		}
}

void QDaqObjectBrowser::slotInsertObject(QDaqObject* obj)
{
	change(obj,true);
}

void QDaqObjectBrowser::slotRemoveObject(QDaqObject* obj)
{
	change(obj,false);
}

void QDaqObjectBrowser::slotCurrentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    Q_UNUSED(previous);
	QDaqObject* obj = items2objects.value(current);
    if (obj) {
        currentObject->setText(obj->fullName());
        emit currentObjectChanged(obj);
    }
}

void QDaqObjectBrowser::slotUpdateItem( QTreeWidgetItem * i)
{
	QDaqObject* obj = items2objects.value(i);
	if (obj)
		i->setData(1,Qt::DisplayRole,obj->metaObject()->className());
}

//***********************************************************************//
QDaqPropertyBrowser::QDaqPropertyBrowser(QWidget *parent) : QWidget(parent)
{
    objectController = new QDaqObjectController(this);

	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->addWidget(objectController);

	setLayout(vlayout);
}
    
QDaqPropertyBrowser::~QDaqPropertyBrowser()
{
}

void QDaqPropertyBrowser::setQDaqObject(QDaqObject* obj)
{
	QDaqObject* old_obj = qobject_cast<QDaqObject*>(objectController->object());
	if (old_obj) 
	{
		disconnect(old_obj,SIGNAL(propertiesChanged()),objectController,SLOT(updateProperties()));
        disconnect(old_obj,SIGNAL(destroyed(QObject*)),this,SLOT(removeQDaqObject()));
		//currentObject->setText("");
	}

	objectController->setObject(obj);
	if (obj)
	{
		connect(obj,SIGNAL(propertiesChanged()),objectController,SLOT(updateProperties()));
        connect(obj,SIGNAL(destroyed(QObject*)),this,SLOT(removeQDaqObject()));
	}
}


//***********************************************************************//
QDaqFunctionBrowser::QDaqFunctionBrowser(QWidget *parent) : QWidget(parent)
{
	methodsTree = new QTreeWidget(this);
	methodsTree->setColumnCount(1);
	methodsTree->setAlternatingRowColors(true);
	methodsTree->setHeaderHidden(true);

	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->addWidget(methodsTree);

	setLayout(vlayout);

}

void QDaqFunctionBrowser::setQDaqObject(QDaqObject* obj)
{
    // TODO
/*    QDaqObject* old_obj = qobject_cast<QDaqObject*>(objectController->object());
    if (old_obj)
    {
        disconnect(old_obj,SIGNAL(destroyed(QObject*)),this,SLOT(removeQDaqObject()));
    }*/

	if (obj)
	{
        connect(obj,SIGNAL(destroyed(QObject*)),this,SLOT(removeQDaqObject()));
		populateMethods(obj->metaObject());
	}
	else
	{
		methodsTree->clear();
	}
}

void QDaqFunctionBrowser::populateMethods(const QMetaObject* metaObject)
{
	methodsTree->clear();

	if (metaObject->superClass()==0) // stop at QObject
        return;

    populateMethods(metaObject->superClass());

	QTreeWidgetItem* iclass = new QTreeWidgetItem(QStringList(metaObject->className()));
	methodsTree->addTopLevelItem(iclass);
	QTreeWidgetItem* islots = new QTreeWidgetItem(iclass, QStringList("Methods"));
	QTreeWidgetItem* isignals = new QTreeWidgetItem(iclass, QStringList("Signals"));
	for(int i=metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
	{
        QMetaMethod m = metaObject->method(i);
#if QT_VERSION >= 0x050000
        if (m.methodType()==QMetaMethod::Slot)
        {
            new QTreeWidgetItem(islots, QStringList(m.methodSignature()));
        }
        else if (m.methodType()==QMetaMethod::Signal)
        {
            new QTreeWidgetItem(isignals, QStringList(m.methodSignature()));
        }
    }
#else
		if (m.methodType()==QMetaMethod::Slot)
		{
			new QTreeWidgetItem(islots, QStringList(m.signature()));
		}
		else if (m.methodType()==QMetaMethod::Signal)
		{
			new QTreeWidgetItem(isignals, QStringList(m.signature()));
		}
	}
#endif


}

