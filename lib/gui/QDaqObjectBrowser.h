#ifndef _RTOBJECTBROWSER_H_
#define _RTOBJECTBROWSER_H_

#include <QSplitter>

class QDaqObjectModel;
class QTreeView;
class QTableWidget;
class QTabWidget;
class QDaqObjectController;
class QPushButton;
class QLineEdit;
class QTreeWidget;

class QDaqPropertyBrowser;
class QDaqFunctionBrowser;

class QDaqObject;

class QDaqObjectBrowser : public QSplitter
{
	Q_OBJECT

public:
    QDaqObjectBrowser(QWidget* p = 0);
    virtual ~QDaqObjectBrowser(void);

protected slots:
    void onItemActivated(const QModelIndex& index);
    //void slotUpdateItem(QTreeWidgetItem* i);

signals:
    void currentObjectChanged(QDaqObject* obj);
    //void updateItem(QTreeWidgetItem* i);

protected:
    QDaqObjectModel* model;
    QTreeView* treeView;
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


