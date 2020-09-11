#include <QCoreApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QTreeWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QHeaderView>
#include <QMetaObject>
#include <QMetaMethod>
#include <QSplitter>

#include "QDaqObjectModel.h"
#include "QDaqObjectController.h"

#include "QDaqObjectBrowser.h"
#include "QDaqRoot.h"

QDaqObjectBrowser::QDaqObjectBrowser(QWidget* p) : QSplitter(p)
{
    model = new QDaqObjectModel(this);

    treeView = new QTreeView(this);
    treeView->setModel(model);
    treeView->setAlternatingRowColors(true);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    treeView->setHeaderHidden(true);

    currentObject = new QLineEdit(this);

    tabWidget = new QTabWidget(this);
    propertyBrowser = new QDaqPropertyBrowser();
    tabWidget->addTab(propertyBrowser, "Properties");
    functionBrowser = new QDaqFunctionBrowser();
    tabWidget->addTab(functionBrowser, "Functions");

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addWidget(currentObject);
    vlayout->addWidget(tabWidget);

    QWidget* rightWidget = new QWidget;
    rightWidget->setLayout(vlayout);

    addWidget(treeView);
    addWidget(rightWidget);

    connect(treeView,SIGNAL(activated(QModelIndex)),this,SLOT(onItemActivated(QModelIndex)));
    connect(treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(onItemActivated(QModelIndex)));

    connect(currentObject,SIGNAL(editingFinished()),this,SLOT(onSetByUser()));

}

QDaqObjectBrowser::~QDaqObjectBrowser(void)
{
}

void QDaqObjectBrowser::onItemActivated(const QModelIndex &index)
{
    QDaqObject* obj = model->objectAt(index);
    currentObject->setText(obj->path());
    propertyBrowser->setQDaqObject(obj);
    functionBrowser->setQDaqObject(obj);
}

void QDaqObjectBrowser::onSetByUser()
{
    QString str = currentObject->text();
    QDaqObject* obj = QDaqObject::fromPath(str);
    if (!obj) return;
    QModelIndex index = model->index(obj);
    treeView->setCurrentIndex(index);
    propertyBrowser->setQDaqObject(obj);
    functionBrowser->setQDaqObject(obj);
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
        old_obj->removeEventFilter(this);
	}

	objectController->setObject(obj);
	if (obj)
	{
		connect(obj,SIGNAL(propertiesChanged()),objectController,SLOT(updateProperties()));
        connect(obj,SIGNAL(destroyed(QObject*)),this,SLOT(removeQDaqObject()));
        obj->installEventFilter(this);
	}
}

bool QDaqPropertyBrowser::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == objectController->object() &&
        event->type() == QEvent::DynamicPropertyChange)
            objectController->updateDynamicProperties();

    // pass the event on to the parent class
    return QWidget::eventFilter(obj, event);
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

