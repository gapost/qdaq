#ifndef _RTOBJECTBROWSER_H_
#define _RTOBJECTBROWSER_H_

#include <QWidget> 
#include <QMap>

class QTreeWidget;
class QTreeWidgetItem;
class QTableWidget;
class QTabWidget;
class QDaqObjectController;
class QPushButton;
class QLineEdit;

class QDaqPropertyBrowser;
class QDaqFunctionBrowser;


class QDaqObject;

class QDaqObjectBrowser : public QWidget
{
	Q_OBJECT

    QMap<QDaqObject*,QTreeWidgetItem*> objects2items;
    QMap<QTreeWidgetItem*,QDaqObject*> items2objects;
public:
    QDaqObjectBrowser(QWidget* p = 0);
    virtual ~QDaqObjectBrowser(void);

protected:
    void insertObject(QTreeWidgetItem* parent, QDaqObject* obj, bool recursive=false);
    void removeObject(QDaqObject* obj, bool recursive = true);
    void change(QDaqObject* obj, bool create);

protected slots:
    void slotCurrentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void slotInsertObject(QDaqObject* obj);
    void slotRemoveObject(QDaqObject* obj);
    void slotUpdateItem(QTreeWidgetItem* i);

signals:
    void currentObjectChanged(QDaqObject* obj);
    void updateItem(QTreeWidgetItem* i);

protected:
	QTreeWidget* treeWidget;
    QTabWidget* tabWidget;
    QDaqPropertyBrowser* propertyBrowser;
    QDaqFunctionBrowser* functionBrowser;
    QLineEdit* currentObject;

};

class QDaqPropertyBrowser : public QWidget
{
	Q_OBJECT
public:
    QDaqPropertyBrowser(QWidget *parent = 0);
    ~QDaqPropertyBrowser();

public slots:
    void setQDaqObject(QDaqObject* obj);

private slots:
    void removeQDaqObject() { setQDaqObject(0); }

protected:
    QDaqObjectController* objectController;
};

class QDaqFunctionBrowser : public QWidget
{
	Q_OBJECT
public:
    QDaqFunctionBrowser(QWidget *parent = 0);

public slots:
    void setQDaqObject(QDaqObject* obj);

private slots:
    void removeQDaqObject() { setQDaqObject(0); }

protected:
	void populateMethods(const QMetaObject* mobj);

protected:
	QTreeWidget* methodsTree;
    QDaqObjectController* objectController;
};


#endif


